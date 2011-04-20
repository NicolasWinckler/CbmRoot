/** LitEnums.h
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2009
 * @version 1.0
 *
 * Header defines enumerations used in the track reconstruction.
 **/

#ifndef LITENUMS_H_
#define LITENUMS_H_

namespace lit {
namespace parallel {

enum LitHitType { kLITSTRIPHIT = 0, kLITPIXELHIT = 1, kLITMIXHIT = 2};

//enum LitStatus { kLITSUCCESS = 0, kLITERROR = 1};

//enum LitTrackQa { kLITGOOD = 0, kLITBAD = 1, kLITGOODMERGE = 2};

//enum LitDetectorId { kLITMUCH = 0, kLITTRD = 1, kLITTOF = 2,  kLITMVD = 3};

} // namespace parallel
} // namespace lit
#endif /*LITENUMS_H_*/
