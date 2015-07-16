
#include "sage3basic.h"

#include "DLX/TileK/language.hpp"
typedef ::DLX::TileK::language_t Dlang; // Directives Language

#include "DLX/KLT/annotations.hpp"
typedef ::DLX::KLT::Annotation<Dlang> Annotation;

#include "KLT/Language/c-family.hpp"
typedef ::KLT::Language::C Hlang; // Host Language
typedef ::KLT::Language::C Klang; // Kernel Language

#include "MDCG/KLT/runtime.hpp"
typedef ::MDCG::KLT::Runtime<Hlang, Klang> Runtime; // Runtime Description

#include "MDCG/KLT/model.hpp"
typedef ::MDCG::KLT::KernelDesc<Hlang, Klang> KernelDesc; // Model for Static Initializer

#include "DLX/KLT/compiler.hpp"

#include <cassert>

int main(int argc, char ** argv) {
  SgProject * project = new SgProject(argc, argv);
  assert(project->numberOfFiles() == 1);

  SgSourceFile * source_file = isSgSourceFile(project->get_fileList()[0]);
  assert(source_file != NULL);

  std::string filename = source_file->get_sourceFileNameWithoutPath();
  std::string basename = filename.substr(0, filename.find_last_of('.'));

  DLX::KLT::compile<Dlang, Hlang, Klang, Runtime, KernelDesc>(project, std::string(KLT_RTL_INC_PATH), std::string(TILEK_RTL_INC_PATH), basename + "-kernel.c", basename + "-data.c");

  project->unparse();

  return 0;
}

