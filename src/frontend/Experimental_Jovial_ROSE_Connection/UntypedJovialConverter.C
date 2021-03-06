#include "sage3basic.h"
#include "UntypedJovialConverter.h"
#include "Jovial_to_ROSE_translation.h"
#include "general_language_translation.h"

#define DEBUG_UNTYPED_CONVERTER 0

using namespace Untyped;
using std::cout;
using std::endl;


bool
UntypedJovialConverter::convertLabel(SgUntypedStatement* ut_stmt, SgStatement* sg_stmt,
                                     SgLabelSymbol::label_type_enum label_type, SgScopeStatement* label_scope)
{
   return UntypedJovialConverter::convertLabel(ut_stmt, sg_stmt, label_scope);
}

bool
UntypedJovialConverter::convertLabel(SgUntypedStatement* ut_stmt, SgStatement* sg_stmt, SgScopeStatement* scope)
{
   SgNode* parent;
   SgUntypedLabelStatement* ut_label_stmt;
   bool hasLabel = false;

   ROSE_ASSERT(scope != NULL);

   parent = ut_stmt->get_parent();
   if (parent != NULL)
     {
        ut_label_stmt = isSgUntypedLabelStatement(parent);
        if (ut_label_stmt != NULL)
          {
             SgLabelSymbol* label_symbol = NULL;
             SgLabelStatement* label_stmt = NULL;

          // This statement has a label get the SgLabelStatement from the scope and insert it
             SgName label_name(ut_label_stmt->get_label_string());

             SgFunctionDefinition * label_scope = SageInterface::getEnclosingFunctionDefinition(scope, true);
             ROSE_ASSERT (label_scope);

             label_symbol = label_scope->lookup_label_symbol(label_name);
             ROSE_ASSERT(label_symbol != NULL);

             label_stmt = label_symbol->get_declaration();
             ROSE_ASSERT(label_stmt != NULL);

             label_stmt->set_statement(sg_stmt);
             sg_stmt->set_parent(label_stmt);

             hasLabel = true;
          }
     }

   return hasLabel;
}

SgDeclarationStatement*
UntypedJovialConverter::convertUntypedJovialCompoolStatement(SgUntypedNameListDeclaration* ut_decl, SgScopeStatement* scope)
{
   ROSE_ASSERT(ut_decl);
   ROSE_ASSERT(ut_decl->get_statement_enum() == General_Language_Translation::e_jovial_compool_stmt);

   SgUntypedNamePtrList ut_names = ut_decl->get_names()->get_name_list();
   ROSE_ASSERT(ut_names.size() == 1);

   SgUntypedName* name = ut_names[0];
   SgJovialCompoolStatement* compool_decl = new SgJovialCompoolStatement(name->get_name());
   setSourcePositionFrom(compool_decl, ut_decl);

   compool_decl->set_definingDeclaration(compool_decl);
   compool_decl->set_firstNondefiningDeclaration(compool_decl);

   SageInterface::appendStatement(compool_decl, scope);

   return compool_decl;
}

SgDeclarationStatement*
UntypedJovialConverter::convertUntypedStructureDeclaration(SgUntypedStructureDeclaration* ut_struct, SgScopeStatement* scope)
{
   SgName type_name = ut_struct->get_name();

   SgUntypedStructureDefinition* ut_table_def = ut_struct->get_definition();
   ROSE_ASSERT(ut_table_def);

   bool has_body      = ut_table_def->get_has_body();
   bool has_type_name = ut_table_def->get_has_type_name();
   bool has_base_type = (has_type_name == false && has_body == false);

// TODO - need to find whether e_jovial_table or e_jovial_block
   SgClassDeclaration::class_types struct_kind = SgClassDeclaration::e_jovial_table;

   SgType* sg_base_type = NULL;
   SgUntypedType* ut_base_type  = ut_table_def->get_base_type();
   if (has_base_type)
      {
         ROSE_ASSERT(ut_base_type != NULL);
         sg_base_type = convertUntypedType(ut_base_type, scope);
      }

#if 0
      cout << "\n-x- convertUntypedStructureDeclaration: specialization for Jovial \n";
      //      cout << "-x- TODO: convertUntypedStructureDeclaration: decl_list size is ";
      //      cout << ut_table_def->get_scope()->get_declaration_list()->get_decl_list().size();
      cout << "-x- table def scope is " << ut_table_def->get_scope() << endl;
      cout << "-x- has_type_name " << has_type_name << endl;
      cout << "-x- has_body " << has_body << endl;
      cout << "-x- has_base_type " << has_base_type << endl;
      cout << "-x- structure type enum " << ut_struct->get_statement_enum() << endl;

      if (has_base_type) cout << "-x- ut_base_type " << ut_base_type << " : " << ut_base_type->class_name() << endl;
      if (has_base_type) cout << "-x- sg_base_type " << sg_base_type << " : " << sg_base_type->class_name() << endl;

      cout << "-x-         modifier list is " << ut_table_def->get_modifiers() << endl;
      ROSE_ASSERT(ut_table_def->get_modifiers() != NULL);
      cout << "-x- size of modifier list is " << ut_table_def->get_modifiers()->get_expressions().size() << endl;

      cout << "..........\n\n";
      cout << "-x- creating SgClassDeclaration for name " << type_name << endl;
#endif

   // This function builds a class declaration and definition with both the defining and nondefining declarations as required
      SgJovialTableStatement * table_decl = SageBuilder::buildJovialTableStatement(type_name, struct_kind, scope);
      ROSE_ASSERT(table_decl);
      setSourcePositionFrom(table_decl, ut_struct);

      int struct_type = ut_struct->get_statement_enum();
      if (struct_type == Jovial_ROSE_Translation::e_block_type_declaration)
         {
            table_decl->set_class_type(SgClassDeclaration::e_jovial_block);
         }
      else if (struct_type == Jovial_ROSE_Translation::e_table_type_declaration)
         {
            table_decl->set_class_type(SgClassDeclaration::e_jovial_table);
         }

      SgType* sg_type = table_decl->get_type();
      ROSE_ASSERT(sg_type);

      SgJovialTableType* sg_table_type = isSgJovialTableType(sg_type);
      ROSE_ASSERT(sg_table_type);

#if 0
      cout << "-x- sg_table_type " << sg_table_type << endl;
      cout << "-x-  base_type " << sg_table_type->get_base_type() << endl;
      cout << "-x- ut_dim_info " << ut_struct->get_dim_info() << endl;
      cout << "-x- found dim_info expr kind is " << ut_struct->get_dim_info()->get_expression_enum() << endl;
#endif

   // Transfer type information
      sg_table_type->set_base_type(sg_base_type);

   // A Jovial table may have array dimensions, set this information in the type

// delete this when possible after ROSETTA change (also delete in Fortran; or delete in parent destructor???)
//    SgExprListExp* shape = convertUntypedExprListExpression(ut_struct->get_dim_info(),/*delete*/true);
      SgExprListExp* shape = convertUntypedExprListExpression(ut_struct->get_dim_info(),/*delete*/false);
      ROSE_ASSERT(shape);

      sg_table_type->set_dim_info(shape);
      sg_table_type->set_rank(shape->get_expressions().size());

#if 0
      cout << "-x- has rank " << sg_table_type->get_rank() << endl;
#endif

      if (ut_table_def->get_modifiers()->get_expressions().size() > 0)
         {
            ROSE_ASSERT(ut_table_def->get_modifiers()->get_expressions().size() == 1);

            cout << "-x- found modifiers: " << ut_table_def->get_modifiers() << " : size is " << ut_table_def->get_modifiers()->get_expressions().size() << endl;

            SgUntypedExpression* words_per_entry = ut_table_def->get_modifiers()->get_expressions()[0];
            ROSE_ASSERT(words_per_entry != NULL);
            cout << "-x- found words_per_entry: " << words_per_entry << " : " << words_per_entry->class_name() << endl;

            if (words_per_entry->get_expression_enum() == Jovial_ROSE_Translation::e_words_per_entry_v)
               {
               // TODO - fix ROSETTA so this doesn't depend on NULL for entry size, has_table_entry_size should be table_entry_enum (or some such)
                  table_decl->set_has_table_entry_size(true);
                  table_decl->set_table_entry_size(NULL);
                  ROSE_ASSERT(isSgUntypedOtherExpression(words_per_entry));
                  delete words_per_entry;
               }

            else if (words_per_entry->get_expression_enum() == Jovial_ROSE_Translation::e_words_per_entry_w)
               {
                  SgExprListExp* sg_expr_list = convertUntypedExprListExpression(ut_table_def->get_modifiers(),/*delete*/false);
                  ROSE_ASSERT(sg_expr_list);
                  ROSE_ASSERT(sg_expr_list->get_expressions().size() == 1);

                  table_decl->set_has_table_entry_size(true);
                  table_decl->set_table_entry_size(sg_expr_list->get_expressions()[0]);
               }
         }

      SgClassDefinition* table_def = table_decl->get_definition();
      ROSE_ASSERT(table_def);

// If there is a (base class) type name, a base class for inheritance needs to be created
#if 0
   if (has_type_name)
      {
         std::string base_type_name = ut_table_def->get_type_name();
         SgClassSymbol* class_symbol = SageInterface::lookupClassSymbolInParentScopes(base_type_name, scope);

         if (class_symbol == NULL)
            {
               cout << "--- convertUntypedStructureDeclaration class_symbol is NULL for table base type name " << base_type_name << endl;
               ROSE_ASSERT(false);
            }

         cout << "-x- class_symbol is " << class_symbol << " : " << class_symbol->class_name() << " : base class name is " << class_symbol->get_name() << endl;

         SgClassDeclaration* base_class_decl = class_symbol->get_declaration();
         ROSE_ASSERT(base_class_decl != NULL);

         cout << "-x- base_class_decl " << base_class_decl << " : " << base_class_decl->class_name() << endl;

         // class decl (NO) and def (YES) are from the base class
         SgBaseClass* base_class = SageBuilder::buildBaseClass(base_class_decl, table_def, /*isVirtual*/false, /*isDirect*/true);
         ROSE_ASSERT(base_class);
      }
#endif

   // Jovial is insensitive to case
   // TODO: src/midend/astDiagnostics/AstConsistencyTests.C, line 6406
   // table_def->setCaseInsensitive(true);

   // Perhaps don't need this
      SgScopeStatement* table_scope = table_def->get_scope();
      ROSE_ASSERT(table_scope);

   // delete untyped structure members that aren't traversed
   //
      if (ut_table_def->get_modifiers()) delete ut_table_def->get_modifiers(); ut_table_def->set_modifiers(NULL);

      SageInterface::appendStatement(table_decl, scope);

   // The table description (SgUntypedStructureDefinition) will be traversed and the table items
   // need to be added to the table_scope.  Pushing the table definition allows the members
   // to be added to the correct scope.
      SageBuilder::pushScopeStack(table_def);

#if 0
      cout << "--- TABLE \n";
//    cout << "---        var_decl is " << var_decl << ": " << var_decl->class_name() << endl;
      cout << "---      table_decl is " << table_decl << ": " << table_decl->class_name() << endl;
      cout << "---       table_def is " << table_def << ": " << table_def->class_name() << endl;
      cout << "---      table_type is " << sg_table_type << ": " << sg_table_type->class_name() << endl;
      cout << "---      table rank is " << sg_table_type->get_rank() << endl;
      cout << "--- table base type is " << sg_table_type->get_base_type() << endl;
      if (has_base_type) cout << "--- table base type is " << sg_table_type->get_base_type()->class_name() << endl;
#endif

      return table_decl;
}

// This has been cleaned up for Jovial (when finished it should be moved to UntypedConverter for Fortran)
//
SgVariableDeclaration* UntypedJovialConverter::
convertUntypedVariableDeclaration (SgUntypedVariableDeclaration* ut_decl, SgScopeStatement* scope)
{
   ROSE_ASSERT(ut_decl != NULL);
   ROSE_ASSERT(scope   != NULL);

   ROSE_ASSERT(scope->variantT() == V_SgBasicBlock || scope->variantT() == V_SgClassDefinition
            || scope->variantT() == V_SgGlobal);  // global scope used for Jovial

   SgUntypedType* ut_type = ut_decl->get_type();
   ROSE_ASSERT(ut_type != NULL);

   SgType* sg_type = convertUntypedType(ut_type, scope);
   ROSE_ASSERT(sg_type != NULL);

// There will be only one variable for Jovial
   SgUntypedInitializedNamePtrList ut_vars = ut_decl->get_variables()->get_name_list();
   SgUntypedInitializedName*  ut_init_name = ut_vars[0];

   SgName var_name = ut_init_name->get_name();
   SgInitializer* sg_initializer = convertUntypedInitializerOnly(ut_init_name);

// The scope specifies the scope for the variable (SgInitializedName) not the variable declaration.
   SgVariableDeclaration* sg_decl = SageBuilder::buildVariableDeclaration_nfi(var_name, sg_type, sg_initializer, scope);
   ROSE_ASSERT(sg_decl != NULL);
   setSourcePositionFrom(sg_decl, ut_decl);

   setDeclarationModifiers(sg_decl, ut_decl->get_modifiers());

#if 0
   cout << "\n--- convertUntypedVariableDeclaration: JOVIAL converter subclass \n";
   cout << "-x- convertUntypedVariableDeclaration:  ut_type enum is " << ut_type->get_type_enum_id() << ": e_table is " << SgUntypedType::e_table << endl;
   cout << "--- convertUntypedVariableDeclaration: var name is " << ut_init_name->get_name() << endl;
   cout << "--- convertUntypedVariableDeclaration: has init is " << ut_init_name->get_has_initializer() << endl;
   cout << "--- convertUntypedVariableDeclaration:    initr is " << sg_initializer << endl;
   cout << "--- convertUntypedVariableDeclaration:  ut_decl is " << ut_decl << ": " << ut_decl->class_name() << endl;
   cout << "--- convertUntypedVariableDeclaration:  ut_type is " << ut_type << ": " << ut_type->class_name() << endl;
   cout << "--- convertUntypedVariableDeclaration:  sg_type is " << sg_type << ": " << sg_type->class_name() << endl;
#endif

   SageInterface::appendStatement(sg_decl, scope);
   convertLabel(ut_decl, sg_decl, scope);

   return sg_decl;
}

// This function is traversed on the way back up the tree, therefore many member variables of the untyped nodes will
// have been deleted and will not be available.  Thus the requirement for var_name, ...
SgVariableDeclaration* UntypedJovialConverter::
convertUntypedVariableDeclaration(SgUntypedVariableDeclaration* ut_decl, SgScopeStatement* scope,
                                  std::string base_type_name, std::string var_name, SgInitializer* var_initializer)
{
   // 1. This is an anonymous type declaration, there should be a type in the symbol table, look it up
   // 2. Get the declaration from the type
   // 3. Convert the untyped variable decl using the base type declaration

   SgClassSymbol* class_symbol = SageInterface::lookupClassSymbolInParentScopes(base_type_name, scope);
   ROSE_ASSERT(class_symbol != NULL);

   SgClassDeclaration* nondef_decl = class_symbol->get_declaration();
   ROSE_ASSERT(nondef_decl != NULL);

   SgDeclarationStatement* def_decl = nondef_decl->get_definingDeclaration();
   ROSE_ASSERT(def_decl != NULL);

   SgType* class_type = class_symbol->get_type();
   ROSE_ASSERT(class_type != NULL);

// The defining declaration of the anonymous struct for the variable has been created while traversing down the tree.
// It should be removed and transfered to the variable declaration as the baseTypeDefiningDeclaration.
   SageInterface::removeStatement(def_decl);

// The scope specifies the scope for the variable (SgInitializedName) not the variable declaration.
// TODO - figure out why the proper scope isn't on the stack, I think the current scope is the scope of the def_decl not its scope

   SgScopeStatement* def_decl_scope = def_decl->get_scope();

   SgVariableDeclaration* sg_decl = SageBuilder::buildVariableDeclaration_nfi(var_name, class_type, var_initializer, def_decl_scope);
   ROSE_ASSERT(sg_decl != NULL);
   setSourcePositionFrom(sg_decl, ut_decl);

   sg_decl->set_variableDeclarationContainsBaseTypeDefiningDeclaration(true);
   sg_decl->set_baseTypeDefiningDeclaration(def_decl);
   def_decl->set_parent(sg_decl);
   nondef_decl->set_parent(sg_decl);

#if 0
   cout << "\n-x- UntypedJovialConverter::convertUntypedVariableDeclaration (with base_type_name): IMPLEMENTING for " << base_type_name << endl;
   cout << "--- found class symbol is " << class_symbol << ": " << class_symbol->class_name() << endl;
   cout << "--- found nondef_decl is " << nondef_decl << ": " << nondef_decl->class_name() << endl;
   cout << "--- found def_decl is " << def_decl << ": " << def_decl->class_name() << endl;
   cout << "--- found class_type is " << class_type << ": " << class_type->class_name() << endl << endl;
   cout << "--- removing def_decl is " << def_decl << ": " << def_decl->class_name() << endl << endl;
   cout << "--- convertUntypedVariableDeclaration(up): probably a scope issue " << def_decl_scope << ": " << def_decl_scope->class_name() << endl;
   cout << "--- convertUntypedVariableDeclaration(up): top of scope stack is " << SageBuilder::topScopeStack() << ": " << SageBuilder::topScopeStack()->class_name() << endl;
   cout << "--- setting def_decl is " << def_decl << ": " << def_decl->class_name() << endl;
   cout << "--- convertUntypedVariableDeclaration(up): var_name is " << var_name << endl;
   cout << "--- convertUntypedVariableDeclaration(up): sg_decl is " << sg_decl << ": " << sg_decl->class_name() << endl;
#endif

   SageInterface::appendStatement(sg_decl, def_decl_scope);

   return NULL;
}

SgStatement*
UntypedJovialConverter::convertUntypedCaseStatement (SgUntypedCaseStatement* ut_stmt, SgNodePtrList& children, SgScopeStatement* scope)
  {
     SgStatement* sg_stmt = UntypedConverter::convertUntypedCaseStatement(ut_stmt, children, scope);

  // If a Jovial CaseAlternative rule doesn't have a FALLTHRU, then create a
  // compiler-generated break statement so that program analysis will be the same as for C.
  //
     if (ut_stmt->get_has_fall_through() == false  &&  isSgSwitchStatement(sg_stmt) == NULL)
       {
          SgBreakStmt* sg_break_stmt = SageBuilder::buildBreakStmt();
          ROSE_ASSERT(sg_break_stmt);

          sg_break_stmt->setCompilerGenerated();
          SageInterface::appendStatement(sg_break_stmt, scope);
       }

     return sg_stmt;
  }

SgStatement*
UntypedJovialConverter::convertUntypedForStatement (SgUntypedForStatement* ut_stmt, SgNodePtrList& children, SgScopeStatement* scope)
   {
      ROSE_ASSERT(children.size() == 4);

      SgStatement* sg_stmt = NULL;
      int stmt_enum = ut_stmt->get_statement_enum();

      SgAssignOp* init_expr = isSgAssignOp(children[0]);
      ROSE_ASSERT(init_expr);

      SgExpression* test_expr = isSgExpression(children[1]);
      ROSE_ASSERT(test_expr);

      SgExpression* incr_expr = isSgExpression(children[2]);
      ROSE_ASSERT(incr_expr);

      SgStatement* loop_body = isSgStatement(children[3]);
      ROSE_ASSERT(loop_body);

#if 0
      cout << "-x- convert ForStatement # children is " << children.size() << endl;
      cout << "-x- convert for: initialization is " << init_expr << ": " << init_expr->class_name() << endl;
      cout << "-x- convert for: increment      is " << incr_expr << ": " << incr_expr->class_name() << endl;
      cout << "-x- convert for: test           is " << test_expr << ": " << test_expr->class_name() << endl;
      cout << "-x- convert for: body           is " << loop_body << ": " << loop_body->class_name() << endl;
#endif

   // The loop body (at least for a single statement) will have been inserted in the scope
      SageInterface::removeStatement(loop_body, scope);

      switch (stmt_enum)
      {
        case Jovial_ROSE_Translation::e_for_by_while_stmt:
        case Jovial_ROSE_Translation::e_for_while_by_stmt:
          {
             SgExprStatement* init_stmt = SageBuilder::buildExprStatement(init_expr);
             SgExprStatement* test_stmt = SageBuilder::buildExprStatement(test_expr);
                                sg_stmt = SageBuilder::buildForStatement(init_stmt, test_stmt, incr_expr, loop_body, NULL);
             break;
          }
        case Jovial_ROSE_Translation::e_for_then_while_stmt:
        case Jovial_ROSE_Translation::e_for_while_then_stmt:
          {
             SgBasicBlock* block_body = isSgBasicBlock(loop_body);
             if (block_body == NULL) {
                block_body = SageBuilder::buildBasicBlock(loop_body);
             }
             ROSE_ASSERT (block_body);

          // TODO: create a SageBuilder function for this
             sg_stmt = new SgJovialForThenStatement(init_expr, incr_expr, test_expr, block_body);

             SageInterface::setSourcePosition(sg_stmt);
             init_expr->set_parent(sg_stmt);
             incr_expr->set_parent(sg_stmt);
             test_expr->set_parent(sg_stmt);
             block_body->set_parent(sg_stmt);

             break;
          }
        default:
          {
             ROSE_ASSERT(0);
          }
      }

      ROSE_ASSERT(sg_stmt != NULL);
      setSourcePositionFrom(sg_stmt, ut_stmt);

      SageInterface::appendStatement(sg_stmt, scope);

      return sg_stmt;
   }
