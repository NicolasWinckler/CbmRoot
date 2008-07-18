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


#ifndef _INPUTHITSPECIALLIST_H
#define _INPUTHITSPECIALLIST_H


#include "../include/trackfinderInputHit.h"
#include <list>


#define minimalArraySize 30		/**< Defines the size of the constant memory. */


/* **************************************************************
 * CLASS inputHitSpecialList					 				*
 * **************************************************************/

class inputHitSpecialList {

protected:

	trackfinderInputHit*                      specialArray[minimalArraySize];	/**< Constant memory to store the added pointers to the hits. */
	std::list<trackfinderInputHit*>           specialAddArray;					/**< Dynamic memory to store the added pointers to the hits. */
	unsigned short                            numberOfEntries;					/**< Variable to store the number of added pointers to hits. */
	unsigned short                            activeObjectPointer;				/**< Variable to access the memory. */
	unsigned short                            internalListObjectPointer;		/**< Variable to store the position of the iterator for accessing the list. */
	std::list<trackfinderInputHit*>::iterator specialAddArrayIterator;			/**< Iterator to access the list. */

public:

/**
 * Default constructor
 */

	inputHitSpecialList();

/**
 * Constructor
 */

	inputHitSpecialList(const inputHitSpecialList& value);

/**
 * Destructor
 */

	virtual ~inputHitSpecialList();

/**
 * operator = ()
 */

	const inputHitSpecialList& operator = (const inputHitSpecialList& value);

/**
 * sets the activeObject to the initial value
 */

	void resetActiveObject();

/**
 * method clears the memory
 */

	void clear();

/**
 * method returns the number of entries
 */

	unsigned short getNumberOfEntries();

/**
 * method pushes the element at the end of the memory.
 */

	bool push(trackfinderInputHit* hit);

/**
 * returns a reference of the activeObject and set the next one
 * to the active object.
 */

	trackfinderInputHit* readActiveObjectAndMakeNextOneActive();

};

#endif
