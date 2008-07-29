//////////////////////////////////////////////////////////////////////
/// (C)opyright 2004
/// 
/// Institute of Computer Science V
/// Prof. M�nner
/// University of Mannheim, Germany
/// 
/// *******************************************************************
/// 
/// Designer(s):   Steinle / Gl��
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
///     - class for errors occuring with the root data object handling
///
/// *******************************************************************
///
/// $Author: csteinle $
/// $Date: 2007-10-19 14:32:56 $
/// $Revision: 1.4 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _DATAROOTOBJECTERROR_H
#define _DATAROOTOBJECTERROR_H


#include "../../MiscLIB/include/errorHandling.h"
#include <string>


/* **************************************************************
 * CLASS dataRootObjectError					 				*
 * **************************************************************/

class dataRootObjectError : public errorHandling {

public:

/**
 * Default constructor
 */

	dataRootObjectError();

/**
 * Default destructor
 */

	virtual ~dataRootObjectError();

};

#endif


#ifndef _CANNOTFINDSTATIONIDERROR_H
#define _CANNOTFINDSTATIONIDERROR_H

/**
 * CLASS cannotFindStationIdError
 */

class cannotFindStationIdError : public dataRootObjectError {

private:

	int stationId;

public:

/**
 * Default constructor
 */

	cannotFindStationIdError();

/**
 * Constructor
 */

	cannotFindStationIdError(int id);

/**
 * Destructor
 */

	virtual ~cannotFindStationIdError();

/**
 * This method displays an error message.
 */

	void errorMsg();

};

#endif


#ifndef _DETECTORISNOTDEFINEDERROR_H
#define _DETECTORISNOTDEFINEDERROR_H

/**
 * CLASS detectorIsNotDefinedError
 */

class detectorIsNotDefinedError : public dataRootObjectError {

public:

/**
 * Default constructor
 */

	detectorIsNotDefinedError();

/**
 * Destructor
 */

	virtual ~detectorIsNotDefinedError();

/**
 * This method displays an error message.
 */

	void errorMsg();

};

#endif


#ifndef _TOOBIGSTATIONORDERNUMBERERROR_H
#define _TOOBIGSTATIONORDERNUMBERERROR_H

/**
 * CLASS tooBigStationOrderNumberError
 */

class tooBigStationOrderNumberError : public dataRootObjectError {

private:

	unsigned int orderNumber;

public:

/**
 * Default constructor
 */

	tooBigStationOrderNumberError();

/**
 * Constructor
 */

	tooBigStationOrderNumberError(unsigned int orderNumber);

/**
 * Destructor
 */

	virtual ~tooBigStationOrderNumberError();

/**
 * This method displays an error message.
 */

	void errorMsg();

};

#endif


#ifndef _TWOTRACKSWITHSAMEIDERROR_H
#define _TWOTRACKSWITHSAMEIDERROR_H


/**
 * CLASS twoTracksWithSameIdError
 */

class twoTracksWithSameIdError : public dataRootObjectError {

private:

	int trackId;

public:

/**
 * Default constructor
 */

	twoTracksWithSameIdError();

/**
 * Constructor
 */

	twoTracksWithSameIdError(int actualTrackId);

/**
 * Destructor
 */

	virtual ~twoTracksWithSameIdError();

/**
 * This method displays an error message.
 */

	void errorMsg();

};

#endif


#ifndef _TOOBIGHITMEMORYINDEXERROR_H
#define _TOOBIGHITMEMORYINDEXERROR_H


/**
 * CLASS tooBigHitMemoryIndexError
 */

class tooBigHitMemoryIndexError : public dataRootObjectError {

public:

/**
 * Default constructor
 */

	tooBigHitMemoryIndexError();

/**
 * Destructor
 */

	virtual ~tooBigHitMemoryIndexError();

/**
 * This method displays an error message.
 */

	void errorMsg();

};

#endif


#ifndef _CANNOTACCESSHISTOGRAMCELLERROR_H
#define _CANNOTACCESSHISTOGRAMCELLERROR_H

/**
 * CLASS cannotAccessHistogramCellError
 */

class cannotAccessHistogramCellError : public dataRootObjectError {

private:

	unsigned short index1;
	unsigned short index2;

public:

/**
 * Default constructor
 */

	cannotAccessHistogramCellError();

/**
 * Constructor
 */

	cannotAccessHistogramCellError(unsigned short index1, unsigned short index2);

/**
 * Destructor
 */

	virtual ~cannotAccessHistogramCellError();

/**
 * This method displays an error message.
 */

	void errorMsg();

};

#endif


#ifndef _CANNOTACCESSHISTOGRAMBORDERERROR_H
#define _CANNOTACCESSHISTOGRAMBORDERERROR_H

/**
 * CLASS cannotAccessHistogramBorderError
 */

class cannotAccessHistogramBorderError : public dataRootObjectError {

private:

	unsigned short index;

public:

/**
 * Default constructor
 */

	cannotAccessHistogramBorderError();

/**
 * Constructor
 */

	cannotAccessHistogramBorderError(unsigned short index);

/**
 * Destructor
 */

	virtual ~cannotAccessHistogramBorderError();

/**
 * This method displays an error message.
 */

	void errorMsg();

};

#endif


#ifndef _TRACKDATAMEMORYISNULLERROR_H
#define _TRACKDATAMEMORYISNULLERROR_H

/**
 * CLASS trackDataMemoryIsNullError
 */

class trackDataMemoryIsNullError : public dataRootObjectError {

public:

/**
 * Default constructor
 */

	trackDataMemoryIsNullError();

/**
 * Destructor
 */

	virtual ~trackDataMemoryIsNullError();

/**
 * This method displays an error message.
 */

	void errorMsg();

};

#endif


#ifndef _CANNOTACCESSMAGNETICFIELDVALUEERROR_H
#define _CANNOTACCESSMAGNETICFIELDVALUEERROR_H

/**
 * CLASS cannotAccessMagneticFieldValueError
 */

class cannotAccessMagneticFieldValueError : public dataRootObjectError {

public:

/**
 * Default constructor
 */

	cannotAccessMagneticFieldValueError();

/**
 * Destructor
 */

	virtual ~cannotAccessMagneticFieldValueError();

/**
 * This method displays an error message.
 */

	void errorMsg();

};

#endif


#ifndef _CANNOTACCESSMAGNETICFIELDERROR_H
#define _CANNOTACCESSMAGNETICFIELDERROR_H

/**
 * CLASS cannotAccessMagneticFieldError
 */

class cannotAccessMagneticFieldError : public dataRootObjectError {

public:

/**
 * Default constructor
 */

	cannotAccessMagneticFieldError();

/**
 * Destructor
 */

	virtual ~cannotAccessMagneticFieldError();

/**
 * This method displays an error message.
 */

	void errorMsg();

};

#endif


#ifndef _CANNOTFINALIZEHITTRACKASSIGNMENTBEFOREINITIALIZINGTRACKSERROR_H
#define _CANNOTFINALIZEHITTRACKASSIGNMENTBEFOREINITIALIZINGTRACKSERROR_H

/**
 * CLASS cannotFinalizeHitTrackAssignmentBeforeInitializingTracksError
 */

class cannotFinalizeHitTrackAssignmentBeforeInitializingTracksError : public dataRootObjectError {

public:

/**
 * Default constructor
 */

	cannotFinalizeHitTrackAssignmentBeforeInitializingTracksError();

/**
 * Destructor
 */

	virtual ~cannotFinalizeHitTrackAssignmentBeforeInitializingTracksError();

/**
 * This method displays an error message.
 */

	void errorMsg();

};

#endif


#ifndef _CANNOTACCESSTRACKPARAMETERERROR_H
#define _CANNOTACCESSTRACKPARAMETERERROR_H

/**
 * CLASS cannotAccessTrackParameterError
 */

class cannotAccessTrackParameterError : public dataRootObjectError {

public:

/**
 * Default constructor
 */

	cannotAccessTrackParameterError();

/**
 * Destructor
 */

	virtual ~cannotAccessTrackParameterError();

/**
 * This method displays an error message.
 */

	void errorMsg();

};

#endif


#ifndef _CANNOTACCESSTRACKINFORMATIONERROR_H
#define _CANNOTACCESSTRACKINFORMATIONERROR_H

/**
 * CLASS cannotAccessTrackInformationError
 */

class cannotAccessTrackInformationError : public dataRootObjectError {

public:

/**
 * Default constructor
 */

	cannotAccessTrackInformationError();

/**
 * Destructor
 */

	virtual ~cannotAccessTrackInformationError();

/**
 * This method displays an error message.
 */

	void errorMsg();

};

#endif


#ifndef _TOOBIGTRACKINDEXERROR_H
#define _TOOBIGTRACKINDEXERROR_H

/**
 * CLASS tooBigTrackIndexError
 */

class tooBigTrackIndexError : public dataRootObjectError {

private:

	unsigned int index;
	unsigned int maximum;

public:

/**
 * Default constructor
 */

	tooBigTrackIndexError();

/**
 * Constructor
 */

	tooBigTrackIndexError(unsigned int index, unsigned int maximum);

/**
 * Destructor
 */

	virtual ~tooBigTrackIndexError();

/**
 * This method displays an error message.
 */

	void errorMsg();

};

#endif
