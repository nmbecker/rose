#ifndef LOWERING_H
#define LOWERING_H

// Author: Markus Schordan 2018.

#include <list>
#include <utility>

class SgNode;
class SgWhileStmt;
class SgDoWhileStmt;
class SgForStatement;
class SgSwitchStatement;

#include "NormalizationOp.h"

namespace SPRAY {

  class Normalization {
  public:
    struct Options {
      enum Mode { ONLY_FUN_CALL_EXP, ALL_EXP };
      Mode mode=ONLY_FUN_CALL_EXP;
      bool normalizeSingleStatements=true;
      bool eliminateForStatements=true; // replaced by while
      bool eliminateWhileStatements=true; // replace by if/goto
      // eliminate operators '||', '&&'
      bool eliminateShortCircuitOperators=false;
      // eliminate operator '?'
      bool eliminateConditionalExpressionOp=false;
      bool normalizeVariableDeclarations=false;
      bool inlining=false;
    } options;

    // applies all available lowering operations
    void normalizeAst(SgNode* root);
    // calls ROSE SageInterface function for inlining
    size_t inlineFunctions(SgNode* root);
    static void setLabelPrefix(std::string prefix);
    static std::string newLabelName();
    /* normalize all Expressions in AST. The original variables remain
     * in the program and are assign the last value of the sequence of
     * operations of an expression. */
    void normalizeExpressionsInAst(SgNode* node, bool onlyNormalizeFunctionCallExpressions=false);

    // moves conditions out of if and switch constructs
    void hoistConditionsInAst(SgNode* node, bool onlyNormalizeFunctionCallExpressions=false);

    // moves conditions out of if and switch constructs. Declares new
    // variable for intermediate result
    void hoistCondition(SgStatement* node);

    // enable/disable inling. By default off.
    void setInliningOption(bool flag);
    bool getInliningOption();
    //void setKeepOmpForStmts(bool flag); TODO
    //bool getKeepOmpForStmts(); TODO

    static SgLabelStatement* createLabel(SgStatement* target);
    static void createGotoStmtAtEndOfBlock(SgLabelStatement* newLabel, SgBasicBlock* block, SgStatement* target);
    static SgGotoStatement* createGotoStmtAndInsertLabel(SgLabelStatement* newLabel, SgStatement* target);
    bool hasFunctionCall(SgExpression* expr);
    // normalizes variable declarations T x=init to T x; x=init; 
    void normalizeAllVariableDeclarations(SgNode* node);
    int inlineDepth=10; // default value, can be overwritten
  private:
    // normalizes all single statements in if-statements to blocks (sage version)
    void normalizeSingleStatementsToBlocks(SgNode* node);
    /* Given 'Type x=init;' is transformed into 'Type x;' and returns 'x=init;'
       return nullptr if provided declaration is in global scope (cannot be normalized) */
    SgStatement* buildNormalizedVariableDeclaration(SgVariableDeclaration* varDecl);
    // create a lowering sequence of NormalizationOp objects
    void createLoweringSequence(SgNode* root);
    // apply lowering sequence generated by createLoweringSequence
    void applyLoweringSequence();
    // transform expression with root ExprStatement into a list of separate assignments
    void normalizeExpression(SgExprStatement* stmt, SgExpression* node);

    void generateTmpVarAssignment(SgExprStatement* stmt, SgExpression* expr);
    // Finds needle in haystack and returns true if found.  Needle is a single node (possibly an invalid pointer and will not be
    // dereferenced) and haystack is the root of an abstract syntax (sub)tree.
    static bool isAstContaining(SgNode *haystack, SgNode *needle);
    // converts for-stmts to while-stmts (uses SageInterface function)
    void convertAllForsToWhiles (SgNode* top);
    /* If the given statement contains any break statements in its body,
       add a new label below the statement and change the breaks into
       gotos to that new label. Addresses for,while,switch stmts.
    */
    void changeBreakStatementsToGotos (SgNode *ast);

    // counter for generating new variable names (currently not used)
    static int32_t tmpVarNr;
    // counter for generating new label names
    static int labelNr;
    static std::string labelPrefix;

    typedef std::list<std::pair<SgStatement*,SgExpression*> > TransformationList;
    TransformationList transformationList;
    std::list<NormalizationOp*> loweringSequence;
  };
  
} // end of namespace SPRAY

#endif
