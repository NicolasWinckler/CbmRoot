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
///     - derived from lut
///     - class for the second look-up-table implemented with a file
///
/// *******************************************************************
///
/// $Author: csteinle $
/// $Date: 2006/11/07 12:48:05 $
/// $Revision: 1.2 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _lutAccess_H
#define _lutAccess_H


#include "../../DataObjectLIB/include/lutAccessFile.h"
#include "lut.h"
#include <string>


/* **************************************************************
 * CLASS lutAccess								 				*
 * **************************************************************/

class lutAccess : public lut {

protected:

	lutHoughBorder* lutMem;					/**< Memory which stores the whole look up table. */
	unsigned long   numberOfEntries;		/**< Variable to store the number of entries in the lutMem. */

/**
 * This method allocates new memory.
 */
	
	void allocateNewMemory(unsigned long number);

public:

/**
 * Default constructor
 */

	lutAccess();

/**
 * Constructor
 * @param dim1Min is the minimal value of the first dimension
 * @param dim1Max is the maximal value of the first dimension
 * @param dim1Step is the stepwidth of the first dimension to build a grid
 * @param dim2Min is the minimal value of the second dimension
 * @param dim2Max is the maximal value of the second dimension
 * @param dim2Step is the stepwidth of the second dimension to build a grid
 */

	lutAccess(double dim1Min, double dim1Max, int dim1Step, double dim2Min, double dim2Max, int dim2Step);

/**
 * Destructor
 */

	virtual ~lutAccess();

/**
 * Method inits the variables based on the detector stations.
 * @param dim1Min is the minimal value of the first dimension
 * @param dim1Max is the maximal value of the first dimension
 * @param dim1Step is the stepwidth of the first dimension to build a grid
 * @param dim2Min is the minimal value of the second dimension
 * @param dim2Max is the maximal value of the second dimension
 * @param dim2Step is the stepwidth of the second dimension to build a grid
 */

	void init(double dim1Min, double dim1Max, int dim1Step, double dim2Min, double dim2Max, int dim2Step);

/**
 * This method sets the magnetic field to use.
 * Caution: This function here has no useful
 * implementation. It is just here because in
 * another subclass of lut, this function can
 * be used and is very important. This function
 * is virtual in the base class.
 */

	void setMagneticField(trackfinderInputMagneticField* magneticField);

/**
 * This method sets the magnetic field factor to use instead of
 * the magneticField.
 * Caution: This function here has no useful
 * implementation. It is just here because in
 * another subclass of lut, this function can
 * be used and is very important. This function
 * is virtual in the base class.
 */

	void setMagneticFieldFactor(double magneticFieldFactor);

/**
 * This method returns the number of corrections.
 * Caution: This function here has no useful
 * implementation. It is just here because in
 * another subclass of lut, this function can
 * be used and is very important. This function
 * is virtual in the base class.
 */

	unsigned long getNumberOfCorrections();

/**
 * This method returns the number of coord corrections.
 * Caution: This function here has no useful
 * implementation. It is just here because in
 * another subclass of lut, this function can
 * be used and is very important. This function
 * is virtual in the base class.
 */

	unsigned long getNumberOfCoordCorrections();

/**
 * This method resets both correction counters.
 * Caution: This function here has no useful
 * implementation. It is just here because in
 * another subclass of lut, this function can
 * be used and is very important. This function
 * is virtual in the base class.
 */

	void resetCorrectionCounter();

/**
 * This method evaluates the value from the lut table.
 * @param hit is the actual hit-object to compute the borders
 * @param if borderPointer is null, the computation would be done in an internal object
 * @return if borderPointer is not null, this object would consist of the computed results
 */

	void evaluate(trackfinderInputHit* hit, lutHoughBorder* borderPointer = NULL);

/**
 * This method clears the prelut table.
 */
	
	void clear();

/**
 * This method returns the value from the lut table.
 * @param index is the actual index for the borders
 */
	
	lutHoughBorder getEntry(unsigned long index);

/**
 * This method adds the value at the end of the lut table.
 * @param value is the actual value for the entry
 */
	
	void addEntry(lutHoughBorder value);

/**
 * This method converts the lut table into a string.
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
 * @param format is the specifier for the format in which the information is written to file
 */

	void write(std::string fileName, std::string name, unsigned short format = SOFTWAREFORMAT);

};

#endif
