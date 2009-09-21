/** CbmLitMaterialEffectsImp.h
 *@author A.Lebedev <andrey.lebedev@gsi.de>
 *@since 2008
 **
 ** The class performs calculation of the material effects, such as multiple
 ** scattering and energy loss, on the track parameters.
 **/

#ifndef CBMLITMATERIALEFFECTSIMP_H_
#define CBMLITMATERIALEFFECTSIMP_H_

#include "CbmLitMaterialEffects.h"
#include "CbmLitFloat.h"

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

	myf CalcThetaSq(
			const CbmLitTrackParam* par,
	        const CbmLitMaterialInfo* mat) const;

	myf BetheBlochSimple(
	        const CbmLitMaterialInfo* mat) const;

	myf BetheBloch(
			const CbmLitTrackParam* par,
	        const CbmLitMaterialInfo* mat) const;

	myf BetheBlochElectron(
			const CbmLitTrackParam* par,
	        const CbmLitMaterialInfo* mat) const;

	myf EnergyLoss(
			const CbmLitTrackParam* par,
	        const CbmLitMaterialInfo* mat) const;

	myf dEdx(
			const CbmLitTrackParam* par,
	        const CbmLitMaterialInfo* mat) const;

	myf CalcQpAfterEloss(
			myf qp,
	        myf eloss) const;

	myf CalcSigmaSqQp(
			   const CbmLitTrackParam* par,
	           const CbmLitMaterialInfo* mat) const;

	myf CalcSigmaSqQpElectron(
	          const CbmLitTrackParam* par,
	          const CbmLitMaterialInfo* mat) const;

	myf MPVEnergyLoss(
	        const CbmLitTrackParam* par,
	        const CbmLitMaterialInfo* mat) const;

	myf BetheHeitler(
	        const CbmLitTrackParam* par,
	        const CbmLitMaterialInfo* mat) const;

	myf PairProduction(
	        const CbmLitTrackParam* par,
	        const CbmLitMaterialInfo* mat) const;

	myf CalcI(
			myf Z) const;
private:
//	myf fEnergyLoss;
	bool fDownstream;
	myf fMass;
	bool fIsElectron;
	bool fIsMuon;
};

#endif /*CBMLITMATERIALEFFECTSIMP_H_*/
