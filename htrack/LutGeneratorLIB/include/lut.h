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
///     - base class for the second look-up-tables
///
/// *******************************************************************
///
/// $Author: csteinle $
/// $Date: 2008-02-29 11:43:28 $
/// $Revision: 1.8 $
///
//////////////////////////////////////////////////////////////////////

#ifndef _LUT_H
#define _LUT_H


#include "../../MiscLIB/include/defs.h"
#include "../../DataObjectLIB/include/lutDefinition.h"
#include "../../DataObjectLIB/include/lutHoughBorder.h"
#include "../../DataRootObjectLIB/include/trackfinderInputHit.h"
#include "../../DataRootObjectLIB/include/trackfinderInputMagneticField.h"
#include <string>


/* **************************************************************
 * CLASS lut									 				*
 * **************************************************************/

class lut {

protected:

	lutDefinition  def;			/**< Object stores the parameter for the lut object. */
	lutHoughBorder border;		/**< Object stores the actual computed border, if no object is provided in the evaluate()-function. */

public:

/**
 * Default constructor
 */

	lut();

/**
 * Constructor
 * @param dim1Min is the minimal value of the first dimension
 * @param dim1Max is the maximal value of the first dimension
 * @param dim1Step is the stepwidth of the first dimension to build a grid
 * @param dim2Min is the minimal value of the second dimension
 * @param dim2Max is the maximal value of the second dimension
 * @param dim2Step is the stepwidth of the second dimension to build a grid
 */

	lut(double dim1Min, double dim1Max, int dim1Step, double dim2Min, double dim2Max, int dim2Step);

/**
 * Destructor
 */

	virtual ~lut();

/**
 * Method initializes the object.
 * @param dim1Min is the minimal value of the first dimension
 * @param dim1Max is the maximal value of the first dimension
 * @param dim1Step is the stepwidth of the first dimension to build a grid
 * @param dim2Min is the minimal value of the second dimension
 * @param dim2Max is the maximal value of the second dimension
 * @param dim2Step is the stepwidth of the second dimension to build a grid
 */

	void init(double dim1Min, double dim1Max, int dim1Step, double dim2Min, double dim2Max, int dim2Step);

/**
 * This method returns the Definition of the lut object.
 */

	lutDefinition getLutDefinition();

/**
 * This method returns the border.
 */

	lutHoughBorder getBorder();

/**
 * This method clears the border.
 */

	void clear(lutHoughBorder* borderPointer = NULL);

/**
 * This method returns the magnetic field to use.
 * Caution: This function here has no useful
 * implementations in some subclasses. So be
 * careful when using.
 */

	virtual trackfinderInputMagneticField* getMagneticField() = 0;

/**
 * This method returns the magnetic field factor to use instead of
 * the magneticField.
 * Caution: This function here has no useful
 * implementations in some subclasses. So be
 * careful when using.
 */

	virtual double getMagneticFieldFactor() = 0;

/**
 * This method sets the magnetic field to use.
 * Caution: This function here has no useful
 * implementations in some subclasses. So be
 * careful when using.
 */

	virtual void setMagneticField(trackfinderInputMagneticField* magneticField) = 0;

/**
 * This method sets the magnetic field factor to use instead of
 * the magneticField.
 * Caution: This function here has no useful
 * implementations in some subclasses. So be
 * careful when using.
 */

	virtual void setMagneticFieldFactor(double magneticFieldFactor) = 0;

/**
 * This method returns the number of corrections.
 * Caution: This function here has no useful
 * implementations in some subclasses. So be
 * careful when using.
 */

	virtual unsigned long getNumberOfCorrections() = 0;

/**
 * This method returns the number of coord corrections.
 * Caution: This function here has no useful
 * implementations in some subclasses. So be
 * careful when using.
 */

	virtual unsigned long getNumberOfCoordCorrections() = 0;

/**
 * This method resets both correction counters.
 * Caution: This function here has no useful
 * implementations in some subclasses. So be
 * careful when using.
 */

	virtual void resetCorrectionCounter() = 0;

/**
 * This method realizes the lut table.
 * @param hit is the actual hit-object to compute the borders
 * @param if borderPointer is null, the computation would be done in an internal object
 * @return if borderPointer is not null, this object would consist of the computed results
 */

	virtual void evaluate(trackfinderInputHit* hit, lutHoughBorder* borderPointer = NULL) = 0;

};

#endif
