/**
 * \file CbmMatch.h
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2013
 *
 * Base data class for storing RECO-to-MC matching information.
 **/

#ifndef CBMMATCH_H_
#define CBMMATCH_H_

#include "TObject.h"
#include <string>
#include <vector>
#include <algorithm>

using std::string;
using std::vector;
using std::pair;

class CbmMatch : public TObject
{
public:
   /**
    * \brief Default constructor.
    */
   CbmMatch();

   /**
    * \brief Destructor.
    */
   virtual ~CbmMatch();

   /* Accessors */
   Int_t GetReferenceId(Int_t i) const { return fReferences[i].first; }
   Int_t GetReferenceWeight(Int_t i) const { return fReferences[i].second; }
   const pair<Int_t, Double_t>& GetReference(Int_t i) const { return fReferences[i]; }
   Int_t GetMatchedReferenceId() const { return fReferences[fMatchedIndex].first; }
   Int_t GetMatchedReferenceWeight() const { return fReferences[fMatchedIndex].second; }
   const vector<pair<Int_t, Double_t> >& GetReferences() const { return fReferences; }
   Int_t GetNofReferences() const { return fReferences.size(); }
   Double_t GetTotalWeight() const { return fTotalWeight; }

   /* Modifiers */
   void AddReference(const CbmMatch* match);
   void AddReference(Int_t referenceId, Double_t referenceWeight = 1.);
   void Clear();

   /**
    * \brief Return string representation of the object.
    * \return String representation of the object.
    **/
   virtual string ToString() const;

protected:
   vector<pair<Int_t, Double_t> > fReferences; // List of indices to MCPoint or MCTrack
   Double_t fTotalWeight; // Sum of all reference weights
   Int_t fMatchedIndex; // Index of the matched reference in fReferences array

   ClassDef(CbmMatch, 1);
};

#endif /* CBMMATCH_H_ */
