#ifndef CBMLITCHECKENERGYLOSS_H_
#define CBMLITCHECKENERGYLOSS_H_

#include "TObject.h"
#include <string>
class TGraph;

class CbmLitCheckEnergyLoss : public TObject {
public:
	CbmLitCheckEnergyLoss();
	virtual ~CbmLitCheckEnergyLoss();

	virtual void Check();
//	virtual void SetParContainers();

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

	ClassDef(CbmLitCheckEnergyLoss, 1);
};

#endif /* CBMLITCHECKENERGYLOSS_H_ */
