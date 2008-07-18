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
///     - read and write access to a file
///
/// *******************************************************************
///
/// $Author: csteinle $
/// $Date: 2006/11/08 12:37:30 $
/// $Revision: 1.3 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _LUTACCESSFILE_H
#define _LUTACCESSFILE_H


#include "../../FileioLIB/include/io.h"


#define numberOfLutAccessFileCmds 9			/**< Definition of the number of parameters gettable from file. */


enum {SOFTWAREFORMAT, HARDWAREFORMAT};


/*
 * struct consisting of all parameters which should be read from file
 */
typedef struct {
	std::string    name;					/**< Variable to store the name of the lut */
	unsigned short format;					/**< Variable to store the format of the file */
	unsigned long  numberOfEntries;			/**< Variable to store the number of entries of the lut */
	double         dim1Min;					/**< Variable to store the minimal value for the first dimension to compute the prelut value. */
	double         dim1Max;					/**< Variable to store the maximal value for the first dimension to compute the prelut value. */
	int            dim1Step;				/**< Variable to store the stepwidth of the first dimension. */
	double         dim2Min;					/**< Variable to store the minimal value for the second dimension to compute the prelut value. */
	double         dim2Max;					/**< Variable to store the maximal value for the second dimension to compute the prelut value. */
	int            dim2Step;				/**< Variable to store the stepwidth of the second dimension. */
} lutAccessFileHeader;


/* **************************************************************
 * CLASS lutFileAccess							 				*
 * **************************************************************/

class lutAccessFile : public io {

private:

	bool                commandID[numberOfLutAccessFileCmds];		/**< Variable to review which parameter comes from the file. */
	lutAccessFileHeader header;										/**< Struct to store all parameters. */
	bool                localMemory;								/**< Variable to store if the memory to read the file is local memory or not. */

protected:

/**
 * This method deletes the local memory if it exists.
 */

	void deleteLocalMemory();

/**
 * This method returns the number of accepted commands.
 */

	int getNumberOfCmds();

/**
 * This method assigns the value to the corresponding parameter
 * which is identified by a specifier.
 */

	bool getHeaderValue(std::string& specifier, std::string& value);

/**
 * This method sets the number of the data structures
 * to the value that all data is read from file. Normally
 * this value should be in the header of the file as one
 * command.
 */

	void readAllData();

/**
 * This method assigns the data to the corresponding structure
 * The whole data structure to read must be in one single line
 * in the file.
 */

	bool getDataValue(std::string& buffer, unsigned long index);

/**
 * This method writes one data structure to the stream.
 * The whole data structure to write must be in one single line
 * in the file.
 */

	std::string setDataValue(unsigned long index);

/**
 * This method is to write the header of the file.
 */

	void writeFileHeader(std::ofstream& fileStream);

/**
 * This method is to set the default values for each header
 * parameter.
 */

	void setHeaderDefValues();

public:

/**
 * Default constructor
 */
	
	lutAccessFile();

/**
 * Destructor
 */

	virtual ~lutAccessFile();

/**
 * This method initializes the object.
 */

	void init();

/**
 * This method returns a reference of the data struct.
 */

	lutAccessFileHeader& getHeaderReference();

/**
 * This method sets the data struct	to the existing one.
 */

	void setHeader(lutAccessFileHeader& structure);

/**
 * This method returns an information string about the object.
 */

	std::string getInfo();

};

#endif
