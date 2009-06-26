#ifndef CBMLITMATERIALEFFECTSIMP_H_
#define CBMLITMATERIALEFFECTSIMP_H_

#include "CbmLitMaterialEffects.h"

class CbmLitTrackParam;
class CbmLitMaterialInfo;

class CbmLitMaterialEffectsImp : public CbmLitMaterialEffects
{
public:
	CbmLitMaterialEffectsImp();
	virtual ~CbmLitMaterialEffectsImp();

	// derived from CbmTool
	virtual LitStatus Initialize();
	virtual LitStatus Finalize();

	LitStatus Update(
			CbmLitTrackParam* par,
	        const CbmLitMaterialInfo* mat,
	        int pdg,
	        bool downstream);
public:

	void AddThinScatter(
			CbmLitTrackParam* par,
	        const CbmLitMaterialInfo* mat) const;

	void AddThickScatter(
			CbmLitTrackParam* par,
	        const CbmLitMaterialInfo* mat) const;

	void AddEnergyLoss(
			CbmLitTrackParam* par,
	        const CbmLitMaterialInfo* mat) const;

	double CalcThetaSq(
			const CbmLitTrackParam* par,
	        const CbmLitMaterialInfo* mat) const;

	double BetheBlochSimple(
	        const CbmLitMaterialInfo* mat) const;

	double BetheBloch(
			const CbmLitTrackParam* par,
	        const CbmLitMaterialInfo* mat) const;

	double BetheBlochElectron(
			const CbmLitTrackParam* par,
	        const CbmLitMaterialInfo* mat) const;

	double EnergyLoss(
			const CbmLitTrackParam* par,
	        const CbmLitMaterialInfo* mat) const;

	double dEdx(
			const CbmLitTrackParam* par,
	        const CbmLitMaterialInfo* mat) const;

	double CalcQpAfterEloss(
			double qp,
	        double eloss) const;

	double CalcSigmaSqQp(
			   const CbmLitTrackParam* par,
	           const CbmLitMaterialInfo* mat) const;

	double CalcSigmaSqQpElectron(
	          const CbmLitTrackParam* par,
	          const CbmLitMaterialInfo* mat) const;

	double MPVEnergyLoss(
	        const CbmLitTrackParam* par,
	        const CbmLitMaterialInfo* mat) const;

	double BetheHeitler(
	        const CbmLitTrackParam* par,
	        const CbmLitMaterialInfo* mat) const;

	double PairProduction(
	        const CbmLitTrackParam* par,
	        const CbmLitMaterialInfo* mat) const;

	double CalcI(
			double Z) const;
private:
	double fEnergyLoss;
	bool fDownstream;
	double fMass;
	bool fIsElectron;
	bool fIsMuon;
};

#endif /*CBMLITMATERIALEFFECTSIMP_H_*/
