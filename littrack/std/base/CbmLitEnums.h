/** CbmLitEnums.h
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2008
 * @version 1.0
 *
 * Header defines enumerations used in the track reconstruction.
 **/

#ifndef CBMLITENUMS_H_
#define CBMLITENUMS_H_

enum LitHitType { kLITSTRIPHIT = 0, kLITPIXELHIT = 1, kLITMIXHIT = 2};

enum LitStatus { kLITSUCCESS = 0, kLITERROR = 1};

enum LitTrackQa { kLITGOOD = 0, kLITBAD = 1, kLITGOODMERGE = 2};

enum LitDetectorId { kLITMUCH = 0, kLITTRD = 1, kLITTOF = 2,  kLITMVD = 3};

#endif /*CBMLITENUMS_H_*/
