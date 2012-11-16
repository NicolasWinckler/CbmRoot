/**
* \file CbmRichEventDisplay.cxx
*
* \author Supriya Das
* \date 2006
**/

#include "CbmRichEventDisplay.h"

#include "CbmRichHit.h"
#include "CbmRichRing.h"
#include "CbmTrackMatch.h"
#include "CbmRichPoint.h"
#include "CbmDrawHist.h"
#include "FairTrackParam.h"

#include "FairMCPoint.h"
#include "CbmMCTrack.h"
#include "FairRootManager.h"

#include "TClonesArray.h"
#include "TEllipse.h"
#include "TCanvas.h"
#include "TH2D.h"

#include <iostream>
#include <map>
#include <sstream>

using namespace std;

CbmRichEventDisplay::CbmRichEventDisplay():
   FairTask("CbmRichEventDisplay"),
   fRichRings(NULL),
   fRichHits(NULL),
   fRichPoints(NULL),
   fRichMatches(NULL),
   fMcTracks(NULL),
   fEventNum(0),
   fDrawRings(true),
   fDrawHits(true),
   fDrawPoints(true),
   fDrawProjections(true)
{
   SetDefaultDrawStyle();
}

CbmRichEventDisplay::~CbmRichEventDisplay()
{
}


InitStatus CbmRichEventDisplay::Init()
{
   FairRootManager* ioman = FairRootManager::Instance();
   if (NULL == ioman) {Fatal("CbmRichEventDisplay::Init", "RootManager not instantiated!");}

   fRichHits = (TClonesArray*) ioman->GetObject("RichHit");
   if (NULL == fRichHits) {Fatal("CbmRichEventDisplay::Init", "No RichHit array!");}

   fRichRings = (TClonesArray*) ioman->GetObject("RichRing");
   if (NULL == fRichRings) { Fatal("CbmRichEventDisplay::Init", "No RichRing array!");}

   fRichPoints = (TClonesArray*) ioman->GetObject("RichPoint");
   if (NULL == fRichPoints) {Fatal("CbmRichEventDisplay::Init", "No RichPoint array!");}

   fRichMatches = (TClonesArray*) ioman->GetObject("RichRingMatch");
   if (NULL == fRichMatches) {Fatal("CbmRichEventDisplay::Init", "No RichRingMatch array!");}

   fRichProjections = (TClonesArray*) ioman->GetObject("RichProjection");
   if (NULL == fRichProjections) {Fatal("CbmRichEventDisplay::Init", "No RichProjection array!");}

   fMcTracks = (TClonesArray*) ioman->GetObject("MCTrack");
   if (NULL == fMcTracks ) {Fatal("CbmRichEventDisplay::Init", "No MCTrack array!");}

   return kSUCCESS;
}

void CbmRichEventDisplay::Exec(
      Option_t* opt)
{
   fEventNum++;
   DrawOneEvent();
}

void CbmRichEventDisplay::DrawOneEvent()
{
   stringstream ss;
   ss << "rich_event_display_event_"<< fEventNum;
   TCanvas *c = new TCanvas(ss.str().c_str(), ss.str().c_str(), 900, 800);
   c->Divide(1, 2);
   c->SetGrid(true, true);
   c->cd(1);
   TH2D* padU = new TH2D("padU", ";X [cm];Y [cm]", 1, -110., 110., 1, 90., 180);
   //padU->SetStats(false);
   padU->Draw();
   DrawOnePmtPlane("up");

   c->cd(2);
   TH2D* padD = new TH2D("padD", ";X [cm];Y [cm]", 1, -110., 110., 1, -180., -90.);
  // padD->SetStats(false);
   padD->Draw();
   DrawOnePmtPlane("down");

}

void CbmRichEventDisplay::DrawOnePmtPlane(
      const string& plane)
{
   // Draw rings
   if (fDrawRings){
      int nofRings = fRichRings->GetEntriesFast();
      for (int iR = 0; iR < nofRings; iR++){
         CbmRichRing* ring = (CbmRichRing*) fRichRings->At(iR);
         if (NULL == ring) continue;
         if ( (plane == "up" && ring->GetCenterY() >= 0.) ||
              (plane == "down" && ring->GetCenterY() < 0.) ){
            DrawCircle(ring);
         }
      }
   }

   // Draw hits
   if (fDrawHits){
      int nofHits = fRichHits->GetEntriesFast();
      for (int iH = 0; iH < nofHits; iH++){
         CbmRichHit* hit = (CbmRichHit*) fRichHits->At(iH);
         if (NULL == hit) continue;
         if ( (plane == "up" && hit->GetY() >= 0.) ||
              (plane == "down" && hit->GetY() < 0.) ){

            TEllipse* hitDr = new TEllipse(hit->GetX(), hit->GetY(), 0.75);
            hitDr->SetFillColor(kRed);
            hitDr->SetLineColor(kRed);
            hitDr->Draw();
         }
      }
   }

   // Draw RICH MC Points
   if (fDrawPoints) {
      int nofPoints = fRichPoints->GetEntriesFast();
      for (int iP = 0; iP < nofPoints; iP++){
         CbmRichPoint* point = (CbmRichPoint*) fRichPoints->At(iP);
         if (NULL == point) continue;
         if ( (plane == "up" && point->GetY() >= 0.) ||
              (plane == "down" && point->GetY() < 0.) ){
            TEllipse* pointDr = new TEllipse(point->GetX(), point->GetY(), 0.4);
            //pointDr->SetFillColor(kBlue);
            //pointDr->SetLineColor(kBlue);
            pointDr->Draw();
         }
      }
   }

   //Draw Track projections
   if (fDrawProjections) {
      int nofProjections = fRichProjections->GetEntriesFast();
      for (int iP = 0; iP < nofProjections; iP++){
         FairTrackParam* pr = (FairTrackParam*) fRichProjections->GetEntriesFast();
         if (NULL == pr) continue;
         if ( (plane == "up" && pr->GetY() >= 0.) ||
              (plane == "down" && pr->GetY() < 0.) ){

            TEllipse* prDr = new TEllipse(pr->GetX(), pr->GetY(), 1.);
            prDr->SetFillColor(kGreen);
            prDr->SetLineColor(kGreen);
            prDr->Draw();
         }
      }
   }
}

void CbmRichEventDisplay::DrawCircle(
      CbmRichRing* ring)
{
   TEllipse* circle = new TEllipse(ring->GetCenterX(), ring->GetCenterY(), ring->GetRadius());
   circle->SetFillStyle(0);
   circle->SetLineWidth(3);
   circle->SetLineColor(kBlue);
   circle->Draw();
   TEllipse* center = new TEllipse(ring->GetCenterX(), ring->GetCenterY(), 0.5);
   center->SetFillColor(kBlue);
   center->SetLineColor(kBlue);
   center->Draw();
}

void CbmRichEventDisplay::Finish()
{

}

ClassImp(CbmRichEventDisplay)
