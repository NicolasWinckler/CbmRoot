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
///     - class for the digital hit look-up-table implemented with a file
///
/// *******************************************************************
///
/// $Author: csteinle $
/// $Date: 2008-02-29 11:43:28 $
/// $Revision: 1.1 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _DIGITALHITACCESS_H
#define _DIGITALHITACCESS_H

#include "../../DataObjectLIB/include/digitalHit.h"
#include "../../DataRootObjectLIB/include/trackfinderInputHit.h"
#include <string>


/* **************************************************************
 * CLASS digitalHitAccess							 			*
 * **************************************************************/

class digitalHitAccess {

protected:

	digitalHit*    lutMem;				/**< Memory which stores the whole digital hit data. */
	unsigned long  numberOfEntries;		/**< Variable to store the number of entries in the lutMem. */

/**
 * This method allocates new memory.
 */
	
	void allocateNewMemory(unsigned long number);

public:

/**
 * Default constructor
 */

	digitalHitAccess();

/**
 * Destructor
 */

	virtual ~digitalHitAccess();

/**
 * Method inits the variables based on the detector stations.
 */

	void init();

/**
 * This method evaluates the value from the prelut table.
 * @param hit is the actual hit-object to compute the borders
 * @return if borderPointer is not null, this object would consist of the computed results
 */

	void evaluate(trackfinderInputHit* hit, digitalHit* digitalHitPointer = NULL);

/**
 * This method clears the prelut table.
 */
	
	void clear();

/**
 * This method returns the value from the digital hit table.
 * @param index is the actual index for the digital hit
 */
	
	digitalHit getEntry(unsigned long index);

/**
 * This method adds the value at the end of the digital hit table.
 * @param value is the actual value for the entry
 */
	
	void addEntry(digitalHit& value);

/**
 * This method converts the prelut table into a string.
 */
	
	std::string toString();

/**
 * method reads a file to get the table
 * @param fileName is the name of the file to read the data
 */

	void read(std::string fileName);

/**
 * method writes a file representing the table
 * @param fileName is the name of the file to write the data
 * @param name is the name of the table which should be written to file
 * @param usage is a string representing the usage of the table in the program which should be written to file
 */

	void write(std::string fileName, std::string name);

};

#endif
