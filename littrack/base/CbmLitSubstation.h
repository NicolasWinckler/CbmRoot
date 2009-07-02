#ifndef CBMLITSUBSTATION_H_
#define CBMLITSUBSTATION_H_

#include "CbmLitFloat.h"
#include <string>

class CbmLitSubstation
{
public:
	CbmLitSubstation();
	virtual ~CbmLitSubstation();

	void SetZ(myf z) {fZ = z;}

	myf GetZ() const {return fZ;}

	virtual std::string ToString() const;

private:
	myf fZ;
};

#endif /*CBMLITSUBSTATION_H_*/
