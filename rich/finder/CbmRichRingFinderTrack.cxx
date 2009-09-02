// --------------------------------------------------------------------------------------
// -----                 CbmRichRingFinderTrack source file                         -----
// ----- implementation: Simeon Lebedev (salebedev@jinr.ru)  -----
// -------------------------------------------------------------------------

#include "CbmRichRingFinderTrack.h"

#include "CbmRichHit.h"
#include "CbmRichRing.h"

#include "FairTrackParam.h"

#include <iostream>
#include <cmath>
#include <algorithm>

using std::cout;
using std::endl;

CbmRichRingFinderTrack::CbmRichRingFinderTrack(Int_t verbose, TString geometry )
{
    cout << "-I- CbmRichRingFinderHoughBase constructor for " << geometry << " RICH geometry"<<endl;
    if (geometry != "compact" && geometry != "large"){
        geometry = "compact";
        cout << "-E- CbmRichRingFinderHoughBase::SetParameters UNKNOWN geometry,  " <<
        "Set default parameters for "<< geometry << " RICH geometry"<<endl;
    }
    //fGeometryType = geometry;
}


// -----   Destructor   ----------------------------------------------------
CbmRichRingFinderTrack::~CbmRichRingFinderTrack()
{

}

void CbmRichRingFinderTrack::HoughTransformReconstruction()
{
//    if (fVerbose > 1) cout << "void CbmRichRingFinderHough::HoughTransformReconstruction()"<< endl;
//
//    Int_t indmin, indmax;
//    Float_t x0, y0;
//
//    fMaxDistance = 7.;
//    fMaxDistanceSq = fMaxDistance*fMaxDistance;
//
//    for (UInt_t iProj = 0; iProj < fTrackProj.size(); iProj++)
//    {
//
//		x0 = fTrackProj[iProj].first;
//		y0 = fTrackProj[iProj].second;
//
//        fCurMinX = x0  - fMaxDistance;
//		fCurMinY = y0 - fMaxDistance;
//
//		DefineLocalAreaAndHits(x0, y0, &indmin, &indmax);
//		HoughTransform(indmin, indmax);
//		FindPeak(indmin, indmax);
//
//    }//main loop

}
// -------------------------------------------------------------------------
ClassImp(CbmRichRingFinderTrack)
