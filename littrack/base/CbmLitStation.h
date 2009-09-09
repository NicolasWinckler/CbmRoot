#ifndef CBMLITSTATION_H_
#define CBMLITSTATION_H_

#include "CbmLitEnums.h"
#include "CbmLitSubstation.h"

#include <string>
#include <vector>
#include <sstream>

class CbmLitStation
{
public:
	CbmLitStation():fType(kLITPIXELHIT){};
	virtual ~CbmLitStation(){};

	void SetType(LitHitType type) {fType = type;}
	void SetSubstations(const std::vector<CbmLitSubstation>& substations){
		fSubstations = substations;
	}
	void AddSubstation(const CbmLitSubstation& substation){
		fSubstations.push_back(substation);
	}

	LitHitType GetType() const {return fType;}
	const CbmLitSubstation& GetSubstation(int i) const {return fSubstations[i];}
	int GetNofSubstations() const {return fSubstations.size();}

	virtual std::string ToString() const {
		std::stringstream ss;
		ss << "Station: type=" << GetType()
			<< " nofSubstations=" << GetNofSubstations() << std::endl;
		return ss.str();;
	}

private:
	LitHitType fType;
	std::vector<CbmLitSubstation> fSubstations;
};

#endif /*CBMLITSTATION_H_*/
