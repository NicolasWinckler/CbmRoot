// ------------------------------------------------------------------------
// -----                       CbmTrdRadiator source fil          -----
// -----                  Created 10/11/04  by M.Kalisky        -----
// -------------------------------------------------------------------------

#include "CbmTrdRadiator.h"

#include "CbmTrdGas.h"

#include "TRandom.h"
#include "TFile.h"
#include "TMath.h"

#include <iostream>
#include <iomanip>
using std::cout;
using std::endl;
using std::setprecision;


// -----   Default constructor   ---------------------------------------
CbmTrdRadiator::CbmTrdRadiator(){
  
  fSpectrum = NULL;
  fMySpectrum = NULL;
  fDetSpectrumA = NULL;
  fDetSpectrum = NULL;

  for(Int_t i=0; i<fNMom; i++){
      fFinal[i] = NULL;
  }

  fSigma     = NULL;
  fSigmaMy   = NULL;
  fSigmaDet  = NULL;

  fELoss = - 1.0;
  fFoilThickCorr = 1.0;
  fGapThickCorr = 1.0;
  fGasThickCorr = 1.0;

  // Set initial parameters defining the radiator
  fNFoils = 130;
  fGapThick   = 0.02;
  fFoilThick  = 0.0013;
  fSimpleTR   = kTRUE;

  CreateHistograms();

}
//-----------------------------------------------------------------------------
// -----  Constructor   --------------------------------------------------
CbmTrdRadiator::CbmTrdRadiator(Bool_t SimpleTR, Int_t Nfoils, Float_t FoilThick, Float_t GapThick){

  fSpectrum = NULL;
  fMySpectrum = NULL;
  fDetSpectrumA = NULL;
  fDetSpectrum = NULL;

  for(Int_t i=0; i<fNMom; i++){
      fFinal[i] = NULL;
  }


  fSigma     = NULL;
  fSigmaMy   = NULL;
  fSigmaDet  = NULL;

  fELoss = -1.0;
  fFoilThickCorr = 1.0;
  fGapThickCorr = 1.0;
  fGasThickCorr = 1.0;


   // Set initial parameters defining the radiator 
   fNFoils = Nfoils;
   fGapThick   = GapThick;
   fFoilThick  = FoilThick;
   fSimpleTR   = SimpleTR;

  CreateHistograms();

}
//-----------------------------------------------------------------------------
// -----   Destructor   ----------------------------------------------------
CbmTrdRadiator::~CbmTrdRadiator()
{
  //    FairRootManager *fManager = FairRootManager::Instance();
  //  fManager->Write();
    if (fSpectrum){
        cout << " -I DELETING fSpectrum " << endl;
	delete fSpectrum;
    }
    if (fMySpectrum) delete fMySpectrum;
    if (fDetSpectrum) delete fDetSpectrum;
    if (fDetSpectrumA) delete fDetSpectrumA;

    for(Int_t i=0; i<fNMom; i++){
	if(fFinal[i]) delete fFinal[i];
    }

}

// ----- Create Histogramms --------------------------------------------------
void CbmTrdRadiator::CreateHistograms(){
  
  // Create the needed histograms

  fSpBinWidth = (Float_t)fSpRange / (Float_t)fSpNBins;

  Float_t SpLower    = 1.0 - 0.5 * fSpBinWidth;
  Float_t SpUpper    = SpLower + (Float_t)fSpRange;

  Char_t name[50];

  if (fSpectrum) delete fSpectrum;
  fSpectrum   = new TH1D("fSpectrum","TR spectrum",fSpNBins,SpLower,SpUpper);

  if (fMySpectrum) delete fMySpectrum;
  fMySpectrum   = new TH1D("fMySpectrum","TR spectrum in Mylar",fSpNBins,SpLower,SpUpper);

  if (fDetSpectrum) delete fDetSpectrum;
  fDetSpectrum = new TH1D("fDetSpectrum", "TR spectrum escaped from detector",fSpNBins,SpLower,SpUpper);

  if (fDetSpectrumA) delete fDetSpectrumA;
  fDetSpectrumA = new TH1D("fDetSpectrumA", "TR spectrum absorbed in detector",fSpNBins,SpLower,SpUpper);

  for(Int_t i=0; i<fNMom; i++){
      sprintf(name,"fFinal%d", i+1);
      //cout<<"name : "<<name<<endl;
      if (fFinal[i]) delete fFinal[i];
      fFinal[i] = new TH1D(name, name, fSpNBins,SpLower,SpUpper);
  }

}

// ----- Init function ----------------------------------------------------
void CbmTrdRadiator::Init(Bool_t SimpleTR, Int_t Nfoils, Float_t FoilThick, Float_t GapThick){

   // Set initial parameters defining the radiator 
   fNFoils = Nfoils;
   fGapThick   = GapThick;
   fFoilThick  = FoilThick;
   fSimpleTR   = SimpleTR;

    CreateHistograms();

    cout << "*********** Initilization of Trd Radiator **************"<<endl;
    cout << "Nr. of foils        : "<< Nfoils << endl;
    cout << "Foil thickness      : "<< setprecision(4) << FoilThick <<" cm"<< endl;
    cout << "Gap thickness       : "<< GapThick << " cm"<<endl;
    cout << "Simple TR production: "<< setprecision(2)<< SimpleTR << endl;
    cout << "************* End of Trd Radiator Init **************"<<endl;


    // material dependend parameters for the radiator material 
    // (polyethylen)
    fFoilDens   = 0.92;      // [g/cm3 ]
    fFoilOmega  = 20.9;      //plasma frequency ( from Anton )

    // material dependent parameters for the gas between the foils of the
    // radiator
    // changed gap density at 16.04.07 FU
    // density of air is 0.001205, 0.00186 
    //fGapDens    = 0.00186;   // [g/cm3]
    fGapDens    = 0.001205;   // [g/cm3]
    fGapOmega   = 0.7;       //plasma frequency  ( from Anton )

    // foil between radiator and gas chamber
    // TODO: implement kapton foil also in trd geometry
    fMyDens   = 1.39;           //  [g/cm3]
    fMyThick  = 0.0025;        // [cm]



    // Get all the gas properties from CbmTrdGas
    CbmTrdGas *fTrdGas = CbmTrdGas::Instance();
    if (fTrdGas==0) {
       fTrdGas = new CbmTrdGas();
       fTrdGas->Init();
    }

    fCom2 = fTrdGas->GetNobleGas();
    fCom1 = fTrdGas->GetCO2();
    fDetType = fTrdGas->GetDetType();
    fGasThick = fTrdGas->GetGasThick();


    // If simplified version is used one didn't calculate the TR
    // for each CbmTrdPoint in full glory, but create at startup
    // some histograms for several momenta which are later used to
    // get the TR much faster.
    if(fSimpleTR == kTRUE){

      ProduceSpectra();
     
        TFile* f1 = new TFile("histos.root", "recreate");

        for (Int_t i=0 ; i < fNMom; i++){
          fFinal[i]->Write();
        }
        f1->Close();
        f1->Delete();
     
    }



}
//----------------------------------------------------------------------------

// ----- Init function ----------------------------------------------------
void CbmTrdRadiator::Init(){

    CreateHistograms();

    cout << "*********** Initilization of Trd Radiator **************"<<endl;
    cout << "Nr. of foils        : "<< fNFoils << endl;
    cout << "Foil thickness      : "<< setprecision(4) << fFoilThick <<" cm"<< endl;
    cout << "Gap thickness       : "<< fGapThick << " cm"<<endl;
    cout << "Simple TR production: "<< setprecision(2)<< fSimpleTR << endl;
    cout << "************* End of Trd Radiator Init **************"<<endl;


    // material dependend parameters for the radiator material 
    // (polyethylen)
    fFoilDens   = 0.92;      // [g/cm3 ]
    fFoilOmega  = 20.9;      //plasma frequency ( from Anton )

    // material dependent parameters for the gas between the foils of the
    // radiator
    // changed gap density at 16.04.07 FU
    // density of air is 0.001205, 0.00186 
    //fGapDens    = 0.00186;   // [g/cm3]
    fGapDens    = 0.001205;   // [g/cm3]
    fGapOmega   = 0.7;       //plasma frequency  ( from Anton )

    // foil between radiator and gas chamber
    // TODO: implement kapton foil also in trd geometry
    fMyDens   = 1.39;           //  [g/cm3]
    fMyThick  = 0.0025;        // [cm]



    // Get all the gas properties from CbmTrdGas
    CbmTrdGas *fTrdGas = CbmTrdGas::Instance();
    if (fTrdGas==0) {
       fTrdGas = new CbmTrdGas();
       fTrdGas->Init();
    }

    fCom2 = fTrdGas->GetNobleGas();
    fCom1 = fTrdGas->GetCO2();
    fDetType = fTrdGas->GetDetType();
    fGasThick = fTrdGas->GetGasThick();


    // If simplified version is used one didn't calculate the TR
    // for each CbmTrdPoint in full glory, but create at startup
    // some histograms for several momenta which are later used to
    // get the TR much faster.
    if(fSimpleTR == kTRUE){

      ProduceSpectra();
     
        TFile* f1 = new TFile("histos.root", "recreate");

        for (Int_t i=0 ; i < fNMom; i++){
          fFinal[i]->Write();
        }
        f1->Close();
        f1->Delete();
     
    }



}
//----------------------------------------------------------------------------

// ---- Spectra Production ---------------------------------------------------
void CbmTrdRadiator::ProduceSpectra(){

    fTrackMomentum = new Double_t[fNMom];

    Double_t trackMomentum[fNMom] = {0.1, 0.25, 0.5, 1.0, 1.5, 2.0, 3.0, 4.0,
                                   5.0, 6.0, 7.0, 8.0, 9.0, 10.0};
    for (Int_t imom = 0; imom < fNMom; imom++) {
      fTrackMomentum[imom] = trackMomentum[imom];
    }

    for(Int_t i=0; i<fNMom; i++){

	fMom.SetXYZ(0.0, 0.0, fTrackMomentum[i]);

	ProcessTR();
        fnTRabs[i] = fnTRab;

	// copy the content of the current fDetSpectrumA into appropriate
	// fFinal

        Float_t tmp = 0;
	for(Int_t j=0; j < fSpNBins; j++){
	    tmp = fDetSpectrumA->GetBinContent(j + 1);
	    fFinal[i]->SetBinContent(j + 1, tmp);
	}

    }

}

//----------------------------------------------------------------------------

// ---- GetTR  ---------------------------------------------------------------
Float_t CbmTrdRadiator::GetTR(TVector3 Mom){

    /*
     * Simplified simulation of the TR
     */

    if(fSimpleTR == kTRUE){

	Float_t m = Mom.Mag();

        // Find correct spectra
        Int_t i=0;
	while(m > fTrackMomentum[i]){
	    i++;
	    if(i == fNMom) {
                i--;
		break;
	    }
	}

        ELoss(i);

    }

    /*
     * Full simulation of the TR production
     */
    else{

	fMom = Mom;
	ProcessTR();
    }

    return fELoss;
}

//----------------------------------------------------------------------------

//----- main TR processing function ------------------------------
void CbmTrdRadiator::ProcessTR(){

  // Compute the angle normalization  factor - for different angles
  // the detector thiknesses are different
  
  Float_t fNormPhi = fMom.Mag() / fMom.Pz();

  // Correct all the thickness according to this factor

  fFoilThickCorr  = TMath::Abs(fFoilThick * fNormPhi);
  fGapThickCorr = TMath::Abs(fGapThick * fNormPhi);
  fGasThickCorr = TMath::Abs(fGasThick * fNormPhi);

  fFirstPass = true;
  // compute the TR spectra in the radiator
  TRspectrum();
  // compute the TR spectra in the mylar foil  
  MyTRspectrum();
  // compute the TR spectra in the detector  
  DetTRspectrum();
  // Loop over photons and compute the E losses
  ELoss(-1);


  if (fDetType == 1) {
    fFirstPass = false;
    Float_t energiefirst = fELoss;
    Float_t fTRAbsfirst = fnTRab;

    // compute the TR spectra in the mylar foil between the two gas layers  
    MyTRspectrum();
    // compute the TR spectra in the second halve of the detector  
    DetTRspectrum();
    // Loop over photons and compute the E losses
    ELoss(-1);
    fELoss += energiefirst; 
    fnTRab += fTRAbsfirst;
  }
}
//----------------------------------------------------------------------------

// ---- Compute the energy losses --------------------------------------------
Int_t CbmTrdRadiator::ELoss(Int_t index){

  // calculate the energy loss of the TR photons in the
  // detector gas. fNTRab is the number of TR photons
  // absorbed in the gas. Since this number is low the
  // actual number comes from a poisson distribution.

    Int_t nPhoton;
    Int_t maxPhoton = 50;
    Double32_t eLoss = 0;
    Float_t eTR = 0;

    if(-1 == index){
	if(0 == fDetSpectrumA){
	    cout <<" -Err :: No input spectra "<<endl;
	    fELoss = -1;
	    return 1;
	}
        nPhoton = gRandom->Poisson(fnTRab);
        if (nPhoton > maxPhoton) nPhoton = maxPhoton;
	for(Int_t i = 0; i < nPhoton; i++){
	    // energy of the photon
	    eTR = fDetSpectrumA->GetRandom();
	    eLoss += eTR;
	}
    }
    else{
	if(0 == fFinal[index]){
	    cout <<" -Err :: No input spectra "<<endl;
	    fELoss = -1;
	    return 1;
	}
        nPhoton = gRandom->Poisson(fnTRabs[index]);
        if (nPhoton > maxPhoton) nPhoton = maxPhoton;
	for(Int_t i = 0; i < nPhoton; i++){
	    // energy of the photon
	    eTR = fFinal[index]->GetRandom();
	    eLoss += eTR;
	}
    }

    // keV->GeV  & set the result
    fELoss = eLoss /1000000.0;

    return 0;

}

//----------------------------------------------------------------------------

//----- TR spectrum ---------------------------------------------------
Int_t CbmTrdRadiator::TRspectrum(){

  // calculate the number of TR photons in the radiator which are not
  // absorbed in the radiator.
  // Formulas are tacken from M. Castellano et al.
  // Computer Physics Communications 61 (1990)


  // Where does this values come from, put them in the header file
  const Float_t kAlpha  = 0.0072973; //  1/137
  const Int_t    kSumMax = 30;

  // calculate the gamma of the particle
  Float_t gamma = GammaF();

  fSpectrum->Reset(); 
  fDetSpectrum->Reset();
  fDetSpectrumA->Reset();

  Float_t kappa = fGapThickCorr / fFoilThickCorr;

  SetSigma(1);

  Float_t stemp = 0; 

  // Loop over energy
  // stemp is the total energy of all created photons
  for (Int_t iBin = 0; iBin < fSpNBins; iBin++) {

    // keV -> eV
    Float_t energyeV = (fSpBinWidth * iBin + 1.0) * 1e3;
    
    Float_t csFoil   = fFoilOmega / energyeV;
    Float_t csGap    = fGapOmega  / energyeV;
    
    Float_t rho1     = energyeV * fFoilThickCorr * 1e4 * 2.5 * (1.0 / (gamma * gamma) + csFoil * csFoil);
    Float_t rho2     = energyeV * fFoilThickCorr * 1e4 * 2.5 * (1.0 / (gamma*gamma) + csGap *csGap);

    // Calculate the sum over the production angle tetan
    Float_t sum = 0;
    for (Int_t iSum = 0; iSum < kSumMax; iSum++) {
      Float_t tetan = (TMath::Pi() * 2.0 * (iSum+1) - (rho1 + kappa * rho2))
	/ (kappa + 1.0);
      if (tetan < 0.0) {
	tetan = 0.0; 
      }
      Float_t aux   = 1.0 / (rho1 + tetan) - 1.0 / (rho2 + tetan);
      sum  += tetan * (aux*aux) * (1.0 - TMath::Cos(rho1 + tetan));

    }
    Float_t conv      = 1.0 - TMath::Exp(-fNFoils * fSigma[iBin]);

    // eV -> keV
    Float_t  energykeV = energyeV * 0.001;
    
    // dN / domega
    Float_t wn        = kAlpha * 4.0 / (fSigma[iBin] * (kappa + 1.0))
	* conv * sum / energykeV;


    // save the result
    fSpectrum->SetBinContent(iBin+1,wn);
    // compute the integral
    stemp += wn;
    
  }
  
  // <nTR> (binsize corr.)
  //Float_t nTR0 = stemp * fSpBinWidth;
  //cout << " No. of photons after radiator" << nTR0 << endl;

  // compute the spectra behind the mylar foil (absorption)
//  MyTRspectrum();

  return 1;
}

//------------------------------------------------------------------

//----- MyTRspectrum -----------------------------------------------
Int_t CbmTrdRadiator::MyTRspectrum(){
    //
    // Computes the spectrum after absorption in Mylar foil
    //

    fMySpectrum->Reset();

    SetSigma(2);

    Float_t stemp = 0;
    for(Int_t iBin = 0; iBin < fSpNBins ; iBin ++){

	Float_t sp = 0;
        if (fFirstPass == true) {
  	  sp = fSpectrum->GetBinContent(iBin+1);
        }
        else {
  	  sp = fDetSpectrum->GetBinContent(iBin+1);
        }
	// compute the absorption coefficient
	Float_t conv = TMath::Exp(-fSigmaMy[iBin]);
	Float_t wn = sp * conv;

	fMySpectrum->SetBinContent(iBin+1,wn);

	stemp += wn;
    }

    //fnTRprod = stemp * fSpBinWidth;
    //cout << "No. of photons after My = "<< fnTRprod << endl;

    // compute the spectrum absorbed in the D & escaped from the D
//    DetTRspectrum();
    return 1;
}
//----------------------------------------------------------------------------

//----- DetTRspectrum ------------------------------------------------
Int_t CbmTrdRadiator::DetTRspectrum(){
  //
  // Computes the spectrum absorbed and passed in the gas mixture
  //

  SetSigma(3);
  Float_t stemp = 0;
  Float_t stemp2 = 0;
  for(Int_t iBin = 0; iBin < fSpNBins; iBin++ ){

      //passed spectrum
      Float_t sp = 0;
      sp = fMySpectrum->GetBinContent(iBin + 1);
      Float_t conv = TMath::Exp(-fSigmaDet[iBin]);
      Float_t wn = sp * conv;

      fDetSpectrum->SetBinContent(iBin + 1, wn);
      stemp += wn;
   
      // absorbed spectrum
      Float_t conv2 = 1 - TMath::Exp(-fSigmaDet[iBin]);
      Float_t wn2 = sp * conv2;

      fDetSpectrumA->SetBinContent(iBin + 1, wn2);
      stemp2 += wn2;

  }

  //Float_t nTR1 = stemp * fSpBinWidth;
  Float_t nTR2 = stemp2 * fSpBinWidth;

  // Save the number of photons absorbed
  fnTRab = nTR2;



  //cout << " No. of photons  escaped: " << nTR1 << endl;
  //cout << " No. of photnos  absorbed in the gas: " <<nTR2<< endl;


  return 1;
}
//----------------------------------------------------------------------------

//----- Gamma factor--------------------------------------------------
Float_t CbmTrdRadiator::GammaF(){
  
  // put this in the header ?
  Float_t massE = 5.1099907e-4;   // GeV/c2

  Float_t p2 = fMom.Mag2();
  Float_t result = TMath::Sqrt(p2 + massE*massE) / massE;

  return result;
}
//----------------------------------------------------------------------------

//----- SetSigma---------------------------------------------------------
void CbmTrdRadiator::SetSigma(Int_t SigmaT){
  //
  // Sets the absorbtion crosssection for the energies of the TR spectrum
  //


  if (SigmaT == 1){
    if (fSigma) delete [] fSigma;
    fSigma       = new Float_t [fSpNBins];
     for (Int_t iBin = 0; iBin < fSpNBins; iBin++) {
      Float_t energykeV = iBin * fSpBinWidth + 1.0;
      fSigma[iBin]       = Sigma(energykeV);
     }
  }

  if (SigmaT == 2){
    if (fSigmaMy) delete [] fSigmaMy;
    fSigmaMy       = new Float_t [fSpNBins];
    for (Int_t iBin = 0; iBin < fSpNBins; iBin++) {
       Float_t energykeV = iBin * fSpBinWidth + 1.0;
       fSigmaMy[iBin]       = SigmaMy(energykeV);
    }
  }

  if (SigmaT == 3){
    if (fSigmaDet) delete [] fSigmaDet;
    fSigmaDet       = new Float_t [fSpNBins];
    for (Int_t iBin = 0; iBin < fSpNBins; iBin++){
      Float_t energykeV = iBin * fSpBinWidth + 1.0;
      fSigmaDet[iBin]     = SigmaDet(energykeV);
    }
  }
}
//----------------------------------------------------------------------------

//----- Sigma-------------------------------------------------------------
Float_t CbmTrdRadiator::Sigma(Float_t energykeV){
  //
  // Calculates the absorbtion crosssection for a one-foil-one-gap-radiator
  //


  // keV -> MeV
  Float_t energyMeV = energykeV * 0.001;

  
  if (energyMeV >= 0.001) {
    Float_t result = (GetMuPo(energyMeV) * fFoilDens * fFoilThickCorr +
	   	      GetMuAir(energyMeV) * fGapDens  * fGapThickCorr);  
    return result;
  }
  else {
    return 1e6;
  } 

}
//----------------------------------------------------------------------------

//----- SigmaMy -------------------------------------------------------
Float_t CbmTrdRadiator::SigmaMy(Float_t energykeV){
  //
  // Calculates the absorbtion crosssection for a one-foil
  //


  // keV -> MeV
  Float_t energyMeV = energykeV * 0.001;

  if (energyMeV >= 0.001) {
    return(GetMuMy(energyMeV) * fMyDens * fMyThick); 
  }
  else {
    return 1e6;
  }
}
//----------------------------------------------------------------------------

//----- SigmaDet --------------------------------------------------------
Float_t CbmTrdRadiator::SigmaDet(Float_t energykeV){
  //
  //Calculates the absorbtion crosssection for a choosed gas
  //

  // keV -> MeV
  Float_t energyMeV = energykeV * 0.001;

  if (energyMeV >= 0.001) {
// densities for CO2 and Xe changed. think density for CO2 is just
// a typo. where the difference for Xe comes from i don't know
// Values are from http://pdg.lbl.gov/AtomicNuclearProperties/
//    return(GetMuCO2(energyMeV) * 0.001806  * fGasThick * fCom1 + 
//	   GetMuXe(energyMeV) * 0.00589 * fGasThick * fCom2); 
    return(GetMuCO2(energyMeV) * 0.00184  * fGasThick * fCom1 + 
	   GetMuXe(energyMeV) * 0.00549 * fGasThick * fCom2); 
  }
  else {
    return 1e6;
  }
}
//----------------------------------------------------------------------------

//----- GetMuPo --------------------------------------------------------
Float_t CbmTrdRadiator::GetMuPo(Float_t energyMeV){
  //
  // Returns the photon absorbtion cross section for polypropylene
  //

  const Int_t kN = 36;

  Float_t mu[kN] = { 1.894E+03, 5.999E+02, 2.593E+02
                    , 7.743E+01, 3.242E+01, 1.643E+01
                    , 9.432E+00, 3.975E+00, 2.088E+00
                    , 7.452E-01, 4.315E-01, 2.706E-01
                    , 2.275E-01, 2.084E-01, 1.970E-01
                    , 1.823E-01, 1.719E-01, 1.534E-01
                    , 1.402E-01, 1.217E-01, 1.089E-01
                    , 9.947E-02, 9.198E-02, 8.078E-02
                    , 7.262E-02, 6.495E-02, 5.910E-02   
                    , 5.064E-02, 4.045E-02, 3.444E-02
                    , 3.045E-02, 2.760E-02, 2.383E-02
		    , 2.145E-02, 1.819E-02, 1.658E-02 };

  Float_t en[kN] = { 1.000E-03, 1.500E-03, 2.000E-03
                    , 3.000E-03, 4.000E-03, 5.000E-03
                    , 6.000E-03, 8.000E-03, 1.000E-02
                    , 1.500E-02, 2.000E-02, 3.000E-02
                    , 4.000E-02, 5.000E-02, 6.000E-02
                    , 8.000E-02, 1.000E-01, 1.500E-01
                    , 2.000E-01, 3.000E-01, 4.000E-01
                    , 5.000E-01, 6.000E-01, 8.000E-01
                    , 1.000E+00, 1.250E+00, 1.500E+00
                    , 2.000E+00, 3.000E+00, 4.000E+00
                    , 5.000E+00, 6.000E+00, 8.000E+00
		    , 1.000E+01, 1.500E+01, 2.000E+01 };

  return Interpolate(energyMeV,en,mu,kN);
}
//----------------------------------------------------------------------------

//----- GetMuAir --------------------------------------------------------
Float_t CbmTrdRadiator::GetMuAir(Float_t energyMeV){
  //
  // Returns the photon absorbtion cross section for Air
  //

  const Int_t kN = 38;



  Float_t mu[kN] = { 0.35854E+04, 0.11841E+04, 0.52458E+03
		      , 0.16143E+03, 0.15722E+03, 0.14250E+03
		      , 0.77538E+02, 0.40099E+02, 0.23313E+02
		      , 0.98816E+01, 0.51000E+01, 0.16079E+01
		      , 0.77536E+00, 0.35282E+00, 0.24790E+00  
		      , 0.20750E+00, 0.18703E+00, 0.16589E+00  
		      , 0.15375E+00, 0.13530E+00, 0.12311E+00  
		      , 0.10654E+00, 0.95297E-01, 0.86939E-01
		      , 0.80390E-01, 0.70596E-01, 0.63452E-01
		      , 0.56754E-01, 0.51644E-01, 0.44382E-01
		      , 0.35733E-01, 0.30721E-01, 0.27450E-01
		      , 0.25171E-01, 0.22205E-01, 0.20399E-01
		      , 0.18053E-01, 0.18057E-01 };

  Float_t en[kN] = { 0.10000E-02, 0.15000E-02, 0.20000E-02
		      , 0.30000E-02, 0.32029E-02, 0.32029E-02 
		      , 0.40000E-02, 0.50000E-02
		      , 0.60000E-02, 0.80000E-02, 0.10000E-01
		      , 0.15000E-01, 0.20000E-01, 0.30000E-01
		      , 0.40000E-01, 0.50000E-01, 0.60000E-01
		      , 0.80000E-01, 0.10000E+00, 0.15000E+00
		      , 0.20000E+00, 0.30000E+00, 0.40000E+00
		      , 0.50000E+00, 0.60000E+00, 0.80000E+00
		      , 0.10000E+01, 0.12500E+01, 0.15000E+01
		      , 0.20000E+01, 0.30000E+01, 0.40000E+01
		      , 0.50000E+01, 0.60000E+01, 0.80000E+01
		      , 0.10000E+02, 0.15000E+02, 0.20000E+02 };

  return Interpolate(energyMeV,en,mu,kN);
}
//----------------------------------------------------------------------------

//----- GetMuXe --------------------------------------------------------
Float_t CbmTrdRadiator::GetMuXe(Float_t energyMeV){
  //
  // Returns the photon absorbtion cross section for xenon
  //

  const Int_t kN = 48;

  Float_t mu[kN] = { 9.413E+03, 8.151E+03, 7.035E+03
                    , 7.338E+03, 4.085E+03, 2.088E+03
                    , 7.780E+02, 3.787E+02, 2.408E+02
                    , 6.941E+02, 6.392E+02, 6.044E+02
                    , 8.181E+02, 7.579E+02, 6.991E+02
                    , 8.064E+02, 6.376E+02, 3.032E+02
                    , 1.690E+02, 5.743E+01, 2.652E+01
                    , 8.930E+00, 6.129E+00, 3.316E+01
                    , 2.270E+01, 1.272E+01, 7.825E+00
                    , 3.633E+00, 2.011E+00, 7.202E-01
                    , 3.760E-01, 1.797E-01, 1.223E-01
                    , 9.699E-02, 8.281E-02, 6.696E-02
                    , 5.785E-02, 5.054E-02, 4.594E-02
                    , 4.078E-02, 3.681E-02, 3.577E-02
                    , 3.583E-02, 3.634E-02, 3.797E-02
		    , 3.987E-02, 4.445E-02, 4.815E-02 };

  Float_t en[kN] = { 1.00000E-03, 1.07191E-03, 1.14900E-03
                    , 1.14900E-03, 1.50000E-03, 2.00000E-03
                    , 3.00000E-03, 4.00000E-03, 4.78220E-03
                    , 4.78220E-03, 5.00000E-03, 5.10370E-03
                    , 5.10370E-03, 5.27536E-03, 5.45280E-03
                    , 5.45280E-03, 6.00000E-03, 8.00000E-03
                    , 1.00000E-02, 1.50000E-02, 2.00000E-02
                    , 3.00000E-02, 3.45614E-02, 3.45614E-02
                    , 4.00000E-02, 5.00000E-02, 6.00000E-02
                    , 8.00000E-02, 1.00000E-01, 1.50000E-01
                    , 2.00000E-01, 3.00000E-01, 4.00000E-01
                    , 5.00000E-01, 6.00000E-01, 8.00000E-01
                    , 1.00000E+00, 1.25000E+00, 1.50000E+00
                    , 2.00000E+00, 3.00000E+00, 4.00000E+00
                    , 5.00000E+00, 6.00000E+00, 8.00000E+00
		    , 1.00000E+01, 1.50000E+01, 2.00000E+01 };

  return Interpolate(energyMeV,en,mu,kN);
}
//----------------------------------------------------------------------------

//----- GetMuCO2 ------------------------------------------------------
Float_t CbmTrdRadiator::GetMuCO2(Float_t energyMeV){
  //
  // Returns the photon absorbtion cross section for CO2
  //

  const Int_t kN = 36;

  Float_t mu[kN] = { 0.39383E+04, 0.13166E+04, 0.58750E+03
                    , 0.18240E+03, 0.77996E+02, 0.40024E+02
                    , 0.23116E+02, 0.96997E+01, 0.49726E+01
                    , 0.15543E+01, 0.74915E+00, 0.34442E+00
                    , 0.24440E+00, 0.20589E+00, 0.18632E+00
                    , 0.16578E+00, 0.15394E+00, 0.13558E+00
                    , 0.12336E+00, 0.10678E+00, 0.95510E-01
                    , 0.87165E-01, 0.80587E-01, 0.70769E-01
                    , 0.63626E-01, 0.56894E-01, 0.51782E-01
                    , 0.44499E-01, 0.35839E-01, 0.30825E-01
                    , 0.27555E-01, 0.25269E-01, 0.22311E-01
		    , 0.20516E-01, 0.18184E-01, 0.17152E-01 };

  Float_t en[kN] = { 0.10000E-02, 0.15000E-02, 0.20000E-02
                    , 0.30000E-02, 0.40000E-02, 0.50000E-02
                    , 0.60000E-02, 0.80000E-02, 0.10000E-01
                    , 0.15000E-01, 0.20000E-01, 0.30000E-01
                    , 0.40000E-01, 0.50000E-01, 0.60000E-01
                    , 0.80000E-01, 0.10000E+00, 0.15000E+00
                    , 0.20000E+00, 0.30000E+00, 0.40000E+00
                    , 0.50000E+00, 0.60000E+00, 0.80000E+00
                    , 0.10000E+01, 0.12500E+01, 0.15000E+01
                    , 0.20000E+01, 0.30000E+01, 0.40000E+01
                    , 0.50000E+01, 0.60000E+01, 0.80000E+01
		    , 0.10000E+02, 0.15000E+02, 0.20000E+02 };

  return Interpolate(energyMeV,en,mu,kN);
}
//----------------------------------------------------------------------------

//----- GetMuMy -------------------------------------------------------
Float_t CbmTrdRadiator::GetMuMy(Float_t energyMeV){
  //
  // Returns the photon absorbtion cross section for mylar
  //

  const Int_t kN = 36;

  Float_t mu[kN] = { 2.911E+03, 9.536E+02, 4.206E+02
                    , 1.288E+02, 5.466E+01, 2.792E+01
                    , 1.608E+01, 6.750E+00, 3.481E+00
                    , 1.132E+00, 5.798E-01, 3.009E-01
                    , 2.304E-01, 2.020E-01, 1.868E-01
                    , 1.695E-01, 1.586E-01, 1.406E-01
                    , 1.282E-01, 1.111E-01, 9.947E-02
                    , 9.079E-02, 8.395E-02, 7.372E-02
                    , 6.628E-02, 5.927E-02, 5.395E-02
                    , 4.630E-02, 3.715E-02, 3.181E-02
                    , 2.829E-02, 2.582E-02, 2.257E-02
                    , 2.057E-02, 1.789E-02, 1.664E-02 };

  Float_t en[kN] = { 1.00000E-03, 1.50000E-03, 2.00000E-03
                    , 3.00000E-03, 4.00000E-03, 5.00000E-03
                    , 6.00000E-03, 8.00000E-03, 1.00000E-02
                    , 1.50000E-02, 2.00000E-02, 3.00000E-02
                    , 4.00000E-02, 5.00000E-02, 6.00000E-02
                    , 8.00000E-02, 1.00000E-01, 1.50000E-01
                    , 2.00000E-01, 3.00000E-01, 4.00000E-01
                    , 5.00000E-01, 6.00000E-01, 8.00000E-01
                    , 1.00000E+00, 1.25000E+00, 1.50000E+00
                    , 2.00000E+00, 3.00000E+00, 4.00000E+00
                    , 5.00000E+00, 6.00000E+00, 8.00000E+00
                    , 1.00000E+01, 1.50000E+01, 2.00000E+01 };

  return Interpolate(energyMeV,en,mu,kN);
}
//----------------------------------------------------------------------------

//----- Interpolate ------------------------------------------------------
Float_t CbmTrdRadiator::Interpolate(Float_t energyMeV
           , Float_t *en, Float_t *mu, Int_t n){
  //
  // Interpolates the photon absorbtion cross section 
  // for a given energy <energyMeV>.
  //

  Float_t de    = 0;
  Int_t    index = 0;
  Int_t    istat = Locate(en,n,energyMeV,index,de);
  if (istat == 0) {
    Float_t result = (mu[index] - de * (mu[index]   - mu[index+1])
		     / (en[index+1] - en[index]  ));
    return result;
  }
  else {
    return 0.0; 
  }
}
//----------------------------------------------------------------------------

//----- Locate ------------------------------------------------------------
Int_t CbmTrdRadiator::Locate(Float_t *xv, Int_t n, Float_t xval
	     , Int_t &kl, Float_t &dx){
  //
  // Locates a point (xval) in a 1-dim grid (xv(n))
  //

  if (xval >= xv[n-1]) return  1;
  if (xval <  xv[0])   return -1;

  Int_t km;
  Int_t kh = n - 1;

  kl = 0;
  while (kh - kl > 1) {
    if (xval < xv[km = (kl+kh)/2]) kh = km; 
    else                           kl = km;
  }
  if (xval < xv[kl] || xval > xv[kl+1] || kl >= n-1) {
    printf("Locate failed xv[%d] %f xval %f xv[%d] %f!!!\n"
          ,kl,xv[kl],xval,kl+1,xv[kl+1]);
    exit(1);
  }

  dx = xval - xv[kl];
  if(xval == 0.001) cout<<"Locat = 0"<<endl;
  return 0;
}
//----------------------------------------------------------------------------


ClassImp(CbmTrdRadiator)
