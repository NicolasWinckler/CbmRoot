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
///     - consists of the information for one point in the fieldmap
///
/// *******************************************************************
///
/// $Author: csteinle $
/// $Date: 2006/07/17 11:27:18 $
/// $Revision: 1.1 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _MAGNETICFIELDVALUE_H
#define _MAGNETICFIELDVALUE_H


/* **************************************************************
 * CLASS magneticFieldValue						 				*
 * **************************************************************/

class magneticFieldValue {

protected:

	double value[3];

public:

/**
 * Default constructor
 */

	magneticFieldValue();

/**
 * Constructor
 */

	magneticFieldValue(const magneticFieldValue& value);

/**
 * Destructor
 */

	virtual ~magneticFieldValue();

/**
 * operator = ()
 */

	const magneticFieldValue& operator = (const magneticFieldValue& value);

/**
 * This method returns the magnetic field value of the x-dimension
 */

	double getFieldX();

/**
 * This method returns the magnetic field value of the y-dimension
 */

	double getFieldY();

/**
 * This method returns the magnetic field value of the z-dimension
 */

	double getFieldZ();

/**
 * This method sets the magnetic field value of the x-dimension
 */

	void setFieldX(double value);

/**
 * This method sets the magnetic field value of the y-dimension
 */

	void setFieldY(double value);

/**
 * This method sets the magnetic field value of the z-dimension
 */

	void setFieldZ(double value);

};

#endif
