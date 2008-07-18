/** CbmMuchHitProducer class
 *
 * @author  A.Kiseleva
 * @version 0.0
 * @since   13.04.06
 * 
 *  Hit producer for MUon CHambers detector
 *
 */

#include <iostream>

#include "TRandom.h"
#include "TVector3.h"
#include "TMath.h"
#include "TStyle.h"
#include "TCanvas.h"

#include "CbmMuchHitProducerIdeal.h"
#include "CbmMCApplication.h"
#include "CbmDetector.h"
#include "CbmRootManager.h"
#include "CbmMuchPoint.h"
#include "CbmMuchHit.h"

using std::cout;
using std::endl;

ClassImp(CbmMuchHitProducerIdeal)


// ---- Constructor ----------------------------------------------------
CbmMuchHitProducerIdeal::CbmMuchHitProducerIdeal(const char *name, Int_t verbose, 
				       Double_t SigmaXY, Double_t SigmaZ )
  :CbmTask(name,verbose)
{
  fVerbose = verbose;
  fSigmaXY = SigmaXY;
  fSigmaZ = SigmaZ;
  fNHits=0;  
  fHitCollection = new TClonesArray("CbmMuchHit", 100);
}
// --------------------------------------------------------------------



// ---- Destructor ----------------------------------------------------
CbmMuchHitProducerIdeal::~CbmMuchHitProducerIdeal()
{
}
// --------------------------------------------------------------------



// ---- Init ----------------------------------------------------------
InitStatus CbmMuchHitProducerIdeal::Init()
{
  fMuchPoints=(TClonesArray *) CbmRootManager::Instance()->GetObject("MuchPoint");  
  Register();
  return kSUCCESS;
}
// --------------------------------------------------------------------


// ---- Exec ----------------------------------------------------------
void CbmMuchHitProducerIdeal::Exec(Option_t * option)
{
  fHitCollection->Clear();
  fNHits = 0;
  Int_t nMuchPoint = fMuchPoints->GetEntries();

  if(fVerbose > 0)cout <<  "-I- CbmMuchHitProducer : " << nMuchPoint
		       << " Much points in this event" << endl;


  CbmMuchPoint *pt1 = NULL;
  TVector3 pos1, pos2;
  Double_t delta_x, delta_y, delta_z;
  Double_t xHit, yHit, zHit;
  Double_t xHitErr, yHitErr, zHitErr;

  for (int j=0; j < nMuchPoint; j++ )
    {        

      pt1 = (CbmMuchPoint*) fMuchPoints->At(j);
      if(NULL == pt1)continue;
      
      // Get point position
      pt1->Position(pos1);
      
      // Smear position
      delta_x = gRandom->Gaus(0, fSigmaXY);
      delta_y = gRandom->Gaus(0, fSigmaXY);
      delta_z = gRandom->Gaus(0, fSigmaZ);
      if(TMath::Abs(delta_x) > 3*fSigmaXY)delta_x = 3*fSigmaXY*delta_x/TMath::Abs(delta_x);
      if(TMath::Abs(delta_y) > 3*fSigmaXY)delta_y = 3*fSigmaXY*delta_y/TMath::Abs(delta_y);
      if(TMath::Abs(delta_z) > 3*fSigmaZ)delta_z = 3*fSigmaZ*delta_z/TMath::Abs(delta_z);
      
      xHit = pos1.X() + delta_x;
      yHit = pos1.Y() + delta_y;
      zHit = pos1.Z() + delta_z;
      
      xHitErr = fSigmaXY;
      yHitErr = fSigmaXY;
      zHitErr = fSigmaZ;
      
      TVector3 hitPos(xHit, yHit, zHit);
      TVector3 hitPosErr(xHitErr, yHitErr, zHitErr);
      TVector3	mom;
      pt1->Momentum(mom);
      AddHit(pt1->GetDetectorID(), hitPos, hitPosErr,j );
    }
  if(fVerbose > 0) cout << "-I- CbmMuchHitProducer : " << fNHits
			<< " Much hits created in this event" << endl;
}
// --------------------------------------------------------------------



// ---- Add Hit to HitCollection --------------------------------------
void CbmMuchHitProducerIdeal::AddHit(Int_t detID, TVector3 &posHit,
				TVector3 &posHitErr, Int_t ref )
{
  new((*fHitCollection)[fNHits]) CbmMuchHit(detID, posHit, posHitErr, ref );
  if(fVerbose > 1) {
    CbmMuchHit* MuchHit = (CbmMuchHit*) fHitCollection->At(fNHits);
    //MuchHit->Print();
    cout << endl;
  }
  fNHits++;
}
// --------------------------------------------------------------------



// ---- Finish --------------------------------------------------------
void CbmMuchHitProducerIdeal::Finish()
{
}
// --------------------------------------------------------------------



// ---- Register ------------------------------------------------------
void CbmMuchHitProducerIdeal::Register()
{
  CbmRootManager::Instance()->Register("MuchHit", "Much",
				       fHitCollection, kTRUE);
}

// ---- SetSigmaXY -----------------------------------------------------
void CbmMuchHitProducerIdeal::SetSigmaXY(Double_t sigma)
{
  fSigmaXY = sigma;
}
// --------------------------------------------------------------------

// ---- SetSigmaZ -----------------------------------------------------
void CbmMuchHitProducerIdeal::SetSigmaZ(Double_t sigma)
{
  fSigmaZ = sigma;
}
// --------------------------------------------------------------------
// ---- GetSigmaXY -----------------------------------------------------
Double_t CbmMuchHitProducerIdeal::GetSigmaXY()
{  
  return  fSigmaXY;
}
// --------------------------------------------------------------------

// ---- GetSigmaZ -----------------------------------------------------
Double_t CbmMuchHitProducerIdeal::GetSigmaZ()
{  
  return  fSigmaZ;
}
// --------------------------------------------------------------------



