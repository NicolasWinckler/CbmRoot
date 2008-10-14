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
///     - class implementing the look-up-tables generation
///
/// *******************************************************************
///
/// $Author: csteinle $
/// $Date: 2008-08-14 12:36:37 $
/// $Revision: 1.2 $
///
//////////////////////////////////////////////////////////////////////

#ifndef _LUTGENERATOR_H
#define _LUTGENERATOR_H


/**
 * Defines all possible versions for the look-up-tables
 */

enum {GENERATERUNGEKUTTALUT, GENERATEANALYTICFORMULALUT, FILELUT, ANALYTICFORMULALUT};


/* **************************************************************
 * CLASS lutGenerator							 				*
 * **************************************************************/

class lutGenerator {

public:

/**
 * Default constructor
 */

	lutGenerator();

/**
 * Destructor
 */

	virtual ~lutGenerator();

};

#endif
