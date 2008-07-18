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
///     - class for errors occuring while interacting with the luts
///
/// *******************************************************************
///
/// $Author: csteinle $
/// $Date: 2007-05-14 15:47:54 $
/// $Revision: 1.4 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _LUTGENERATORERROR_H
#define _LUTGENERATORERROR_H


#include "../../MiscLIB/include/errorHandling.h"


/* **************************************************************
 * CLASS lutGeneratorError						 				*
 * **************************************************************/

class lutGeneratorError : public errorHandling {

public:

/**
 * Default constructor
 */

	lutGeneratorError();

/**
 * Destructor
 */

	virtual ~lutGeneratorError();

};

#endif


#ifndef _NOHITERROR_H
#define _NOHITERROR_H

/**
 * CLASS noHitError
 */

class noHitError : public lutGeneratorError {

public:

/**
 * Default constructor
 */

	noHitError();

/**
 * Destructor
 */

	virtual ~noHitError();

/**
 * This method displays an error message.
 */

	void errorMsg();

};

#endif


#ifndef _MAGNETICFIELDFACTORCANNOTBEZEROERROR_H
#define _MAGNETICFIELDFACTORCANNOTBEZEROERROR_H

/**
 * CLASS magneticFieldFactorCannotBeZero
 */

class magneticFieldFactorCannotBeZero : public lutGeneratorError {

public:

/**
 * Default constructor
 */

	magneticFieldFactorCannotBeZero();

/**
 * Destructor
 */

	virtual ~magneticFieldFactorCannotBeZero();

/**
 * This method displays an error message.
 */

	void errorMsg();

};

#endif


#ifndef _TOOBIGINDEXFORMEMORYERROR_H
#define _TOOBIGINDEXFORMEMORYERROR_H

/**
 * CLASS tooBigIndexForMemoryError
 */

class tooBigIndexForMemoryError : public lutGeneratorError {

private:

	unsigned long index;
	unsigned long numberOfEntries;

public:

/**
 * Default constructor
 */

	tooBigIndexForMemoryError();

/**
 * Constructor
 */

	tooBigIndexForMemoryError(unsigned long index, unsigned long numberOfEntries);

/**
 * Destructor
 */

	virtual ~tooBigIndexForMemoryError();

/**
 * This method displays an error message.
 */

	void errorMsg();

};

#endif


#ifndef _CANNOTACCESSLUTMEMERROR_H
#define _CANNOTACCESSLUTMEMERROR_H

/**
 * CLASS cannotAccessLutMemError
 */

class cannotAccessLutMemError : public lutGeneratorError {

public:

/**
 * Default constructor
 */

	cannotAccessLutMemError();

/**
 * Destructor
 */

	virtual ~cannotAccessLutMemError();

/**
 * This method displays an error message.
 */

	void errorMsg();

};

#endif


#ifndef _CANNOTGETDEFINITIONOFUNDEFINEDOBJECTERROR_H
#define _CANNOTGETDEFINITIONOFUNDEFINEDOBJECTERROR_H

/**
 * CLASS cannotGetDefinitionOfUndefinedObjectError
 */

class cannotGetDefinitionOfUndefinedObjectError : public lutGeneratorError {

public:

/**
 * Default constructor
 */

	cannotGetDefinitionOfUndefinedObjectError();

/**
 * Destructor
 */

	virtual ~cannotGetDefinitionOfUndefinedObjectError();

/**
 * This method displays an error message.
 */

	void errorMsg();

};

#endif


#ifndef _CANNOTGETBORDEROFUNDEFINEDOBJECTERROR_H
#define _CANNOTGETBORDEROFUNDEFINEDOBJECTERROR_H

/**
 * CLASS cannotGetBorderOfUndefinedObjectError
 */

class cannotGetBorderOfUndefinedObjectError : public lutGeneratorError {

public:

/**
 * Default constructor
 */

	cannotGetBorderOfUndefinedObjectError();

/**
 * Destructor
 */

	virtual ~cannotGetBorderOfUndefinedObjectError();

/**
 * This method displays an error message.
 */

	void errorMsg();

};

#endif


#ifndef _CANNOTCLEARBORDEROFUNDEFINEDOBJECTERROR_H
#define _CANNOTCLEARBORDEROFUNDEFINEDOBJECTERROR_H

/**
 * CLASS cannotClearBorderOfUndefinedObjectError
 */

class cannotClearBorderOfUndefinedObjectError : public lutGeneratorError {

public:

/**
 * Default constructor
 */

	cannotClearBorderOfUndefinedObjectError();

/**
 * Destructor
 */

	virtual ~cannotClearBorderOfUndefinedObjectError();

/**
 * This method displays an error message.
 */

	void errorMsg();

};

#endif


#ifndef _CANNOTGETMAGNETICFIELDOFUNDEFINEDOBJECTERROR_H
#define _CANNOTGETMAGNETICFIELDOFUNDEFINEDOBJECTERROR_H

/**
 * CLASS cannotGetMagneticFieldOfUndefinedObjectError
 */

class cannotGetMagneticFieldOfUndefinedObjectError : public lutGeneratorError {

public:

/**
 * Default constructor
 */

	cannotGetMagneticFieldOfUndefinedObjectError();

/**
 * Destructor
 */

	virtual ~cannotGetMagneticFieldOfUndefinedObjectError();

/**
 * This method displays an error message.
 */

	void errorMsg();

};

#endif


#ifndef _CANNOTGETMAGNETICFIELDFACTOROFUNDEFINEDOBJECTERROR_H
#define _CANNOTGETMAGNETICFIELDFACTOROFUNDEFINEDOBJECTERROR_H

/**
 * CLASS cannotGetMagneticFieldFactorOfUndefinedObjectError
 */

class cannotGetMagneticFieldFactorOfUndefinedObjectError : public lutGeneratorError {

public:

/**
 * Default constructor
 */

	cannotGetMagneticFieldFactorOfUndefinedObjectError();

/**
 * Destructor
 */

	virtual ~cannotGetMagneticFieldFactorOfUndefinedObjectError();

/**
 * This method displays an error message.
 */

	void errorMsg();

};

#endif


#ifndef _CANNOTSETMAGNETICFIELDOFUNDEFINEDOBJECTERROR_H
#define _CANNOTSETMAGNETICFIELDOFUNDEFINEDOBJECTERROR_H

/**
 * CLASS cannotSetMagneticFieldOfUndefinedObjectError
 */

class cannotSetMagneticFieldOfUndefinedObjectError : public lutGeneratorError {

public:

/**
 * Default constructor
 */

	cannotSetMagneticFieldOfUndefinedObjectError();

/**
 * Destructor
 */

	virtual ~cannotSetMagneticFieldOfUndefinedObjectError();

/**
 * This method displays an error message.
 */

	void errorMsg();

};

#endif


#ifndef _CANNOTSETMAGNETICFIELDFACTOROFUNDEFINEDOBJECTERROR_H
#define _CANNOTSETMAGNETICFIELDFACTOROFUNDEFINEDOBJECTERROR_H

/**
 * CLASS cannotSetMagneticFieldFactorOfUndefinedObjectError
 */

class cannotSetMagneticFieldFactorOfUndefinedObjectError : public lutGeneratorError {

public:

/**
 * Default constructor
 */

	cannotSetMagneticFieldFactorOfUndefinedObjectError();

/**
 * Destructor
 */

	virtual ~cannotSetMagneticFieldFactorOfUndefinedObjectError();

/**
 * This method displays an error message.
 */

	void errorMsg();

};

#endif


#ifndef _CANNOTACCESSCORRECTIONSOFUNDEFINEDOBJECTERROR_H
#define _CANNOTACCESSCORRECTIONSOFUNDEFINEDOBJECTERROR_H

/**
 * CLASS cannotAccessCorrectionsOfUndefinedObjectError
 */

class cannotAccessCorrectionsOfUndefinedObjectError : public lutGeneratorError {

public:

/**
 * Default constructor
 */

	cannotAccessCorrectionsOfUndefinedObjectError();

/**
 * Destructor
 */

	virtual ~cannotAccessCorrectionsOfUndefinedObjectError();

/**
 * This method displays an error message.
 */

	void errorMsg();

};

#endif


#ifndef _CANNOTEVALUATEBORDEROFUNDEFINEDOBJECTERROR_H
#define _CANNOTEVALUATEBORDEROFUNDEFINEDOBJECTERROR_H

/**
 * CLASS cannotEvaluateBorderOfUndefinedObjectError
 */

class cannotEvaluateBorderOfUndefinedObjectError : public lutGeneratorError {

public:

/**
 * Default constructor
 */

	cannotEvaluateBorderOfUndefinedObjectError();

/**
 * Destructor
 */

	virtual ~cannotEvaluateBorderOfUndefinedObjectError();

/**
 * This method displays an error message.
 */

	void errorMsg();

};

#endif
