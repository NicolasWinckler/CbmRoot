/* *******************************************************************
// (C)opyright 2004
// 
// Institute of Computer Science V
// Prof. Männer
// University of Mannheim, Germany
// 
// *******************************************************************
// 
// Designer(s):   Steinle / Gläß
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
//     - base class for the first look-up-tables
//
// *******************************************************************
//
// $Author: csteinle $
// $Date: 2006/07/17 11:35:45 $
// $Revision: 1.1 $
//
// *******************************************************************/


#include "../include/prelut.h"


/****************************************************************
 * Default constructor											*
 ****************************************************************/

prelut::prelut() {

	init(0, 0, 0, 0, 0);
	border.start  = 0;
	border.stop   = 0;

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

prelut::prelut(double dim3Min, double dim3Max, int dim3Step, double dim3StartEntry, double dim3StopEntry) {

	init(dim3Min, dim3Max, dim3Step, dim3StartEntry, dim3StopEntry);
	border.start  = 0;
	border.stop   = 0;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

prelut::~prelut() {

}

/****************************************************************
 * Method inits the variables based on the detector stations.	*
 ****************************************************************/

void prelut::init(double dim3Min, double dim3Max, int dim3Step, double dim3StartEntry, double dim3StopEntry) {

	def.dim3Min  = dim3Min;
	def.dim3Max  = dim3Max;
	def.dim3Step = dim3Step;
	if (dim3StartEntry > dim3StopEntry)
		def.dim3StartEntry = dim3StopEntry;
	else
		def.dim3StartEntry = dim3StartEntry;
	def.dim3StopEntry = dim3StopEntry;

}

/****************************************************************
 * This method returns the Definition of the prelut object.		*
 ****************************************************************/

prelutDefinition prelut::getPrelutDefinition() {

	return def;

}

/****************************************************************
 * This method returns the border.								*
 ****************************************************************/

prelutHoughBorder prelut::getBorder() {

	return border;

}
