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
///     - class for errors occuring while doing some analysis operations
///
/// *******************************************************************
///
/// $Author: csteinle $
/// $Date: 2006/12/12 13:04:34 $
/// $Revision: 1.2 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _ANALYSISERROR_H
#define _ANALYSISERROR_H


#include "../../MiscLIB/include/errorHandling.h"


/* **************************************************************
 * CLASS analysisError											*
 * **************************************************************/

class analysisError : public errorHandling {

public:

/**
 * Default constructor
 */

	analysisError();

/**
 * Default destructor
 */

	virtual ~analysisError();

};

#endif


#ifndef _WINDOWDIVISIONERROR_H
#define _WINDOWDIVISIONERROR_H

/**
 * CLASS windowDivisionError
 */

class windowDivisionError : public analysisError {

public:

/**
 * Default constructor
 */

	windowDivisionError();

/**
 * Destructor
 */

	virtual ~windowDivisionError();

/**
 * This method displays an error message.
 */

	void errorMsg();

};

#endif


#ifndef _WINDOWNOTFOUNDERROR_H
#define _WINDOWNOTFOUNDERROR_H

/**
 * CLASS windowNotFoundError
 */

class windowNotFoundError : public analysisError {

public:

/**
 * Default constructor
 */

	windowNotFoundError();

/**
 * Destructor
 */

	virtual ~windowNotFoundError();

/**
 * This method displays an error message.
 */

	void errorMsg();

};

#endif


#ifndef _FAKEPASSINGGRADINGRERROR_H
#define _FAKEPASSINGGRADINGRERROR_H

/**
 * CLASS fakePassingGradingRError
 */

class fakePassingGradingRError : public analysisError {

public:

/**
 * Default constructor
 */

	fakePassingGradingRError();

/**
 * Destructor
 */

	virtual ~fakePassingGradingRError();

/**
 * This method displays an error message.
 */

	void errorMsg();

};

#endif


#ifndef _MAGNETICFIELDNOTFOUNDERROR_H
#define _MAGNETICFIELDNOTFOUNDERROR_H

/**
 * CLASS magneticFieldNotFoundError
 */

class magneticFieldNotFoundError : public analysisError {

public:

/**
 * Default constructor
 */

	magneticFieldNotFoundError();

/**
 * Destructor
 */

	virtual ~magneticFieldNotFoundError();

/**
 * This method displays an error message.
 */

	void errorMsg();

};

#endif


#ifndef _CANNOTACCESSMAGNETFIELDANALYSERERROR_H
#define _CANNOTACCESSMAGNETFIELDANALYSERERROR_H

/**
 * CLASS cannotAccessMagnetfieldAnalyser
 */

class cannotAccessMagnetfieldAnalyser : public analysisError {

public:

/**
 * Default constructor
 */

	cannotAccessMagnetfieldAnalyser();

/**
 * Destructor
 */

	virtual ~cannotAccessMagnetfieldAnalyser();

/**
 * This method displays an error message.
 */

	void errorMsg();

};

#endif


#ifndef _CANNOTACCESSDISPLAYINFOERROR_H
#define _CANNOTACCESSDISPLAYINFOERROR_H

/**
 * CLASS cannotAccessDisplayInfoError
 */

class cannotAccessDisplayInfoError : public analysisError {

public:

/**
 * Default constructor
 */

	cannotAccessDisplayInfoError();

/**
 * Destructor
 */

	virtual ~cannotAccessDisplayInfoError();

/**
 * This method displays an error message.
 */

	void errorMsg();

};

#endif


#ifndef _CANNOTACCESSFINDABLETRACKSMEMORYERROR_H
#define _CANNOTACCESSFINDABLETRACKSMEMORYERROR_H

/**
 * CLASS cannotAccessFindableTracksMemoryError
 */

class cannotAccessFindableTracksMemoryError : public analysisError {

public:

/**
 * Default constructor
 */

	cannotAccessFindableTracksMemoryError();

/**
 * Destructor
 */

	virtual ~cannotAccessFindableTracksMemoryError();

/**
 * This method displays an error message.
 */

	void errorMsg();

};

#endif


#ifndef _CANNOTACCESSFIRSTANALYSISLUTERROR_H
#define _CANNOTACCESSFIRSTANALYSISLUTERROR_H

/**
 * CLASS cannotAccessFirstAnalysisLutError
 */

class cannotAccessFirstAnalysisLutError : public analysisError {

public:

/**
 * Default constructor
 */

	cannotAccessFirstAnalysisLutError();

/**
 * Destructor
 */

	virtual ~cannotAccessFirstAnalysisLutError();

/**
 * This method displays an error message.
 */

	void errorMsg();

};

#endif


#ifndef _CANNOTACCESSSECONDANALYSISLUTERROR_H
#define _CANNOTACCESSSECONDANALYSISLUTERROR_H

/**
 * CLASS cannotAccessSecondAnalysisLutError
 */

class cannotAccessSecondAnalysisLutError : public analysisError {

public:

/**
 * Default constructor
 */

	cannotAccessSecondAnalysisLutError();

/**
 * Destructor
 */

	virtual ~cannotAccessSecondAnalysisLutError();

/**
 * This method displays an error message.
 */

	void errorMsg();

};

#endif


#ifndef _CANNOTACCESSALLMAGNETFIELDDISTRIBUTIONSERROR_H
#define _CANNOTACCESSALLMAGNETFIELDDISTRIBUTIONSERROR_H

/**
 * CLASS cannotAccessAllMagnetfieldFactorDistributionsError
 */

class cannotAccessAllMagnetfieldFactorDistributionsError : public analysisError {

public:

/**
 * Default constructor
 */

	cannotAccessAllMagnetfieldFactorDistributionsError();

/**
 * Destructor
 */

	virtual ~cannotAccessAllMagnetfieldFactorDistributionsError();

/**
 * This method displays an error message.
 */

	void errorMsg();

};

#endif


#ifndef _CANNOTACCESSMAGNETFIELDFACTORANALYSERERROR_H
#define _CANNOTACCESSMAGNETFIELDFACTORANALYSERERROR_H

/**
 * CLASS cannotAccessMagnetfieldFactorAnalyserError
 */

class cannotAccessMagnetfieldFactorAnalyserError : public analysisError {

public:

/**
 * Default constructor
 */

	cannotAccessMagnetfieldFactorAnalyserError();

/**
 * Destructor
 */

	virtual ~cannotAccessMagnetfieldFactorAnalyserError();

/**
 * This method displays an error message.
 */

	void errorMsg();

};

#endif


#ifndef _INDEXISOUTOFDISPLAYRANGEERROR_H
#define _INDEXISOUTOFDISPLAYRANGEERROR_H

/**
 * CLASS indexIsOutOfDisplayRangeError
 */

class indexIsOutOfDisplayRangeError : public analysisError {

protected:

	unsigned short index;
	unsigned short numberOfDisplays;

public:

/**
 * Default constructor
 */

	indexIsOutOfDisplayRangeError();

/**
 * Constructor
 */

	indexIsOutOfDisplayRangeError(unsigned short index, unsigned short numberOfDisplays);

/**
 * Destructor
 */

	virtual ~indexIsOutOfDisplayRangeError();

/**
 * This method displays an error message.
 */

	void errorMsg();

};

#endif


#ifndef _INDEXISOUTOFFACTORRANGEERROR_H
#define _INDEXISOUTOFFACTORRANGEERROR_H

/**
 * CLASS indexIsOutOfFactorRangeError
 */

class indexIsOutOfFactorRangeError : public analysisError {

protected:

	unsigned short index;
	unsigned short numberOfFactors;

public:

/**
 * Default constructor
 */

	indexIsOutOfFactorRangeError();

/**
 * Constructor
 */

	indexIsOutOfFactorRangeError(unsigned short index, unsigned short numberOfFactors);

/**
 * Destructor
 */

	virtual ~indexIsOutOfFactorRangeError();

/**
 * This method displays an error message.
 */

	void errorMsg();

};

#endif


#ifndef _TOOMANYACTIVESTATIONSERROR_H
#define _TOOMANYACTIVESTATIONSERROR_H

/**
 * CLASS tooManyActiveStationsError
 */

class tooManyActiveStationsError : public analysisError {

protected:

	unsigned short actualNumberOfStations;
	unsigned short maximumNumberOfStations;

public:

/**
 * Default constructor
 */

	tooManyActiveStationsError();

/**
 * Constructor
 */

	tooManyActiveStationsError(unsigned short actualNumberOfStations, unsigned short maximumNumberOfStations);

/**
 * Destructor
 */

	virtual ~tooManyActiveStationsError();

/**
 * This method displays an error message.
 */

	void errorMsg();

};

#endif


#ifndef _MAGNETICFIELDNOTFOUNDFORCOMPARISSONTOFACTORSERROR_H
#define _MAGNETICFIELDNOTFOUNDFORCOMPARISSONTOFACTORSERROR_H

/**
 * CLASS magneticFieldNotFoundForComparissonToFactorsError
 */

class magneticFieldNotFoundForComparissonToFactorsError : public analysisError {

public:

/**
 * Default constructor
 */

	magneticFieldNotFoundForComparissonToFactorsError();

/**
 * Destructor
 */

	virtual ~magneticFieldNotFoundForComparissonToFactorsError();

/**
 * This method displays an error message.
 */

	void errorMsg();

};

#endif


#ifndef _CANNOTACCESSTRACKCOORDINATESERROR_H
#define _CANNOTACCESSTRACKCOORDINATESERROR_H

/**
 * CLASS cannotAccessTrackCoordinatesError
 */

class cannotAccessTrackCoordinatesError : public analysisError {

public:

/**
 * Default constructor
 */

	cannotAccessTrackCoordinatesError();

/**
 * Destructor
 */

	virtual ~cannotAccessTrackCoordinatesError();

/**
 * This method displays an error message.
 */

	void errorMsg();

};

#endif
