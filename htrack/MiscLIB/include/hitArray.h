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
///     - global definition of the data type used for the hit memory
///
/// *******************************************************************
///
/// $Author: csteinle $
/// $Date: 2006/07/17 11:35:53 $
/// $Revision: 1.1 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _HITARRAY_H
#define _HITARRAY_H


#include "../../DataRootObjectLIB/include/inputHitSpecialMem.h"
#include "../../DataRootObjectLIB/include/inputHitStlMem.h"


/**
 * This two objects are equal of functionality but different in the implementation.
 * So here you can define the one which should be used.
 * HINT: Use the one which is finally faster!!!
 */

#define hitArray inputHitSpecialMem		/**< Definition sets all hitArray objects to inputHitSpecialMem objects. */
//#define hitArray inputHitStlMem		/**< Definition sets all hitArray objects to inputHitStlMem objects. */


#endif
