#ifndef CBMLITSTATION_H_
#define CBMLITSTATION_H_

#include "CbmLitEnums.h"
#include "CbmLitSubstation.h"

#include "TObject.h"

#include <string>
#include <vector>

class CbmLitStation
{
public:
	CbmLitStation();
	virtual ~CbmLitStation();
	
	void SetType(LitHitType type) {fType = type;}
	void SetSubstations(const std::vector<CbmLitSubstation>& substations){
		fSubstations = substations;
	}
	void AddSubstation(const CbmLitSubstation& substation){
		fSubstations.push_back(substation);
	}
	
	LitHitType GetType() const {return fType;}	
	const CbmLitSubstation& GetSubstation(Int_t i) const {return fSubstations[i];}
	Int_t GetNofSubstations() const {return fSubstations.size();}
	
	virtual std::string ToString() const;

private:
	LitHitType fType;	
	std::vector<CbmLitSubstation> fSubstations;
	
	ClassDef(CbmLitStation, 1);
};

#endif /*CBMLITSTATION_H_*/
