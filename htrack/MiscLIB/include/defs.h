//////////////////////////////////////////////////////////////////////
/// (C)opyright 2004
/// 
/// Institute of Computer Science V
/// Prof. M‰nner
/// University of Mannheim, Germany
/// 
/// *******************************************************************
/// 
/// Designer(s):   Steinle / Gl‰ﬂ
/// 
/// *******************************************************************
/// 
/// Project:     Trackfinder for CBM-Project at GSI-Darmstadt, Germany
/// 
/// *******************************************************************
/// 
/// Description:
///
///   file:
///     - global definition of structs, defines and enums for project
///
/// *******************************************************************
///
/// $Author: csteinle $
/// $Date: 2008-02-29 11:43:43 $
/// $Revision: 1.9 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _DEFS_H
#define _DEFS_H


/**
 * LUTVERSION defines the version of the lut object which is
 * used. Version 1 has the following rules:
 * - The pos2 value is always not smaller than the pos2
 *   value before.
 * - The pos1 value is always equal or bigger than the pos1
 *   value before.
 * Version 2 has the following additional rules:
 * - The pos2 value is always 1 bigger than the pos2 value
 *   before.
 * - No warning is printed
 * Version 3 has the following additional rules:
 * - The pos1 value is always equal or 1 bigger than the
 *   pos1 value before.
 * - No warning is printed
 * Version 4 has the following additional rules:
 * - A warning would be printed just if the rule is hitted once or more
 * Version 5:
 * - A warning is printed all the time
 */

#define LUTVERSION 3


/**
 * This definition enables that the hits of just one track can enter the histogram
 */
//#define DEBUGJUSTONEGOODTRACK    1
#undef DEBUGJUSTONEGOODTRACK


/**
 * This definition is just used in combination with DEBUGJUSTONEGOODTRACKINDEX
 * to workaround the displaying of all tracks
 */
#define INVALIDTRACKINDEX -1


/**
 * This definition makes the library code suitable to run in the cbmrootframework
 * context or to run in a standalone context.
 */

//#undef CBMROOTFRAMEWORK
#define CBMROOTFRAMEWORK

/**
 * This definition makes the library code suitable to run in a
 * very old version of the the cbmrootframework context
 * belonging to the input access (hits, points).
 */

#undef CBMROOTFRAMEWORKINPUTCOMPATIBILITY
//#define CBMROOTFRAMEWORKINPUTCOMPATIBILITY

/**
 * This definition makes the library code suitable to run in an
 * old version of the the cbmrootframework context belonging to
 * the hit access.
 */

#undef CBMROOTFRAMEWORKHITCOMPATIBILITY
//#define CBMROOTFRAMEWORKHITCOMPATIBILITY

/**
 * This definition enables all warning Messages which are
 * not neccessary anymore. This messages was used in some
 * older versions of the program.
 */

//#define ENABLEALLWARNINGS
#undef ENABLEALLWARNINGS

/**
 * For the standalone version one have to setup the correct version of the root
 * framework.
 */

#ifndef CBMROOTFRAMEWORK

#define ROOTFRAMEWORK 51000

#endif


/* 
 * If defined, then no analysis of trackfinding would be done.
 * To this it is also not possible to do some analysis because
 * there are just the same objects than in hardware. That means
 * that the hits for a peak can also not be delivered.
 */

//#define NOANALYSIS
#undef NOANALYSIS


#endif
