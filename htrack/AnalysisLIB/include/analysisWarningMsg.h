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
///     - base class for warnings occuring during analysis
///
/// *******************************************************************
///
/// $Author: csteinle $
/// $Date: 2007-06-06 14:18:40 $
/// $Revision: 1.10 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _ANALYSISWARNINGMSG_H
#define _ANALYSISWARNINGMSG_H


#include "../../DataObjectLIB/include/histogramSpace.h"
#include "../../DataObjectLIB/include/trackCoordinates.h"
#include "../../DataRootObjectLIB/include/trackfinderInputTrack.h"
#include <string>


//#define NOANALYSISWARNINGMESSAGE	/**< If this is defined, no message would be written to the standard output. */
#undef NOANALYSISWARNINGMESSAGE


/* **************************************************************
 * CLASS analysisWarningMsg						 				*
 * **************************************************************/

class analysisWarningMsg {

public:

/**
 * Default constructor
 */

	analysisWarningMsg();

/**
 * Destructor
 */

	virtual ~analysisWarningMsg();

/**
 * This method prints a message.
 */

	void printMsg(std::string message);

/**
 * This method displays a warning message.
 */

	virtual void warningMsg() = 0;

};

#endif


#ifndef _TRACKNOTFOUNDWARNINGMSG_H
#define _TRACKNOTFOUNDWARNINGMSG_H

/**
 * CLASS trackNotFoundWarningMsg
 */

class trackNotFoundWarningMsg : public analysisWarningMsg {

private:

	trackfinderInputTrack* track;

public:

/**
 * Default constructor
 */

	trackNotFoundWarningMsg();

/**
 * Constructor
 */

	trackNotFoundWarningMsg(trackfinderInputTrack* actualTrack);

/**
 * Destructor
 */

	virtual ~trackNotFoundWarningMsg();

/**
 * This method displays a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _DISPLAYANALYSERNOTFOUNDWARNINGMSG_H
#define _DISPLAYANALYSERNOTFOUNDWARNINGMSG_H

/**
 * CLASS displayAnalyserNotFoundWarningMsg
 */

class displayAnalyserNotFoundWarningMsg : public analysisWarningMsg {

public:

/**
 * Default constructor
 */

	displayAnalyserNotFoundWarningMsg();

/**
 * Destructor
 */

	virtual ~displayAnalyserNotFoundWarningMsg();

/**
 * This method displays a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _DISPLAYCANNOTBEWRITTENWARNINGMSG_H
#define _DISPLAYCANNOTBEWRITTENWARNINGMSG_H

/**
 * CLASS displayCannotBeWrittenToFileWarningMsg
 */

class displayCannotBeWrittenToFileWarningMsg : public analysisWarningMsg {

public:

/**
 * Default constructor
 */

	displayCannotBeWrittenToFileWarningMsg();

/**
 * Destructor
 */

	virtual ~displayCannotBeWrittenToFileWarningMsg();

/**
 * This method displays a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _DISPLAYNOTFOUNDWARNINGMSG_H
#define _DISPLAYNOTFOUNDWARNINGMSG_H

/**
 * CLASS displayNotFoundWarningMsg
 */

class displayNotFoundWarningMsg : public analysisWarningMsg {

public:

/**
 * Default constructor
 */

	displayNotFoundWarningMsg();

/**
 * Destructor
 */

	virtual ~displayNotFoundWarningMsg();

/**
 * This method display a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _CANNOTCREATESIGNATUREWARNINGMSG_H
#define _CANNOTCREATESIGNATUREWARNINGMSG_H

/**
 * CLASS cannotCreateSignatureWarningMsg
 */

class cannotCreateSignatureWarningMsg : public analysisWarningMsg {

public:

/**
 * Default constructor
 */

	cannotCreateSignatureWarningMsg();

/**
 * Destructor
 */

	virtual ~cannotCreateSignatureWarningMsg();

/**
 * This method display a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _CANNOTDRAWWARNINGMSG_H
#define _CANNOTDRAWWARNINGMSG_H

/**
 * CLASS cannotDrawWarningMsg
 */

class cannotDrawWarningMsg : public analysisWarningMsg {

public:

/**
 * Default constructor
 */

	cannotDrawWarningMsg();

/**
 * Destructor
 */

	virtual ~cannotDrawWarningMsg();

/**
 * This method display a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _CANNOTWRITEWARNINGMSG_H
#define _CANNOTWRITEWARNINGMSG_H

/**
 * CLASS cannotWriteWarningMsg
 */

class cannotWriteWarningMsg : public analysisWarningMsg {

public:

/**
 * Default constructor
 */

	cannotWriteWarningMsg();

/**
 * Destructor
 */

	virtual ~cannotWriteWarningMsg();

/**
 * This method display a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _NOTALLDISPLAYSCANBEDRAWNWARNINGMSG_H
#define _NOTALLDISPLAYSCANBEDRAWNWARNINGMSG_H

/**
 * CLASS notAllDisplaysCanBeDrawnWarningMsg
 */

class notAllDisplaysCanBeDrawnWarningMsg : public analysisWarningMsg {

public:

/**
 * Default constructor
 */

	notAllDisplaysCanBeDrawnWarningMsg();

/**
 * Destructor
 */

	virtual ~notAllDisplaysCanBeDrawnWarningMsg();

/**
 * This method display a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _CANNOTACCESSPARAMETERWARNINGMSG_H
#define _CANNOTACCESSPARAMETERWARNINGMSG_H

/**
 * CLASS cannotAccessParameterWarningMsg
 */

class cannotAccessParameterWarningMsg : public analysisWarningMsg {

public:

/**
 * Default constructor
 */

	cannotAccessParameterWarningMsg();

/**
 * Destructor
 */

	virtual ~cannotAccessParameterWarningMsg();

/**
 * This method display a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _CANNOTACCESSTRACKWARNINGMSG_H
#define _CANNOTACCESSTRACKWARNINGMSG_H

/**
 * CLASS cannotAccessTrackWarningMsg
 */

class cannotAccessTrackWarningMsg : public analysisWarningMsg {

public:

/**
 * Default constructor
 */

	cannotAccessTrackWarningMsg();

/**
 * Destructor
 */

	virtual ~cannotAccessTrackWarningMsg();

/**
 * This method display a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _CANNOTGETDISPLAYENTRIESWARNINGMSG_H
#define _CANNOTGETDISPLAYENTRIESWARNINGMSG_H

/**
 * CLASS cannotGetDisplayEntriesWarningMsg
 */

class cannotGetDisplayEntriesWarningMsg : public analysisWarningMsg {

public:

/**
 * Default constructor
 */

	cannotGetDisplayEntriesWarningMsg();

/**
 * Destructor
 */

	virtual ~cannotGetDisplayEntriesWarningMsg();

/**
 * This method display a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _NOHITINDETECTORSTATIONWARNINGMSG_H
#define _NOHITINDETECTORSTATIONWARNINGMSG_H

/**
 * CLASS noHitInDetectorStationWarningMsg
 */

class noHitInDetectorStationWarningMsg : public analysisWarningMsg {

private:

	unsigned short index;

public:

/**
 * Default constructor
 */

	noHitInDetectorStationWarningMsg();

/**
 * Constructor
 */

	noHitInDetectorStationWarningMsg(unsigned short actualIndex);

/**
 * Destructor
 */

	virtual ~noHitInDetectorStationWarningMsg();

/**
 * This method display a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _NOANALYSISMOMENTUMOUTPUTFILENAMESPECIFIEDWARNINGMSG_H
#define _NOANALYSISMOMENTUMOUTPUTFILENAMESPECIFIEDWARNINGMSG_H

/**
 * CLASS noAnalysisMomentumOutputFileNameSpecifiedWarningMsg
 */

class noAnalysisMomentumOutputFileNameSpecifiedWarningMsg : public analysisWarningMsg {

public:

/**
 * Default constructor
 */

	noAnalysisMomentumOutputFileNameSpecifiedWarningMsg();

/**
 * Destructor
 */

	virtual ~noAnalysisMomentumOutputFileNameSpecifiedWarningMsg();

/**
 * This method displays a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _NOANALYSISPROJECTIONOUTPUTFILENAMESPECIFIEDWARNINGMSG_H
#define _NOANALYSISPROJECTIONOUTPUTFILENAMESPECIFIEDWARNINGMSG_H

/**
 * CLASS noAnalysisProjectionOutputFileNameSpecifiedWarningMsg
 */

class noAnalysisProjectionOutputFileNameSpecifiedWarningMsg : public analysisWarningMsg {

public:

/**
 * Default constructor
 */

	noAnalysisProjectionOutputFileNameSpecifiedWarningMsg();

/**
 * Destructor
 */

	virtual ~noAnalysisProjectionOutputFileNameSpecifiedWarningMsg();

/**
 * This method displays a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _ENABLEJUSTONEDISPLAYWARNINGMSG_H
#define _ENABLEJUSTONEDISPLAYWARNINGMSG_H

/**
 * CLASS enableJustOneDisplayWarningMsg
 */

class enableJustOneDisplayWarningMsg : public analysisWarningMsg {

public:

/**
 * Default constructor
 */

	enableJustOneDisplayWarningMsg();

/**
 * Destructor
 */

	virtual ~enableJustOneDisplayWarningMsg();

/**
 * This method displays a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _CANNOTSETPOINTWARNINGMSG_H
#define _CANNOTSETPOINTWARNINGMSG_H

/**
 * CLASS cannotSetPointWarningMsg
 */

class cannotSetPointWarningMsg : public analysisWarningMsg {

private:

	int index;

public:

/**
 * Default constructor
 */

	cannotSetPointWarningMsg();

/**
 * Constructor
 */

	cannotSetPointWarningMsg(int index);

/**
 * Destructor
 */

	virtual ~cannotSetPointWarningMsg();

/**
 * This method displays a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _POSITIONISOUTOFSPACEWARNINGMSG_H
#define _POSITIONISOUTOFSPACEWARNINGMSG_H

/**
 * CLASS positionIsOutOfSpaceWarningMsg
 */

class positionIsOutOfSpaceWarningMsg : public analysisWarningMsg {

private:

	histogramSpace*   space;
	trackCoordinates* coordinates;

public:

/**
 * Default constructor
 */

	positionIsOutOfSpaceWarningMsg();

/**
 * Constructor
 */

	positionIsOutOfSpaceWarningMsg(histogramSpace* space, trackCoordinates* coordinates);

/**
 * Destructor
 */

	virtual ~positionIsOutOfSpaceWarningMsg();

/**
 * This method displays a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _NUMBEROFWRONGTHIRDDIMENSIONBORDERSFOUNDWARNINGMSG_H
#define _NUMBEROFWRONGTHIRDDIMENSIONBORDERSFOUNDWARNINGMSG_H

/**
 * CLASS numberOfWrongThirdDimensionBordersFoundWarningMsg
 */

class numberOfWrongThirdDimensionBordersFoundWarningMsg : public analysisWarningMsg {

private:

	int number;

public:

/**
 * Default constructor
 */

	numberOfWrongThirdDimensionBordersFoundWarningMsg();

/**
 * Constructor
 */

	numberOfWrongThirdDimensionBordersFoundWarningMsg(int number);

/**
 * Destructor
 */

	virtual ~numberOfWrongThirdDimensionBordersFoundWarningMsg();

/**
 * This method displays a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _SETMAGNETFIELDFACTORWARNINGMSG_H
#define _SETMAGNETFIELDFACTORWARNINGMSG_H

/**
 * CLASS setMagnetfieldFactorWarningMsg
 */

class setMagnetfieldFactorWarningMsg : public analysisWarningMsg {

private:

	unsigned short stationIndex;
	double         magnetfieldFactor;

public:

/**
 * Default constructor
 */

	setMagnetfieldFactorWarningMsg();

/**
 * Constructor
 */

	setMagnetfieldFactorWarningMsg(unsigned short stationIndex, double magnetfieldFactor);

/**
 * Destructor
 */

	virtual ~setMagnetfieldFactorWarningMsg();

/**
 * This method displays a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _ANALYSISLUTCORRECTIONWARNINGMSG_H
#define _ANALYSISLUTCORRECTIONWARNINGMSG_H

/**
 * CLASS analysisLutCorrectionWarningMsg
 */

class analysisLutCorrectionWarningMsg : public analysisWarningMsg {

private:

	unsigned int borderCorrections;
	unsigned int borderPosCorrections;	

public:

/**
 * Default constructor
 */

	analysisLutCorrectionWarningMsg();

/**
 * Constructor
 */

	analysisLutCorrectionWarningMsg(unsigned long corrections, unsigned long posCorrections);

/**
 * Destructor
 */

	virtual ~analysisLutCorrectionWarningMsg();

/**
 * This method displays a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _ROOTHISTOGRAMNOTFOUNDWARNINGMSG_H
#define _ROOTHISTOGRAMNOTFOUNDWARNINGMSG_H

/**
 * CLASS rootHistogramNotFoundWarningMsg
 */

class rootHistogramNotFoundWarningMsg : public analysisWarningMsg {

public:

/**
 * Default constructor
 */

	rootHistogramNotFoundWarningMsg();

/**
 * Destructor
 */

	virtual ~rootHistogramNotFoundWarningMsg();

/**
 * This method display a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _CANNOTUSETOROOTBEFOREINITIALIZINGWARNINGMSG_H
#define _CANNOTUSETOROOTBEFOREINITIALIZINGWARNINGMSG_H

/**
 * CLASS cannotUseToRootBeforeInitializingWarningMsg
 */

class cannotUseToRootBeforeInitializingWarningMsg : public analysisWarningMsg {

public:

/**
 * Default constructor
 */

	cannotUseToRootBeforeInitializingWarningMsg();

/**
 * Destructor
 */

	virtual ~cannotUseToRootBeforeInitializingWarningMsg();

/**
 * This method display a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _TRACKWITHHITSINCORRECTLAYERDISTRIBUTIONWARNINGMSG_H
#define _TRACKWITHHITSINCORRECTLAYERDISTRIBUTIONWARNINGMSG_H

/**
 * CLASS trackWithHitsInCorrectLayerDistributionWarningMsg
 */

class trackWithHitsInCorrectLayerDistributionWarningMsg : public analysisWarningMsg {

private:

	unsigned int*  distribution;
	unsigned short numberOfEntries;

public:

/**
 * Default constructor
 */

	trackWithHitsInCorrectLayerDistributionWarningMsg();

/**
 * Constructor
 */

	trackWithHitsInCorrectLayerDistributionWarningMsg(unsigned int* distribution, unsigned short numberOfEntries);

/**
 * Destructor
 */

	virtual ~trackWithHitsInCorrectLayerDistributionWarningMsg();

/**
 * This method display a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _TRACKWITHHITSWITHCORRECTSLOPEDISTRIBUTIONWARNINGMSG_H
#define _TRACKWITHHITSWITHCORRECTSLOPEDISTRIBUTIONWARNINGMSG_H

/**
 * CLASS trackWithHitsWithCorrectSlopeDistributionWarningMsg
 */

class trackWithHitsWithCorrectSlopeDistributionWarningMsg : public analysisWarningMsg {

private:

	unsigned int*  distribution;
	unsigned short numberOfEntries;

public:

/**
 * Default constructor
 */

	trackWithHitsWithCorrectSlopeDistributionWarningMsg();

/**
 * Constructor
 */

	trackWithHitsWithCorrectSlopeDistributionWarningMsg(unsigned int* distribution, unsigned short numberOfEntries);

/**
 * Destructor
 */

	virtual ~trackWithHitsWithCorrectSlopeDistributionWarningMsg();

/**
 * This method display a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _TRACKWITHHITSINLAYERDISTRIBUTIONWARNINGMSG_H
#define _TRACKWITHHITSINLAYERDISTRIBUTIONWARNINGMSG_H

/**
 * CLASS trackWithHitsInLayerDistributionWarningMsg
 */

class trackWithHitsInLayerDistributionWarningMsg : public analysisWarningMsg {

private:

	unsigned int*  distribution;
	unsigned short numberOfEntries;

public:

/**
 * Default constructor
 */

	trackWithHitsInLayerDistributionWarningMsg();

/**
 * Constructor
 */

	trackWithHitsInLayerDistributionWarningMsg(unsigned int* distribution, unsigned short numberOfEntries);

/**
 * Destructor
 */

	virtual ~trackWithHitsInLayerDistributionWarningMsg();

/**
 * This method display a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _ARRAYTODISPLAYISNOTACCESSIBLEWARNINGMSG_H
#define _ARRAYTODISPLAYISNOTACCESSIBLEWARNINGMSG_H

/**
 * CLASS arrayToDisplayIsNotAccessibleWarningMsg
 */

class arrayToDisplayIsNotAccessibleWarningMsg : public analysisWarningMsg {

public:

/**
 * Default constructor
 */

	arrayToDisplayIsNotAccessibleWarningMsg();

/**
 * Destructor
 */

	virtual ~arrayToDisplayIsNotAccessibleWarningMsg();

/**
 * This method display a warning message.
 */

	void warningMsg();

};

#endif



#ifndef _TRACKWITHSIGNATUREDISTRIBUTIONWARNINGMSG_H
#define _TRACKWITHSIGNATUREDISTRIBUTIONWARNINGMSG_H

/**
 * CLASS trackWithSignatureDistributionWarningMsg
 */

class trackWithSignatureDistributionWarningMsg : public analysisWarningMsg {

private:

	unsigned int*  distribution;
	unsigned short numberOfEntries;

public:

/**
 * Default constructor
 */

	trackWithSignatureDistributionWarningMsg();

/**
 * Constructor
 */

	trackWithSignatureDistributionWarningMsg(unsigned int* distribution, unsigned short numberOfEntries);

/**
 * Destructor
 */

	virtual ~trackWithSignatureDistributionWarningMsg();

/**
 * This method display a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _TRACKWITHHITSINCORRECTCELLDISTRIBUTIONWARNINGMSG_H
#define _TRACKWITHHITSINCORRECTCELLDISTRIBUTIONWARNINGMSG_H

/**
 * CLASS trackWithHitsInCorrectCellDistributionWarningMsg
 */

class trackWithHitsInCorrectCellDistributionWarningMsg : public analysisWarningMsg {

private:

	unsigned int*  distribution;
	unsigned short numberOfEntries;

public:

/**
 * Default constructor
 */

	trackWithHitsInCorrectCellDistributionWarningMsg();

/**
 * Constructor
 */

	trackWithHitsInCorrectCellDistributionWarningMsg(unsigned int* distribution, unsigned short numberOfEntries);

/**
 * Destructor
 */

	virtual ~trackWithHitsInCorrectCellDistributionWarningMsg();

/**
 * This method display a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _TRACKWITHHITSINCELLDISTRIBUTIONWARNINGMSG_H
#define _TRACKWITHHITSINCELLDISTRIBUTIONWARNINGMSG_H

/**
 * CLASS trackWithHitsInCellDistributionWarningMsg
 */

class trackWithHitsInCellDistributionWarningMsg : public analysisWarningMsg {

private:

	unsigned int*  distribution;
	unsigned short numberOfEntries;

public:

/**
 * Default constructor
 */

	trackWithHitsInCellDistributionWarningMsg();

/**
 * Constructor
 */

	trackWithHitsInCellDistributionWarningMsg(unsigned int* distribution, unsigned short numberOfEntries);

/**
 * Destructor
 */

	virtual ~trackWithHitsInCellDistributionWarningMsg();

/**
 * This method display a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _MIDDLEDISTANCEOFMAXIMUMANDCORRECTCELLWARNINGMSG_H
#define _MIDDLEDISTANCEOFMAXIMUMANDCORRECTCELLWARNINGMSG_H

/**
 * CLASS middleDistanceOfMaximumAndCorrectCellWarningMsg
 */

class middleDistanceOfMaximumAndCorrectCellWarningMsg : public analysisWarningMsg {

private:

	double distance;

public:

/**
 * Default constructor
 */

	middleDistanceOfMaximumAndCorrectCellWarningMsg();

/**
 * Constructor
 */

	middleDistanceOfMaximumAndCorrectCellWarningMsg(double distance);

/**
 * Destructor
 */

	virtual ~middleDistanceOfMaximumAndCorrectCellWarningMsg();

/**
 * This method displays a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _MIDDLEDISTANCEOFMAXIMUMANDCORRECTLAYERWARNINGMSG_H
#define _MIDDLEDISTANCEOFMAXIMUMANDCORRECTLAYERWARNINGMSG_H

/**
 * CLASS middleDistanceOfMaximumAndCorrectLayerWarningMsg
 */

class middleDistanceOfMaximumAndCorrectLayerWarningMsg : public analysisWarningMsg {

private:

	double distance;

public:

/**
 * Default constructor
 */

	middleDistanceOfMaximumAndCorrectLayerWarningMsg();

/**
 * Constructor
 */

	middleDistanceOfMaximumAndCorrectLayerWarningMsg(double distance);

/**
 * Destructor
 */

	virtual ~middleDistanceOfMaximumAndCorrectLayerWarningMsg();

/**
 * This method displays a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _CANNOTFILLNOTEXISTINGDISPLAYWARNINGMSG_H
#define _CANNOTFILLNOTEXISTINGDISPLAYWARNINGMSG_H

/**
 * CLASS cannotFillNotExistingDisplayWarningMsg
 */

class cannotFillNotExistingDisplayWarningMsg : public analysisWarningMsg {

public:

/**
 * Default constructor
 */

	cannotFillNotExistingDisplayWarningMsg();

/**
 * Destructor
 */

	virtual ~cannotFillNotExistingDisplayWarningMsg();

/**
 * This method displays a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _DISPLAYCANNOTBEREADFROMFILEWARNINGMSG_H
#define _DISPLAYCANNOTBEREADFROMFILEWARNINGMSG_H

/**
 * CLASS displayCannotBeReadFromFileWarningMsg
 */

class displayCannotBeReadFromFileWarningMsg : public analysisWarningMsg {

private:

	std::string name;
	std::string title;
	std::string fileName;

public:

/**
 * Default constructor
 */

	displayCannotBeReadFromFileWarningMsg();

/**
 * Constructor
 */

	displayCannotBeReadFromFileWarningMsg(std::string name, std::string title, std::string fileName);

/**
 * Destructor
 */

	virtual ~displayCannotBeReadFromFileWarningMsg();

/**
 * This method displays a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _DISPLAYNOTACCESSIBLEWARNINGMSG_H
#define _DISPLAYNOTACCESSIBLEWARNINGMSG_H

/**
 * CLASS displayNotAccessibleWarningMsg
 */

class displayNotAccessibleWarningMsg : public analysisWarningMsg {

public:

/**
 * Default constructor
 */

	displayNotAccessibleWarningMsg();

/**
 * Destructor
 */

	virtual ~displayNotAccessibleWarningMsg();

/**
 * This method displays a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _CANNOTWRITENOTENABLEDLAYERWARNINGMSG_H
#define _CANNOTWRITENOTENABLEDLAYERWARNINGMSG_H

/**
 * CLASS cannotWriteNotEnabledLayerWarningMsg
 */

class cannotWriteNotEnabledLayerWarningMsg : public analysisWarningMsg {

private:

	unsigned short layerToWrite;
	unsigned short enabledLayer;

public:

/**
 * Default constructor
 */

	cannotWriteNotEnabledLayerWarningMsg();

/**
 * Constructor
 */

	cannotWriteNotEnabledLayerWarningMsg(unsigned short layerToWrite, unsigned short enabledLayer);

/**
 * Destructor
 */

	virtual ~cannotWriteNotEnabledLayerWarningMsg();

/**
 * This method display a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _CANNOTSHOWNOTENABLEDLAYERWARNINGMSG_H
#define _CANNOTSHOWNOTENABLEDLAYERWARNINGMSG_H

/**
 * CLASS cannotShowNotEnabledLayerWarningMsg
 */

class cannotShowNotEnabledLayerWarningMsg : public analysisWarningMsg {

private:

	unsigned short layerToShow;
	unsigned short enabledLayer;

public:

/**
 * Default constructor
 */

	cannotShowNotEnabledLayerWarningMsg();

/**
 * Constructor
 */

	cannotShowNotEnabledLayerWarningMsg(unsigned short layerToShow, unsigned short enabledLayer);

/**
 * Destructor
 */

	virtual ~cannotShowNotEnabledLayerWarningMsg();

/**
 * This method display a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _ENABLELAYERISOUTOFRANGEWARNINGMSG_H
#define _ENABLELAYERISOUTOFRANGEWARNINGMSG_H

/**
 * CLASS enableLayerIsOutOfRangeWarningMsg
 */

class enableLayerIsOutOfRangeWarningMsg : public analysisWarningMsg {

private:

	unsigned short layer;
	unsigned short layerMin;
	unsigned short layerMax;

public:

/**
 * Default constructor
 */

	enableLayerIsOutOfRangeWarningMsg();

/**
 * Constructor
 */

	enableLayerIsOutOfRangeWarningMsg(unsigned short layer, unsigned short LayerMin, unsigned short LayerMax);

/**
 * Destructor
 */

	virtual ~enableLayerIsOutOfRangeWarningMsg();

/**
 * This method display a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _CANNOTSHOWHISTOGRAMWITHOUTWRITINGTOROOTWARNINGMSG_H
#define _CANNOTSHOWHISTOGRAMWITHOUTWRITINGTOROOTWARNINGMSG_H

/**
 * CLASS cannotShowHistogramWithoutWritingToRootWarningMsg
 */

class cannotShowHistogramWithoutWritingToRootWarningMsg : public analysisWarningMsg {

public:

/**
 * Default constructor
 */

	cannotShowHistogramWithoutWritingToRootWarningMsg();

/**
 * Destructor
 */

	virtual ~cannotShowHistogramWithoutWritingToRootWarningMsg();

/**
 * This method displays a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _TRACKWITHMOMENTAERRORDISTRIBUTIONWARNINGMSG_H
#define _TRACKWITHMOMENTAERRORDISTRIBUTIONWARNINGMSG_H

/**
 * CLASS trackWithMomentaErrorDistributionWarningMsg
 */

class trackWithMomentaErrorDistributionWarningMsg : public analysisWarningMsg {

private:

	unsigned int*  distribution;
	unsigned short numberOfEntries;

public:

/**
 * Default constructor
 */

	trackWithMomentaErrorDistributionWarningMsg();

/**
 * Constructor
 */

	trackWithMomentaErrorDistributionWarningMsg(unsigned int* distribution, unsigned short numberOfEntries);

/**
 * Destructor
 */

	virtual ~trackWithMomentaErrorDistributionWarningMsg();

/**
 * This method display a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _TRACKWITHMOMENTAXDISTRIBUTIONWARNINGMSG_H
#define _TRACKWITHMOMENTAXDISTRIBUTIONWARNINGMSG_H

/**
 * CLASS trackWithMomentaXDistributionWarningMsg
 */

class trackWithMomentaXDistributionWarningMsg : public analysisWarningMsg {

private:

	unsigned int*  distribution;
	unsigned short numberOfEntries;
	double         min;
	double         max;

public:

/**
 * Default constructor
 */

	trackWithMomentaXDistributionWarningMsg();

/**
 * Constructor
 */

	trackWithMomentaXDistributionWarningMsg(unsigned int* distribution, unsigned short numberOfEntries, double min, double max);

/**
 * Destructor
 */

	virtual ~trackWithMomentaXDistributionWarningMsg();

/**
 * This method display a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _TRACKWITHMOMENTAYDISTRIBUTIONWARNINGMSG_H
#define _TRACKWITHMOMENTAYDISTRIBUTIONWARNINGMSG_H

/**
 * CLASS trackWithMomentaYDistributionWarningMsg
 */

class trackWithMomentaYDistributionWarningMsg : public analysisWarningMsg {

private:

	unsigned int*  distribution;
	unsigned short numberOfEntries;
	double         min;
	double         max;

public:

/**
 * Default constructor
 */

	trackWithMomentaYDistributionWarningMsg();

/**
 * Constructor
 */

	trackWithMomentaYDistributionWarningMsg(unsigned int* distribution, unsigned short numberOfEntries, double min, double max);

/**
 * Destructor
 */

	virtual ~trackWithMomentaYDistributionWarningMsg();

/**
 * This method display a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _TRACKWITHMOMENTAZDISTRIBUTIONWARNINGMSG_H
#define _TRACKWITHMOMENTAZDISTRIBUTIONWARNINGMSG_H

/**
 * CLASS trackWithMomentaZDistributionWarningMsg
 */

class trackWithMomentaZDistributionWarningMsg : public analysisWarningMsg {

private:

	unsigned int*  distribution;
	unsigned short numberOfEntries;
	double         min;
	double         max;

public:

/**
 * Default constructor
 */

	trackWithMomentaZDistributionWarningMsg();

/**
 * Constructor
 */

	trackWithMomentaZDistributionWarningMsg(unsigned int* distribution, unsigned short numberOfEntries, double min, double max);

/**
 * Destructor
 */

	virtual ~trackWithMomentaZDistributionWarningMsg();

/**
 * This method display a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _TRACKWITHCOORDINATEXDISTRIBUTIONWARNINGMSG_H
#define _TRACKWITHCOORDINATEXDISTRIBUTIONWARNINGMSG_H

/**
 * CLASS trackWithCoordinateXDistributionWarningMsg
 */

class trackWithCoordinateXDistributionWarningMsg : public analysisWarningMsg {

private:

	unsigned int*  distribution;
	unsigned short numberOfEntries;

public:

/**
 * Default constructor
 */

	trackWithCoordinateXDistributionWarningMsg();

/**
 * Constructor
 */

	trackWithCoordinateXDistributionWarningMsg(unsigned int* distribution, unsigned short numberOfEntries);

/**
 * Destructor
 */

	virtual ~trackWithCoordinateXDistributionWarningMsg();

/**
 * This method display a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _TRACKWITHCOORDINATEYDISTRIBUTIONWARNINGMSG_H
#define _TRACKWITHCOORDINATEYDISTRIBUTIONWARNINGMSG_H

/**
 * CLASS trackWithCoordinateYDistributionWarningMsg
 */

class trackWithCoordinateYDistributionWarningMsg : public analysisWarningMsg {

private:

	unsigned int*  distribution;
	unsigned short numberOfEntries;

public:

/**
 * Default constructor
 */

	trackWithCoordinateYDistributionWarningMsg();

/**
 * Constructor
 */

	trackWithCoordinateYDistributionWarningMsg(unsigned int* distribution, unsigned short numberOfEntries);

/**
 * Destructor
 */

	virtual ~trackWithCoordinateYDistributionWarningMsg();

/**
 * This method display a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _TRACKWITHCOORDINATEZDISTRIBUTIONWARNINGMSG_H
#define _TRACKWITHCOORDINATEZDISTRIBUTIONWARNINGMSG_H

/**
 * CLASS trackWithCoordinateZDistributionWarningMsg
 */

class trackWithCoordinateZDistributionWarningMsg : public analysisWarningMsg {

private:

	unsigned int*  distribution;
	unsigned short numberOfEntries;

public:

/**
 * Default constructor
 */

	trackWithCoordinateZDistributionWarningMsg();

/**
 * Constructor
 */

	trackWithCoordinateZDistributionWarningMsg(unsigned int* distribution, unsigned short numberOfEntries);

/**
 * Destructor
 */

	virtual ~trackWithCoordinateZDistributionWarningMsg();

/**
 * This method display a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _TRACKWITHMOMENTADISTRIBUTIONWARNINGMSG_H
#define _TRACKWITHMOMENTADISTRIBUTIONWARNINGMSG_H

/**
 * CLASS trackWithMomentaDistributionWarningMsg
 */

class trackWithMomentaDistributionWarningMsg : public analysisWarningMsg {

private:

	unsigned int* distribution;
	unsigned int  numberOfEntries;

public:

/**
 * Default constructor
 */

	trackWithMomentaDistributionWarningMsg();

/**
 * Constructor
 */

	trackWithMomentaDistributionWarningMsg(unsigned int* distribution, unsigned int numberOfEntries);

/**
 * Destructor
 */

	virtual ~trackWithMomentaDistributionWarningMsg();

/**
 * This method display a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _TRACKWITHCOORDINATEDISTRIBUTIONWARNINGMSG_H
#define _TRACKWITHCOORDINATEDISTRIBUTIONWARNINGMSG_H

/**
 * CLASS trackWithCoordinateDistributionWarningMsg
 */

class trackWithCoordinateDistributionWarningMsg : public analysisWarningMsg {

private:

	unsigned int* distribution;
	unsigned int  numberOfEntries;

public:

/**
 * Default constructor
 */

	trackWithCoordinateDistributionWarningMsg();

/**
 * Constructor
 */

	trackWithCoordinateDistributionWarningMsg(unsigned int* distribution, unsigned int numberOfEntries);

/**
 * Destructor
 */

	virtual ~trackWithCoordinateDistributionWarningMsg();

/**
 * This method display a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _NUMBEROFTRACKSWHICHCANNOTBEFOUNDWARNINGMSG_H
#define _NUMBEROFTRACKSWHICHCANNOTBEFOUNDWARNINGMSG_H

/**
 * CLASS numberOfTracksWhichCannotBeFoundWarningMsg
 */

class numberOfTracksWhichCannotBeFoundWarningMsg : public analysisWarningMsg {

private:

	unsigned long numberOfTracks;

public:

/**
 * Default constructor
 */

	numberOfTracksWhichCannotBeFoundWarningMsg();

/**
 * Constructor
 */

	numberOfTracksWhichCannotBeFoundWarningMsg(unsigned long numberOfTracks);

/**
 * Destructor
 */

	virtual ~numberOfTracksWhichCannotBeFoundWarningMsg();

/**
 * This method display a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _NUMBEROFTRACKSWITHGOODPRELUTSIGNATUREWARNINGMSG_H
#define _NUMBEROFTRACKSWITHGOODPRELUTSIGNATUREWARNINGMSG_H

/**
 * CLASS numberOfTracksWithGoodPrelutSignatureWarningMsg
 */

class numberOfTracksWithGoodPrelutSignatureWarningMsg : public analysisWarningMsg {

private:

	unsigned long numberOfTracks;

public:

/**
 * Default constructor
 */

	numberOfTracksWithGoodPrelutSignatureWarningMsg();

/**
 * Constructor
 */

	numberOfTracksWithGoodPrelutSignatureWarningMsg(unsigned long numberOfTracks);

/**
 * Destructor
 */

	virtual ~numberOfTracksWithGoodPrelutSignatureWarningMsg();

/**
 * This method display a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _NUMBEROFTRACKSWITHGOODLUTSIGNATUREWARNINGMSG_H
#define _NUMBEROFTRACKSWITHGOODLUTSIGNATUREWARNINGMSG_H

/**
 * CLASS numberOfTracksWithGoodLutSignatureWarningMsg
 */

class numberOfTracksWithGoodLutSignatureWarningMsg : public analysisWarningMsg {

private:

	unsigned long numberOfTracks;

public:

/**
 * Default constructor
 */

	numberOfTracksWithGoodLutSignatureWarningMsg();

/**
 * Constructor
 */

	numberOfTracksWithGoodLutSignatureWarningMsg(unsigned long numberOfTracks);

/**
 * Destructor
 */

	virtual ~numberOfTracksWithGoodLutSignatureWarningMsg();

/**
 * This method display a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _NUMBEROFGOODPRELUTSIGNATUREWARNINGMSG_H
#define _NUMBEROFGOODPRELUTSIGNATUREWARNINGMSG_H

/**
 * CLASS numberOfGoodPrelutSignatureWarningMsg
 */

class numberOfGoodPrelutSignatureWarningMsg : public analysisWarningMsg {

private:

	unsigned long numberOfSignatures;

public:

/**
 * Default constructor
 */

	numberOfGoodPrelutSignatureWarningMsg();

/**
 * Constructor
 */

	numberOfGoodPrelutSignatureWarningMsg(unsigned long numberOfSignatures);

/**
 * Destructor
 */

	virtual ~numberOfGoodPrelutSignatureWarningMsg();

/**
 * This method display a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _NUMBEROFGOODLUTSIGNATUREWARNINGMSG_H
#define _NUMBEROFGOODLUTSIGNATUREWARNINGMSG_H

/**
 * CLASS numberOfGoodLutSignatureWarningMsg
 */

class numberOfGoodLutSignatureWarningMsg : public analysisWarningMsg {

private:

	unsigned long numberOfSignatures;

public:

/**
 * Default constructor
 */

	numberOfGoodLutSignatureWarningMsg();

/**
 * Constructor
 */

	numberOfGoodLutSignatureWarningMsg(unsigned long numberOfSignatures);

/**
 * Destructor
 */

	virtual ~numberOfGoodLutSignatureWarningMsg();

/**
 * This method display a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _GOODPRELUTSIGNATUREWARNINGMSG_H
#define _GOODPRELUTSIGNATUREWARNINGMSG_H

/**
 * CLASS goodPrelutSignatureWarningMsg
 */

class goodPrelutSignatureWarningMsg : public analysisWarningMsg {

private:

	std::string stringTable;

public:

/**
 * Default constructor
 */

	goodPrelutSignatureWarningMsg();

/**
 * Constructor
 */

	goodPrelutSignatureWarningMsg(std::string stringTable);

/**
 * Destructor
 */

	virtual ~goodPrelutSignatureWarningMsg();

/**
 * This method display a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _GOODLUTSIGNATUREWARNINGMSG_H
#define _GOODLUTSIGNATUREWARNINGMSG_H

/**
 * CLASS goodLutSignatureWarningMsg
 */

class goodLutSignatureWarningMsg : public analysisWarningMsg {

private:

	std::string stringTable;

public:

/**
 * Default constructor
 */

	goodLutSignatureWarningMsg();

/**
 * Constructor
 */

	goodLutSignatureWarningMsg(std::string stringTable);

/**
 * Destructor
 */

	virtual ~goodLutSignatureWarningMsg();

/**
 * This method display a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _USEDTABLESWARNINGMSG_H
#define _USEDTABLESWARNINGMSG_H

/**
 * CLASS usedTablesWarningMsg
 */

class usedTablesWarningMsg : public analysisWarningMsg {

private:

	std::string stringTable;

public:

/**
 * Default constructor
 */

	usedTablesWarningMsg();

/**
 * Constructor
 */

	usedTablesWarningMsg(std::string stringTable);

/**
 * Destructor
 */

	virtual ~usedTablesWarningMsg();

/**
 * This method display a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _TRACKWITHSAMECOORDINATEDISTRIBUTIONWARNINGMSG_H
#define _TRACKWITHSAMECOORDINATEDISTRIBUTIONWARNINGMSG_H

/**
 * CLASS trackWithSameCoordinateDistributionWarningMsg
 */

class trackWithSameCoordinateDistributionWarningMsg : public analysisWarningMsg {

private:

	unsigned int* distribution;
	unsigned int  numberOfEntries;

public:

/**
 * Default constructor
 */

	trackWithSameCoordinateDistributionWarningMsg();

/**
 * Constructor
 */

	trackWithSameCoordinateDistributionWarningMsg(unsigned int* distribution, unsigned int numberOfEntries);

/**
 * Destructor
 */

	virtual ~trackWithSameCoordinateDistributionWarningMsg();

/**
 * This method display a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _VOLUMENOTFOUNDWARNINGMSG_H
#define _VOLUMENOTFOUNDWARNINGMSG_H

/**
 * CLASS volumeNotFoundWarningMsg
 */

class volumeNotFoundWarningMsg : public analysisWarningMsg {

public:

/**
 * Default constructor
 */

	volumeNotFoundWarningMsg();

/**
 * Destructor
 */

	virtual ~volumeNotFoundWarningMsg();

/**
 * This method display a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _LISTOFVOLUMESNOTFOUNDWARNINGMSG_H
#define _LISTOFVOLUMESNOTFOUNDWARNINGMSG_H

/**
 * CLASS listOfVolumesNotFoundWarningMsg
 */

class listOfVolumesNotFoundWarningMsg : public analysisWarningMsg {

public:

/**
 * Default constructor
 */

	listOfVolumesNotFoundWarningMsg();

/**
 * Destructor
 */

	virtual ~listOfVolumesNotFoundWarningMsg();

/**
 * This method display a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _LISTOFTRACKSNOTFOUNDWARNINGMSG_H
#define _LISTOFTRACKSNOTFOUNDWARNINGMSG_H

/**
 * CLASS listOfTracksNotFoundWarningMsg
 */

class listOfTracksNotFoundWarningMsg : public analysisWarningMsg {

public:

/**
 * Default constructor
 */

	listOfTracksNotFoundWarningMsg();

/**
 * Destructor
 */

	virtual ~listOfTracksNotFoundWarningMsg();

/**
 * This method display a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _CURRENTTRACKNOTFOUNDWARNINGMSG_H
#define _CURRENTTRACKNOTFOUNDWARNINGMSG_H

/**
 * CLASS currentTrackNotFoundWarningMsg
 */

class currentTrackNotFoundWarningMsg : public analysisWarningMsg {

public:

/**
 * Default constructor
 */

	currentTrackNotFoundWarningMsg();

/**
 * Destructor
 */

	virtual ~currentTrackNotFoundWarningMsg();

/**
 * This method display a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _TIMEVIOLATIONWARNINGMSG_H
#define _TIMEVIOLATIONWARNINGMSG_H

/**
 * CLASS timeViolationWarningMsg
 */

class timeViolationWarningMsg : public analysisWarningMsg {

private:

	double time1;
	double time2;

public:

/**
 * Default constructor
 */

	timeViolationWarningMsg();

/**
 * Constructor
 */

	timeViolationWarningMsg(double time1, double time2);

/**
 * Destructor
 */

	virtual ~timeViolationWarningMsg();

/**
 * This method display a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _NUMBEROFTRACKSINALLLAYERSWARNINGMSG_H
#define _NUMBEROFTRACKSINALLLAYERSWARNINGMSG_H

/**
 * CLASS numberOfTracksInAllLayersWarningMsg
 */

class numberOfTracksInAllLayersWarningMsg : public analysisWarningMsg {

private:

	unsigned int* distribution;
	unsigned int  numberOfEntries;
	unsigned int  numberOfSummedEventsToAverage;

public:

/**
 * Default constructor
 */

	numberOfTracksInAllLayersWarningMsg();

/**
 * Constructor
 */

	numberOfTracksInAllLayersWarningMsg(unsigned int* distribution, unsigned int numberOfEntries, unsigned int numberOfSummedEventsToAverage);

/**
 * Destructor
 */

	virtual ~numberOfTracksInAllLayersWarningMsg();

/**
 * This method display a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _NUMBEROFTRACKDENSITIESINALLLAYERSWARNINGMSG_H
#define _NUMBEROFTRACKDENSITIESINALLLAYERSWARNINGMSG_H

/**
 * CLASS numberOfTrackDensitiesInAllLayersWarningMsg
 */

class numberOfTrackDensitiesInAllLayersWarningMsg : public analysisWarningMsg {

private:

	int*          distribution;
	unsigned int  numberOfEntries;
	unsigned int  numberOfSummedEventsToAverage;

public:

/**
 * Default constructor
 */

	numberOfTrackDensitiesInAllLayersWarningMsg();

/**
 * Constructor
 */

	numberOfTrackDensitiesInAllLayersWarningMsg(int* distribution, unsigned int numberOfEntries, unsigned int numberOfSummedEventsToAverage);

/**
 * Destructor
 */

	virtual ~numberOfTrackDensitiesInAllLayersWarningMsg();

/**
 * This method display a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _NUMBEROFMINIMALTRACKSINALLCOLUMNSWARNINGMSG_H
#define _NUMBEROFMINIMALTRACKSINALLCOLUMNSWARNINGMSG_H

/**
 * CLASS numberOfMinimalTracksInAllColumnsWarningMsg
 */

class numberOfMinimalTracksInAllColumnsWarningMsg : public analysisWarningMsg {

private:

	unsigned int* distribution;
	unsigned int  numberOfEntries;

public:

/**
 * Default constructor
 */

	numberOfMinimalTracksInAllColumnsWarningMsg();

/**
 * Constructor
 */

	numberOfMinimalTracksInAllColumnsWarningMsg(unsigned int* distribution, unsigned int numberOfEntries);

/**
 * Destructor
 */

	virtual ~numberOfMinimalTracksInAllColumnsWarningMsg();

/**
 * This method display a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _NUMBEROFAVERAGETRACKSINALLCOLUMNSWARNINGMSG_H
#define _NUMBEROFAVERAGETRACKSINALLCOLUMNSWARNINGMSG_H

/**
 * CLASS numberOfAverageTracksInAllColumnsWarningMsg
 */

class numberOfAverageTracksInAllColumnsWarningMsg : public analysisWarningMsg {

private:

	double*      distribution;
	unsigned int numberOfEntries;
	unsigned int numberOfSummedEventsToAverage;

public:

/**
 * Default constructor
 */

	numberOfAverageTracksInAllColumnsWarningMsg();

/**
 * Constructor
 */

	numberOfAverageTracksInAllColumnsWarningMsg(double* distribution, unsigned int numberOfEntries, unsigned int numberOfSummedEventsToAverage);

/**
 * Destructor
 */

	virtual ~numberOfAverageTracksInAllColumnsWarningMsg();

/**
 * This method display a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _NUMBEROFMAXIMALTRACKSINALLCOLUMNSWARNINGMSG_H
#define _NUMBEROFMAXIMALTRACKSINALLCOLUMNSWARNINGMSG_H

/**
 * CLASS numberOfMaximalTracksInAllColumnsWarningMsg
 */

class numberOfMaximalTracksInAllColumnsWarningMsg : public analysisWarningMsg {

private:

	unsigned int* distribution;
	unsigned int  numberOfEntries;

public:

/**
 * Default constructor
 */

	numberOfMaximalTracksInAllColumnsWarningMsg();

/**
 * Constructor
 */

	numberOfMaximalTracksInAllColumnsWarningMsg(unsigned int* distribution, unsigned int numberOfEntries);

/**
 * Destructor
 */

	virtual ~numberOfMaximalTracksInAllColumnsWarningMsg();

/**
 * This method display a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _SIZEOFSEPARATORFIFOSWARNINGMSG_H
#define _SIZEOFSEPARATORFIFOSWARNINGMSG_H

/**
 * CLASS sizeOfSeparatorFifosWarningMsg
 */

class sizeOfSeparatorFifosWarningMsg : public analysisWarningMsg {

private:

	unsigned int* distribution;
	unsigned int  numberOfEntries;

public:

/**
 * Default constructor
 */

	sizeOfSeparatorFifosWarningMsg();

/**
 * Constructor
 */

	sizeOfSeparatorFifosWarningMsg(unsigned int* distribution, unsigned int numberOfEntries);

/**
 * Destructor
 */

	virtual ~sizeOfSeparatorFifosWarningMsg();

/**
 * This method display a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _YOUCANNOTENABLETRACKWITHSAMEMOMENTAANDCOORDINATEDISTRIBUTIONWARNINGMSG_H
#define _YOUCANNOTENABLETRACKWITHSAMEMOMENTAANDCOORDINATEDISTRIBUTIONWARNINGMSG_H

/**
 * CLASS youCannotEnableTrackWithSameMomentaAndCoordinateDistributionWarningMsg
 */

class youCannotEnableTrackWithSameMomentaAndCoordinateDistributionWarningMsg : public analysisWarningMsg {

public:

/**
 * Default constructor
 */

	youCannotEnableTrackWithSameMomentaAndCoordinateDistributionWarningMsg();

/**
 * Destructor
 */

	virtual ~youCannotEnableTrackWithSameMomentaAndCoordinateDistributionWarningMsg();

/**
 * This method displays a warning message.
 */

	void warningMsg();

};

#endif
