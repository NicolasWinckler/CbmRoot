/**
 * \file CbmRichGeoOpt.cxx
 *
 * \author Tariq Mahmoud <t.mahmoud@gsi.de>
 * \date 2012
 **/

#include "CbmRichGeoOpt.h"

#include "TH1D.h"
#include "TH1.h"
#include "TCanvas.h"
#include "TClonesArray.h"

#include "CbmMCTrack.h"
#include "FairTrackParam.h"
#include "CbmRichHit.h"
#include "FairMCPoint.h"
#include "CbmDrawHist.h"
#include "CbmTrackMatchNew.h"
#include "CbmRichRing.h"
#include "CbmRichHit.h"

#include "std/utils/CbmLitUtils.h"

#include <iostream>
#include <string>
#include <boost/assign/list_of.hpp>

using namespace std;
using boost::assign::list_of;

CbmRichGeoOpt::CbmRichGeoOpt()
  : FairTask("CbmRichGeoOpt"),
    fRichPoints(NULL),
    fMcTracks(NULL),
    fRefPoints(NULL),
    fEventNum(0)
{
}

CbmRichGeoOpt::~CbmRichGeoOpt()
{

}

InitStatus CbmRichGeoOpt::Init()
{
   cout << "CbmRichGeoOpt::Init"<<endl;
   FairRootManager* ioman = FairRootManager::Instance();
   if (NULL == ioman) { Fatal("CbmRichGeoOpt::Init","RootManager not instantised!"); }

   fRichPoints = (TClonesArray*) ioman->GetObject("RichPoint");
   if ( NULL == fRichPoints) { Fatal("CbmRichGeoOpt::Init","No RichPoint array!"); }

   fRefPoints = (TClonesArray*) ioman->GetObject("RefPlanePoint");
   if ( NULL == fRefPoints) { Fatal("CbmRichGeoOpt::Init","No fRefPoints array!"); }

   fMcTracks = (TClonesArray*) ioman->GetObject("MCTrack");
   if ( NULL == fMcTracks) { Fatal("CbmRichGeoOpt::Init","No MCTrack array!"); }

   InitHistograms();

   return kSUCCESS;
}

void CbmRichGeoOpt::Exec(
      Option_t* option)
{
   fEventNum++;

   cout << "CbmRichGeoOpt, event No. " <<  fEventNum << endl;

 /*  Int_t nofPoints = fRichPoints->GetEntriesFast();
   for (int i = 0; i < nofPoints; i++) {
	   CbmRichPoint* point = fRichPoints->At(i);
	   if (point == NULL) continue;

   }*/

}



void CbmRichGeoOpt::InitHistograms()
{


}



void CbmRichGeoOpt::Finish()
{


}



ClassImp(CbmRichGeoOpt)

