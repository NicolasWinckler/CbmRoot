/** LitTrack.h
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2009
 * @version 1.0
 *
 * Track data class.
 **/

#ifndef LITTRACK_H_
#define LITTRACK_H_

#include "LitTypes.h"
#include "LitTrackParam.h"
#include "LitHit.h"

#include <vector>

namespace lit {
namespace parallel {

/* Class implements scalar track data.
 * It is used for input and output scalar data to
 * tracking algorithm. */
class LitScalTrack
{
public:
   /* Constructor */
   LitScalTrack():
      fParamFirst(),
      fParamLast(),
      fChiSq(0.),
      fNDF(1),
      fNofMissingHits(0),
      fPreviousTrackId(0),
      fIsGood(true) {
      fHits.reserve(30);
   }

   /* Destructor */
   virtual ~LitScalTrack() {}

   /* Adds hit to track
    * @param hit Pointer to hit */
   void AddHit(LitScalPixelHit* hit) {
      fHits.push_back(hit);
   }

   /* @return Number of hits in track */
   unsigned short GetNofHits() const {
      return fHits.size();
   }

   /* In general should not be used. One has to add hits
    * using AddHit method. Currently used in track selection
    * algorithm. */
   void SetNofHits(unsigned short nofHits) {
      return fHits.resize(nofHits);
   }

   /* @param index Index of the hit
    * @return Pointer to the hit */
   const LitScalPixelHit* GetHit(unsigned short index) const {
      return fHits[index];
   }

   /* @return First track parameter */
   const LitTrackParamScal& GetParamFirst() const {
      return fParamFirst;
   }

   /* Set first track parameter
    * @param param Pointer to track parameter */
   void SetParamFirst(const LitTrackParamScal& param) {
      fParamFirst = param;
   }

   /* @return Last track parameter */
   const LitTrackParamScal& GetParamLast() const {
      return fParamLast;
   }

   /* Set last track parameter
    * @param param Pointer to track parameter */
   void SetParamLast(const LitTrackParamScal& param) {
      fParamLast = param;
   }

   /* @return Chi-square */
   fscal GetChiSq() const {
      return fChiSq;
   }

   /* Sets chi-square
    * @param chiSq Chi-square value */
   void SetChiSq(fscal chiSq) {
      fChiSq = chiSq;
   }

   /* Increases chi-square by dChiSq
    * @param dChiSq value of dChiSq */
   void IncChiSq(fscal dChiSq) {
      fChiSq += dChiSq;
   }

   /* @return Number of degrees of freedom */
   unsigned short GetNDF() const {
      return fNDF;
   }

   /* Sets number of degrees of freedom
    * @param NDF Value */
   void SetNDF(unsigned short NDF) {
      fNDF = NDF;
   }

   /* @return Number of missing hits */
   unsigned short GetNofMissingHits() const {
      return fNofMissingHits;
   }

   /* Sets number of missing hits
    * @param nofMissingHits Value */
   void SetNofMissingHits(unsigned short nofMissingHits) {
      fNofMissingHits = nofMissingHits;
   }

   /* Increases number of missing hits by dNofMissingHits
    * @param dNofMissingHits Value */
   void IncNofMissingHits(unsigned short dNofMissingHits = 1) {
      fNofMissingHits += dNofMissingHits;
   }

   /* @return Previous track id */
   unsigned short GetPreviousTrackId() const {
      return fPreviousTrackId;
   }

   /* Sets previous trackId
    * @param previousTrackId Value */
   void SetPreviousTrackId(unsigned short previousTrackId) {
      fPreviousTrackId = previousTrackId;
   }

   /* @return Is good track */
   bool IsGood() const {
      return fIsGood;
   }

   /* Sets is good track
    * @param isGood Value */
   void IsGood(bool isGood) {
      fIsGood = isGood;
   }

   /* Returns std::string representation for the class */
   std::string ToString() const {
      return "LitTrack: nofHits=" + lit::parallel::ToString<int>(GetNofHits())
            + " chiSq=" + lit::parallel::ToString<fscal>(GetChiSq())
            + " NDF=" + lit::parallel::ToString<int>(GetNDF())
            + " nofMissingHits=" + lit::parallel::ToString<int>(GetNofMissingHits())
            + " previousTrackId=" + lit::parallel::ToString<int>(GetPreviousTrackId())
            + " paramFirst=" + GetParamFirst().ToString()
            + " paramLast=" + GetParamLast().ToString() + "\n";
   }

   /* Operator << for convenient output to std::ostream */
   friend std::ostream& operator<<(std::ostream& strm, const LitScalTrack& track) {
      strm << track.ToString();
      return strm;
   }

private:
   PixelHitArray fHits; // Array of hits
   LitTrackParamScal fParamFirst; // First track parameter
   LitTrackParamScal fParamLast; // Last track parameter
   fscal fChiSq; // Chi-square of the track
   unsigned short fNDF; // Number of degrees of freedom
   unsigned short fNofMissingHits; // Number of missing hits
   unsigned short fPreviousTrackId; // Id of the track seed
   bool fIsGood; // true id track is "good"
};



/* Class implements track data. */
template<class T>
class LitTrack
{
public:
   /* Constructor */
   LitTrack():
      paramLast(),
      chiSq(0.) {
      hits.reserve(30);
   }

   /* Destructor */
   virtual ~LitTrack(){}

   /* Adds hit to track
    * @param hit Pointer to hit */
   void AddHit(LitPixelHit<T>* hit) {
      hits.push_back(hit);
   }

   /* @return Number of hits in track */
   unsigned short GetNofHits() const {
      return hits.size();
   }

   /* Returns std::string representation for the class */
   std::string ToString() const {
      return "LitTrack: nofHits=" + lit::parallel::ToString<int>(GetNofHits())
            + " chiSq=" + lit::parallel::ToString<T>(chiSq)
            + " paramLast=" + paramLast.ToString() + "\n";
   }

   /* Operator << for convenient output to std::ostream */
   friend std::ostream& operator<<(std::ostream& strm, const LitTrack& track) {
      strm << track.ToString();
      return strm;
   }

public:
   LitTrackParam<T> paramLast; // Last parameter of the track
   std::vector<LitPixelHit<T>*> hits; // Array of hits
   fscal chiSq; // chi-square of the track
} _fvecalignment;

/* Some typedefs for convenience */
typedef LitTrack<fvec> LitTrackVec;
typedef LitTrack<fscal> LitTrackScal;



/* Some typedefs for convenience */
typedef std::vector<LitScalTrack*> TrackArray;
typedef std::vector<LitScalTrack*>::iterator TrackIterator;

} // namespace parallel
} // namespace lit
#endif /* LITTRACK_H_ */
