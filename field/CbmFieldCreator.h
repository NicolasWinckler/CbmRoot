// -------------------------------------------------------------------------
// -----                    CbmFieldCreator header file                  -----
// -----                Created 15/01/07  by M. Al-Turany              -----
// -------------------------------------------------------------------------


#ifndef CBMFIELDCREATOR_H
#define CBMFIELDCREATOR_H

#include "CbmField.h"
#include "CbmFieldFactory.h"
#include "CbmFieldPar.h"

class CbmFieldCreator : public CbmFieldFactory 
{
public:
    CbmFieldCreator();
    virtual ~CbmFieldCreator();
    virtual CbmField* createCbmField();
    virtual void SetParm();
    ClassDef(CbmFieldCreator,1);
protected:
    CbmFieldPar* fFieldPar;
};
#endif //CBMFIELDCREATOR_H
