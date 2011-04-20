/** LitTrackFinder.h
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2010
 * @version 1.0
 **
 ** Interface class for fast tracking algorithm.
 **/

#ifndef LITTRACKFINDER_H_
#define LITTRACKFINDER_H_

namespace lit {
namespace parallel {

class LitTrackFinder
{
public:
   LitTrackFinder() {};
   virtual ~LitTrackFinder() {};

   virtual void DoFind(
      LitScalPixelHit* hits[],
      unsigned int nofHits,
      LitScalTrack* trackSeeds[],
      unsigned int nofTrackSeeds,
      LitScalTrack* tracks[],
      unsigned int& nofTracks) = 0;
};

} // namespace parallel
} // namespace lit
#endif /* LITTRACKFINDER_H_ */
