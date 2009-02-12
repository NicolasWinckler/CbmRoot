// -------------------------------------------------------------------------
// -----                    CbmFieldCreator header file                  -----
// -----                Created 15/01/07  by M. Al-Turany              -----
// -------------------------------------------------------------------------


#ifndef CBMFIELDCREATOR_H
#define CBMFIELDCREATOR_H

#include "FairField.h"
#include "FairFieldFactory.h"
#include "CbmFieldPar.h"

class CbmFieldCreator : public FairFieldFactory 
{
public:
    CbmFieldCreator();
    virtual ~CbmFieldCreator();
    virtual FairField* createFairField();
    virtual void SetParm();
    ClassDef(CbmFieldCreator,1);
protected:
    CbmFieldPar* fFieldPar;
};
#endif //CBMFIELDCREATOR_H
