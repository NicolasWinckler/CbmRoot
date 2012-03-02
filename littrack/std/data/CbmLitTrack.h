/**
 * \file CbmLitTrack.h
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2008
 * \brief Base data class for track.
 **/

#ifndef CBMLITTRACK_H_
#define CBMLITTRACK_H_

#include "base/CbmLitTypes.h"
#include "base/CbmLitEnums.h"
#include "data/CbmLitTrackParam.h"
#include "data/CbmLitFitNode.h"
#include "data/CbmLitHit.h"

#include <string>
#include <vector>
#include <algorithm>
using std::string;
using std::vector;
using std::sort;

/**
 * \class CbmLitTrack
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2008
 * \brief Base data class for track.
 **/
class CbmLitTrack
{
public:
   /**
    * \brief Constructor.
    */
   CbmLitTrack():
      fHits(),
      fParamFirst(),
      fParamLast(),
      fFitNodes(),
      fQuality(kLITGOOD),
      fChi2(0),
      fNDF(0),
      fPreviousTrackId(0),
      fLastPlaneId(0),
      fPDG(211),
      fNofMissingHits(0),
      fRefId(-1) {}

   /**
    * \brief Destructor.
    */
   virtual ~CbmLitTrack() {}

   /* Getters */
   int GetNofHits() const { return fHits.size(); }
   LitTrackQa GetQuality() const { return fQuality; }
   litfloat GetChi2() const { return fChi2; }
   int GetNDF() const { return fNDF; }
   int GetPreviousTrackId() const { return fPreviousTrackId; }
   int GetPDG() const {return fPDG;};
   const CbmLitTrackParam* GetParamFirst() const { return &fParamFirst; }
   const CbmLitTrackParam* GetParamLast() const { return &fParamLast; }
   int GetLastPlaneId() const { return fLastPlaneId; };
   const CbmLitHit* GetHit(int index) const { return fHits[index];}
   const vector<const CbmLitHit*>& GetHits() const { return fHits;}
   const CbmLitFitNode* GetFitNode(int index) const {return &fFitNodes[index];}
   const vector<CbmLitFitNode>& GetFitNodes() const {return fFitNodes;}
   int GetNofMissingHits() const {return fNofMissingHits;}
   int GetRefId() const {return fRefId;}

   /* Setters */
   void SetQuality(LitTrackQa quality) { fQuality = quality; }
   void SetChi2(litfloat chi2) { fChi2 = chi2; }
   void SetNDF(int ndf) { fNDF = ndf; }
   void SetPreviousTrackId(int id) { fPreviousTrackId = id; }
   void SetPDG(int pdg) { fPDG = pdg; }
   void SetParamFirst(const CbmLitTrackParam* par) { fParamFirst = *par; }
   void SetParamLast(const CbmLitTrackParam* par) { fParamLast = *par; }
   /* TODO temporarily needed for equal_range algorithm */
   void SetNofHits(int nofHits) { fHits.resize(nofHits); }
   void SetLastPlaneId(int lastPlaneId) { fLastPlaneId = lastPlaneId; }
   void SetFitNodes(const vector<CbmLitFitNode>& nodes) {fFitNodes = nodes;}
   void SetNofMissingHits(int nofMissingHits) {fNofMissingHits = nofMissingHits;}
   void SetRefId(int refId) {fRefId = refId;}

   /**
    * \brief Add hit to track. No additional memory is allocated for hit.
    */
   void AddHit(const CbmLitHit* hit) {
      fHits.push_back(hit);
   }

   /**
    * \brief Remove all hits from track. Do not delete memory.
    */
   void ClearHits() {
      fHits.clear();
   }

   /**
    * \brief Remove hit and corresponding fit node.
    */
   void RemoveHit(int index) {
      fHits.erase(fHits.begin() + index);
      if (!fFitNodes.empty()) { fFitNodes.erase(fFitNodes.begin() + index); };
   }

   /**
    * \brief Sort hits by Z position.
    * \param[in] downstream If downstream is true than hits are sorted in downstream direction otherwise in upstream direction.
    */
   void SortHits(bool downstream = true) {
      if (downstream) { sort(fHits.begin(), fHits.end(), CompareHitPtrZLess()); }
      else { sort(fHits.begin(), fHits.end(), CompareHitPtrZMore()); }
   }

   /**
    * \brief Return true if track parameters are correct.
    * \return True if track parameters are correct.
    */
   bool CheckParams() const {
      std::vector<litfloat> covFirst(fParamFirst.GetCovMatrix());
      std::vector<litfloat> covLast(fParamLast.GetCovMatrix());
      for (int i = 0; i < 15; i++) {
         if (std::abs(covFirst[i]) > 10000. ||
               std::abs(covLast[i]) > 10000.) { return false; }
      }
      if (GetNofHits() < 1) { return false; }
      return true;
   }

   /**
    * \brief Return string representation of class.
    * \return String representation of class.
    */
   string ToString() const {
      std::stringstream ss;
      ss << "Track: quality=" <<  fQuality << ", chi2=" << fChi2
         << ", ndf=" << fNDF << ", previousTrackId=" << fPreviousTrackId
         << ", lastPlaneId=" << fLastPlaneId << ", pdg=" << fPDG
         << ", nofHits=" << fHits.size() << ", nofFitNodes=" << fFitNodes.size() << std::endl;
      return ss.str();
   }

private:
   vector<const CbmLitHit*> fHits; // Array of hits
   CbmLitTrackParam fParamFirst; // First track parameter
   CbmLitTrackParam fParamLast; // Last track parameter
   vector<CbmLitFitNode> fFitNodes; // Array of fit nodes
   LitTrackQa fQuality; // Track quality
   litfloat fChi2; // Chi-square
   int fNDF; // Number of degrees of freedom
   int fPreviousTrackId; // Index of the previous track, i.e. STS
   int fLastPlaneId; // Last detector plane where track has a hit
   int fPDG; // PDG code
   int fNofMissingHits; // Number of missing hits
   int fRefId; // Reference to MC
};

/**
 * \brief Comparator for STL sorting algorithms.
 */
class CompareTrackPtrChi2OverNdfLess :
   public std::binary_function<
   const CbmLitTrack*,
   const CbmLitTrack*,
   bool>
{
public:
   bool operator()(const CbmLitTrack* track1, const CbmLitTrack* track2) const {
      return ( (track1->GetChi2() / track1->GetNDF()) < (track2->GetChi2() / track2->GetNDF()) );
   }
};


/**
 * \brief Comparator for STL sorting algorithms.
 */
class CompareTrackPtrPrevTrackIdLess :
   public std::binary_function<
   const CbmLitTrack*,
   const CbmLitTrack*,
   bool>
{
public:
   bool operator()(const CbmLitTrack* track1, const CbmLitTrack* track2) const {
      return track1->GetPreviousTrackId() < track2->GetPreviousTrackId();
   }
};


/**
 * \brief Comparator for STL sorting algorithms.
 */
class CompareTrackPtrNofHitsMore :
   public std::binary_function<
   const CbmLitTrack*,
   const CbmLitTrack*,
   bool>
{
public:
   bool operator()(const CbmLitTrack* track1, const CbmLitTrack* track2) const {
      return track1->GetNofHits() > track2->GetNofHits();
   }
};


/**
 * \brief Comparator for STL sorting algorithms.
 */
class CompareTrackPtrNofHitsLess :
   public std::binary_function<
   const CbmLitTrack*,
   const CbmLitTrack*,
   bool>
{
public:
   bool operator()(const CbmLitTrack* track1, const CbmLitTrack* track2) const {
      return track1->GetNofHits() < track2->GetNofHits();
   }
};


/**
 * \brief Comparator for STL sorting algorithms.
 */
class CompareTrackPtrLastPlaneIdMore :
   public std::binary_function<
   const CbmLitTrack*,
   const CbmLitTrack*,
   bool>
{
public:
   bool operator()(const CbmLitTrack* track1, const CbmLitTrack* track2) const {
      return track1->GetLastPlaneId() > track2->GetLastPlaneId();
   }
};

#endif /*CBMLITTRACK_H_*/
