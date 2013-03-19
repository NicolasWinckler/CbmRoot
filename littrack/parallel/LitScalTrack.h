/**
 * \file LitScalTrack.h
 * \brief Scalar track data class.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2009
 */

#ifndef LITSCALTRACK_H_
#define LITSCALTRACK_H_

#include "LitTrackParam.h"
#include "LitScalPixelHit.h"
#include <string>
#include <sstream>
#include <vector>

using std::stringstream;
using std::vector;
using std::string;

namespace lit {
namespace parallel {

/**
 * \class LitScalTrack
 * \brief Scalar track data class.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2009
 *
 * It is used for input and output scalar data to
 * tracking algorithm.
 */
class LitScalTrack
{
public:
   /**
    * \brief Constructor.
    */
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

   /**
    * \brief Destructor.
    */
   virtual ~LitScalTrack() {}

   /**
    * \brief Adds hit to track.
    * @param hit Pointer to hit.
    */
   void AddHit(LitScalPixelHit* hit) {
      fHits.push_back(hit);
   }

   /**
    * \brief Returns number of hits in track.
    * \return Number of hits in track.
    */
   unsigned short GetNofHits() const {
      return fHits.size();
   }

   /**
    * \brief Depricated.
    *
    * In general should not be used. One has to add hits
    * using AddHit method. Currently used in track selection
    * algorithm.
    */
   void SetNofHits(unsigned short nofHits) {
      return fHits.resize(nofHits);
   }

   /**
    * \brief Returns pointer to the hit.
    * \param[in] index Index of the hit in track.
    * \return Pointer to hit.
    */
   const LitScalPixelHit* GetHit(unsigned short index) const {
      return fHits[index];
   }

   /**
    * \brief Returns first parameter of the track.
    * \return First track parameter.
    */
   const LitTrackParamScal& GetParamFirst() const {
      return fParamFirst;
   }

   /**
    * \brief Sets first track parameter.
    * \param[in] param Reference to track parameter to be set.
    */
   void SetParamFirst(const LitTrackParamScal& param) {
      fParamFirst = param;
   }

   /**
    * \brief Returns last parameter of the track.
    * \return Last track parameter.
    */
   const LitTrackParamScal& GetParamLast() const {
      return fParamLast;
   }

   /**
    * \brief Sets last track parameter.
    * \param[in] param Reference to track parameter to be set.
    */
   void SetParamLast(const LitTrackParamScal& param) {
      fParamLast = param;
   }

   /**
    * \brief Returns chi square of the track.
    * \return Chi square of the track.
    */
   fscal GetChiSq() const {
      return fChiSq;
   }

   /**
    * \brief Sets chi square.
    * \param[in] chiSq Chi square value to be set.
    */
   void SetChiSq(fscal chiSq) {
      fChiSq = chiSq;
   }

   /**
    * \brief Increases chi square by dChiSq.
    * \param[in] dChiSq Value of dChiSq.
    */
   void IncChiSq(fscal dChiSq) {
      fChiSq += dChiSq;
   }

   /**
    * \brief Returns number of degrees of freedom.
    * \return Number of degrees of freedom.
    */
   unsigned short GetNDF() const {
      return fNDF;
   }

   /**
    * \brief Sets number of degrees of freedom.
    * \param[in] NDF NDF value to be set.
    */
   void SetNDF(unsigned short NDF) {
      fNDF = NDF;
   }

   /**
    * \brief Returns number of missing hits.
    * \return Number of missing hits.
    */
   unsigned short GetNofMissingHits() const {
      return fNofMissingHits;
   }

   /**
    * \brief Sets number of missing hits.
    * \param[in] nofMissingHits Number of missing hits to be set.
    */
   void SetNofMissingHits(unsigned short nofMissingHits) {
      fNofMissingHits = nofMissingHits;
   }

   /**
    * \brief Increases number of missing hits by dNofMissingHits.
    * \param[in] dNofMissingHits Value of dNofMissingHits.
    */
   void IncNofMissingHits(unsigned short dNofMissingHits = 1) {
      fNofMissingHits += dNofMissingHits;
   }

   /**
    * \brief Return Previous track index.
    * \return Previous track id.
    */
   unsigned short GetPreviousTrackId() const {
      return fPreviousTrackId;
   }

   /**
    * \brief Sets previous trackId.
    * \param previousTrackId Value of previous track index.
    */
   void SetPreviousTrackId(unsigned short previousTrackId) {
      fPreviousTrackId = previousTrackId;
   }

   /**
    * \brief Returns true if track is good.
    * \return True if track is good.
    */
   bool IsGood() const {
      return fIsGood;
   }

   /**
    * \brief Sets is good track.
    * \param[in] isGood Value.
    */
   void IsGood(bool isGood) {
      fIsGood = isGood;
   }

   /**
    * \brief Returns string representation of the class.
    * \return String representation of the class.
    */
   string ToString() const {
      stringstream ss;
      ss << "LitTrack: nofHits=" << GetNofHits() << " chiSq=" << GetChiSq()
         << " NDF=" << GetNDF() << " nofMissingHits=" << GetNofMissingHits()
         << " previousTrackId=" << GetPreviousTrackId()
         << " paramFirst=" << GetParamFirst() << " paramLast=" << GetParamLast() << "\n";
      return ss.str();
   }

   /**
    * \brief Operator << for convenient output to ostream.
    * \return Insertion stream in order to be able to call a succession of insertion operations.
    */
   friend ostream& operator<<(ostream& strm, const LitScalTrack& track) {
      strm << track.ToString();
      return strm;
   }

private:
   vector<LitScalPixelHit*> fHits; // Array of hits
   LitTrackParamScal fParamFirst; // First track parameter
   LitTrackParamScal fParamLast; // Last track parameter
   fscal fChiSq; // Chi-square of the track
   unsigned short fNDF; // Number of degrees of freedom
   unsigned short fNofMissingHits; // Number of missing hits
   unsigned short fPreviousTrackId; // Id of the track seed
   bool fIsGood; // true id track is "good"
};

} // namespace parallel
} // namespace littrack


#endif /* LITSCALTRACK_H_ */
