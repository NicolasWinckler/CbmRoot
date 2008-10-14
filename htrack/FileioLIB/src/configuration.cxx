/* *******************************************************************
// (C)opyright 2004
// 
// Institute of Computer Science V
// Prof. Männer
// University of Mannheim, Germany
// 
// *******************************************************************
// 
// Designer(s):   Steinle
// 
// *******************************************************************
// 
// Project:     Trackfinder for CBM-Project at GSI-Darmstadt, Germany
// 
// *******************************************************************
// 
// Description:
//
//   class:
//     - derived from io
//     - base class for accessing a configuration file
//
// *******************************************************************
//
// $Author: csteinle $
// $Date: 2008-08-14 12:34:44 $
// $Revision: 1.4 $
//
// *******************************************************************/


#include "../include/fileioError.h"
#include "../include/fileioWarningMsg.h"
#include "../include/configuration.h"


/****************************************************************
 * Default constructor											*
 ****************************************************************/

configuration::configuration() : fileio() {

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

configuration::configuration(int numberOfTchars, char** tchars) : fileio() {

	init(numberOfTchars, tchars);

}
configuration::configuration(std::string name) : fileio() {

	init(name);

}
configuration::configuration(char* name) : fileio() {

	init(name);

}

/****************************************************************
 * Default destructor											*
 ****************************************************************/

configuration::~configuration() {

}

/****************************************************************
 * This method initializes the object.							*
 ****************************************************************/

void configuration::init() {
	
	initFile();

}

/****************************************************************
 * This method initializes the object.							*
 * Dependencies:												*
 * - Fileio::setFileName										*
 ****************************************************************/

void configuration::init(int numberOfTchars, char** tchars) {
	
	setFileName(numberOfTchars, tchars);
	init();

}
void configuration::init(std::string name) {
	
	setFileName(name);
	init();

}
void configuration::init(char* name) {
	
	setFileName(name);
	init();

}

/****************************************************************
 * This method sets the name of the file.						*
 * Dependencies:												*
 * - Fileio::setFileName										*
 ****************************************************************/

void configuration::setFileName(int numberOfTchars, char** tchars) {

	std::string temp;

	/* no filename specified, use default */
	if ((numberOfTchars < 1) || (tchars == NULL))
		temp = getDefFileName();
	/* filename specified */
	else {
		temp = tchars[0];
		for (int i = 1; i < numberOfTchars; i++) {
			temp += ' ';
			temp += tchars[i];
		}
	}

	fileio::setFileName(temp);

}
void configuration::setFileName(std::string name) {

	fileio::setFileName(name);

}
void configuration::setFileName(char* name) {

	fileio::setFileName(name);

}

/****************************************************************
 * This method initializes the file.							*
 * Dependencies:												*
 * - Configuration::Subclass::getDefFileName					*
 * - Configuration::Subclass::setHeaderDefValues				*
 * - Subclass::writeFileHeader									*
 * - Fileio::readFileHeader										*
 * Warnings:													*
 * - cannotOpenFileWarningMsg									*
 * - writeDefaultFileWarningMsg									*
 * Errors														*
 * - noFilenameSpecifiedError									*
 * - cannotOpenFileError										*
 * - cannotCloseFileError										*
 ****************************************************************/

void configuration::initFile() {

	std::ifstream readFileStream;
	std::ofstream writeFileStream;
	cannotOpenFileWarningMsg*   cannotOpenFile;
	writeDefaultFileWarningMsg* writeDefaultFile;

	cannotOpenFile   = NULL;
	writeDefaultFile = NULL;
	if (getFileName().empty() == true)
		throw noFilenameSpecifiedError(FILEIOLIB);
	else
		readFileStream.open(getFileName().c_str());
	/* if configuration file does not exist, write one */
	if (readFileStream.fail()) {
		cannotOpenFile = new cannotOpenFileWarningMsg();
		cannotOpenFile->warningMsg();
		if(cannotOpenFile != NULL) {
			delete cannotOpenFile;
			cannotOpenFile = NULL;
		}
		fileio::setFileName(getDefFileName());
		setHeaderDefValues();
		if (getWriteDefaultHeader()) {
			writeDefaultFile =  new writeDefaultFileWarningMsg(getFileName());
			writeDefaultFile->warningMsg();
			if(writeDefaultFile != NULL) {
				delete writeDefaultFile;
				writeDefaultFile = NULL;
			}
			writeFileStream.open(getFileName().c_str());
			if (writeFileStream.fail())
				throw cannotOpenFileError(FILEIOLIB, getFileName());
			else {
				writeFileHeader(writeFileStream);
				writeFileStream.close();
				if (writeFileStream.fail())
					throw cannotCloseFileError(FILEIOLIB, getFileName());
			}
		}
	}
	/* use configuration file */
	else {
		readFileHeader(readFileStream);
		readFileStream.close();
		if (readFileStream.fail())
			throw cannotCloseFileError(FILEIOLIB, getFileName());
	}

}

/****************************************************************
 * This method reads the file.									*
 * Dependencies:												*
 * - Fileio::readFileHeader										*
 * Warnings														*
 * - Fileio::readFileHeader										*
 * Errors														*
 * - noFilenameSpecifiedError									*
 * - cannotOpenFileError										*
 * - cannotCloseFileError										*
 * - Fileio::readFileHeader										*
 ****************************************************************/

void configuration::readFile() {

	std::ifstream readFileStream;

	if (getFileName().empty() == true)
		throw noFilenameSpecifiedError(FILEIOLIB);
	else
		readFileStream.open(getFileName().c_str());
	/* if file does not exist */
	if(readFileStream.fail())
		throw cannotOpenFileError(FILEIOLIB, getFileName());
	/* read file */
	else {
		readFileHeader(readFileStream);
		readFileStream.close();
		if (readFileStream.fail())
			throw cannotCloseFileError(FILEIOLIB, getFileName());
	}

}

/****************************************************************
 * This method writes the file.									*
 ****************************************************************/

void configuration::writeFile() {

	std::ofstream writeFileStream;

	if (getFileName().empty() == true)
		throw noFilenameSpecifiedError(FILEIOLIB);
	else
		writeFileStream.open(getFileName().c_str());
	/* if file cannot be created */
	if(writeFileStream.fail())
		throw cannotCreateFileError(FILEIOLIB, getFileName());
	/* write file */
	else {
		writeFileHeader(writeFileStream);
		writeFileStream.close();
		if (writeFileStream.fail())
			throw cannotCloseFileError(FILEIOLIB, getFileName());
	}
}
