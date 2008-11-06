/* *******************************************************************
// (C)opyright 2005
// 
// Institute of Computer Science V
// Prof. Männer
// University of Mannheim, Germany
// 
// *******************************************************************
// 
// Designer(s):   Steinle
// 
// *******************************************************************
// 
// Project:     Trackfinder for CBM-Project at GSI-Darmstadt, Germany
// 
// *******************************************************************
// 
// Description:
//
//   class:
//     - class to implement the Runge-Kutta approach by using the
//       GEANE package
//
// *******************************************************************
//
// $Author: csteinle $
// $Date: 2008-10-29 11:04:02 $
// $Revision: 1.0 $
//
// *******************************************************************/


#include "../include/rungeKuttaInterface.h"


/****************************************************************
 * CLASS rungeKuttaInterface					 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

rungeKuttaInterface::rungeKuttaInterface() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

rungeKuttaInterface::~rungeKuttaInterface() {

}

/****************************************************************
 * method evaluates all hits in the detector stations for a		*
 * given track parameter and combines the values				*
 ****************************************************************/

std::list<trackfinderInputHit> rungeKuttaInterface::evaluate(trackParameter& parameter) {

	std::list<trackfinderInputHit> returnValue;

	returnValue.clear();

/**/

	return returnValue;

}
