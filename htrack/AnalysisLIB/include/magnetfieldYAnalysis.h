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
///     - implements methods to analyze the magnetfield
///
/// *******************************************************************
///
/// $Author: csteinle $
/// $Date: 2006/07/17 11:26:54 $
/// $Revision: 1.1 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _MAGNETFIELDYANALYSIS_H
#define _MAGNETFIELDYANALYSIS_H


#include "magnetfieldDirectionAnalysis.h"


/**
 * CLASS magnetfieldYAnalysis
 */

class magnetfieldYAnalysis : public magnetfieldDirectionAnalysis {

protected:

/**
 * returns the corresponding string, which is drawn on that axi,
 * where the impulse is shown.
 */

	const char* getTitleOfXAxi();

/**
 * returns the corresponding modifier for the first constant
 * dimension.
 */

	const char* getDim1Modifier();

/**
 * returns the corresponding modifier for the second constant
 * dimension.
 */

	const char* getDim2Modifier();

/**
 * returns the corresponding string for the name identification.
 */

	const char* getNameOfBxDisplay();

/**
 * returns the corresponding string for the title.
 */

	const char* getTitleOfBxDisplay();

/**
 * returns the corresponding string for the name identification.
 */

	const char* getNameOfByDisplay();

/**
 * returns the corresponding string for the title.
 */

	const char* getTitleOfByDisplay();

/**
 * returns the corresponding string for the name identification.
 */

	const char* getNameOfBzDisplay();

/**
 * returns the corresponding string for the title.
 */

	const char* getTitleOfBzDisplay();

/**
 * returns true, if the saving should be in subdirectories.
 */

	bool getSavingInSubDirectory();

/**
 * returns the corresponding string for the saving subdirectory.
 */

	const char* getNameOfSavingSubDirectory();

public:

/**
 * Default constructor
 */

	magnetfieldYAnalysis();

/**
 * Destructor
 */

	virtual ~magnetfieldYAnalysis();

};

#endif
