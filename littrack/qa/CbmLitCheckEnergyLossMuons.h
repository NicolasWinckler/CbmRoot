#ifndef CBMLITCHECKENERGYLOSSMUONS_H_
#define CBMLITCHECKENERGYLOSSMUONS_H_

#include "CbmLitTask.h"
#include <string>
class TGraph;

class CbmLitCheckEnergyLossMuons : public CbmLitTask
{
public:
	CbmLitCheckEnergyLossMuons();
	virtual ~CbmLitCheckEnergyLossMuons();

	virtual void Check();

	void FillTableIron();
	void FillTableTungsten();
	void FillTableCarbon();
	void CreateGraphs();
	void CalcEloss();
	void DrawGraphs();

	void SetMaterial(const std::string& material) {
		fMat = material;
	}

private:
	Double_t fMom[26];
	TGraph* fTable[4];
	TGraph* fCalc[4];
	std::string fMat;

	ClassDef(CbmLitCheckEnergyLossMuons, 1);
};

#endif /* CBMLITCHECKENERGYLOSSMUONS_H_ */
