#ifndef CBMLITPDG_H_
#define CBMLITPDG_H_

class CbmLitPDG
{
public:
	CbmLitPDG();
	virtual ~CbmLitPDG();

	static double GetMass(
			int pdg);

	static bool IsElectron(
			int pdg);

	static bool IsMuon(
			int pdg);
};

#endif /*CBMLITPDG_H_*/
