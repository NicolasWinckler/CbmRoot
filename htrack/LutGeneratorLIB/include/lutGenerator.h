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
///     - class implementing the look-up-tables generation
///
/// *******************************************************************
///
/// $Author: csteinle $
/// $Date: 2007-05-15 10:14:38 $
/// $Revision: 1.1 $
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
