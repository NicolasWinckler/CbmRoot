#ifndef CBMLITSUBSTATION_H_
#define CBMLITSUBSTATION_H_

#include "TObject.h"

#include <string>

class CbmLitSubstation
{
public:
	CbmLitSubstation();
	virtual ~CbmLitSubstation();
	
	void SetZ(Double_t z) {fZ = z;}
	
	Double_t GetZ() const {return fZ;}
	
	virtual std::string ToString() const;

private:
	Double_t fZ;
	
	ClassDef(CbmLitSubstation, 1);
};

#endif /*CBMLITSUBSTATION_H_*/
