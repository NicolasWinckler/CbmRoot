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
///     - derived from errorHandling
///     - base class for errors occuring during trackfinding
///
/// *******************************************************************
///
/// $Author: csteinle $
/// $Date: 2006/07/17 11:36:26 $
/// $Revision: 1.1 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _TRACKFINDERERROR_H
#define _TRACKFINDERERROR_H


#include "../../MiscLIB/include/errorHandling.h"


/* **************************************************************
 * CLASS trackfinderError						 				*
 * **************************************************************/

class trackfinderError : public errorHandling {

public:

/**
 * Default constructor
 */

	trackfinderError();

/**
 * Destructor
 */

	virtual ~trackfinderError();

};

#endif


#ifndef _CANNOTACCESSHOUGHMEMMANAGERERROR_H
#define _CANNOTACCESSHOUGHMEMMANAGERERROR_H


/**
 * CLASS cannotAccessHoughMemManagerError
 */

class cannotAccessHoughMemManagerError : public trackfinderError {

public:

/**
 * Default constructor
 */

	cannotAccessHoughMemManagerError();

/**
 * Destructor
 */

	virtual ~cannotAccessHoughMemManagerError();

/**
 * This method displays an error message.
 */

	void errorMsg();

};

#endif


#ifndef _CANNOTACCESSHISTOGRAMMEMMANAGERERROR_H
#define _CANNOTACCESSHISTOGRAMMEMMANAGERERROR_H


/**
 * CLASS cannotAccessHistogramMemManagerError
 */

class cannotAccessHistogramMemManagerError : public trackfinderError {

public:

/**
 * Default constructor
 */

	cannotAccessHistogramMemManagerError();

/**
 * Destructor
 */

	virtual ~cannotAccessHistogramMemManagerError();

/**
 * This method displays an error message.
 */

	void errorMsg();

};

#endif


#ifndef _CANNOTACCESSHOUGHTRANSFORMERROR_H
#define _CANNOTACCESSHOUGHTRANSFORMERROR_H


/**
 * CLASS cannotAccessHoughTransformError
 */

class cannotAccessHoughTransformError : public trackfinderError {

public:

/**
 * Default constructor
 */

	cannotAccessHoughTransformError();

/**
 * Destructor
 */

	virtual ~cannotAccessHoughTransformError();

/**
 * This method displays an error message.
 */

	void errorMsg();

};

#endif


#ifndef _CANNOTACCESSHISTOGRAMTRANSFORMERROR_H
#define _CANNOTACCESSHISTOGRAMTRANSFORMERROR_H


/**
 * CLASS cannotAccessHistogramTransformError
 */

class cannotAccessHistogramTransformError : public trackfinderError {

public:

/**
 * Default constructor
 */

	cannotAccessHistogramTransformError();

/**
 * Destructor
 */

	virtual ~cannotAccessHistogramTransformError();

/**
 * This method displays an error message.
 */

	void errorMsg();

};

#endif
