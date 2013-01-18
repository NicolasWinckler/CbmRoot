/**
 * \file CbmLitRadLengthQa.cxx
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2013
 */

#include "CbmLitRadLengthQa.h"
#include "CbmHistManager.h"
#include "FairRootManager.h"
#include "FairRadLenPoint.h"
#include "CbmDrawHist.h"

#include "TClonesArray.h"
#include "TH1.h"
#include "TProfile2D.h"
#include "TCanvas.h"

CbmLitRadLengthQa::CbmLitRadLengthQa():
   fHM(NULL),
   fOutputDir(""),
   fRadLen(NULL)
{

}

CbmLitRadLengthQa::~CbmLitRadLengthQa()
{
   if (fHM) delete fHM;
}

InitStatus CbmLitRadLengthQa::Init()
{
   fHM = new CbmHistManager();
   CreateHistograms();
   ReadDataBranches();
   return kSUCCESS;
}

void CbmLitRadLengthQa::Exec(
    Option_t* opt)
{
   static Int_t eventNo = 0;
   eventNo++;
   if (0 == eventNo % 10000) {
      std::cout << "-I- CbmLitRadLengthQa::Exec: eventNo=" << eventNo << std::endl;
   }

   const Int_t nofTracks = 1; // Number of tracks in each event in the simulation. One track is used for simplicity.
   vector<Double_t> radLengthOnTrack(nofTracks, 0.0); //trackID, vector with points on track
   vector<Double_t> trackLength(nofTracks, 0.0); //trackID, vector with points on track

   Double_t x, y;
   for (Int_t iRL = 0; iRL < fRadLen->GetEntriesFast(); iRL++) {
      FairRadLenPoint* point = (FairRadLenPoint*) fRadLen->At(iRL);

      TVector3 pos = point->GetPosition();
      TVector3 posOut = point->GetPositionOut();
      TVector3 res = posOut - pos;

      const TVector3 middle2 = (posOut + pos);
      x = middle2.X() / 2.;
      y = middle2.Y() / 2.;
//         const Double_t z = middle2.Z() / 2;

      const Double_t radThick = res.Mag() / point->GetRadLength();
      radLengthOnTrack[point->GetTrackID()] += radThick;
      trackLength[point->GetTrackID()] += res.Mag();

      // cout << radThick << endl;
      // cout << pos.X() << " " << pos.Y() << " " << pos.Z() << " " << endl;
      // cout << posOut.X() << " " << posOut.Y() << " " << posOut.Z() << " " << endl;

//         Int_t iSta =  pos.Z()/10 - 3 + 0.5; // suppose equdistant stations at 30-100 cm
//         Int_t iStaOut = posOut.Z()/10 - 3 + 0.5;
//         if ( iSta != iStaOut ) continue;
//         if ( iSta >= NStations || iSta < 0 ) continue;
//         RadThick[iSta] += radThick;
   }

//      for ( Int_t i = 0; i < NStations; ++i ) {
//         // cout << i << " " << x << " " << y << " " << RadThick[i] << endl;
//         hStaRadLen[i]->Fill( x, y, RadThick[i] * 100 );
//      }

   for (Int_t k = 0; k < radLengthOnTrack.size(); k++) {
      fHM->H1("hrl_RadLength_Total_H1")->Fill(radLengthOnTrack[k] * 100);
      fHM->H1("hrl_TrackLength_Total_H1")->Fill(trackLength[k]);
      fHM->P2("hrl_RadLength_Total_P2")->Fill(x, y, radLengthOnTrack[k] * 100);
   }
}

void CbmLitRadLengthQa::Finish()
{
   fHM->WriteToFile();
   Draw();
}

void CbmLitRadLengthQa::ReadDataBranches()
{
   FairRootManager* ioman = FairRootManager::Instance();
   assert(ioman != NULL);
   fRadLen = (TClonesArray*) ioman->GetObject("RadLen");
}

void CbmLitRadLengthQa::CreateHistograms()
{
   const Double_t radius = 550;
   const Int_t nofBins = 100;
   fHM->Add("hrl_TrackLength_Total_H1", new TH1D("hrl_TrackLength_Total_H1", "hrl_TrackLength_Total_H1;Track length [cm];Entries", nofBins, 0, 0));
   fHM->Add("hrl_RadLength_Total_H1", new TH1D("hrl_RadLength_Total_H1", "hrl_RadLength_Total_H1;Radiation thickness [%];Entries", nofBins, 0, 0));
   fHM->Add("hrl_RadLength_Total_P2", new TProfile2D("hrl_RadLength_Total_P2", "hrl_RadLength_Total_P2;X [cm];Y [cm];Radiation thickness [%]", nofBins, -radius, radius, nofBins, -radius, radius));
}

void CbmLitRadLengthQa::Draw()
{
   TCanvas* canvas1 = new TCanvas("hrl_TrackLength_Total_H1", "hrl_TrackLength_Total_H1", 1000, 1000);
   DrawH1(fHM->H1("hrl_TrackLength_Total_H1"));

   TCanvas* canvas2 = new TCanvas("hrl_RadLength_Total_H1", "hrl_RadLength_Total_H1", 1000, 1000);
   DrawH1(fHM->H1("hrl_RadLength_Total_H1"));

   TCanvas* canvas3 = new TCanvas("hTotalRadLenH2", "hTotalRadLenH2", 1000, 1000);
   DrawH2(fHM->H2("hrl_RadLength_Total_P2"));
}

ClassImp(CbmLitRadLengthQa);
