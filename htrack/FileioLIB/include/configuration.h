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
///     - derived from io
///     - base class for accessing a configuration file
///
/// *******************************************************************
///
/// $Author: csteinle $
/// $Date: 2008-08-14 12:34:44 $
/// $Revision: 1.3 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _CONFIGURATION_H
#define _CONFIGURATION_H


#include "fileio.h"


/* **************************************************************
 * CLASS configuration							 				*
 * **************************************************************/

class configuration : public fileio {

protected:

/**
 * This method is to get the default name of the file.
 */

	virtual std::string getDefFileName() = 0;

/**
 * This method must return true if a default file should be
 * written.
 */

	virtual bool getWriteDefaultHeader() = 0;

public:

/**
 * Default constructor
 */

	configuration();

/**
 * Constructor
 * @param name is the name of the file to access
 */

	configuration(int numberOfTchars, char** tchars);
	configuration(std::string name);
	configuration(char* name);

/**
 * Destructor
 */

	virtual ~configuration();

/**
 * This method initializes the object.
 */

	void init();
	void init(int numberOfTchars, char** tchars);
	void init(std::string name);
	void init(char* name);

/**
 * This method sets the name of the file.
 */

	void setFileName(int numberOfTchars, char** tchars);
	void setFileName(std::string name);
	void setFileName(char* name);

/**
 * This method initializes the file.
 */

	void initFile();

/**
 * This method reads the file.
 */

	void readFile();

/**
 * This method writes the file.
 */

	void writeFile();

};

#endif
