// -------------------------------------------------------------------------
// -----                 CbmEcalRecParticle header file                -----
// -----                 Created 04.07.2005  by Yu.Kharlov
// -------------------------------------------------------------------------

  /* $Id: CbmEcalRecParticle.h,v 1.1 2006/06/22 14:02:17 kharlov Exp $ */

  /* History of cvs commits:
   *
   * $Log: CbmEcalRecParticle.h,v $
   * Revision 1.1  2006/06/22 14:02:17  kharlov
   * First upload of reconstruction classes for Full MC
   *
   */

/**  CbmEcalRecParticle.h
 *@author Yu.Kharlov <Y.Kharlov at gsi.de>
 **
 ** Reconstructed particle in ECAL, characterized by reconstructed 4-momentum
 **/

#ifndef CBMECALRECPARTICLE_H
#define CBMECALRECPARTICLE_H

#include "TObject.h"
#include "TLorentzVector.h"


class CbmEcalClusterV1;
class CbmEcalCell;

class CbmEcalRecParticle : public TObject
{
friend class CbmEcalRecoSlow;
public:

  /** Default constructor **/
  CbmEcalRecParticle();

  /** Constructor with arguments
   *@param px,py,pz,E  4 components of LorentzVector
   **/
  CbmEcalRecParticle(Double_t px, Double_t py, 
		     Double_t pz, Double_t E,
		     Double_t x=-1111, Double_t y=-1111, Double_t z=-1111,
		     Int_t id=0, Double_t chi2=-1111, CbmEcalClusterV1* cl=NULL,
		     CbmEcalCell* cell=NULL, Int_t type=0);

  CbmEcalRecParticle(Int_t clnum, Int_t cellnum, Double_t px, Double_t py, 
		     Double_t pz, Double_t E,
		     Double_t x=-1111, Double_t y=-1111, Double_t z=-1111,
		     Int_t id=0, Double_t chi2=-1111, Int_t type=0);
  /** Copy constructor **/
  CbmEcalRecParticle(const CbmEcalRecParticle& obj) { *this = obj; };

  /** Destructor **/
  virtual ~CbmEcalRecParticle();

  /** Accessors **/
  TLorentzVector GetMomentum() const {return TLorentzVector(fPx,fPy,fPz,fE);}
  Double32_t Px() const {return fPx;}
  Double32_t Py() const {return fPy;}
  Double32_t Pz() const {return fPz;}
  Double32_t E() const {return fE;}
  Double32_t X() const {return fX;}
  Double32_t Y() const {return fY;}
  Double32_t Z() const {return fZ;}
  Double_t Chi2() const {return fChi2;}
  void SetChi2(Double_t chi2) {fChi2=chi2;}
  Int_t ClusterNum() const {return fClusterNum;}
  Int_t CellNum() const {return fCellNum;}
  Int_t PdgId() const {return fId;}
  Int_t CellType() const {return fType;}
  /** Output to screen **/
  virtual void Print(const Option_t* opt = "") const;

  Int_t MCTrack() const {return fMCTrack;}
  void SetMCTrack(Int_t num) {fMCTrack=num;}
protected:

  Double32_t fPx;  // px component of momentum 4-vector
  Double32_t fPy;  // py component of momentum 4-vector
  Double32_t fPz;  // pz component of momentum 4-vector
  Double32_t fE;   // E  component of momentum 4-vector

  /** Reconstructed position in calorimeter **/
  Double32_t fX;
  Double32_t fY;		
  Double32_t fZ;

  /** PdgId of the particle **/
  Int_t fId;

  /** Chi^2 of the cluster **/
  Double_t fChi2;

  /** Type of cell with maximum energy deposition**/
  Int_t fType;

  /** Reference to cluster **/
  Int_t fClusterNum;		//!
  /** A seed for the photon **/
  Int_t fCellNum;			//!
  /** A mother tracks number **/
  Int_t fMCTrack;

  ClassDef(CbmEcalRecParticle,1)

};

#endif
