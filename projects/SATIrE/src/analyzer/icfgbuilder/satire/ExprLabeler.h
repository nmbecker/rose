// Copyright 2005,2006,2007 Markus Schordan, Gergo Barany
// $Id: ExprLabeler.h,v 1.3 2008-05-19 12:38:26 gergo Exp $

#ifndef H_EXPRLABELER
#define H_EXPRLABELER

#include <satire_rose.h>

#include "cfg_support.h"
#include "IrCreation.h"

// preorder -- but it doesn't really matter
class ExprLabeler : public AstSimpleProcessing
{
public:
    ExprLabeler(int);
    int get_expnum() const;

protected:
    void visit(SgNode *);

private:
    ExprLabeler();
    int expnum;
};

#endif
