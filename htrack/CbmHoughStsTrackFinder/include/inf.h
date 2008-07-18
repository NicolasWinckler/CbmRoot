//////////////////////////////////////////////////////////////////////
/// (C)opyright 2004
/// 
/// Institute of Computer Science V
/// Prof. M�nner
/// University of Mannheim, Germany
/// 
/// *******************************************************************
/// 
/// Author(s):   Steinle / Gl��
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
///     - reads some parameters from a configuration file
///
/// *******************************************************************
///
/// $Author: csteinle $
/// $Date: 2007-06-06 14:18:59 $
/// $Revision: 1.14 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _INF_H
#define _INF_H


#include "../../FileioLIB/include/configuration.h"
#include "../../MiscLIB/include/bitArray.h"


/**
 * Defines all possible formats of the input file
 */
enum {ASCIIFILEFORMAT, ROOTFILEFORMAT};


/*
 * number of commands in the file
 */
#define numberOfCmds     115		/**< Definition of the number of parameters gettable from file. */


/*
 * struct consisting of all parameters which should be read from file
 */
typedef struct {
	std::string    inputFileName;									/**< Variable to store the filename of the input data file. */
	int            inputFileFormat;									/**< Variable to store the fileformat of the input data file. */
	int            inputMinTracks;									/**< Variable to store the cut for an event if the findable tracks are less. */
	int            inputMaxTracks;									/**< Variable to store the cut for an event if the findable tracks are more. */
	bitArray       inputDetectorMask;								/**< Variable to store the information which detector should be not taken into the computation. */
	std::string    inputFileNameMagneticField;						/**< Variable to store the filename of the file consisting of the magnetic field map. */
	bool           inputMagneticFieldIsRootFile;					/**< Variable to store if the file consisting of the magnetic field map is a root-file. */
	std::string    inputMapNameMagneticField;						/**< Variable to store the name of the magnetic field map in the root-file. */
	unsigned short inputMagneticFieldIntegrationStepwidthPerStation;/**< Variable to store the stepwidth of the points for the integration of the magnetic field. */
	double         inputMagneticFieldIntegrationFactor;				/**< Variable to store the factor for correcting the integration of the magnetic field. */
	bool           inputDisableAutomaticMagneticField;				/**< Variable to store if the magnetic field should be used directly from the framework or not. */
	std::string    inputFileNameDetector;							/**< Variable to store the name of the file which consists of the detector's geometry definition ascii file. */
	int            inputNumberOfVolumesInfrontOfSTS;				/**< Variable to store the number of geometric volumes which are established in front of the STS volumes. Needed to compute the correct volumeID. */
	bool           inputDisableAutomaticDetector;					/**< Variable to store if the detector should be used directly from the framework or not. */
	int            inputCodingTableMode;							/**< Variable to store the mode which then determines the tables used for coding in the program. */
	int            inputGradingPTableMode;							/**< Variable to store the mode which then determines the tables used for gradingP in the program. */
	int            inputGradingRTableMode;							/**< Variable to store the mode which then determines the tables used for gradingR in the program. */
	std::string    inputCodingTableFileName;						/**< Variable to store the name of the file which is used to read the coding table if enabled. */
	std::string    inputGradingPTableFileName;						/**< Variable to store the name of the file which is used to read the gradingP table if enabled. */
	std::string    inputGradingRTableFileName;						/**< Variable to store the name of the file which is used to read the gradingR table if enabled. */
	bool           inputCodingTableWrite;							/**< Variable to store if the coding table should be written into the file. */
	bool           inputGradingPTableWrite;							/**< Variable to store if the gradingP table should be written into the file. */
	bool           inputGradingRTableWrite;							/**< Variable to store if the gradingR table should be written into the file. */
	int            trackfinderHitProducer;							/**< Variable to store the way to get hits from points. */
	bool           trackfinderReadPointsFromFile;					/**< Variable to store, if the points should be read out of a file. */
	bool           trackfinderReadHitsFromFile;						/**< Variable to store, if the hits should be read out of a file. */
	bool           trackfinderReadMapsHits;							/**< Variable to store, if the hits from maps should be accessed. */
	bool           trackfinderReadHybridHits;						/**< Variable to store, if the hits from hybrid should be accessed. */
	bool           trackfinderReadStripHits;						/**< Variable to store, if the hits from strip should be accessed. */
	unsigned short trackfinderLutsVersion;							/**< Variable to store the version of both used look-up-tables. */
	std::string    trackfinderPrelutFileName;						/**< Variable to store the name of the file which is used to access the prelut. */
	std::string    trackfinderLutFileName;							/**< Variable to store the name of the file which is used to access the lut. */
	double         trackfinderGammaMin;								/**< Variable to store the minimal value of the first dimension of prelut */
	double         trackfinderGammaMax;								/**< Variable to store the maximal value of the first dimension of prelut */
	int            trackfinderGammaStep;							/**< Variable to store the stepwidth of the first dimension of prelut to build a grid */
	double         trackfinderThetaMin;								/**< Variable to store the minimal value of the first dimension of lut */
	double         trackfinderThetaMax;								/**< Variable to store the maximal value of the first dimension of lut */
	int            trackfinderThetaStep;							/**< Variable to store the stepwidth of the first dimension of lut to build a grid */
	double         trackfinderPrelutRadiusMin;						/**< Variable to store the minimal value of the second dimension of prelut */
	double         trackfinderPrelutRadiusMax;						/**< Variable to store the maximal value of the second dimension of prelut */
	double         trackfinderLutRadiusMin;							/**< Variable to store the minimal value of the second dimension of lut */
	double         trackfinderLutRadiusMax;							/**< Variable to store the maximal value of the second dimension of lut */
	int            trackfinderLutRadiusStep;						/**< Variable to store the stepwidth of the second dimension of lut to build a grid */
	unsigned short trackfinderMinClassCoding;						/**< Variable to store the minimal classification in the coding table to be taken into account */
	unsigned short trackfinderMinClassGradingP;						/**< Variable to store the minimal classification in the gradingP table to be taken into account */
	unsigned short trackfinderMinClassGradingR;						/**< Variable to store the minimal classification in the gradingR table to be taken into account */
	unsigned short trackfinderFirstFilterNeighborhoodDim1ClearRadius;	/**< Variable to store the region of interest for the first filter in the first dimension */
	unsigned short trackfinderFirstFilterNeighborhoodDim2ClearRadius;	/**< Variable to store the region of interest for the first filter in the second dimension */
	unsigned short trackfinderSecondFilterNeighborhoodDim1ClearRadius;	/**< Variable to store the region of interest for the second filter in the first dimension */
	unsigned short trackfinderSecondFilterNeighborhoodDim2ClearRadius;	/**< Variable to store the region of interest for the second filter in the second dimension */
	unsigned short trackfinderSecondFilterNeighborhoodDim3ClearRadius;	/**< Variable to store the region of interest for the second filter in the third dimension */
	bool           trackfinderWriteTracksToFile;					/**< Variable to store if the computed tracks should be written directly to the file or not. */
	std::string    analysisOutputFileName;							/**< Variable to store the name of the file to place the graphical output */
	double         analysisThresholdForP;							/**< Variable to store the minimal momentum of tracks to be taken into account */
	bool           analysisInitConfiguration;						/**< Variable to store, if a summary about the configuration should be printed to the standard output stream. */
	bool           analysisInitDetector;							/**< Variable to store, if a summary about the detector should be printed to the standard output stream. */
	bool           analysisInitMemory;								/**< Variable to store, if a summary about the needed memory should be printed to the standard output stream. */
	bool           analysisInitEvent;								/**< Variable to store, if a summary about the event should be printed to the standard output stream. */
	bool           analysisInitClassPriority;						/**< Variable to store, if a summary about the class priorities should be printed to the standard output stream. */
	bool           analysisInitQualityEFGCEventAbsolute;			/**< Variable to store, if a summary about the efficiency, the fakes, the ghosts and the clones with absolute numbers for each event should be printed to the standard output stream. */
	bool           analysisInitQualityEFGCEventRelative;			/**< Variable to store, if a summary about the efficiency, the fakes, the ghosts and the clones with percentage numbers for each event should be printed to the standard output stream. */
	bool           analysisInitQualityEFGCTotalAbsolute;			/**< Variable to store, if a summary about the efficiency, the fakes, the ghosts and the clones with accumulated absolute numbers for all events should be printed to the standard output stream. */
	bool           analysisInitQualityEFGCTotalRelative;			/**< Variable to store, if a summary about the efficiency, the fakes, the ghosts and the clones with accumulated percentage numbers for all events should be printed to the standard output stream. */
	bool           analysisInitMomentumEFGCEventPzEFGC;				/**< Variable to store, if a graphical summary about the efficiency, the fakes, the ghosts and the clones according to the momentum should be made for each event. */
	bool           analysisInitMomentumEFGCEventPtEFGC;				/**< Variable to store, if a graphical summary about the efficiency, the fakes, the ghosts and the clones according to the transversale impulse should be made for each event. */
	bool           analysisInitMomentumEFGCTotalPzEFGC;				/**< Variable to store, if a graphical summary about the efficiency, the fakes, the ghosts and the clones according to the momentum should be made accumulated for all events. */
	bool           analysisInitMomentumEFGCTotalPtEFGC;				/**< Variable to store, if a graphical summary about the efficiency, the fakes, the ghosts and the clones according to the transversale impulse should be made accumulated for all events. */
	bool           analysisInitProjectionEFGCNEvent12EFGCN;			/**< Variable to store, if a graphical histogram occupancy summary about the efficiency, the fakes, the ghosts, the clones and the not-found tracks should be made for each event. */
	bool           analysisInitProjectionEFGCNEvent13EFGCN;			/**< Variable to store, if a graphical histogram occupancy summary about the efficiency, the fakes, the ghosts, the clones and the not-found tracks should be made for each event. */
	bool           analysisInitProjectionEFGCNEvent32EFGCN;			/**< Variable to store, if a graphical histogram occupancy summary about the efficiency, the fakes, the ghosts, the clones and the not-found tracks should be made for each event. */
	bool           analysisInitProjectionEFGCNTotal12EFGCN;			/**< Variable to store, if a graphical histogram occupancy summary about the efficiency, the fakes, the ghosts, the clones and the not-found tracks should be made accumulated for all events. */
	bool           analysisInitProjectionEFGCNTotal13EFGCN;			/**< Variable to store, if a graphical histogram occupancy summary about the efficiency, the fakes, the ghosts, the clones and the not-found tracks should be made accumulated for all events. */
	bool           analysisInitProjectionEFGCNTotal32EFGCN;			/**< Variable to store, if a graphical histogram occupancy summary about the efficiency, the fakes, the ghosts, the clones and the not-found tracks should be made accumulated for all events. */
	bool           analysisInitMomentumEvent;						/**< Variable to store the enabling of the graphical analysis for each event */
	bool           analysisInitMomentumTotal;						/**< Variable to store the enabling of the graphical analysis accumulated for all events */
	bool           analysisInitMomentumDisplay;						/**< Variable to store the enabling of the online graphic display */
	bool           analysisInitMomentumToRoot;						/**< Variable to store the enabling of the graphics writing to file */
	bool           analysisInitProjectionEvent;						/**< Variable to store the enabling of the graphical analysis for each event */
	bool           analysisInitProjectionTotal;						/**< Variable to store the enabling of the graphical analysis accumulated for all events */
	bool           analysisInitProjectionDisplay;					/**< Variable to store the enabling of the online graphic display */
	bool           analysisInitProjectionToRoot;					/**< Variable to store the enabling of the graphics writing to file */
	bool           analysisInitMagnetfieldX;						/**< Variable to store the enabling of the magnetfield analysis of the x dimension */
	bool           analysisInitMagnetfieldY;						/**< Variable to store the enabling of the magnetfield analysis of the y dimension */
	bool           analysisInitMagnetfieldZ;						/**< Variable to store the enabling of the magnetfield analysis of the z dimension */
	bool           analysisInitMagnetfieldDisplay;					/**< Variable to store the enabling of the online magnetfield display */
	bool           analysisInitMagnetfieldToRoot;					/**< Variable to store the enabling of the magnetfields' writing to file */
	bool           analysisInitMagnetfieldConstantForEachEvent;		/**< Variable to store the enabling of the magnetfield constant analysis for each event */
	bool           analysisInitWeightedMagnetfieldConstant;			/**< Variable to store the enabling of the weights for the magnetfield constant analysis */
	bool           analysisInitMagnetfieldConstantDisplay;			/**< Variable to store the enabling of the online magnetfield constant display */
	bitArray       analysisMagnetfieldConstantDisplayMask;			/**< Variable to store the information which magnetfield constant should not be drawn onto screen, if this feature is enabled. */
	bool           analysisInitMagnetfieldConstantToRoot;			/**< Variable to store the enabling of the magnetfields' constant writing to file */
	bool           analysisInitMagnetfieldVSConstants;				/**< Variable to store the enabling of the comparisson between the magnetfield and the constants */
	bool           analysisInitPrelutGoodness;						/**< Variable to store the enabling of the prelut analysis */
	bool           analysisInitLutGoodness;							/**< Variable to store the enabling of the lut analysis */
	bool           analysisInitHoughTransformGoodness;				/**< Variable to store the enabling of the Hough transform analysis */
	bool           analysisInitQuantizationGoodness;				/**< Variable to store the enabling of the quantization analysis */
	bool           analysisInitCreatedHistogramToRoot;				/**< Variable to store the enabling of the created histogram writing to file */
	bool           analysisInitEncodedHistogramToRoot;				/**< Variable to store the enabling of the encoded histogram writing to file */
	bool           analysisInitFilteredHistogramToRoot;				/**< Variable to store the enabling of the filtered histogram writing to file */
	bool           analysisInitJustOneCreatedHistogramToRoot;		/**< Variable to store the enabling of just one layer of the created histogram is accessible */
	bool           analysisInitJustOneEncodedHistogramToRoot;		/**< Variable to store the enabling of just one layer of encoded histogram is accessible */
	bool           analysisInitJustOneFilteredHistogramToRoot;		/**< Variable to store the enabling of just one layer of filtered histogram is accessible */
	bool           analysisInitCreatedHistogramToShow;				/**< Variable to store the enabling to show one layer of the created histogram */
	bool           analysisInitEncodedHistogramToShow;				/**< Variable to store the enabling to show one layer of the encoded histogram */
	bool           analysisInitFilteredHistogramToShow;				/**< Variable to store the enabling to show one layer of the filtered histogram */
	unsigned short analysisInitHistogramLayer;						/**< Variable to store the layer of the histogram which should be accessible */
	bool           analysisInitNumberOfTracksPerColumn;				/**< Variable to store the enabling of the evaluation of the number of tracks per column in all histogram layers  */
	bool           analysisInitNumberOfTracksPerLayer;				/**< Variable to store the enabling of the evaluation of the number of tracks per histogram layer  */
	bool           analysisInitTotalAnalysis;						/**< Variable to store the enabling of the total analysis. */
	bool           analysisInitTotalAnalysisDisplay;				/**< Variable to store, if the total analysis arrays should be displayed. */
	unsigned short analysisInitPercentageOfHitsInSignature;			/**< Variable to store the precentage of the number of hits which must be found to accept the signature. */
	unsigned short analysisInitPercentageOfTracksForSignature;		/**< Variable to store the percentage of the real tracks which must be found with the accepted signatures. */
	unsigned long  analysisInitAnalysisResultWarnings;				/**< Variable to store which result of an analysis should be printed on the standard output screen by a warning message. */
	unsigned long  analysisInitAnalysisResultDisplays;				/**< Variable to store which result of an analysis should be displayed with a graphical output screen. */
	bool           initStatus;										/**< Variable to store the enabling of the printing of the process information to the standard output stream */
} InfData;


/****************************************************************
 * CLASS inf									 				*
 ****************************************************************/

class inf : public configuration {

private:

	bool    commandID[numberOfCmds];	/**< Variable to review which parameter comes from the file. */
	InfData config;						/**< Struct to store all parameters. */

/**
 * This method assigns the value to the corresponding parameter
 * which is identified by a specifier.
 */

	bool getInputHeaderValue(std::string& specifier, std::string& value);

/**
 * This method assigns the value to the corresponding parameter
 * which is identified by a specifier.
 */

	bool getInitialisationHeaderValue(std::string& specifier, std::string& value);

/**
 * This method assigns the value to the corresponding parameter
 * which is identified by a specifier.
 */

	bool getTrackfinderHeaderValue(std::string& specifier, std::string& value);

/**
 * This method assigns the value to the corresponding parameter
 * which is identified by a specifier.
 */

	bool getQualityHeaderValue(std::string& specifier, std::string& value);

/**
 * This method assigns the value to the corresponding parameter
 * which is identified by a specifier.
 */

	bool getMomentumHeaderValue(std::string& specifier, std::string& value);

/**
 * This method assigns the value to the corresponding parameter
 * which is identified by a specifier.
 */

	bool getProjectionHeaderValue(std::string& specifier, std::string& value);

/**
 * This method assigns the value to the corresponding parameter
 * which is identified by a specifier.
 */

	bool getMagnetfieldHeaderValue(std::string& specifier, std::string& value);

/**
 * This method assigns the value to the corresponding parameter
 * which is identified by a specifier.
 */

	bool getGoodnessHeaderValue(std::string& specifier, std::string& value);

/**
 * This method assigns the value to the corresponding parameter
 * which is identified by a specifier.
 */

	bool getHistogramHeaderValue(std::string& specifier, std::string& value);

/**
 * This method assigns the value to the corresponding parameter
 * which is identified by a specifier.
 */

	bool getHardwareHeaderValue(std::string& specifier, std::string& value);

/**
 * This method assigns the value to the corresponding parameter
 * which is identified by a specifier.
 */

	bool getTotalAnalysisHeaderValue(std::string& specifier, std::string& value);

protected:

/**
 * This method returns the number of accepted commands.
 */

	int getNumberOfCmds();

/**
 * This method is to get the default name of the file.
 */

	std::string getDefFileName();

/**
 * This method must return true if a default file should be
 * written.
 */

	bool getWriteDefaultHeader();

/**
 * This method is to set the default values for each header
 * parameter.
 */

	void setHeaderDefValues();

/**
 * This method assigns the value to the corresponding parameter
 * which is identified by a specifier.
 */

	bool getHeaderValue(std::string& specifier, std::string& value);

/**
 * This method is to write the header of the file.
 */

	void writeFileHeader(std::ofstream& fileStream);

public:

/**
 * Default constructor
 */

	inf();

/**
 * Default constructor
 */

	inf(InfData& setup);
	inf(int numberOfTchars, char** tchars);
	inf(std::string name);
	inf(char* name);

/**
 * Destructor
 */
	
	virtual ~inf();

/**
 * This method returns a reference of the data struct.
 */

	InfData& getDataReference();

/**
 * This method returns an information string about the object.
 */

	std::string getInfo();

};

#endif
