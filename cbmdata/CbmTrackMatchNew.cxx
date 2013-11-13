/**
 * \file CbmTrackMatchNew.cxx
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2013
 **/

#include "CbmTrackMatchNew.h"

#include <sstream>

using std::make_pair;
using std::stringstream;

CbmTrackMatchNew::CbmTrackMatchNew() :
      CbmMatch(),
      fNofTrueHits(0),
      fNofWrongHits(0)
{

}

CbmTrackMatchNew::~CbmTrackMatchNew()
{

}

string CbmTrackMatchNew::ToString() const
{
   stringstream ss;
   ss << "CbmTrackMatchNew: ";
   Int_t nofReferences = GetNofReferences();
   ss << "nofReferences=" << nofReferences << " | ";
   for (Int_t i = 0; i < nofReferences; i++) {
      const pair<Int_t, Double_t>& ref = fReferences[i];
      ss << "(" << ref.first << "," << ref.second << ") ";
   }
   ss << "| totalWeight=" << fTotalWeight << ", matchedIndex="
         << fMatchedIndex << " nofTrueHits=" << fNofTrueHits
         << " nofWrongHits=" << fNofWrongHits << std::endl;
   return ss.str();
}

ClassImp(CbmTrackMatchNew);
