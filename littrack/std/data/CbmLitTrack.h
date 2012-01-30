/** CbmLitTrack.h
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2008
 * @version 1.0
 **
 ** Base data class for track.
 **/

#ifndef CBMLITTRACK_H_
#define CBMLITTRACK_H_

#include "base/CbmLitFloat.h"
#include "base/CbmLitTypes.h"
#include "base/CbmLitEnums.h"
#include "data/CbmLitTrackParam.h"
#include "data/CbmLitFitNode.h"

#include <string>

class CbmLitTrack
{
public:
   /* Constructor */
   CbmLitTrack();

   /* Destructor */
   virtual ~CbmLitTrack();

   /* Assignment operator */
   CbmLitTrack& operator=(const CbmLitTrack& track);

   /* Copy constructor */
   CbmLitTrack(const CbmLitTrack& track);

   /* Returns total number of hits in track */
   int GetNofHits() const { return fHits.size(); }

   /* Returns number of hits for a specified planeId */
   int GetNofHits(int planeId);

   /* Returns track quality */
   LitTrackQa GetQuality() const { return fQuality; }

   /* Returns chi-square */
   litfloat GetChi2() const { return fChi2; }

   /* Returns NDF */
   int GetNDF() const { return fNDF; }

   /* Returns previous track index */
   int GetPreviousTrackId() const { return fPreviousTrackId; }

   /* Returns PDG code */
   int GetPDG() const {return fPDG;};

   /* Returns pointer to the first track parameter */
   const CbmLitTrackParam* GetParamFirst() const { return &fParamFirst; }

   /* Returns pointer to the last track parameter */
   const CbmLitTrackParam* GetParamLast() const { return &fParamLast; }

   /* Returns index of the last detector plane where track has a hit */
   int GetLastPlaneId() const { return fLastPlaneId; };

   /* Returns hit pointer */
   const CbmLitHit* GetHit(int index) const { return fHits[index];}

   /* Returns array of hits */
   const HitPtrVector& GetHits() const { return fHits;}

   /* Returns fit node */
   const CbmLitFitNode* GetFitNode(int index) const {return &fFitNodes[index];}

   /* Returns array of fit nodes */
   const FitNodeVector& GetFitNodes() const {return fFitNodes;}

   /* Returns a pair of hit iterators for a specified detector plane.
    * I.e. hits from iterator.first to iterator.second (excluding iterator.second)
    * belong to the same detector plane with index planeId. */
   HitPtrIteratorPair GetHits(int planeId);

   /* Returns pairs of hit iterators for all detector planes */
   void GetHitBounds(std::vector<HitPtrIteratorPair>& bounds);

   /* Returns number of missing hits */
   int GetNofMissingHits() const {return fNofMissingHits;}

   /* Returns reference to MC */
   int GetRefId() const {return fRefId;}

   /* Sets track quality */
   void SetQuality(LitTrackQa quality) { fQuality = quality; }

   /* Sets chi-square */
   void SetChi2(litfloat chi2) { fChi2 = chi2; }

   /* Sets NDF */
   void SetNDF(int ndf) { fNDF = ndf; }

   /* Sets previous track index */
   void SetPreviousTrackId(int id) { fPreviousTrackId = id; }

   /* Sets PDG code */
   void SetPDG(int pdg) { fPDG = pdg; }

   /* Sets first track parameter */
   void SetParamFirst(const CbmLitTrackParam* par) { fParamFirst = *par; }

   /* Sets last track parameter */
   void SetParamLast(const CbmLitTrackParam* par) { fParamLast = *par; }

   /* TODO temporarily needed for equal_range algorithm */
   void SetNofHits(int nofHits) { fHits.resize(nofHits); }

   /* Sets index of the last detector plane where track has a hit */
   void SetLastPlaneId(int lastPlaneId) { fLastPlaneId = lastPlaneId; }

   /* Adds hit to the track */
   void AddHit(const CbmLitHit* hit);

   /* Sets array of fit nodes */
   void SetFitNodes(const FitNodeVector& nodes) {fFitNodes = nodes;}

   /* Sorts hits by Z position
    * @param downstream If downstream is true than hits are sorted in
    * downstream direction otherwise in upstream direction. */
   void SortHits(bool downstream = true);

   /* Sets number of missing hits */
   void SetNofMissingHits(int nofMissingHits) {fNofMissingHits = nofMissingHits;}

   /* Sets reference to MC */
   void SetRefId(int refId) {fRefId = refId;}

   /* Removes hit */
   void RemoveHit(int index);

   /* Checks track parameters if they are correct */
   bool CheckParams() const;

   /* Removes all hits from track */
   void ClearHits();

   /* Returns std::string representation of the class */
   std::string ToString() const;

private:

   /* Array of hits */
   HitPtrVector fHits;
   /* First track parameter */
   CbmLitTrackParam fParamFirst;
   /* Last track parameter */
   CbmLitTrackParam fParamLast;
   /* Array of fit nodes */
   FitNodeVector fFitNodes;
   /* Track quality */
   LitTrackQa fQuality;
   /* Chi-square */
   litfloat fChi2;
   /* Number of degrees of freedom */
   int fNDF;
   /* Index of the previous track, i.e. STS */
   int fPreviousTrackId;
   /* Last detector plane where track has a hit */
   int fLastPlaneId;
   /* PDG code */
   int fPDG;
   /* Number of missing hits */
   int fNofMissingHits;
   /* Reference to MC */
   int fRefId;
};

#endif /*CBMLITTRACK_H_*/
