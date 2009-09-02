// --------------------------------------------------------------------------------------
// -----                 CbmRichRingFinderHough source file                         -----
// ----- Algorithm idea: G.A. Ososkov (ososkov@jinr.ru) and Simeon Lebedev (salebedev@jinr.ru)                            -----
// ----- Implementation: Simeon Lebedev (salebedev@jinr.ru)  and Andrei Lebedev (alebedev@jinr.ru)-----

//This program performs the procedure of the ring recognition from data obtained from
//the CBM RICH detector.

#include "CbmRichRingFinderHough.h"
//#include "CbmRichFuzzyKE.h"

#include "CbmRichHit.h"
#include "CbmRichRing.h"

#include "TString.h"
#include "TStopwatch.h"
#include "TSystem.h"

#include <iostream>
#include <cmath>
#include <set>
#include <algorithm>

using std::cout;
using std::endl;
using std::vector;

// -----   Standard constructor   ------------------------------------------
CbmRichRingFinderHough::CbmRichRingFinderHough  ( Int_t verbose, TString geometry )
{
    cout << "-I- CbmRichRingFinderHough constructor for " << geometry << " RICH geometry"<<endl;
    if (geometry != "compact" && geometry != "large"){
        geometry = "compact";
        cout << "-E- CbmRichRingFinderHough::SetParameters UNKNOWN geometry,  " <<
        "Set default parameters for "<< geometry << " RICH geometry"<<endl;
    }
    fGeometryType = geometry;

}

// -----   Destructor   ----------------------------------------------------
CbmRichRingFinderHough::~CbmRichRingFinderHough()
{

}

void CbmRichRingFinderHough::HoughTransformReconstruction()
{
    Int_t indmin, indmax;

    for (UInt_t iHit = 0; iHit < fData.size(); iHit++)
    {
        if (fData[iHit].fIsUsed == true) continue;

        fCurMinX = fData[iHit].fX  - fMaxDistance;
		fCurMinY = fData[iHit].fY - fMaxDistance;

		DefineLocalAreaAndHits(fData[iHit].fX, fData[iHit].fY , &indmin, &indmax);
		HoughTransform(indmin, indmax);
		FindPeak(indmin, indmax);

    }//main loop over hits
}



ClassImp(CbmRichRingFinderHough)
