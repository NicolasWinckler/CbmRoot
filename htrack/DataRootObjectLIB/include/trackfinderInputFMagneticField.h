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
///     - consists of the information for the magnetic field
///
/// *******************************************************************
///
/// $Author: csteinle $
/// $Date: 2007-10-19 14:33:02 $
/// $Revision: 1.4 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _TRACKFINDERINPUTFMAGNETICFIELD_H
#define _TRACKFINDERINPUTFMAGNETICFIELD_H


#include "trackfinderInputMagneticField.h"


/* **************************************************************
 * CLASS trackfinderInputFMagneticField							*
 * **************************************************************/

class trackfinderInputFMagneticField : public trackfinderInputMagneticField {

protected:

	bool disableAutomaticMagneticField;		/**< Variable to store if the magnetic field should be used directly from the framework or not. */

public:

/**
 * class definition which is needed for TObject from root.
 */

	ClassDef(trackfinderInputFMagneticField, 1);

/**
 * Default constructor
 */

	trackfinderInputFMagneticField();

/**
 * Constructor
 */

	trackfinderInputFMagneticField(const trackfinderInputFMagneticField& value);
	trackfinderInputFMagneticField(const char* fileName, bool isRootFile = true, const char* mapName = "NewMap", unsigned short magneticFieldIntegrationStepwidthPerStation = 10, double magneticFieldIntegrationFactor = 1.0, bool disableAutomaticMagneticField = true);
	trackfinderInputFMagneticField(std::string fileName, bool isRootFile = true, std::string mapName = "NewMap", unsigned short magneticFieldIntegrationStepwidthPerStation = 10, double magneticFieldIntegrationFactor = 1.0, bool disableAutomaticMagneticField = true);
	trackfinderInputFMagneticField(unsigned short numberOfMagnetfieldFactors);
	trackfinderInputFMagneticField(CbmField* field, unsigned short magneticFieldIntegrationStepwidthPerStation = 10, double magneticFieldIntegrationFactor = 1.0, bool disableAutomaticMagneticField = true);
	trackfinderInputFMagneticField(CbmField* field, const char* fileName, bool isRootFile = true, const char* mapName = "NewMap", unsigned short magneticFieldIntegrationStepwidthPerStation = 10, double magneticFieldIntegrationFactor = 1.0, unsigned short numberOfMagnetfieldFactors = 0, bool disableAutomaticMagneticField = true);
	trackfinderInputFMagneticField(CbmField* field, std::string fileName, bool isRootFile = true, std::string mapName = "NewMap", unsigned short magneticFieldIntegrationStepwidthPerStation = 10, double magneticFieldIntegrationFactor = 1.0, unsigned short numberOfMagnetfieldFactors = 0, bool disableAutomaticMagneticField = true);

/**
 * Destructor
 */

	virtual ~trackfinderInputFMagneticField();

/**
 * operator = ()
 */

	const trackfinderInputFMagneticField& operator = (const trackfinderInputFMagneticField& value);

/**
 * method inits the fieldmap.
 */

	void initialize();
	void init(const char* fileName, bool isRootFile = true, const char* mapName = "NewMap", unsigned short magneticFieldIntegrationStepwidthPerStation = 10, double magneticFieldIntegrationFactor = 1.0, unsigned short numberOfMagnetfieldFactors = 0, bool disableAutomaticMagneticField = true);
	void init(std::string fileName, bool isRootFile = true, std::string mapName = "NewMap", unsigned short magneticFieldIntegrationStepwidthPerStation = 10, double magneticFieldIntegrationFactor = 1.0, unsigned short numberOfMagnetfieldFactors = 0, bool disableAutomaticMagneticField = true);
	void init(CbmField* field, unsigned short magneticFieldIntegrationStepwidthPerStation = 10, double magneticFieldIntegrationFactor = 1.0, unsigned short numberOfMagnetfieldFactors = 0, bool disableAutomaticMagneticField = true);
	void init(CbmField* field, const char* fileName, bool isRootFile = true, const char* mapName = "NewMap", unsigned short magneticFieldIntegrationStepwidthPerStation = 10, double magneticFieldIntegrationFactor = 1.0, unsigned short numberOfMagnetfieldFactors = 0, bool disableAutomaticMagneticField = true);
	void init(CbmField* field, std::string fileName, bool isRootFile = true, std::string mapName = "NewMap", unsigned short magneticFieldIntegrationStepwidthPerStation = 10, double magneticFieldIntegrationFactor = 1.0, unsigned short numberOfMagnetfieldFactors = 0, bool disableAutomaticMagneticField = true);

/**
 * This method returns the size of the reserved memory for
 * the source data.
 * @param dimension formats the returnvalue to B, kB, MB or GB
 */

	double getReservedSizeOfData(unsigned short dimension = 0);

/**
 * This method returns the size of the allocated memory for
 * the source data.
 * @param dimension formats the returnvalue to B, kB, MB or GB
 */

	double getAllocatedSizeOfData(unsigned short dimension = 0);

/**
 * This method returns the size of the used memory for
 * the source data.
 * @param dimension formats the returnvalue to B, kB, MB or GB
 */

	double getUsedSizeOfData(unsigned short dimension = 0);

};

#endif
