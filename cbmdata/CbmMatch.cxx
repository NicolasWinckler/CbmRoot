/**
 * \file CbmMatch.cxx
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2013
 **/

#include "CbmMatch.h"

#include <sstream>

using std::make_pair;
using std::stringstream;

CbmMatch::CbmMatch() :
      fReferences(),
      fTotalWeight(0.),
      fMatchedIndex(-1)
{

}

CbmMatch::~CbmMatch()
{

}

string CbmMatch::ToString() const
{
   stringstream ss;
   ss << "CbmMatch: ";
   Int_t nofReferences = GetNofReferences();
   ss << "nofReferences=" << nofReferences << " | ";
   for (Int_t i = 0; i < nofReferences; i++) {
      const pair<Int_t, Double_t>& ref = fReferences[i];
      ss << "(" << ref.first << "," << ref.second << ") ";
   }
   ss << "| totalWeight=" << fTotalWeight << ", matchedIndex="
         << fMatchedIndex << std::endl;
   return ss.str();
}

void CbmMatch::AddReference(const CbmMatch* match)
{
   const vector<pair<Int_t, Double_t> >& refs = match->GetReferences();
   Int_t nofReferences = match->GetNofReferences();
   for (Int_t i = 0; i < nofReferences; i++) {
      const pair<Int_t, Double_t>& ref = refs[i];
      AddReference(ref.first, ref.second);
   }
}

void CbmMatch::AddReference(Int_t referenceId, Double_t referenceWeight)
{
   Int_t addedIndex = -1;
   Int_t nofReferences = GetNofReferences();
   for (Int_t i = 0; i < nofReferences; i++) {
      pair<Int_t, Double_t>& ref = fReferences[i];
      if (ref.first == referenceId) {
         ref.second += referenceWeight;
         addedIndex = i;
         break;
      }
   }
   if (addedIndex < 0) {
      fReferences.push_back(make_pair(referenceId, referenceWeight));
      addedIndex = fReferences.size() - 1;
   }

   fTotalWeight += referenceWeight;
   if (fMatchedIndex < 0) {
      fMatchedIndex = addedIndex;
   } else {
      if (fReferences[addedIndex].second > fReferences[fMatchedIndex].second) {
         fMatchedIndex = addedIndex;
      }
   }
}

void CbmMatch::Clear()
{
   fReferences.clear();
   fTotalWeight = 0.;
   fMatchedIndex = -1;
}

ClassImp(CbmMatch);
