/** LitEnums.h
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2009
 * @version 1.0
 *
 * Enumerations used in the track reconstruction.
 **/

#ifndef LITENUMS_H_
#define LITENUMS_H_

namespace lit {
namespace parallel {

/* Type of the hit and station.
 * kLITSTRIPHIT - strip like hit (u, phi),
 * kLITPIXELHIT - pixel like hit (x, y),
 * kLITMIXEDHIT - used for stations which produce both strip and pixel hits */
enum LitHitType { kLITSTRIPHIT = 0, kLITPIXELHIT = 1, kLITMIXHIT = 2};

} // namespace parallel
} // namespace lit
#endif /*LITENUMS_H_*/
