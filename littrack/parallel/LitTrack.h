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

namespace lit {
namespace parallel {

const unsigned char MAX_NOF_HITS_IN_TRACK = 30;

class LitScalTrack
{
public:
   LitScalTrack():
      paramFirst(),
      paramLast(),
      chiSq(0.),
      NDF(1),
      nofHits(0),
      nofMissingHits(0),
      previouseTrackId(0) {
   }

   void AddHit(LitScalPixelHit* hit) {
      hits[nofHits++] = hit;
   }

   LitTrackParam<fscal> paramFirst;
   LitTrackParam<fscal> paramLast;
   fscal chiSq;
   unsigned short NDF;
   LitScalPixelHit* hits[MAX_NOF_HITS_IN_TRACK];
   unsigned short nofHits;
   unsigned short nofMissingHits;
   unsigned short previouseTrackId;

   friend std::ostream& operator<<(std::ostream& strm, const LitScalTrack& track) {
      strm << "LitTrack: " << "nofHits=" << track.nofHits << " chiSq=" << track.chiSq
           << " NDF=" << track.NDF << " nofMissingHits=" << track.nofMissingHits
           << " previouseTrackId=" << track.previouseTrackId
           << " paramFirst=" << track.paramFirst << " paramLast=" << track.paramLast
           << std::endl;
      return strm;
   }
};


template<class T>
class LitTrack
{
public:
   LitTrack():
//    chiSq(0.),
//    NDF(1),
      nofHits(0)//,
//    nofMissingHits(0),
//    previouseTrackId(0)
   {
   }

   void AddHit(LitPixelHit<T>* hit) {
      hits[nofHits++] = hit;
   }

// LitTrackParam<T> paramFirst;
   LitTrackParam<T> paramLast;
// T chiSq;
// unsigned short NDF;
   LitPixelHit<T>* hits[MAX_NOF_HITS_IN_TRACK];
   unsigned short nofHits;
// unsigned short nofMissingHits;
// unsigned short previouseTrackId;

   friend std::ostream& operator<<(std::ostream& strm, const LitTrack& track) {
      strm << "LitTrack: " << "nofHits=" << track.nofHits << " chiSq=" << track.chiSq
//       << " NDF=" << track.NDF
//       << " nofMissingHits=" << track.nofMissingHits
//       << " previouseTrackId=" << track.previouseTrackId
//    << " paramFirst=" << track.paramFirst
           << " paramLast=" << track.paramLast
           << std::endl;
      return strm;
   }
} _fvecalignment;

typedef LitTrack<fvec> LitTrackVec;
typedef LitTrack<fscal> LitTrackScal;

} // namespace parallel
} // namespace lit
#endif /* LITTRACK_H_ */
