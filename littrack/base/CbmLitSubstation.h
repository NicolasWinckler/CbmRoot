#ifndef CBMLITSUBSTATION_H_
#define CBMLITSUBSTATION_H_

#include <string>

class CbmLitSubstation
{
public:
	CbmLitSubstation();
	virtual ~CbmLitSubstation();

	void SetZ(double z) {fZ = z;}

	double GetZ() const {return fZ;}

	virtual std::string ToString() const;

private:
	double fZ;
};

#endif /*CBMLITSUBSTATION_H_*/
