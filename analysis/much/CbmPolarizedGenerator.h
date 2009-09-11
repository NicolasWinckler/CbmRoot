// -------------------------------------------------------------------------
// -----             CbmPolarizedGenerator header file                 -----
// -----              Created 11/09/09  by E. Kryshen                  -----
// -------------------------------------------------------------------------

/** CbmPolarizedGenerator.h
 ** @author Evgeny Kryshen <e.kryshen@gsi.de>
 **
 ** The CbmPolarizedGenerator generates polarized vector mesons with a given
 ** distribution over pt [ dn/dpt = pt*exp(-mt/T) ]
 ** and rapidity (Gaussian) with fixed multiplicity per event.
 ** Mesons are forced to decay via dielectron or dimuon channel.
 ** Derived from FairGenerator.
 ** Polarization parameter alpha is set in helicity or Collins-Soper 
 ** reference frame.
 */


#ifndef CBMPOLARIZEDGENERATOR_H
#define CBMPOLARIZEDGENERATOR_H

#include "FairGenerator.h"
#include "TF1.h"
#include "TVector3.h"

using namespace std;

class FairPrimaryGenerator;

class CbmPolarizedGenerator : public FairGenerator
{
public:
  typedef enum { kNoPol = 0, kColSop = 1, kHelicity = 2} Frame_t;
  typedef enum { kDiElectron = 1, kDiMuon = 2} DecayMode_t;
  
  /** Default constructor. */
  CbmPolarizedGenerator();

  /** Constructor with PDG-ID, multiplicity
   **@param pdgid Particle type (PDG encoding)
   **@param mult  Multiplicity (default is 1)
   **/
  CbmPolarizedGenerator(Int_t pdgid, Int_t mult=1);

  /** Destructor */
  virtual ~CbmPolarizedGenerator() {};

  /** Modifiers */
  inline void SetPDGType        (Int_t pdg)  {fPDGType = pdg;  };
  inline void SetMultiplicity   (Int_t mult) {fMult    = mult; };
  inline void SetDistributionPt (Double_t T=0.154319) {fT=T;};
  inline void SetDistributionY  (Double_t y0=1.98604, Double_t sigma=0.617173) {fY0=y0;fSigma=sigma;};
  inline void SetRangePt        (Double_t ptMin=0, Double_t ptMax=3) {fPtMin=ptMin;fPtMax=ptMax;};
  Bool_t GenerateDaughters(const TVector3 p, FairPrimaryGenerator* primGen);
  void SetAlpha(Double_t alpha=0)                  {fAlpha=alpha;}
  void SetRefFrame(Frame_t frame=kColSop)          {fFrame=frame;}
  void SetDecayMode(DecayMode_t decayMode=kDiMuon) {fDecayMode=decayMode;}
  void SetBeamMomentum(Double_t mom)               {fBeamMomentum = mom;}

  /** Initializer */
  void Init();

  /** Creates an event with given type and multiplicity.
   **@param primGen  pointer to the CbmPrimaryGenerator
   */
  virtual Bool_t ReadEvent(FairPrimaryGenerator* primGen);

private:
  Int_t    fPDGType;      ///< Particle type (PDG encoding)
  Int_t    fMult;         ///< Multiplicity
  Double_t fT;            ///< Temperature in the Pt distribution
  Double_t fY0;           ///< Mean rapidity
  Double_t fSigma;        ///< Simga in the rapidity distribution
  Double_t fPtMin;        ///< Max value of Pt
  Double_t fPtMax;        ///< Min value of Pt
  Double_t fPDGMass;      ///< Particle mass [GeV]
  TF1*     fDistPt;       //! Pointer to the Pt function
  Double_t fAlpha;        //< Polarization parameter in the helicity frame
  Frame_t     fFrame;     //< Choice of decay products
  DecayMode_t fDecayMode; //< Reference system for polarization  
  Double_t fBeamMomentum; //< Beam momentum in lab frame
  TF1*     fPol;          //! Polarization function

  ClassDef(CbmPolarizedGenerator,1);
};

#endif
