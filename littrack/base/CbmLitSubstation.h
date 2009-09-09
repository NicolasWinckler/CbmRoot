#ifndef CBMLITSUBSTATION_H_
#define CBMLITSUBSTATION_H_

#include "CbmLitFloat.h"
#include <string>
#include <sstream>

class CbmLitSubstation
{
public:
	CbmLitSubstation(): fZ(0.) {};
	virtual ~CbmLitSubstation() {};

	void SetZ(myf z) {fZ = z;}

	myf GetZ() const {return fZ;}

	virtual std::string ToString() const {
		std::stringstream ss;
		ss << "Substation: z=" << GetZ() << std::endl;
		return ss.str();;
	}

private:
	myf fZ;
};

#endif /*CBMLITSUBSTATION_H_*/
