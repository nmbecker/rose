
#ifndef __DLX_KLT_COMPILER_HPP__
#define __DLX_KLT_COMPILER_HPP__

#include "DLX/Core/parser.hpp"
#include "DLX/Core/frontend.hpp"
#include "DLX/Core/compiler.hpp"
#include "DLX/KLT/loop-trees.hpp"

#include "KLT/Core/loop-trees.hpp"
#include "KLT/Core/data.hpp"
#include "KLT/Core/loop-tiler.hpp"
#include "KLT/Core/loop-mapper.hpp"
#include "KLT/Core/data-flow.hpp"
#include "KLT/Core/cg-config.hpp"
#include "KLT/Core/generator.hpp"
#include "KLT/Core/kernel.hpp"
#include "KLT/Core/mfb-klt.hpp"

#include "MFB/Sage/driver.hpp"
#include "MFB/Sage/class-declaration.hpp"
#include "MFB/Sage/variable-declaration.hpp"

#include "MDCG/model-builder.hpp"
#include "MDCG/static-initializer.hpp"

namespace DLX {

namespace KLT {

template <class Language_in, class Language_out, class Runtime, class KernelDesc>
void compile(SgProject * project, const std::string & include_path, const std::string & kernel_file, const std::string & static_file) {

  typedef ::DLX::Directives::directive_t<Language_in> directive_t;

  typedef ::DLX::KLT::Annotation<Language_in> Annotation;
  typedef ::KLT::LoopTrees<Annotation> LoopTrees;
  typedef ::KLT::Kernel<Annotation, Language_out, Runtime> Kernel;

  // Declarations

    ::DLX::Frontend::Frontend<Language_in> frontend;

    ::MFB::Driver< ::MFB::KLT> driver(project);

    ::MDCG::ModelBuilder model_builder(driver);

    unsigned model = Runtime::loadAPI(model_builder, include_path);

    ::KLT::Generator<Annotation, Language_out, Runtime, ::MFB::Driver< ::MFB::KLT> > generator(driver, kernel_file);
    ::KLT::CG_Config<Annotation, Language_out, Runtime> cg_config;

  // Initialize language description

    Language_in::init();

  // Run DLX frontend

    if (!frontend.parseDirectives(project)) {
      std::cerr << "Error in FrontEnd !!!" << std::endl;
      exit(1);
    }

  // Extract LoopTrees

    std::map<directive_t *, LoopTrees *> loop_trees;
    std::map<SgForStatement *, typename LoopTrees::loop_t *> loop_map;
    ::DLX::KLT::extractLoopTrees(frontend.directives, loop_trees, loop_map);

  // Generate Kernels

    std::vector<Kernel *> all_kernels;
    std::map<directive_t *, Kernel *> kernel_map;

    typename std::map<directive_t *, LoopTrees *>::const_iterator it_loop_tree;
    for (it_loop_tree = loop_trees.begin(); it_loop_tree != loop_trees.end(); it_loop_tree++) {
      // Generate Kernels
      std::set<std::list<Kernel *> > kernel_lists;
      generator.generate(*(it_loop_tree->second), kernel_lists, cg_config);

      // Assume only one implementation of the kernel made of one kernel
      assert(kernel_lists.size() == 1);
      assert(kernel_lists.begin()->size() == 1);
      Kernel * kernels = *kernel_lists.begin()->begin();
      assert(kernels->getKernels().size() == 1);

      kernel_map.insert(std::pair<directive_t *, Kernel *>(it_loop_tree->first, kernels));
      all_kernels.push_back(kernels);
    }

  // Replace annotated code by host runtime code

  typename std::map<directive_t *, Kernel *>::const_iterator it_kernel;
  for (it_kernel = kernel_map.begin(); it_kernel != kernel_map.end(); it_kernel++) {
    typename Language_in::kernel_construct_t * kernel_construct = Language_in::isKernelConstruct(it_kernel->first->construct);
    assert(kernel_construct != NULL);
    Runtime::useSymbolsHost(driver, driver.getFileID(kernel_construct->assoc_nodes.kernel_region->get_scope()));
    SageInterface::replaceStatement(kernel_construct->assoc_nodes.kernel_region, ::KLT::intantiateOnHost<Annotation, Language_out, Runtime>(it_kernel->second));
  }

  // Stores static description of the generated kernels

    unsigned host_data_file_id = driver.create(boost::filesystem::path(static_file));
      driver.setUnparsedFile(host_data_file_id);
      driver.setCompiledFile(host_data_file_id);
    Runtime::useSymbolsHost(driver, host_data_file_id);

    ::MDCG::StaticInitializer static_initializer(driver);

    std::ostringstream decl_name;
      decl_name << "kernel_desc";

    std::set< ::MDCG::Model::class_t> classes;
    model_builder.get(model).lookup< ::MDCG::Model::class_t>("kernel_desc_t", classes);
    assert(classes.size() == 1);

    static_initializer.createArrayPointer<KernelDesc>(
      *(classes.begin()),
      all_kernels.size(),
      all_kernels.begin(),
      all_kernels.end(),
      host_data_file_id,
      decl_name.str()
    );

  // Removes all pragma

    std::vector<SgPragmaDeclaration * > pragma_decls = SageInterface::querySubTree<SgPragmaDeclaration>(project);
    std::vector<SgPragmaDeclaration * >::iterator it_pragma_decl;
    for (it_pragma_decl = pragma_decls.begin(); it_pragma_decl != pragma_decls.end(); it_pragma_decl++) {
      std::string directive_string = (*it_pragma_decl)->get_pragma()->get_pragma();
      if (::DLX::Frontend::consume_label(directive_string, Language_in::language_label))
        SageInterface::removeStatement(*it_pragma_decl);
    }
}

} // namespace DLX::KLT

} // namespace DLX

#endif /* __DLX_KLT_COMPILER_HPP__ */

