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
/// $Date: 2007-04-16 10:40:52 $
/// $Revision: 1.7 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _DEFS_H
#define _DEFS_H


/**
 * This definition enables that the hits of just one track can enter the histogram
 */
//#define DEBUGJUSTONEGOODTRACK    1
#undef DEBUGJUSTONEGOODTRACK


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
