/** CbmLitMaterialEffectsImp.h
 *@author A.Lebedev <andrey.lebedev@gsi.de>
 *@since 2008
 **
 ** Class performs calculation of the material effects (multiple
 ** scattering and energy loss).
 **/

#ifndef CBMLITMATERIALEFFECTSIMP_H_
#define CBMLITMATERIALEFFECTSIMP_H_

#include "base/CbmLitFloat.h"
#include "interface/CbmLitMaterialEffects.h"

class CbmLitTrackParam;
class CbmLitMaterialInfo;

class CbmLitMaterialEffectsImp : public CbmLitMaterialEffects
{
public:
   /* Constructor */
   CbmLitMaterialEffectsImp();

   /* Destructor */
   virtual ~CbmLitMaterialEffectsImp();

   /* Inherited from CbmLitMaterialEffects */
   LitStatus Update(
      CbmLitTrackParam* par,
      const CbmLitMaterialInfo* mat,
      int pdg,
      bool downstream);

// LitStatus Update2(
//       CbmLitTrackParam* par,
//         const CbmLitMaterialInfo* mat);
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

   litfloat CalcThetaSq(
      const CbmLitTrackParam* par,
      const CbmLitMaterialInfo* mat) const;

   litfloat BetheBlochSimple(
      const CbmLitMaterialInfo* mat) const;

   litfloat BetheBloch(
      const CbmLitTrackParam* par,
      const CbmLitMaterialInfo* mat) const;

   litfloat BetheBlochElectron(
      const CbmLitTrackParam* par,
      const CbmLitMaterialInfo* mat) const;

   litfloat EnergyLoss(
      const CbmLitTrackParam* par,
      const CbmLitMaterialInfo* mat) const;

   litfloat dEdx(
      const CbmLitTrackParam* par,
      const CbmLitMaterialInfo* mat) const;

   litfloat CalcQpAfterEloss(
      litfloat qp,
      litfloat eloss) const;

   litfloat CalcSigmaSqQp(
      const CbmLitTrackParam* par,
      const CbmLitMaterialInfo* mat) const;

   litfloat CalcSigmaSqQpElectron(
      const CbmLitTrackParam* par,
      const CbmLitMaterialInfo* mat) const;

   litfloat MPVEnergyLoss(
      const CbmLitTrackParam* par,
      const CbmLitMaterialInfo* mat) const;

   litfloat BetheHeitler(
      const CbmLitTrackParam* par,
      const CbmLitMaterialInfo* mat) const;

   litfloat PairProduction(
      const CbmLitTrackParam* par,
      const CbmLitMaterialInfo* mat) const;

   litfloat CalcI(
      litfloat Z) const;
private:
   /* Propagation direction */
   bool fDownstream;
   /* Hypothesis on particle mass */
   litfloat fMass;
   /* True if particle is an electron or positron */
   bool fIsElectron;
   /* True if particle is muon */
   bool fIsMuon;
};

#endif /*CBMLITMATERIALEFFECTSIMP_H_*/
