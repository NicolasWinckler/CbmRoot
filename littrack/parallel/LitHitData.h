/**
 * \file LitHitData.h
 * \brief Class for accessing hits in track reconstruction.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2010
 **/

#ifndef LITHITDATA_H_
#define LITHITDATA_H_

#include "LitHit.h"

#include <vector>
#include <sstream>
using std::vector;
using std::string;
using std::stringstream;
using std::ostream;

namespace lit {
namespace parallel {

/**
 * \class LitHitData
 * \brief Class for accessing hits in track reconstruction.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2009
 */
template<class T>
class LitHitData
{
public:
   /**
    * \brief Constructor.
    */
   LitHitData():
      fMaxErrX(),
      fMaxErrY(),
      fHits(),
      fNofStations(0){}

   /**
    * \brief Destructor.
    */
   virtual ~LitHitData() {}

   /**
    * \brief Set number of stations.
    * \param[in] layout Detector layout.
    */
   void SetNofStations(
      unsigned char nofStations) {
      fNofStations = nofStations;
      fHits.resize(nofStations);
      fMaxErrX.resize(nofStations);
      fMaxErrY.resize(nofStations);
      for(unsigned char i = 0; i < nofStations; i++) {
         fHits[i].reserve(1500);
      }
   }

   /**
    * \brief Add hit.
    * \param[in] station Station index.
    * \param[in] hit Pointer to hit to be added.
    */
   void AddHit(
      int station,
      LitScalPixelHit* hit) {
      fHits[station].push_back(hit);
      fMaxErrX[station] = max(hit->Dx, fMaxErrX[station]);
      fMaxErrY[station] = max(hit->Dy, fMaxErrY[station]);
   }

   /**
    * \brief Return hit pointer.
    * \param[in] station Station index.
    * \param[in] hitId Hit index in array of hits for specified station.
    * \return Hit pointer.
    */
   const LitScalPixelHit* GetHit(
      int station,
      int hitId) const {
      return fHits[station][hitId];
   }

   /**
    * \brief Return vector of hits.
    * \param[in] station Station index.
    * \return Vector of hits.
    */
   const vector<LitScalPixelHit*>& GetHits(
      int station) {
      return fHits[station];
   }

   /**
    * \brief Return number of hits for specified station.
    * \param[in] station Station index.
    * \return Number of hits in station.
    */
   unsigned int GetNofHits(
      int station) const {
      return fHits[station].size();
   }

   /**
    * \brief Return maximum X error in [cm] for specified station.
    * \param[in] station Station index in station group.
    * \return Hit error.
    */
   fscal GetMaxErrX(
      int station) const {
      return fMaxErrX[station];
   }

   /**
    * \brief Return maximum Y error in [cm] for specified station.
    * \param[in] station Station index in station group.
    * \return Hit error.
    */
   fscal GetMaxErrY(
      int station) const {
      return fMaxErrY[station];
   }

   /**
    * \brief Clear hit arrays.
    */
   void Clear() {
      for(unsigned int i = 0; i < fNofStations; i++) {
         fHits[i].clear();
         fHits[i].reserve(1500);
         fMaxErrX[i] = 0.;
         fMaxErrY[i] = 0.;
      }
   }

   /**
    * \brief Returns string representation of the class.
    * \return String representation of the class.
    */
   string ToString() const {
      stringstream ss;
      ss << "HitDataElectron:\n";
      for(int i = 0; i < fNofStations; i++) {
         ss << "   station " << i << ": " << GetNofHits(i) << " hits, "
            << "maxerrx=" << GetMaxErrX(i) << ", maxerry=" << GetMaxErrY(i) << "\n";
      }
      return ss.str();
   }

   /**
    * \brief Operator << for convenient output to ostream.
    * \return Insertion stream in order to be able to call a succession of insertion operations.
    */
   friend ostream& operator<<(ostream& strm, const LitHitDataElectron& hitData) {
      strm << hitData.ToString();
      return strm;
   }

private:
   vector<vector<LitScalPixelHit*> > fHits; // Array of hits
   vector<fscal> fMaxErrX; // Array of maximum X error for each station
   vector<fscal> fMaxErrY; // Array of maximum Y error for each station
   unsigned char fNofStations; // Number of stations
} _fvecalignment;

/**
 * \typedef LitHitData<fscal> LitHitDataScal
 * \brief Scalar version of LitHitData.
 */
typedef LitHitData<fscal> LitHitDataScal;

/**
 * \typedef LitHitData<fvec> LitHitDataVec
 * \brief Vector version of LitHitData.
 */
typedef LitHitData<fvec> LitHitDataVec;

} // namespace parallel
} // namespace lit
#endif /* LITHITDATA_H_ */
