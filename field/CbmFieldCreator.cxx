#include "CbmFieldCreator.h"

#include "CbmFieldConst.h"
#include "CbmFieldMap.h"
#include "CbmFieldMapSym2.h"
#include "CbmFieldMapSym3.h"
#include "CbmBsField.h"
#include "CbmFieldMapDistorted.h"
#include "CbmFieldMapSym1.h"

#include "FairRunAna.h"
#include "FairRuntimeDb.h"

using std::cout;
using std::cerr;
using std::endl;

static CbmFieldCreator gCbmFieldCreator;

CbmFieldCreator::CbmFieldCreator()
	:FairFieldFactory()
{
	fCreator=this;
        fFieldPar=0;
}
CbmFieldCreator::~CbmFieldCreator()
{
}
void CbmFieldCreator::SetParm()
{
  FairRunAna *Run = FairRunAna::Instance();
  FairRuntimeDb *RunDB = Run->GetRuntimeDb();
  fFieldPar = (CbmFieldPar*) RunDB->getContainer("CbmFieldPar");

}

FairField* CbmFieldCreator::createCbmField()
{ 
  FairField *fMagneticField=0;

  if ( ! fFieldPar ) {
    cerr << "-E-  No field parameters available!"
	 << endl;
  }else{
	// Instantiate correct field type
	Int_t fType = fFieldPar->GetType();
	if      ( fType == 0 ) fMagneticField = new CbmFieldConst(fFieldPar);
	else if ( fType == 1 ) fMagneticField = new CbmFieldMap(fFieldPar);
	else if ( fType == 2 ) fMagneticField = new CbmFieldMapSym2(fFieldPar);
	else if ( fType == 3 ) fMagneticField = new CbmFieldMapSym3(fFieldPar);
	else if ( fType == kTypeDistorted ) fMagneticField = new CbmFieldMapDistorted(fFieldPar); 
	else if ( fType == 5 ) fMagneticField = new CbmFieldMapSym1(fFieldPar);
	else if ( fType == 6 ) fMagneticField = new CbmBsField(fFieldPar);
	else cerr << "-W- FairRunAna::GetField: Unknown field type " << fType
		<< endl;
	cout << "New field at " << fMagneticField << ", type " << fType << endl;
	// Initialise field
	if ( fMagneticField ) {
		fMagneticField->Init();
		fMagneticField->Print();
	}
  }
  return fMagneticField;
}


ClassImp(CbmFieldCreator)
