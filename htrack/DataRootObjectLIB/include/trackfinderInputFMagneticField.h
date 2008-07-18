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
///     - consists of the information for the magnetic field
///
/// *******************************************************************
///
/// $Author: csteinle $
/// $Date: 2006/12/28 13:09:04 $
/// $Revision: 1.3 $
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

};

#endif
