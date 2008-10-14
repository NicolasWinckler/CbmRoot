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
///     - consists of the information for one point in the fieldmap
///
/// *******************************************************************
///
/// $Author: csteinle $
/// $Date: 2008-08-14 12:32:43 $
/// $Revision: 1.2 $
///
//////////////////////////////////////////////////////////////////////


#include "../include/magneticFieldValue.h"


/**
 * Default constructor
 */

magneticFieldValue::magneticFieldValue() {

	value[0] = 0;
	value[1] = 0;
	value[2] = 0;

}

/**
 * Constructor
 */

magneticFieldValue::magneticFieldValue(const magneticFieldValue& value) {

	this->value[0] = value.value[0];
	this->value[1] = value.value[1];
	this->value[2] = value.value[2];

}

/**
 * Destructor
 */

magneticFieldValue::~magneticFieldValue() {

}

/**
 * operator = ()
 */

const magneticFieldValue& magneticFieldValue::operator = (const magneticFieldValue& value) {

	this->value[0] = value.value[0];
	this->value[1] = value.value[1];
	this->value[2] = value.value[2];

	return *this;

}

/**
 * This method returns the magnetic field value of the x-dimension
 */

double magneticFieldValue::getFieldX() {

	return value[0];

}

/**
 * This method returns the magnetic field value of the y-dimension
 */

double magneticFieldValue::getFieldY() {

	return value[1];

}

/**
 * This method returns the magnetic field value of the z-dimension
 */

double magneticFieldValue::getFieldZ() {

	return value[2];

}

/**
 * This method sets the magnetic field value of the x-dimension
 */

void magneticFieldValue::setFieldX(double value) {

	this->value[0] = value;

}

/**
 * This method sets the magnetic field value of the y-dimension
 */

void magneticFieldValue::setFieldY(double value) {

	this->value[1] = value;

}

/**
 * This method sets the magnetic field value of the z-dimension
 */

void magneticFieldValue::setFieldZ(double value) {

	this->value[2] = value;

}
