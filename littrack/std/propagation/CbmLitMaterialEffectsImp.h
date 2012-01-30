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
   /* Propagation direction */
   bool fDownstream;
   /* Hypothesis on particle mass */
   myf fMass;
   /* True if particle is an electron or positron */
   bool fIsElectron;
   /* True if particle is muon */
   bool fIsMuon;
};

#endif /*CBMLITMATERIALEFFECTSIMP_H_*/
