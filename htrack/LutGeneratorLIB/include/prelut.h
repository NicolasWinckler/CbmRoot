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
///     - base class for the first look-up-tables
///
/// *******************************************************************
///
/// $Author: csteinle $
/// $Date: 2006/07/17 11:35:42 $
/// $Revision: 1.1 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _PRELUT_H
#define _PRELUT_H


#include "../../DataObjectLIB/include/prelutDefinition.h"
#include "../../DataObjectLIB/include/prelutHoughBorder.h"
#include "../../DataRootObjectLIB/include/trackfinderInputHit.h"


/* **************************************************************
 * CLASS prelut									 				*
 * **************************************************************/

class prelut {

protected:

	prelutDefinition  def;		/**< Object stores the parameter for the lut object. */
	prelutHoughBorder border;	/**< Object stores the actual computed border, if no object is provided in the evaluate()-function. */

public:

/**
 * Default constructor
 */

	prelut();

/**
 * Constructor
 * @param dim3Min is the minimal value of the first dimension
 * @param dim3Max is the maximal value of the first dimension
 * @param dim3Step is the stepwidth of the first dimension to build a grid
 * @param dim3StartEntry is the minimal value of the second dimension
 * @param dim3StopEntry is the maximal value of the second dimension
 */

	prelut(double dim3Min, double dim3Max, int dim3Step, double dim3StartEntry, double dim3StopEntry);

/**
 * Destructor
 */

	virtual ~prelut();

/**
 * Method inits the variables based on the detector stations.
 * @param dim3Min is the minimal value of the first dimension
 * @param dim3Max is the maximal value of the first dimension
 * @param dim3Step is the stepwidth of the first dimension to build a grid
 * @param dim3StartEntry is the minimal value of the second dimension
 * @param dim3StopEntry is the maximal value of the second dimension
 */

	void init(double dim3Min, double dim3Max, int dim3Step, double dim3StartEntry, double dim3StopEntry);

/**
 * This method returns the Definition of the prelut object.
 */

	prelutDefinition getPrelutDefinition();

/**
 * This method returns the border.
 */

	prelutHoughBorder getBorder();

/**
 * This method evaluates the value from the prelut table.
 * @param hit is the actual hit-object to compute the borders
 * @param if borderPointer is null, the computation would be done in an internal object
 * @return if borderPointer is not null, this object would consist of the computed results
 */

	virtual void evaluate(trackfinderInputHit* hit, prelutHoughBorder* borderPointer = NULL) = 0;

};

#endif
