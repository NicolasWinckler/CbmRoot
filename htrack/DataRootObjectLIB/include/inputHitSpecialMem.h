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
///   class:
///     - consists of the hits and a special addon for the special memory
///
/// *******************************************************************
///
/// $Author: csteinle $
/// $Date: 2006/07/17 11:27:34 $
/// $Revision: 1.1 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _INPUTHITSPECIALMEM_H
#define _INPUTHITSPECIALMEM_H


#include "../../MiscLIB/include/memoryDef.h"
#include "inputHitSpecialArray.h"
#include "inputHitSpecialList.h"


#define numberOfMemories maxDimSH				/**< Defines the number of memories in parallel. Ordinary there is one memory for each station of the detector. */
#define typeOfMemories   inputHitSpecialArray	/**< Defines the object which is used to implement one memory. */


/* **************************************************************
 * CLASS inputHitSpecialMem						 				*
 * **************************************************************/

class inputHitSpecialMem {

protected:

	typeOfMemories specialMem[numberOfMemories];		/**< Array to store the hits in the memory of the corresponding detector station. */

public:

/**
 * Default constructor
 */

	inputHitSpecialMem();

/**
 * Constructor
 */

	inputHitSpecialMem(const inputHitSpecialMem& value);

/**
 * Destructor
 */

	virtual ~inputHitSpecialMem();

/**
 * operator = ()
 */

	const inputHitSpecialMem& operator = (const inputHitSpecialMem& value);

/**
 * method resets the memory
 */

	void reset();

/**
 * method returns the number of memories
 */

	unsigned short getNumberOfMemories();

/**
 * operator []
 */

	typeOfMemories& operator [] (size_t index);

/**
 * This method returns the size of the used memory for
 * hits.
 */

	size_t getUsedSizeOfMemory();

/**
 * This method returns the size of the allocated memory for
 * hits.
 */

	size_t getAllocatedSizeOfMemory();

};

#endif
