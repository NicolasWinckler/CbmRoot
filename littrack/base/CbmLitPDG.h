#ifndef CBMLITPDG_H_
#define CBMLITPDG_H_

#include "TObject.h"

class CbmLitPDG
{
public:
	CbmLitPDG();
	virtual ~CbmLitPDG();
	
	static Double_t GetMass(
			Int_t pdg);
	
	static Bool_t IsElectron(
			Int_t pdg);
	
	ClassDef(CbmLitPDG,1);
};

#endif /*CBMLITPDG_H_*/
