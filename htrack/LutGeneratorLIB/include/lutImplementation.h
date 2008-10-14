//////////////////////////////////////////////////////////////////////
/// (C)opyright 2004
/// 
/// Institute of Computer Science V
/// Prof. Männer
/// University of Mannheim, Germany
/// 
/// *******************************************************************
/// 
/// Designer(s):   Steinle
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
///     - class for the implementation of both look-up-tables
///
/// *******************************************************************
///
/// $Author: csteinle $
/// $Date: 2008-08-14 12:36:37 $
/// $Revision: 1.3 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _LUTIMPLEMENTATION_H
#define _LUTIMPLEMENTATION_H


#include "../../DataObjectLIB/include/histogramSpace.h"
#include "../../DataRootObjectLIB/include/lutBorder.h"
#include "prelut.h"
#include "lut.h"


/* **************************************************************
 * CLASS lutImplementation						 				*
 * **************************************************************/

class lutImplementation {

protected:

	histogramSpace** space;				/**< Object to store the needed values to compute the trackAnalogInformation object based on the trackDigitalInformation object. */
	bool             isSpaceLocal;		/**< Variable to store if the space object is allocated locally or not. */
	prelut*          firstLut;			/**< Object to implement the first look-up-table of type prelut. */
	lut*             secondLut;			/**< Object to implement the second look-up-table of type lut. */

public:

/**
 * Default constructor
 */

	lutImplementation();

/**
 * Constructor
 * @param dim1Min is the minimal value of the first dimension
 * @param dim1Max is the maximal value of the first dimension
 * @param dim1Step is the stepwidth of the first dimension to build a grid
 * @param dim2Min is the minimal value of the second dimension
 * @param dim2Max is the maximal value of the second dimension
 * @param dim2Step is the stepwidth of the second dimension to build a grid
 * @param dim3Min is the minimal value of the first dimension
 * @param dim3Max is the maximal value of the first dimension
 * @param dim3Step is the stepwidth of the first dimension to build a grid
 * @param dim3StartEntry is the minimal value of the second dimension
 * @param dim3StopEntry is the maximal value of the second dimension
 * @param space is the size of the histogram in the first three dimensions
 */

	lutImplementation(double dim1Min, double dim1Max, int dim1Step, double dim2Min, double dim2Max, int dim2Step);
	lutImplementation(double dim3Min, double dim3Max, int dim3Step, double dim3StartEntry, double dim3StopEntry);
	lutImplementation(double dim1Min, double dim1Max, int dim1Step, double dim2Min, double dim2Max, int dim2Step, double dim3Min, double dim3Max, int dim3Step, double dim3StartEntry, double dim3StopEntry);
	lutImplementation(histogramSpace** space, double dim3StartEntry, double dim3StopEntry);
	lutImplementation(const lutImplementation& value);

/**
 * Destructor
 */

	virtual ~lutImplementation();

/**
 * operator = ()
 */

	const lutImplementation& operator = (const lutImplementation& value);

/**
 * Method initializes the object.
 * @param dim1Min is the minimal value of the first dimension
 * @param dim1Max is the maximal value of the first dimension
 * @param dim1Step is the stepwidth of the first dimension to build a grid
 * @param dim2Min is the minimal value of the second dimension
 * @param dim2Max is the maximal value of the second dimension
 * @param dim2Step is the stepwidth of the second dimension to build a grid
 * @param dim3Min is the minimal value of the first dimension
 * @param dim3Max is the maximal value of the first dimension
 * @param dim3Step is the stepwidth of the first dimension to build a grid
 * @param dim3StartEntry is the minimal value of the second dimension
 * @param dim3StopEntry is the maximal value of the second dimension
 * @param space is the size of the histogram in the first three dimensions
 */

	void init(double dim1Min, double dim1Max, int dim1Step, double dim2Min, double dim2Max, int dim2Step);
	void init(double dim3Min, double dim3Max, int dim3Step, double dim3StartEntry, double dim3StopEntry);
	void init(double dim1Min, double dim1Max, int dim1Step, double dim2Min, double dim2Max, int dim2Step, double dim3Min, double dim3Max, int dim3Step, double dim3StartEntry, double dim3StopEntry);
	void init(histogramSpace** space, double dim3StartEntry, double dim3StopEntry);
	/* if writing the init function for the file lut, compare the dimensions of the histogram with the space object to determin the correctness of the file */

/**
 * This method returns the double pointer to the
 * histogramSpace object.
 */

	histogramSpace** getSpacePointer();

/**
 * This method returns the Definition of the lut object.
 */

	lutDefinition getLutDefinition();

/**
 * This method returns the Definition of the prelut object.
 */

	prelutDefinition getPrelutDefinition();

/**
 * This method returns the lut border.
 */

	lutHoughBorder getLutBorder();

/**
 * This method returns the prelut border.
 */

	prelutHoughBorder getPrelutBorder();

/**
 * This method returns both borders.
 */

	lutBorder getBorder();

/**
 * This method clears the border.
 */

	void clear(lutHoughBorder* borderPointer = NULL);

/**
 * This method sets the prelut range to use.
 * Caution: This function here has no useful
 * implementations in some subclasses. So be
 * careful when using.
 */

	void setPrelutRange(double rangeStart, double rangeStop);

/**
 * This method returns the magnetic field to use.
 * Caution: This function here has no useful
 * implementations in some subclasses. So be
 * careful when using.
 */

	trackfinderInputMagneticField* getMagneticField();

/**
 * This method returns the magnetic field factor to use instead of
 * the magneticField.
 * Caution: This function here has no useful
 * implementations in some subclasses. So be
 * careful when using.
 */

	double getMagneticFieldFactor();

/**
 * This method sets the magnetic field to use.
 * Caution: This function here has no useful
 * implementations in some subclasses. So be
 * careful when using.
 */

	void setMagneticField(trackfinderInputMagneticField* magneticField);

/**
 * This method sets the magnetic field factor to use instead of
 * the magneticField.
 * Caution: This function here has no useful
 * implementations in some subclasses. So be
 * careful when using.
 */

	void setMagneticFieldFactor(double magneticFieldFactor);

/**
 * This method returns the number of corrections.
 * Caution: This function here has no useful
 * implementations in some subclasses. So be
 * careful when using.
 */

	unsigned long getNumberOfCorrections();

/**
 * This method returns the number of coord corrections.
 * Caution: This function here has no useful
 * implementations in some subclasses. So be
 * careful when using.
 */

	unsigned long getNumberOfCoordCorrections();

/**
 * This method resets both correction counters.
 * Caution: This function here has no useful
 * implementations in some subclasses. So be
 * careful when using.
 */

	void resetCorrectionCounter();

/**
 * This method evaluates the lut table.
 * @param hit is the actual hit-object to compute the borders
 * @param if borderPointer is null, the computation would be done in an internal object
 * @return if borderPointer is not null, this object would consist of the computed results
 */

	void evaluateLut(trackfinderInputHit* hit, lutHoughBorder* borderPointer);

/**
 * This method evaluates the prelut table.
 * @param hit is the actual hit-object to compute the borders
 * @param if borderPointer is null, the computation would be done in an internal object
 * @return if borderPointer is not null, this object would consist of the computed results
 */

	void evaluatePrelut(trackfinderInputHit* hit, prelutHoughBorder* borderPointer);

/**
 * This method evaluates both tables.
 * @param hit is the actual hit-object to compute the borders
 * @param if borderPointer is null, the computation would be done in an internal object
 * @return if borderPointer is not null, this object would consist of the computed results
 */

	void evaluate(trackfinderInputHit* hit, lutBorder* borderPointer);

};

#endif
