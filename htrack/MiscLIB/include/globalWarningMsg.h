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
///     - base class for warnings occuring during access of some globals
///
/// *******************************************************************
///
/// $Author: csteinle $
/// $Date: 2007-05-18 12:26:59 $
/// $Revision: 1.3 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _GLOBALWARNINGMSG_H
#define _GLOBALWARNINGMSG_H


#include "../include/projects.h"
#include <string>


//#define NOGLOBALWARNINGMESSAGE	/**< If this is defined, no message would be written to the standard output. */
#undef NOGLOBALWARNINGMESSAGE


/**
 * CLASS globalWarningMsg
 */

class globalWarningMsg {

public:

/**
 * Default constructor
 */

	globalWarningMsg();

/**
 * Destructor
 */

	virtual ~globalWarningMsg();

/**
 * This method prints a message.
 */

	void printMsg(std::string source, std::string message);

/**
 * This method displays a warning message.
 */

	virtual void warningMsg() = 0;

};

#endif


#ifndef _GSTYLENOTFOUNDWARNINGMSG_H
#define _GSTYLENOTFOUNDWARNINGMSG_H

/**
 * CLASS gStyleNotFoundWarningMsg
 */

class gStyleNotFoundWarningMsg : public globalWarningMsg {

private:

	std::string warningSource;

public:

/**
 * Default constructor
 */

	gStyleNotFoundWarningMsg();

/**
 * Constructor
 */

	gStyleNotFoundWarningMsg(std::string actualWarningSource);

/**
 * Destructor
 */

	virtual ~gStyleNotFoundWarningMsg();

/**
 * This method displays a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _GDIRECTORYNOTFOUNDWARNINGMSG_H
#define _GDIRECTORYNOTFOUNDWARNINGMSG_H

/**
 * CLASS gDirectoryNotFoundWarningMsg
 */

class gDirectoryNotFoundWarningMsg : public globalWarningMsg {

private:

	std::string warningSource;

public:

/**
 * Default constructor
 */

	gDirectoryNotFoundWarningMsg();

/**
 * Constructor
 */

	gDirectoryNotFoundWarningMsg(std::string actualWarningSource);

/**
 * Destructor
 */

	virtual ~gDirectoryNotFoundWarningMsg();

/**
 * This method displays a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _GEOMANAGERNOTFOUNDWARNINGMSG_H
#define _GEOMANAGERNOTFOUNDWARNINGMSG_H

/**
 * CLASS geoManagerNotFoundWarningMsg
 */

class geoManagerNotFoundWarningMsg : public globalWarningMsg {

private:

	std::string warningSource;

public:

/**
 * Default constructor
 */

	geoManagerNotFoundWarningMsg();

/**
 * Constructor
 */

	geoManagerNotFoundWarningMsg(std::string actualWarningSource);

/**
 * Destructor
 */

	virtual ~geoManagerNotFoundWarningMsg();

/**
 * This method displays a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _HOUGHLUTCORRECTIONWARNINGMSG_H
#define _HOUGHLUTCORRECTIONWARNINGMSG_H

/**
 * CLASS houghLutCorrectionWarningMsg
 */

class houghLutCorrectionWarningMsg : public globalWarningMsg {

private:

	std::string   warningSource;
	unsigned long borderCorrections;
	unsigned long borderPosCorrections;	

public:

/**
 * Default constructor
 */

	houghLutCorrectionWarningMsg();

/**
 * Constructor
 */

	houghLutCorrectionWarningMsg(std::string actualWarningSource, unsigned long corrections, unsigned long posCorrections);

/**
 * Destructor
 */

	virtual ~houghLutCorrectionWarningMsg();

/**
 * This method displays a warning message.
 */

	void warningMsg();

};

#endif
