// -------------------------------------------------------------------------
// -----         CbmAnaHypYPtGenerator source file                     -----
// -----          Created 03/10/04  by E. Kryshen                      -----
// -------------------------------------------------------------------------

#include "TRandom.h"
#include "CbmAnaHypYPtGenerator.h"
#include "FairPrimaryGenerator.h"
#include "TParticlePDG.h"
#include "TDatabasePDG.h"
#include "TF1.h"
#include "TMath.h"

// ------------------------------------------------------------------------
CbmAnaHypYPtGenerator::CbmAnaHypYPtGenerator():FairGenerator(){
  // Default constructor
  fPDGType =  -1;
  fMult    =   0;
}

// ------------------------------------------------------------------------
CbmAnaHypYPtGenerator::CbmAnaHypYPtGenerator(Int_t pdgid, Int_t mult):FairGenerator(){
  // Constructor. Set default distributions
  fPDGType = pdgid;
  fMult    = mult;
  SetDistributionPt();
  SetDistributionY();
  SetRangePt();
}
// ------------------------------------------------------------------------
void CbmAnaHypYPtGenerator::Init(){
  // Initialize generator

  // Check for particle type
  TDatabasePDG* pdgBase = TDatabasePDG::Instance();
  TParticlePDG *particle = pdgBase->GetParticle(fPDGType);
  if (! particle) Fatal("CbmAnaHypYPtGenerator","PDG code %d not defined.",fPDGType);
  fPDGMass = particle->Mass();
  //gRandom->SetSeed(0);
  fDistPt = new TF1("distPt","x*exp(-sqrt(x*x+[1]*[1])/[0])",fPtMin,fPtMax);
  fDistPt->SetParameters(fT,fPDGMass);
}

// ------------------------------------------------------------------------
Bool_t CbmAnaHypYPtGenerator::ReadEvent(FairPrimaryGenerator* primGen){
  Double_t phi, pt, y, mt, px, py, pz;

  // Generate particles
  for (Int_t k = 0; k < fMult; k++) {

    phi = gRandom->Uniform(0,TMath::TwoPi());
    pt  = fDistPt->GetRandom(fPtMin,fPtMax);
    px  = pt*TMath::Cos(phi);
    py  = pt*TMath::Sin(phi);
    y   = gRandom->Gaus(fY0,fSigma);
    mt  = TMath::Sqrt(fPDGMass*fPDGMass + pt*pt);
    pz  = mt * TMath::SinH(y);
    printf("pt=%f y=%f\n",pt,y);
    primGen->AddTrack(fPDGType, px, py, pz, 0, 0, 0);
  }
  return kTRUE;

}
// ------------------------------------------------------------------------

ClassImp(CbmAnaHypYPtGenerator)

