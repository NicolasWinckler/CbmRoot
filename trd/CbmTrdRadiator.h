// -------------------------------------------------------------------------
// -----                         CbmTrdRadiator header file       -----
// -----                  Created 10/11/04  by M.Kalisky         -----
// -------------------------------------------------------------------------

/**  CbmTrdRadiator.h
 *@author M.Kalisky <m.kalisky@gsi.de>
 **
 ** Serves for Transition Radiation computation for regular radiator
 ** The output is energy loss of an electron via Transition Radiation
 **/

#ifndef CBMTRDRADIATOR_H
#define CBMTRDRADIATOR_H

#include "TVector3.h"
#include "TH1D.h"

class CbmTrdRadiator 
{
  
 public:
  
  /** Default constructor **/
  CbmTrdRadiator();

  /** Constructor **/
  CbmTrdRadiator(Bool_t SimpleTR, Int_t Nfoils, Float_t FoilThick, 
                 Float_t GapThick);

  /** Destructor **/
   virtual ~CbmTrdRadiator();
  
  /** Create the needed histograms **/
   void CreateHistograms();

  /** Init function **/
   void Init(Bool_t SimpleTR, Int_t Nfoils, Float_t FoilThick, Float_t GapTick);
  /** Init function **/
   void Init();

  /** Spectra production **/
   void ProduceSpectra();

  /** Final energy loss computation **/
  Int_t  ELoss(Int_t index);

  /** Main function for TR production **/
   void ProcessTR();

  /** Compute the TR spectrum **/
   Int_t TRspectrum();

  /** Compute the TR spectrum in the Mylar foil **/
   Int_t MyTRspectrum();

  /** Compute the TR spectrum in the Detector **/
   Int_t DetTRspectrum();

  /** Compute the gamma factor **/
   Float_t GammaF();

  /** Compute the sigma coeff. for one-gap-one-foil radiator **/
   Float_t Sigma(Float_t energykeV);

  /** Compute the sigma coeff. for the mylar foil **/
   Float_t SigmaMy(Float_t energykeV);

  /** Compute the sigma coeff. for the detector gas **/
   Float_t SigmaDet(Float_t energykeV);
  
  /** Computation of photon absorption cross sections **/
   Float_t      GetMuPo(Float_t energyMeV);
   Float_t      GetMuAir(Float_t energyMeV);
   Float_t      GetMuXe(Float_t energyMeV);
   Float_t      GetMuCO2(Float_t energyMeV);
   Float_t      GetMuMy(Float_t energyMeV);

  /** Calculate the TR for a given momentum **/
   Float_t      GetTR(TVector3 mom);

  /** Interpolate between given points of table **/
   Float_t Interpolate(Float_t energyMeV
               , Float_t *en, Float_t *mu, Int_t n);

  /** Locate a point in 1-dim grid **/
   Int_t Locate(Float_t *xv, Int_t n, Float_t xval
               , Int_t &kl, Float_t &dx);


  /** Setters for private data memebers **/
   void          SetNFoils(Int_t n)      { fNFoils    = n; }
   void          SetFoilThick(Float_t t) { fFoilThick = t; }
   void          SetGapThick(Float_t t)  { fGapThick  = t; }
   void          SetSigma(Int_t SigmaT);
 
private:

  /* Input parameters to be set */
 
  Int_t     fDetType;     // 0: GSI  1: Muenster-Bucarest 
  Bool_t    fFirstPass;   // used for MB-Chamber with two gas gaps
  Bool_t    fSimpleTR;    // mode of the TR production
  Int_t     fNFoils;      // Number of foils in the radiator stack
  Float_t   fFoilThick;   // Thickness of the foils (cm)
  Float_t   fGapThick;    // Thickness of gaps between the foils (cm)

  Float_t   fGasThick;    // Thickness of hte active gas volume.


  /* Parameters fixed in Init() */

  Float_t   fFoilDens;    // Density of the radiator foils (g/cm^3) 
  Float_t   fGapDens;     // Dens. of gas in the radiator gaps (g/cm^3)
  Float_t   fFoilOmega;   // Plasma frequency of the radiator foils
  Float_t   fGapOmega;    // Plasma freq. of gas

  /* Parameters after correction of the angle of the particle */

  Float_t fFoilThickCorr;
  Float_t fGapThickCorr;
  Float_t fGasThickCorr;



  Float_t   fnTRprod;     // <nTR> produced
  Int_t     fSigmaT;      // set the absorption material

  Float_t   fMyDens;
  Float_t   fMyThick;
  
  Float_t   fCom1;        // first component of the gas
  Float_t   fCom2;        // second component of the gas

  static const Int_t     fSpNBins=100; // Number of Bins for histos 
  static const Int_t     fSpRange=50;  // Maximum (keV) of eloss spectrum 
  Float_t   fSpBinWidth;               // Bin width=fSpNBins/fSpRange

  Float_t*  fSigma;       // [fSpNBins] Array of sigma values
  Float_t*  fSigmaMy;     // [fSpNBins] Array of sigma values
  Float_t*  fSigmaDet;    // [fSpNBins] Array of sigma values

  TH1D*     fSpectrum;    // TR photon energy spectrum
  TH1D*     fMySpectrum;  // TR spectra in Mylar foil
  TH1D*     fDetSpectrumA;// TR absorbed in Detector
  TH1D*     fDetSpectrum; // TR passed through Detector

  static const Int_t fNMom=14; // number of momentum spectra  
  Double_t *fTrackMomentum;    // [fNMom] Track momenta for which spectra
                               // are available

  TH1D*     fFinal[fNMom];     // Absorption spectra for different momenta
  Float_t   fnTRabs[fNMom];    // <nTR> absorbed for differnt momenta
  Float_t   fnTRab;            // <nTR> absorbed

  Float_t   fELoss;       // The real result - in GeV

  TVector3  fMom;         // momentum of the electron

  ClassDef(CbmTrdRadiator,1)
};

#endif
