//////////////////////////////////////////////////////////////////////
/// (C)opyright 2005
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
///     - class to implement the Runge-Kutta approach by using the
///       GEANE package
///
/// *******************************************************************
///
/// $Author: csteinle $
/// $Date: 2008-10-29 11:09:52 $
/// $Revision: 1.0 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _RUNGEKUTTAAPPROACH_H
#define _RUNGEKUTTAAPPROACH_H


#include "../../DataObjectLIB/include/trackParameter.h"
#include "../include/trackfinderInputHit.h"
#include <list>


/* **************************************************************
 * CLASS rungeKuttaInterface					 				*
 * **************************************************************/

class rungeKuttaInterface {

public:

/**
 * Default constructor
 */

	rungeKuttaInterface();

/**
 * Destructor
 */

	virtual ~rungeKuttaInterface();

/**
 * method evaluates all hits in the detector stations for a
 * given track parameter
 */

	std::list<trackfinderInputHit> evaluate(trackParameter& parameter);

};

#endif
