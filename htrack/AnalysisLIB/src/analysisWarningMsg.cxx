/* *******************************************************************
// (C)opyright 2004
// 
// Institute of Computer Science V
// Prof. M�ner
// University of Mannheim, Germany
// 
// *******************************************************************
// 
// Designer(s):   Steinle / Gl�
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
//     - base class for warnings occuring during analysis
//
// *******************************************************************
//
// $Author: csteinle $
// $Date: 2007-06-19 14:30:25 $
// $Revision: 1.14 $
//
// *******************************************************************/


#include "../../MiscLIB/include/projects.h"
#include "../../MiscLIB/include/conversionRoutines.h"
#include "../../MiscLIB/include/bitArray.h"
#include "../include/analysisWarningMsg.h"
#include <iostream>


/****************************************************************
 * CLASS analysisWarningMsg						 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

analysisWarningMsg::analysisWarningMsg() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

analysisWarningMsg::~analysisWarningMsg() {

}

/****************************************************************
 * This method prints a message.				 				*
 ****************************************************************/

void analysisWarningMsg::printMsg(std::string message) {

#ifndef NOANALYSISWARNINGMESSAGE

	std::cerr << std::endl;
	
	std::cerr << ANALYSISLIB << " WARNING: ";
	
	if (!message.empty())
		std::cerr << message;
	
	std::cerr << std::endl;

#endif

}

/****************************************************************
 * CLASS trackNotFoundWarningMsg				 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

trackNotFoundWarningMsg::trackNotFoundWarningMsg() : analysisWarningMsg() {

	track = NULL;

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

trackNotFoundWarningMsg::trackNotFoundWarningMsg(trackfinderInputTrack* actualTrack) : analysisWarningMsg() {

	track = actualTrack;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

trackNotFoundWarningMsg::~trackNotFoundWarningMsg() {

}

/****************************************************************
 * This method displays a warning message.		 				*
 ****************************************************************/

void trackNotFoundWarningMsg::warningMsg() {

	std::string temp;
	char        intBuffer[intConversionDigits+1];
	char        doubleBuffer[doubleConversion+1];

	temp = "The following track is not found:\n";
	temp += " TrackId: ";
	itos(track->getTrackIndex(), intBuffer, 10, intConversionDigits);
	temp += intBuffer;
	temp += "\n";
	temp += "  PdgCode: ";
	itos(track->getPdgCode(), intBuffer, 10, intConversionDigits);
	temp += intBuffer;
	temp += "\n";
	temp += "  Charge: ";
	dtos(track->getCharge(), doubleBuffer, doubleConversionDigits);
	temp += doubleBuffer;
	temp += "\n";
	temp += "  momemtumX: ";
	dtos(track->getMomX(), doubleBuffer, doubleConversionDigits);
	temp += doubleBuffer;
	temp += "\n";
	temp += "  momentumY: ";
	dtos(track->getMomY(), doubleBuffer, doubleConversionDigits);
	temp += doubleBuffer;
	temp += "\n";
	temp += "  momentumZ: ";
	dtos(track->getMomZ(), doubleBuffer, doubleConversionDigits);
	temp += doubleBuffer;
	printMsg(temp);

}


/****************************************************************
 * CLASS displayAnalyserNotFoundWarningMsg		 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

displayAnalyserNotFoundWarningMsg::displayAnalyserNotFoundWarningMsg() : analysisWarningMsg() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

displayAnalyserNotFoundWarningMsg::~displayAnalyserNotFoundWarningMsg() {

}

/****************************************************************
 * This method displays a warning message.		 				*
 ****************************************************************/

void displayAnalyserNotFoundWarningMsg::warningMsg() {

	printMsg("The display analyser is not found!!!\nCheck the init()-functions for correctness.\nCaution: The ...ToRoot()-function must be the last init()-function for displays.");

}


/****************************************************************
 * CLASS displayCannotBeWrittenToFileWarningMsg	 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

displayCannotBeWrittenToFileWarningMsg::displayCannotBeWrittenToFileWarningMsg() : analysisWarningMsg() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

displayCannotBeWrittenToFileWarningMsg::~displayCannotBeWrittenToFileWarningMsg() {

}

/****************************************************************
 * This method displays a warning message.		 				*
 ****************************************************************/

void displayCannotBeWrittenToFileWarningMsg::warningMsg() {

	printMsg("A display cannot be written to file!!!");

}


/****************************************************************
 * CLASS displayNotFoundWarningMsg				 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

displayNotFoundWarningMsg::displayNotFoundWarningMsg() : analysisWarningMsg() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

displayNotFoundWarningMsg::~displayNotFoundWarningMsg() {

}

/****************************************************************
 * This method displayDirectories a warning message.	 		*
 ****************************************************************/

void displayNotFoundWarningMsg::warningMsg() {

	printMsg("A display is not found during writing to file!!!");

}


/****************************************************************
 * CLASS cannotCreateSignatureWarningMsg		 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

cannotCreateSignatureWarningMsg::cannotCreateSignatureWarningMsg() : analysisWarningMsg() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

cannotCreateSignatureWarningMsg::~cannotCreateSignatureWarningMsg() {

}

/****************************************************************
 * This method displayDirectories a warning message.	 		*
 ****************************************************************/

void cannotCreateSignatureWarningMsg::warningMsg() {

	printMsg("The signature for the enabled displays cannot be created!!!");

}


/****************************************************************
 * CLASS cannotDrawWarningMsg					 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

cannotDrawWarningMsg::cannotDrawWarningMsg() : analysisWarningMsg() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

cannotDrawWarningMsg::~cannotDrawWarningMsg() {

}

/****************************************************************
 * This method displayDirectories a warning message.	 		*
 ****************************************************************/

void cannotDrawWarningMsg::warningMsg() {

	printMsg("The display can not be drawn!!!");

}


/****************************************************************
 * CLASS cannotWriteWarningMsg					 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

cannotWriteWarningMsg::cannotWriteWarningMsg() : analysisWarningMsg() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

cannotWriteWarningMsg::~cannotWriteWarningMsg() {

}

/****************************************************************
 * This method displayDirectories a warning message.	 		*
 ****************************************************************/

void cannotWriteWarningMsg::warningMsg() {

	printMsg("The display can not be written!!!");

}


/****************************************************************
 * CLASS notAllDisplaysCanBeDrawnWarningMsg		 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

notAllDisplaysCanBeDrawnWarningMsg::notAllDisplaysCanBeDrawnWarningMsg() : analysisWarningMsg() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

notAllDisplaysCanBeDrawnWarningMsg::~notAllDisplaysCanBeDrawnWarningMsg() {

}

/****************************************************************
 * This method displayDirectories a warning message.	 		*
 ****************************************************************/

void notAllDisplaysCanBeDrawnWarningMsg::warningMsg() {

	printMsg("There is not enough space to draw all displays!!!");

}


/****************************************************************
 * CLASS cannotAccessParameterWarningMsg		 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

cannotAccessParameterWarningMsg::cannotAccessParameterWarningMsg() : analysisWarningMsg() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

cannotAccessParameterWarningMsg::~cannotAccessParameterWarningMsg() {

}

/****************************************************************
 * This method displayDirectories a warning message.	 		*
 ****************************************************************/

void cannotAccessParameterWarningMsg::warningMsg() {

	printMsg("The parameter for the display entries can not be accessed!!!");

}


/****************************************************************
 * CLASS cannotAccessTrackWarningMsg			 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

cannotAccessTrackWarningMsg::cannotAccessTrackWarningMsg() : analysisWarningMsg() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

cannotAccessTrackWarningMsg::~cannotAccessTrackWarningMsg() {

}

/****************************************************************
 * This method displayDirectories a warning message.	 		*
 ****************************************************************/

void cannotAccessTrackWarningMsg::warningMsg() {

	printMsg("The track for the display entries can not be accessed!!!");

}


/****************************************************************
 * CLASS cannotGetDisplayEntriesWarningMsg		 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

cannotGetDisplayEntriesWarningMsg::cannotGetDisplayEntriesWarningMsg() : analysisWarningMsg() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

cannotGetDisplayEntriesWarningMsg::~cannotGetDisplayEntriesWarningMsg() {

}

/****************************************************************
 * This method displayDirectories a warning message.	 		*
 ****************************************************************/

void cannotGetDisplayEntriesWarningMsg::warningMsg() {

	printMsg("The display entries can not be accessed!!!");

}


/****************************************************************
 * CLASS noHitInDetectorStationWarningMsg		 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

noHitInDetectorStationWarningMsg::noHitInDetectorStationWarningMsg() : analysisWarningMsg() {

	index = (unsigned short)-1;

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

noHitInDetectorStationWarningMsg::noHitInDetectorStationWarningMsg(unsigned short actualIndex) : analysisWarningMsg() {

	index = actualIndex;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

noHitInDetectorStationWarningMsg::~noHitInDetectorStationWarningMsg() {

}

/****************************************************************
 * This method displayDirectories a warning message.	 		*
 ****************************************************************/

void noHitInDetectorStationWarningMsg::warningMsg() {

	std::string temp;
	char        buffer[intConversionDigits+1];

	temp = "There is no hit found in the station with the index ";
	itos(index, buffer, 10, intConversionDigits);
	temp += buffer;
	temp += " of the detector!!!";
	printMsg(temp);

}


/****************************************************************
 * CLASS noAnalysisMomentumOutputFileNameSpecifiedWarningMsg	*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

noAnalysisMomentumOutputFileNameSpecifiedWarningMsg::noAnalysisMomentumOutputFileNameSpecifiedWarningMsg() : analysisWarningMsg() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

noAnalysisMomentumOutputFileNameSpecifiedWarningMsg::~noAnalysisMomentumOutputFileNameSpecifiedWarningMsg() {

}

/****************************************************************
 * This method displays a warning message.		 				*
 ****************************************************************/

void noAnalysisMomentumOutputFileNameSpecifiedWarningMsg::warningMsg() {

	printMsg("The name of the output file for the momentum analyser is not specified!!!");

}


/****************************************************************
 * CLASS noAnalysisProjectionOutputFileNameSpecifiedWarningMsg	*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

noAnalysisProjectionOutputFileNameSpecifiedWarningMsg::noAnalysisProjectionOutputFileNameSpecifiedWarningMsg() : analysisWarningMsg() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

noAnalysisProjectionOutputFileNameSpecifiedWarningMsg::~noAnalysisProjectionOutputFileNameSpecifiedWarningMsg() {

}

/****************************************************************
 * This method displays a warning message.		 				*
 ****************************************************************/

void noAnalysisProjectionOutputFileNameSpecifiedWarningMsg::warningMsg() {

	printMsg("The name of the output file for the projection analyser is not specified!!!");

}


/****************************************************************
 * CLASS enableJustOneDisplayWarningMsg							*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

enableJustOneDisplayWarningMsg::enableJustOneDisplayWarningMsg() : analysisWarningMsg() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

enableJustOneDisplayWarningMsg::~enableJustOneDisplayWarningMsg() {

}

/****************************************************************
 * This method displays a warning message.		 				*
 ****************************************************************/

void enableJustOneDisplayWarningMsg::warningMsg() {

	printMsg("You actually enable a second display. So the previous one is disabled!!!");

}


/****************************************************************
 * CLASS cannotSetPointWarningMsg								*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

cannotSetPointWarningMsg::cannotSetPointWarningMsg() : analysisWarningMsg() {

	index = 0;

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

cannotSetPointWarningMsg::cannotSetPointWarningMsg(int index) : analysisWarningMsg() {

	this->index = index;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

cannotSetPointWarningMsg::~cannotSetPointWarningMsg() {

}

/****************************************************************
 * This method displays a warning message.		 				*
 ****************************************************************/

void cannotSetPointWarningMsg::warningMsg() {

	std::string temp;
	char        buffer[intConversionDigits+1];

	temp =  "The values for the point with index ";
	itos(index, buffer, 10, intConversionDigits);
	temp += buffer;
	temp += " cannot be set.\nMaybe the display is not enabled or the index is out of bounds!!!";
	printMsg(temp);

}


/****************************************************************
 * CLASS positionIsOutOfSpaceWarningMsg							*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

positionIsOutOfSpaceWarningMsg::positionIsOutOfSpaceWarningMsg() : analysisWarningMsg() {

	space       = NULL;
	coordinates = NULL;

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

positionIsOutOfSpaceWarningMsg::positionIsOutOfSpaceWarningMsg(histogramSpace* space, trackCoordinates* coordinates) : analysisWarningMsg() {

	this->space       = space;
	this->coordinates = coordinates;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

positionIsOutOfSpaceWarningMsg::~positionIsOutOfSpaceWarningMsg() {

}

/****************************************************************
 * This method displays a warning message.		 				*
 ****************************************************************/

void positionIsOutOfSpaceWarningMsg::warningMsg() {

	std::string temp;
	char        buffer[intConversionDigits+1];

	temp =  "The position";
	if (coordinates != NULL) {
		temp += " (";
		itos(coordinates->get(DIM1), buffer, 10, intConversionDigits);
		temp += buffer;
		temp += " ,";
		itos(coordinates->get(DIM2), buffer, 10, intConversionDigits);
		temp += buffer;
		temp += " ,";
		itos(coordinates->get(DIM3), buffer, 10, intConversionDigits);
		temp += buffer;
		temp += ")";
	}
	temp += " is not in the specified histogram maximal range";
	if (space != NULL) {
		temp += " of (";
		itos(space->getStep(DIM1), buffer, 10, intConversionDigits);
		temp += buffer;
		temp += " ,";
		itos(space->getStep(DIM2), buffer, 10, intConversionDigits);
		temp += buffer;
		temp += " ,";
		itos(space->getStep(DIM3), buffer, 10, intConversionDigits);
		temp += buffer;
		temp += ")";
	}
	temp += "!!!";
	printMsg(temp);

}


/****************************************************************
 * CLASS numberOfWrongThirdDimensionBordersFoundWarningMsg		*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

numberOfWrongThirdDimensionBordersFoundWarningMsg::numberOfWrongThirdDimensionBordersFoundWarningMsg() : analysisWarningMsg() {

	number = 0;

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

numberOfWrongThirdDimensionBordersFoundWarningMsg::numberOfWrongThirdDimensionBordersFoundWarningMsg(int number) : analysisWarningMsg() {

	this->number = number;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

numberOfWrongThirdDimensionBordersFoundWarningMsg::~numberOfWrongThirdDimensionBordersFoundWarningMsg() {

}

/****************************************************************
 * This method displays a warning message.		 				*
 ****************************************************************/

void numberOfWrongThirdDimensionBordersFoundWarningMsg::warningMsg() {

	std::string temp;
	char        buffer[intConversionDigits+1];

	temp =  "There are ";
	itos(number, buffer, 10, intConversionDigits);
	temp += buffer;
	temp += " hits found with the correct 2D entry out of range of the prelut border!!!";
	printMsg(temp);

}


/****************************************************************
 * CLASS setMagnetfieldFactorWarningMsg							*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

setMagnetfieldFactorWarningMsg::setMagnetfieldFactorWarningMsg() : analysisWarningMsg() {

	stationIndex      = 0;
	magnetfieldFactor = 0;

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

setMagnetfieldFactorWarningMsg::setMagnetfieldFactorWarningMsg(unsigned short stationIndex, double magnetfieldFactor) : analysisWarningMsg() {

	this->stationIndex      = stationIndex;
	this->magnetfieldFactor = magnetfieldFactor;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

setMagnetfieldFactorWarningMsg::~setMagnetfieldFactorWarningMsg() {

}

/****************************************************************
 * This method displays a warning message.		 				*
 ****************************************************************/

void setMagnetfieldFactorWarningMsg::warningMsg() {

	std::string temp;
	char        shortBuffer[shortConversionDigits+1];
	char        doubleBuffer[doubleConversion+1];

	temp =  "The magnetfieldFactor of the station with the index ";
	ustos(stationIndex, shortBuffer, 10, shortConversionDigits);
	temp += shortBuffer;
	temp += " is set to ";
	dtos(magnetfieldFactor, doubleBuffer, doubleConversionDigits);
	temp += doubleBuffer;
	temp += "!!!";
	printMsg(temp);

}


/****************************************************************
 * CLASS analysisLutCorrectionWarningMsg			 			*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

analysisLutCorrectionWarningMsg::analysisLutCorrectionWarningMsg() : analysisWarningMsg() {

	borderCorrections    = 0;
	borderPosCorrections = 0;

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

analysisLutCorrectionWarningMsg::analysisLutCorrectionWarningMsg(unsigned long corrections, unsigned long posCorrections) : analysisWarningMsg() {

	borderCorrections    = corrections;
	borderPosCorrections = posCorrections;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

analysisLutCorrectionWarningMsg::~analysisLutCorrectionWarningMsg() {

}

/****************************************************************
 * This method displays a warning message.		 				*
 ****************************************************************/

void analysisLutCorrectionWarningMsg::warningMsg() {

	std::string temp;
	char buffer[longConversionDigits+1];

	temp = "There are ";
	ultos(borderPosCorrections, buffer, 10, longConversionDigits);
	temp += buffer;
	temp += " corrections which are done while lut calculation.\n";
	ultos(borderCorrections, buffer, 10, longConversionDigits);
	temp += buffer;
	temp += " different IDs are affected\nSo in the middle there are ";
	ultos(((unsigned long)(borderPosCorrections/borderCorrections+0.5)), buffer, 10, longConversionDigits);
	temp += buffer;
	temp += " positions corrected per ID!!!";
	printMsg(temp);

}


/****************************************************************
 * CLASS rootHistogramNotFoundWarningMsg		 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

rootHistogramNotFoundWarningMsg::rootHistogramNotFoundWarningMsg() : analysisWarningMsg() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

rootHistogramNotFoundWarningMsg::~rootHistogramNotFoundWarningMsg() {

}

/****************************************************************
 * This method displayDirectories a warning message.	 		*
 ****************************************************************/

void rootHistogramNotFoundWarningMsg::warningMsg() {

	printMsg("A root histogram to draw a display is not found!!!");

}


/****************************************************************
 * CLASS cannotUseToRootBeforeInitializing		 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

cannotUseToRootBeforeInitializingWarningMsg::cannotUseToRootBeforeInitializingWarningMsg() : analysisWarningMsg() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

cannotUseToRootBeforeInitializingWarningMsg::~cannotUseToRootBeforeInitializingWarningMsg() {

}

/****************************************************************
 * This method displayDirectories a warning message.	 		*
 ****************************************************************/

void cannotUseToRootBeforeInitializingWarningMsg::warningMsg() {

	printMsg("You cannot use ...ToRoot before initializing the analysis!!!");

}


/****************************************************************
 * CLASS trackWithHitsInCorrectLayerDistributionWarningMsg		*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

trackWithHitsInCorrectLayerDistributionWarningMsg::trackWithHitsInCorrectLayerDistributionWarningMsg() : analysisWarningMsg() {

	distribution    = NULL;
	numberOfEntries = 0;

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

trackWithHitsInCorrectLayerDistributionWarningMsg::trackWithHitsInCorrectLayerDistributionWarningMsg(unsigned int* distribution, unsigned short numberOfEntries) : analysisWarningMsg() {

	this->distribution    = distribution;
	this->numberOfEntries = numberOfEntries;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

trackWithHitsInCorrectLayerDistributionWarningMsg::~trackWithHitsInCorrectLayerDistributionWarningMsg() {

}

/****************************************************************
 * This method displayDirectories a warning message.	 		*
 ****************************************************************/

void trackWithHitsInCorrectLayerDistributionWarningMsg::warningMsg() {

	unsigned short i;
	bool           zeroDistribution;
	std::string    temp;
	unsigned int   numberOfTracks;
	char           buffer[intConversionDigits+1];

	zeroDistribution = true;
	temp             = "Distribution of the tracks which have the percentage of hits in the correct layer:\n";

	if (distribution != NULL) {

		numberOfTracks = 0;
		for (i = 0; i < numberOfEntries; i++)
			numberOfTracks += distribution[i];

		/* This is just to avoid the division with zero */
		if (numberOfTracks == 0)
			numberOfTracks = 1;

		for (i = 0; i < numberOfEntries; i++) {

			if (distribution[i] > 0) {

				zeroDistribution = false;
				if (i == 0) {

					temp += "Hits:  0% - <";
					uitos((unsigned int)((((double)(i + 1) * 100) / (double)numberOfEntries) + 0.5), buffer, 10, intConversionDigits);
					temp += buffer;
					temp += "%\t=> #Tracks: ";

				}
				else {

					temp += "Hits: ";
					uitos((unsigned int)((((double)i * 100) / (double)numberOfEntries) + 0.5), buffer, 10, intConversionDigits);
					temp += buffer;
					temp += "% - <";
					uitos((unsigned int)((((double)(i + 1) * 100) / (double)numberOfEntries) + 0.5), buffer, 10, intConversionDigits);
					temp += buffer;
					temp += "%\t=> #Tracks: ";

				}

				uitos((unsigned int)((((double)distribution[i] * 100) / (double)numberOfTracks) + 0.5), buffer, 10, intConversionDigits);
				temp += buffer;
				temp += "% (";
				uitos(distribution[i], buffer, 10, intConversionDigits);
				temp += buffer;
				temp += ")\n";

			}

		}

	}

	if (zeroDistribution)
		temp += "The distribution is zero allover!!!\n";

	printMsg(temp);

}


/****************************************************************
 * CLASS trackWithHitsWithCorrectSlopeDistributionWarningMsg	*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

trackWithHitsWithCorrectSlopeDistributionWarningMsg::trackWithHitsWithCorrectSlopeDistributionWarningMsg() : analysisWarningMsg() {

	distribution    = NULL;
	numberOfEntries = 0;

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

trackWithHitsWithCorrectSlopeDistributionWarningMsg::trackWithHitsWithCorrectSlopeDistributionWarningMsg(unsigned int* distribution, unsigned short numberOfEntries) : analysisWarningMsg() {

	this->distribution    = distribution;
	this->numberOfEntries = numberOfEntries;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

trackWithHitsWithCorrectSlopeDistributionWarningMsg::~trackWithHitsWithCorrectSlopeDistributionWarningMsg() {

}

/****************************************************************
 * This method displayDirectories a warning message.	 		*
 ****************************************************************/

void trackWithHitsWithCorrectSlopeDistributionWarningMsg::warningMsg() {

	unsigned short i;
	bool           zeroDistribution;
	std::string    temp;
	unsigned int   numberOfTracks;
	char           buffer[intConversionDigits+1];

	zeroDistribution = true;
	temp             = "Distribution of the tracks which have the percentage of hits with the correct slope:\n";

	if (distribution != NULL) {

		numberOfTracks = 0;
		for (i = 0; i < numberOfEntries; i++)
			numberOfTracks += distribution[i];

		/* This is just to avoid the division with zero */
		if (numberOfTracks == 0)
			numberOfTracks = 1;

		for (i = 0; i < numberOfEntries; i++) {

			if (distribution[i] > 0) {

				zeroDistribution = false;
				if (i == 0) {

					temp += "Hits:  0% - <";
					uitos((unsigned int)((((double)(i + 1) * 100) / (double)numberOfEntries) + 0.5), buffer, 10, intConversionDigits);
					temp += buffer;
					temp += "%\t=> #Tracks: ";

				}
				else {

					temp += "Hits: ";
					uitos((unsigned int)((((double)i * 100) / (double)numberOfEntries) + 0.5), buffer, 10, intConversionDigits);
					temp += buffer;
					temp += "% - <";
					uitos((unsigned int)((((double)(i + 1) * 100) / (double)numberOfEntries) + 0.5), buffer, 10, intConversionDigits);
					temp += buffer;
					temp += "%\t=> #Tracks: ";

				}

				uitos((unsigned int)((((double)distribution[i] * 100) / (double)numberOfTracks) + 0.5), buffer, 10, intConversionDigits);
				temp += buffer;
				temp += "% (";
				uitos(distribution[i], buffer, 10, intConversionDigits);
				temp += buffer;
				temp += ")\n";

			}

		}

	}

	if (zeroDistribution)
		temp += "The distribution is zero allover!!!\n";

	printMsg(temp);

}


/****************************************************************
 * CLASS trackWithHitsInLayerDistributionWarningMsg				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

trackWithHitsInLayerDistributionWarningMsg::trackWithHitsInLayerDistributionWarningMsg() : analysisWarningMsg() {

	distribution    = NULL;
	numberOfEntries = 0;

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

trackWithHitsInLayerDistributionWarningMsg::trackWithHitsInLayerDistributionWarningMsg(unsigned int* distribution, unsigned short numberOfEntries) : analysisWarningMsg() {

	this->distribution    = distribution;
	this->numberOfEntries = numberOfEntries;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

trackWithHitsInLayerDistributionWarningMsg::~trackWithHitsInLayerDistributionWarningMsg() {

}

/****************************************************************
 * This method displayDirectories a warning message.	 		*
 ****************************************************************/

void trackWithHitsInLayerDistributionWarningMsg::warningMsg() {

	unsigned short i;
	bool           zeroDistribution;
	std::string    temp;
	unsigned int   numberOfTracks;
	char           buffer[intConversionDigits+1];

	zeroDistribution = true;
	temp             = "Distribution of the tracks which have the percentage of hits in one layer:\n";

	if (distribution != NULL) {

		numberOfTracks = 0;
		for (i = 0; i < numberOfEntries; i++)
			numberOfTracks += distribution[i];

		/* This is just to avoid the division with zero */
		if (numberOfTracks == 0)
			numberOfTracks = 1;

		for (i = 0; i < numberOfEntries; i++) {

			if (distribution[i] > 0) {

				zeroDistribution = false;
				if (i == 0) {

					temp += "Hits:  0% - <";
					uitos((unsigned int)((((double)(i + 1) * 100) / (double)numberOfEntries) + 0.5), buffer, 10, intConversionDigits);
					temp += buffer;
					temp += "%\t=> #Tracks: ";

				}
				else {

					temp += "Hits: ";
					uitos((unsigned int)((((double)i * 100) / (double)numberOfEntries) + 0.5), buffer, 10, intConversionDigits);
					temp += buffer;
					temp += "% - <";
					uitos((unsigned int)((((double)(i + 1) * 100) / (double)numberOfEntries) + 0.5), buffer, 10, intConversionDigits);
					temp += buffer;
					temp += "%\t=> #Tracks: ";

				}

				uitos((unsigned int)((((double)distribution[i] * 100) / (double)numberOfTracks) + 0.5), buffer, 10, intConversionDigits);
				temp += buffer;
				temp += "% (";
				uitos(distribution[i], buffer, 10, intConversionDigits);
				temp += buffer;
				temp += ")\n";

			}

		}

	}

	if (zeroDistribution)
		temp += "The distribution is zero allover!!!\n";

	printMsg(temp);

}


/****************************************************************
 * CLASS arrayToDisplayIsNotAccessibleWarningMsg				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

arrayToDisplayIsNotAccessibleWarningMsg::arrayToDisplayIsNotAccessibleWarningMsg() : analysisWarningMsg() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

arrayToDisplayIsNotAccessibleWarningMsg::~arrayToDisplayIsNotAccessibleWarningMsg() {

}

/****************************************************************
 * This method displayDirectories a warning message.	 		*
 ****************************************************************/

void arrayToDisplayIsNotAccessibleWarningMsg::warningMsg() {

	printMsg("The array with the information to display is not accessible!!!");

}


/****************************************************************
 * CLASS trackWithSignatureDistributionWarningMsg				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

trackWithSignatureDistributionWarningMsg::trackWithSignatureDistributionWarningMsg() : analysisWarningMsg() {

	distribution    = NULL;
	numberOfEntries = 0;

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

trackWithSignatureDistributionWarningMsg::trackWithSignatureDistributionWarningMsg(unsigned int* distribution, unsigned short numberOfEntries) : analysisWarningMsg() {

	this->distribution    = distribution;
	this->numberOfEntries = numberOfEntries;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

trackWithSignatureDistributionWarningMsg::~trackWithSignatureDistributionWarningMsg() {

}

/****************************************************************
 * This method displayDirectories a warning message.	 		*
 ****************************************************************/

void trackWithSignatureDistributionWarningMsg::warningMsg() {

	unsigned short i;
	bitArray       signature;
	bool           zeroDistribution;
	std::string    temp;
	unsigned int   numberOfTracks;
	char           buffer[intConversionDigits+1];

	zeroDistribution = true;
	temp             = "Distribution of the tracks which have the signature of hits:\n";

	if (distribution != NULL) {

		numberOfTracks = 0;
		for (i = 0; i < numberOfEntries; i++)
			numberOfTracks += distribution[i];

		/* This is just to avoid the division with zero */
		if (numberOfTracks == 0)
			numberOfTracks = 1;

		for (i = 0; i < numberOfEntries; i++) {

			if (distribution > 0) {

				zeroDistribution = false;
				signature = bitArray(i);

				temp += "Signature: ";
				temp += signature.toString();
				temp += " \t=> #Tracks: ";
				uitos((unsigned int)((((double)distribution[i] * 100) / (double)numberOfTracks) + 0.5), buffer, 10, intConversionDigits);
				temp += buffer;
				temp += "% (";
				uitos(distribution[i], buffer, 10, intConversionDigits);
				temp += buffer;
				temp += ")\n";

			}

		}

	}

	if (zeroDistribution)
		temp += "The distribution is zero allover!!!\n";

	printMsg(temp);

}


/****************************************************************
 * CLASS trackWithHitsInCorrectCellDistributionWarningMsg		*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

trackWithHitsInCorrectCellDistributionWarningMsg::trackWithHitsInCorrectCellDistributionWarningMsg() : analysisWarningMsg() {

	distribution    = NULL;
	numberOfEntries = 0;

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

trackWithHitsInCorrectCellDistributionWarningMsg::trackWithHitsInCorrectCellDistributionWarningMsg(unsigned int* distribution, unsigned short numberOfEntries) : analysisWarningMsg() {

	this->distribution    = distribution;
	this->numberOfEntries = numberOfEntries;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

trackWithHitsInCorrectCellDistributionWarningMsg::~trackWithHitsInCorrectCellDistributionWarningMsg() {

}

/****************************************************************
 * This method displayDirectories a warning message.	 		*
 ****************************************************************/

void trackWithHitsInCorrectCellDistributionWarningMsg::warningMsg() {

	unsigned short i;
	bitArray       signature;
	bool           zeroDistribution;
	std::string    temp;
	unsigned int   numberOfTracks;
	char           buffer[intConversionDigits+1];

	zeroDistribution = true;
	temp             = "Distribution of the tracks which have the signature of hits in the correct cell:\n";

	if (distribution != NULL) {

		numberOfTracks = 0;
		for (i = 0; i < numberOfEntries; i++)
			numberOfTracks += distribution[i];

		/* This is just to avoid the division with zero */
		if (numberOfTracks == 0)
			numberOfTracks = 1;

		for (i = 0; i < numberOfEntries; i++) {

			if (distribution[i] > 0) {

				zeroDistribution = false;
				signature = bitArray(i);

				temp += "Signature: ";
				temp += signature.toString();
				temp += " \t=> #Tracks: ";
				uitos((unsigned int)((((double)distribution[i] * 100) / (double)numberOfTracks) + 0.5), buffer, 10, intConversionDigits);
				temp += buffer;
				temp += "% (";
				uitos(distribution[i], buffer, 10, intConversionDigits);
				temp += buffer;
				temp += ")\n";

			}

		}

	}

	if (zeroDistribution)
		temp += "The distribution is zero allover!!!\n";

	printMsg(temp);

}


/****************************************************************
 * CLASS trackWithHitsInCellDistributionWarningMsg				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

trackWithHitsInCellDistributionWarningMsg::trackWithHitsInCellDistributionWarningMsg() : analysisWarningMsg() {

	distribution    = NULL;
	numberOfEntries = 0;

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

trackWithHitsInCellDistributionWarningMsg::trackWithHitsInCellDistributionWarningMsg(unsigned int* distribution, unsigned short numberOfEntries) : analysisWarningMsg() {

	this->distribution    = distribution;
	this->numberOfEntries = numberOfEntries;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

trackWithHitsInCellDistributionWarningMsg::~trackWithHitsInCellDistributionWarningMsg() {

}

/****************************************************************
 * This method displayDirectories a warning message.	 		*
 ****************************************************************/

void trackWithHitsInCellDistributionWarningMsg::warningMsg() {

	unsigned short i;
	bitArray       signature;
	bool           zeroDistribution;
	std::string    temp;
	unsigned int   numberOfTracks;
	char           buffer[intConversionDigits+1];

	zeroDistribution = true;
	temp             = "Distribution of the tracks which have the signature of hits in one cell:\n";

	if (distribution != NULL) {

		numberOfTracks = 0;
		for (i = 0; i < numberOfEntries; i++)
			numberOfTracks += distribution[i];

		/* This is just to avoid the division with zero */
		if (numberOfTracks == 0)
			numberOfTracks = 1;

		for (i = 0; i < numberOfEntries; i++) {

			if (distribution[i] > 0) {

				zeroDistribution = false;
				signature = bitArray(i);

				temp += "Signature: ";
				temp += signature.toString();
				temp += " \t=> #Tracks: ";
				uitos((unsigned int)((((double)distribution[i] * 100) / (double)numberOfTracks) + 0.5), buffer, 10, intConversionDigits);
				temp += buffer;
				temp += "% (";
				uitos(distribution[i], buffer, 10, intConversionDigits);
				temp += buffer;
				temp += ")\n";

			}

		}

	}

	if (zeroDistribution)
		temp += "The distribution is zero allover!!!\n";

	printMsg(temp);

}


/****************************************************************
 * CLASS middleDistanceOfMaximumAndCorrectCellWarningMsg		*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

middleDistanceOfMaximumAndCorrectCellWarningMsg::middleDistanceOfMaximumAndCorrectCellWarningMsg() : analysisWarningMsg() {

	distance = 0;

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

middleDistanceOfMaximumAndCorrectCellWarningMsg::middleDistanceOfMaximumAndCorrectCellWarningMsg(double distance) : analysisWarningMsg() {

	this->distance = distance;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

middleDistanceOfMaximumAndCorrectCellWarningMsg::~middleDistanceOfMaximumAndCorrectCellWarningMsg() {

}

/****************************************************************
 * This method displays a warning message.		 				*
 ****************************************************************/

void middleDistanceOfMaximumAndCorrectCellWarningMsg::warningMsg() {

	std::string temp;
	char        buffer[doubleConversion+1];

	temp  = "The middle distance of the cell with the maximum hits and the correct cell for all tracks is: ";
	dtos(distance, buffer, doubleConversionDigits);
	temp += buffer;
	temp += "!!!";
	printMsg(temp);

}


/****************************************************************
 * CLASS middleDistanceOfMaximumAndCorrectLayerWarningMsg		*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

middleDistanceOfMaximumAndCorrectLayerWarningMsg::middleDistanceOfMaximumAndCorrectLayerWarningMsg() : analysisWarningMsg() {

	distance = 0;

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

middleDistanceOfMaximumAndCorrectLayerWarningMsg::middleDistanceOfMaximumAndCorrectLayerWarningMsg(double distance) : analysisWarningMsg() {

	this->distance = distance;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

middleDistanceOfMaximumAndCorrectLayerWarningMsg::~middleDistanceOfMaximumAndCorrectLayerWarningMsg() {

}

/****************************************************************
 * This method displays a warning message.		 				*
 ****************************************************************/

void middleDistanceOfMaximumAndCorrectLayerWarningMsg::warningMsg() {

	std::string temp;
	char        buffer[doubleConversion+1];

	temp  = "The middle distance of the layer with the maximum hits and the correct cell for all tracks is: ";
	dtos(distance, buffer, doubleConversionDigits);
	temp += buffer;
	temp += "!!!";
	printMsg(temp);

}


/****************************************************************
 * CLASS cannotFillNotExistingDisplayWarningMsg					*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

cannotFillNotExistingDisplayWarningMsg::cannotFillNotExistingDisplayWarningMsg() : analysisWarningMsg() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

cannotFillNotExistingDisplayWarningMsg::~cannotFillNotExistingDisplayWarningMsg() {

}

/****************************************************************
 * This method displays a warning message.		 				*
 ****************************************************************/

void cannotFillNotExistingDisplayWarningMsg::warningMsg() {
	
	printMsg("You cannot fill a display which is not existing!!!");

}


/****************************************************************
 * CLASS displayCannotBeReadFromFileWarningMsg					*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

displayCannotBeReadFromFileWarningMsg::displayCannotBeReadFromFileWarningMsg() : analysisWarningMsg() {

	name.clear();
	title.clear();
	fileName.clear();

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

displayCannotBeReadFromFileWarningMsg::displayCannotBeReadFromFileWarningMsg(std::string name, std::string title, std::string fileName) : analysisWarningMsg() {

	this->name     = name;
	this->title    = title;
	this->fileName = fileName;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

displayCannotBeReadFromFileWarningMsg::~displayCannotBeReadFromFileWarningMsg() {

}

/****************************************************************
 * This method displays a warning message.		 				*
 ****************************************************************/

void displayCannotBeReadFromFileWarningMsg::warningMsg() {
	
	std::string temp;

	temp  = "The display with the name '";
	temp += name;
	temp += "' and the title '";
	temp += title;
	temp += "' cannot be found either in the file '";
	temp += fileName;
	temp += "' nor in gDirectory!!!";
	
	printMsg(temp);

}


/****************************************************************
 * CLASS displayNotAccessibleWarningMsg							*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

displayNotAccessibleWarningMsg::displayNotAccessibleWarningMsg() : analysisWarningMsg() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

displayNotAccessibleWarningMsg::~displayNotAccessibleWarningMsg() {

}

/****************************************************************
 * This method displays a warning message.		 				*
 ****************************************************************/

void displayNotAccessibleWarningMsg::warningMsg() {
	
	printMsg("The display is not accessible!!!");

}


/****************************************************************
 * CLASS cannotWriteNotEnabledLayerWarningMsg	 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

cannotWriteNotEnabledLayerWarningMsg::cannotWriteNotEnabledLayerWarningMsg() : analysisWarningMsg() {

	layerToWrite = 0;
	enabledLayer = 0;

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

cannotWriteNotEnabledLayerWarningMsg::cannotWriteNotEnabledLayerWarningMsg(unsigned short layerToWrite, unsigned short enabledLayer) : analysisWarningMsg() {

	this->layerToWrite = layerToWrite;
	this->enabledLayer = enabledLayer;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

cannotWriteNotEnabledLayerWarningMsg::~cannotWriteNotEnabledLayerWarningMsg() {

}

/****************************************************************
 * This method displayDirectories a warning message.	 		*
 ****************************************************************/

void cannotWriteNotEnabledLayerWarningMsg::warningMsg() {

	std::string temp;
	char        buffer[shortConversionDigits+1];

	temp = "You cannot write the layer ";
	ustos(layerToWrite, buffer, 10, shortConversionDigits);
	temp += buffer;
	temp += " into the file, because you enabled layer ";
	ustos(enabledLayer, buffer, 10, shortConversionDigits);
	temp += buffer;
	temp += "!!!";
	printMsg(temp);

}


/****************************************************************
 * CLASS cannotShowNotEnabledLayerWarningMsg	 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

cannotShowNotEnabledLayerWarningMsg::cannotShowNotEnabledLayerWarningMsg() : analysisWarningMsg() {

	layerToShow  = 0;
	enabledLayer = 0;

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

cannotShowNotEnabledLayerWarningMsg::cannotShowNotEnabledLayerWarningMsg(unsigned short layerToShow, unsigned short enabledLayer) : analysisWarningMsg() {

	this->layerToShow  = layerToShow;
	this->enabledLayer = enabledLayer;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

cannotShowNotEnabledLayerWarningMsg::~cannotShowNotEnabledLayerWarningMsg() {

}

/****************************************************************
 * This method displayDirectories a warning message.	 		*
 ****************************************************************/

void cannotShowNotEnabledLayerWarningMsg::warningMsg() {

	std::string temp;
	char        buffer[shortConversionDigits+1];

	temp = "You cannot show the layer ";
	ustos(layerToShow, buffer, 10, shortConversionDigits);
	temp += buffer;
	temp += ", because you enabled layer ";
	ustos(enabledLayer, buffer, 10, shortConversionDigits);
	temp += buffer;
	temp += "!!!";
	printMsg(temp);

}


/****************************************************************
 * CLASS enableLayerIsOutOfRangeWarningMsg		 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

enableLayerIsOutOfRangeWarningMsg::enableLayerIsOutOfRangeWarningMsg() : analysisWarningMsg() {

	layer    = 0;
	layerMin = 0;
	layerMax = 0;

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

enableLayerIsOutOfRangeWarningMsg::enableLayerIsOutOfRangeWarningMsg(unsigned short layer, unsigned short layerMin, unsigned short layerMax) : analysisWarningMsg() {

	this->layer    = layer;
	this->layerMin = layerMin;
	this->layerMax = layerMax;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

enableLayerIsOutOfRangeWarningMsg::~enableLayerIsOutOfRangeWarningMsg() {

}

/****************************************************************
 * This method displayDirectories a warning message.	 		*
 ****************************************************************/

void enableLayerIsOutOfRangeWarningMsg::warningMsg() {

	std::string temp;
	char        buffer[shortConversionDigits+1];

	temp = "You cannot set the layer to ";
	ustos(layer, buffer, 10, shortConversionDigits);
	temp += buffer;
	temp += ", because this is out of range [";
	ustos(layerMin, buffer, 10, shortConversionDigits);
	temp += buffer;
	temp += ", ";
	ustos(layerMax, buffer, 10, shortConversionDigits);
	temp += buffer;
	temp += "]!!!";
	printMsg(temp);

}


/****************************************************************
 * CLASS cannotShowHistogramWithoutWritingToRootWarningMsg		*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

cannotShowHistogramWithoutWritingToRootWarningMsg::cannotShowHistogramWithoutWritingToRootWarningMsg() : analysisWarningMsg() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

cannotShowHistogramWithoutWritingToRootWarningMsg::~cannotShowHistogramWithoutWritingToRootWarningMsg() {

}

/****************************************************************
 * This method displays a warning message.		 				*
 ****************************************************************/

void cannotShowHistogramWithoutWritingToRootWarningMsg::warningMsg() {
	
	printMsg("For showing a histogram layer you have to enable at least this one to be written to file!!!");

}


/****************************************************************
 * CLASS trackWithMomentaErrorDistributionWarningMsg			*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

trackWithMomentaErrorDistributionWarningMsg::trackWithMomentaErrorDistributionWarningMsg() : analysisWarningMsg() {

	distribution    = NULL;
	numberOfEntries = 0;

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

trackWithMomentaErrorDistributionWarningMsg::trackWithMomentaErrorDistributionWarningMsg(unsigned int* distribution, unsigned short numberOfEntries) : analysisWarningMsg() {

	this->distribution    = distribution;
	this->numberOfEntries = numberOfEntries;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

trackWithMomentaErrorDistributionWarningMsg::~trackWithMomentaErrorDistributionWarningMsg() {

}

/****************************************************************
 * This method displayDirectories a warning message.	 		*
 ****************************************************************/

void trackWithMomentaErrorDistributionWarningMsg::warningMsg() {

	unsigned short i;
	bool           zeroDistribution;
	std::string    temp;
	unsigned int   numberOfTracks;
	char           buffer[intConversionDigits+1];

	zeroDistribution = true;
	temp  = "Distribution of the tracks which have the percentage of error in the momenta:\n";

	if (distribution != NULL) {

		numberOfTracks = 0;
		for (i = 0; i < numberOfEntries; i++)
			numberOfTracks += distribution[i];

		/* This is just to avoid the division with zero */
		if (numberOfTracks == 0)
			numberOfTracks = 1;

		for (i = 0; i < numberOfEntries; i++) {

			if (distribution[i] > 0) {

				zeroDistribution = false;
				if (i == 0) {

					temp += "Error:  0% - <";
					uitos((unsigned int)((((double)(i + 1) * 100) / (double)numberOfEntries) + 0.5), buffer, 10, intConversionDigits);
					temp += buffer;
					temp += "%\t=> #Tracks: ";

				}
				else {

					temp += "Error: ";
					uitos((unsigned int)((((double)i * 100) / (double)numberOfEntries) + 0.5), buffer, 10, intConversionDigits);
					temp += buffer;
					temp += "% - <";
					uitos((unsigned int)((((double)(i + 1) * 100) / (double)numberOfEntries) + 0.5), buffer, 10, intConversionDigits);
					temp += buffer;
					temp += "%\t=> #Tracks: ";

				}

				uitos((unsigned int)((((double)distribution[i] * 100) / (double)numberOfTracks) + 0.5), buffer, 10, intConversionDigits);
				temp += buffer;
				temp += "% (";
				uitos(distribution[i], buffer, 10, intConversionDigits);
				temp += buffer;
				temp += ")\n";

			}

		}

	}

	if (zeroDistribution)
		temp += "The distribution is zero allover!!!\n";

	printMsg(temp);

}


/****************************************************************
 * CLASS trackWithMomentaXDistributionWarningMsg				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

trackWithMomentaXDistributionWarningMsg::trackWithMomentaXDistributionWarningMsg() : analysisWarningMsg() {

	distribution    = NULL;
	numberOfEntries = 0;
	min             = 0;
	max             = 0;

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

trackWithMomentaXDistributionWarningMsg::trackWithMomentaXDistributionWarningMsg(unsigned int* distribution, unsigned short numberOfEntries, double min, double max) : analysisWarningMsg() {

	this->distribution    = distribution;
	this->numberOfEntries = numberOfEntries;
	this->min             = min;
	this->max             = max;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

trackWithMomentaXDistributionWarningMsg::~trackWithMomentaXDistributionWarningMsg() {

}

/****************************************************************
 * This method displayDirectories a warning message.	 		*
 ****************************************************************/

void trackWithMomentaXDistributionWarningMsg::warningMsg() {

	unsigned short i;
	bool           zeroDistribution;
	std::string    temp;
	unsigned int   numberOfTracks;
	char           doubleBuffer[doubleConversion+1];
	char           intBuffer[intConversionDigits+1];

	zeroDistribution = true;
	temp             = "Distribution of the tracks which have the momenta in X:\n";

	if (distribution != NULL) {

		numberOfTracks = 0;
		for (i = 0; i < numberOfEntries; i++)
			numberOfTracks += distribution[i];

		/* This is just to avoid the division with zero */
		if (numberOfTracks == 0)
			numberOfTracks = 1;

		for (i = 0; i < numberOfEntries; i++) {

			if (distribution[i] > 0) {

				zeroDistribution = false;
				temp += "Momentum: ";
				dtos(((double)i * (max - min)) / (double)(numberOfEntries - 1) + min, doubleBuffer, doubleConversionDigits);
				temp += doubleBuffer;
				temp += "GeV/c\t=> #Tracks: ";

				uitos((unsigned int)((((double)distribution[i] * 100) / (double)numberOfTracks) + 0.5), intBuffer, 10, intConversionDigits);
				temp += intBuffer;
				temp += "% (";
				uitos(distribution[i], intBuffer, 10, intConversionDigits);
				temp += intBuffer;
				temp += ")\n";

			}

		}

	}

	if (zeroDistribution)
		temp += "The distribution is zero allover!!!\n";

	printMsg(temp);

}


/****************************************************************
 * CLASS trackWithMomentaYDistributionWarningMsg				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

trackWithMomentaYDistributionWarningMsg::trackWithMomentaYDistributionWarningMsg() : analysisWarningMsg() {

	distribution    = NULL;
	numberOfEntries = 0;
	min             = 0;
	max             = 0;

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

trackWithMomentaYDistributionWarningMsg::trackWithMomentaYDistributionWarningMsg(unsigned int* distribution, unsigned short numberOfEntries, double min, double max) : analysisWarningMsg() {

	this->distribution    = distribution;
	this->numberOfEntries = numberOfEntries;
	this->min             = min;
	this->max             = max;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

trackWithMomentaYDistributionWarningMsg::~trackWithMomentaYDistributionWarningMsg() {

}

/****************************************************************
 * This method displayDirectories a warning message.	 		*
 ****************************************************************/

void trackWithMomentaYDistributionWarningMsg::warningMsg() {

	unsigned short i;
	bool           zeroDistribution;
	std::string    temp;
	unsigned int   numberOfTracks;
	char           doubleBuffer[doubleConversion+1];
	char           intBuffer[intConversionDigits+1];

	zeroDistribution = true;
	temp             = "Distribution of the tracks which have the momenta in Y:\n";

	if (distribution != NULL) {

		numberOfTracks = 0;
		for (i = 0; i < numberOfEntries; i++)
			numberOfTracks += distribution[i];

		/* This is just to avoid the division with zero */
		if (numberOfTracks == 0)
			numberOfTracks = 1;

		for (i = 0; i < numberOfEntries; i++) {

			if (distribution[i] > 0) {

				zeroDistribution = false;
				temp += "Momentum: ";
				dtos(((double)i * (max - min)) / (double)(numberOfEntries - 1) + min, doubleBuffer, doubleConversionDigits);
				temp += doubleBuffer;
				temp += "GeV/c\t=> #Tracks: ";

				uitos((unsigned int)((((double)distribution[i] * 100) / (double)numberOfTracks) + 0.5), intBuffer, 10, intConversionDigits);
				temp += intBuffer;
				temp += "% (";
				uitos(distribution[i], intBuffer, 10, intConversionDigits);
				temp += intBuffer;
				temp += ")\n";

			}

		}

	}

	if (zeroDistribution)
		temp += "The distribution is zero allover!!!\n";

	printMsg(temp);

}


/****************************************************************
 * CLASS trackWithMomentaZDistributionWarningMsg				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

trackWithMomentaZDistributionWarningMsg::trackWithMomentaZDistributionWarningMsg() : analysisWarningMsg() {

	distribution    = NULL;
	numberOfEntries = 0;
	min             = 0;
	max             = 0;

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

trackWithMomentaZDistributionWarningMsg::trackWithMomentaZDistributionWarningMsg(unsigned int* distribution, unsigned short numberOfEntries, double min, double max) : analysisWarningMsg() {

	this->distribution    = distribution;
	this->numberOfEntries = numberOfEntries;
	this->min             = min;
	this->max             = max;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

trackWithMomentaZDistributionWarningMsg::~trackWithMomentaZDistributionWarningMsg() {

}

/****************************************************************
 * This method displayDirectories a warning message.	 		*
 ****************************************************************/

void trackWithMomentaZDistributionWarningMsg::warningMsg() {

	unsigned short i;
	bool           zeroDistribution;
	std::string    temp;
	unsigned int   numberOfTracks;
	char           doubleBuffer[doubleConversion+1];
	char           intBuffer[intConversionDigits+1];

	zeroDistribution = true;
	temp  = "Distribution of the tracks which have the momenta in Z:\n";

	if (distribution != NULL) {

		numberOfTracks = 0;
		for (i = 0; i < numberOfEntries; i++)
			numberOfTracks += distribution[i];

		/* This is just to avoid the division with zero */
		if (numberOfTracks == 0)
			numberOfTracks = 1;

		for (i = 0; i < numberOfEntries; i++) {

			if (distribution[i] > 0) {

				zeroDistribution = false;
				temp += "Momentum: ";
				dtos(((double)i * (max - min)) / (double)(numberOfEntries - 1) + min, doubleBuffer, doubleConversionDigits);
				temp += doubleBuffer;
				temp += "GeV/c\t=> #Tracks: ";

				uitos((unsigned int)((((double)distribution[i] * 100) / (double)numberOfTracks) + 0.5), intBuffer, 10, intConversionDigits);
				temp += intBuffer;
				temp += "% (";
				uitos(distribution[i], intBuffer, 10, intConversionDigits);
				temp += intBuffer;
				temp += ")\n";

			}

		}

	}

	if (zeroDistribution)
		temp += "The distribution is zero allover!!!\n";

	printMsg(temp);

}


/****************************************************************
 * CLASS trackWithCoordinateXDistributionWarningMsg				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

trackWithCoordinateXDistributionWarningMsg::trackWithCoordinateXDistributionWarningMsg() : analysisWarningMsg() {

	distribution    = NULL;
	numberOfEntries = 0;

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

trackWithCoordinateXDistributionWarningMsg::trackWithCoordinateXDistributionWarningMsg(unsigned int* distribution, unsigned short numberOfEntries) : analysisWarningMsg() {

	this->distribution    = distribution;
	this->numberOfEntries = numberOfEntries;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

trackWithCoordinateXDistributionWarningMsg::~trackWithCoordinateXDistributionWarningMsg() {

}

/****************************************************************
 * This method displayDirectories a warning message.	 		*
 ****************************************************************/

void trackWithCoordinateXDistributionWarningMsg::warningMsg() {

	unsigned short i;
	bool           zeroDistribution;
	std::string    temp;
	unsigned int   numberOfTracks;
	char           buffer[intConversionDigits+1];

	zeroDistribution = true;
	temp             = "Distribution of the tracks which have the coordinate in X:\n";

	if (distribution != NULL) {

		numberOfTracks = 0;
		for (i = 0; i < numberOfEntries; i++)
			numberOfTracks += distribution[i];

		/* This is just to avoid the division with zero */
		if (numberOfTracks == 0)
			numberOfTracks = 1;

		for (i = 0; i < numberOfEntries; i++) {

			if (distribution[i] > 0) {

				zeroDistribution = false;
				uitos(i, buffer, 10, intConversionDigits);
				temp += buffer;
				temp += "\t=> #Tracks: ";

				uitos((unsigned int)((((double)distribution[i] * 100) / (double)numberOfTracks) + 0.5), buffer, 10, intConversionDigits);
				temp += buffer;
				temp += "% (";
				uitos(distribution[i], buffer, 10, intConversionDigits);
				temp += buffer;
				temp += ")\n";

			}

		}

	}

	if (zeroDistribution)
		temp += "The distribution is zero allover!!!\n";

	printMsg(temp);

}


/****************************************************************
 * CLASS trackWithCoordinateYDistributionWarningMsg				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

trackWithCoordinateYDistributionWarningMsg::trackWithCoordinateYDistributionWarningMsg() : analysisWarningMsg() {

	distribution    = NULL;
	numberOfEntries = 0;

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

trackWithCoordinateYDistributionWarningMsg::trackWithCoordinateYDistributionWarningMsg(unsigned int* distribution, unsigned short numberOfEntries) : analysisWarningMsg() {

	this->distribution    = distribution;
	this->numberOfEntries = numberOfEntries;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

trackWithCoordinateYDistributionWarningMsg::~trackWithCoordinateYDistributionWarningMsg() {

}

/****************************************************************
 * This method displayDirectories a warning message.	 		*
 ****************************************************************/

void trackWithCoordinateYDistributionWarningMsg::warningMsg() {

	unsigned short i;
	bool           zeroDistribution;
	std::string    temp;
	unsigned int   numberOfTracks;
	char           buffer[intConversionDigits+1];

	zeroDistribution = true;
	temp             = "Distribution of the tracks which have the coordinate in Y:\n";

	if (distribution != NULL) {

		numberOfTracks = 0;
		for (i = 0; i < numberOfEntries; i++)
			numberOfTracks += distribution[i];

		/* This is just to avoid the division with zero */
		if (numberOfTracks == 0)
			numberOfTracks = 1;

		for (i = 0; i < numberOfEntries; i++) {

			if (distribution[i] > 0) {

				zeroDistribution = false;
				uitos(i, buffer, 10, intConversionDigits);
				temp += buffer;
				temp += "\t=> #Tracks: ";

				uitos((unsigned int)((((double)distribution[i] * 100) / (double)numberOfTracks) + 0.5), buffer, 10, intConversionDigits);
				temp += buffer;
				temp += "% (";
				uitos(distribution[i], buffer, 10, intConversionDigits);
				temp += buffer;
				temp += ")\n";

			}

		}

	}

	if (zeroDistribution)
		temp += "The distribution is zero allover!!!\n";

	printMsg(temp);

}


/****************************************************************
 * CLASS trackWithCoordinateZDistributionWarningMsg				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

trackWithCoordinateZDistributionWarningMsg::trackWithCoordinateZDistributionWarningMsg() : analysisWarningMsg() {

	distribution    = NULL;
	numberOfEntries = 0;

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

trackWithCoordinateZDistributionWarningMsg::trackWithCoordinateZDistributionWarningMsg(unsigned int* distribution, unsigned short numberOfEntries) : analysisWarningMsg() {

	this->distribution    = distribution;
	this->numberOfEntries = numberOfEntries;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

trackWithCoordinateZDistributionWarningMsg::~trackWithCoordinateZDistributionWarningMsg() {

}

/****************************************************************
 * This method displayDirectories a warning message.	 		*
 ****************************************************************/

void trackWithCoordinateZDistributionWarningMsg::warningMsg() {

	unsigned short i;
	bool           zeroDistribution;
	std::string    temp;
	unsigned int   numberOfTracks;
	char           buffer[intConversionDigits+1];

	zeroDistribution = true;
	temp             = "Distribution of the tracks which have the coordinate in Z:\n";

	if (distribution != NULL) {

		numberOfTracks = 0;
		for (i = 0; i < numberOfEntries; i++)
			numberOfTracks += distribution[i];

		/* This is just to avoid the division with zero */
		if (numberOfTracks == 0)
			numberOfTracks = 1;

		for (i = 0; i < numberOfEntries; i++) {

			if (distribution[i] > 0) {

				zeroDistribution = false;
				uitos(i, buffer, 10, intConversionDigits);
				temp += buffer;
				temp += "\t=> #Tracks: ";

				uitos((unsigned int)((((double)distribution[i] * 100) / (double)numberOfTracks) + 0.5), buffer, 10, intConversionDigits);
				temp += buffer;
				temp += "% (";
				uitos(distribution[i], buffer, 10, intConversionDigits);
				temp += buffer;
				temp += ")\n";

			}

		}

	}

	if (zeroDistribution)
		temp += "The distribution is zero allover!!!\n";

	printMsg(temp);

}


/****************************************************************
 * CLASS trackWithMomentaDistributionWarningMsg					*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

trackWithMomentaDistributionWarningMsg::trackWithMomentaDistributionWarningMsg() : analysisWarningMsg() {

	distribution    = NULL;
	numberOfEntries = 0;

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

trackWithMomentaDistributionWarningMsg::trackWithMomentaDistributionWarningMsg(unsigned int* distribution, unsigned int numberOfEntries) : analysisWarningMsg() {

	this->distribution    = distribution;
	this->numberOfEntries = numberOfEntries;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

trackWithMomentaDistributionWarningMsg::~trackWithMomentaDistributionWarningMsg() {

}

/****************************************************************
 * This method displayDirectories a warning message.	 		*
 ****************************************************************/

void trackWithMomentaDistributionWarningMsg::warningMsg() {

	unsigned int i;
	bool         zeroDistribution;
	std::string  temp;
	char         buffer[intConversionDigits+1];

	zeroDistribution = true;
	temp  = "Distribution of the tracks which have the momenta:\n";

	if (distribution != NULL) {

		for (i = 0; i < numberOfEntries; i++) {

			if (distribution[i] > 0) {

				zeroDistribution = false;
				uitos(i, buffer, 10, intConversionDigits);
				temp += buffer;
				temp += "\t=> #Tracks: ";
				uitos(distribution[i], buffer, 10, intConversionDigits);
				temp += buffer;
				temp += ")\n";

			}

		}

	}

	if (zeroDistribution)
		temp += "The distribution is zero allover!!!\n";

	printMsg(temp);

}


/****************************************************************
 * CLASS trackWithCoordinateDistributionWarningMsg				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

trackWithCoordinateDistributionWarningMsg::trackWithCoordinateDistributionWarningMsg() : analysisWarningMsg() {

	distribution    = NULL;
	numberOfEntries = 0;

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

trackWithCoordinateDistributionWarningMsg::trackWithCoordinateDistributionWarningMsg(unsigned int* distribution, unsigned int numberOfEntries) : analysisWarningMsg() {

	this->distribution    = distribution;
	this->numberOfEntries = numberOfEntries;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

trackWithCoordinateDistributionWarningMsg::~trackWithCoordinateDistributionWarningMsg() {

}

/****************************************************************
 * This method displayDirectories a warning message.	 		*
 ****************************************************************/

void trackWithCoordinateDistributionWarningMsg::warningMsg() {

	unsigned int i;
	bool         zeroDistribution;
	std::string  temp;
	char         buffer[intConversionDigits+1];

	zeroDistribution = true;
	temp  = "Distribution of the tracks which have the coordinate:\n";

	if (distribution != NULL) {

		for (i = 0; i < numberOfEntries; i++) {

			if (distribution[i] > 0) {

				zeroDistribution = false;
				uitos(i, buffer, 10, intConversionDigits);
				temp += buffer;
				temp += "\t=> #Tracks: ";
				uitos(distribution[i], buffer, 10, intConversionDigits);
				temp += buffer;
				temp += ")\n";

			}

		}

	}

	if (zeroDistribution)
		temp += "The distribution is zero allover!!!\n";

	printMsg(temp);

}


/****************************************************************
 * CLASS numberOfTracksWhichCannotBeFoundWarningMsg				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

numberOfTracksWhichCannotBeFoundWarningMsg::numberOfTracksWhichCannotBeFoundWarningMsg() : analysisWarningMsg() {

	numberOfTracks = 0;

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

numberOfTracksWhichCannotBeFoundWarningMsg::numberOfTracksWhichCannotBeFoundWarningMsg(unsigned long numberOfTracks) : analysisWarningMsg() {

	this->numberOfTracks = numberOfTracks;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

numberOfTracksWhichCannotBeFoundWarningMsg::~numberOfTracksWhichCannotBeFoundWarningMsg() {

}

/****************************************************************
 * This method displayDirectories a warning message.	 		*
 ****************************************************************/

void numberOfTracksWhichCannotBeFoundWarningMsg::warningMsg() {

	std::string  temp;
	char         buffer[longConversionDigits+1];

	temp  = "There are ";
	ultos(numberOfTracks, buffer, 10, longConversionDigits);
	temp += buffer;
	temp += " tracks which cannot be found, because they are in the same histogram cell than another track!!!\n";

	printMsg(temp);

}


/****************************************************************
 * CLASS numberOfTracksWithGoodPrelutSignatureWarningMsg		*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

numberOfTracksWithGoodPrelutSignatureWarningMsg::numberOfTracksWithGoodPrelutSignatureWarningMsg() : analysisWarningMsg() {

	numberOfTracks = 0;

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

numberOfTracksWithGoodPrelutSignatureWarningMsg::numberOfTracksWithGoodPrelutSignatureWarningMsg(unsigned long numberOfTracks) : analysisWarningMsg() {

	this->numberOfTracks = numberOfTracks;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

numberOfTracksWithGoodPrelutSignatureWarningMsg::~numberOfTracksWithGoodPrelutSignatureWarningMsg() {

}

/****************************************************************
 * This method displayDirectories a warning message.	 		*
 ****************************************************************/

void numberOfTracksWithGoodPrelutSignatureWarningMsg::warningMsg() {

	std::string  temp;
	char         buffer[longConversionDigits+1];

	temp  = "There are ";
	ultos(numberOfTracks, buffer, 10, longConversionDigits);
	temp += buffer;
	temp += " tracks which have an acceptable signature belonging to the prelut!!!\n";

	printMsg(temp);

}


/****************************************************************
 * CLASS numberOfTracksWithGoodLutSignatureWarningMsg			*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

numberOfTracksWithGoodLutSignatureWarningMsg::numberOfTracksWithGoodLutSignatureWarningMsg() : analysisWarningMsg() {

	numberOfTracks = 0;

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

numberOfTracksWithGoodLutSignatureWarningMsg::numberOfTracksWithGoodLutSignatureWarningMsg(unsigned long numberOfTracks) : analysisWarningMsg() {

	this->numberOfTracks = numberOfTracks;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

numberOfTracksWithGoodLutSignatureWarningMsg::~numberOfTracksWithGoodLutSignatureWarningMsg() {

}

/****************************************************************
 * This method displayDirectories a warning message.	 		*
 ****************************************************************/

void numberOfTracksWithGoodLutSignatureWarningMsg::warningMsg() {

	std::string  temp;
	char         buffer[longConversionDigits+1];

	temp  = "There are ";
	ultos(numberOfTracks, buffer, 10, longConversionDigits);
	temp += buffer;
	temp += " tracks which have an acceptable signature belonging to the lut!!!\n";

	printMsg(temp);

}


/****************************************************************
 * CLASS numberOfGoodPrelutSignatureWarningMsg					*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

numberOfGoodPrelutSignatureWarningMsg::numberOfGoodPrelutSignatureWarningMsg() : analysisWarningMsg() {

	numberOfSignatures = 0;

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

numberOfGoodPrelutSignatureWarningMsg::numberOfGoodPrelutSignatureWarningMsg(unsigned long numberOfSignatures) : analysisWarningMsg() {

	this->numberOfSignatures = numberOfSignatures;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

numberOfGoodPrelutSignatureWarningMsg::~numberOfGoodPrelutSignatureWarningMsg() {

}

/****************************************************************
 * This method displayDirectories a warning message.	 		*
 ****************************************************************/

void numberOfGoodPrelutSignatureWarningMsg::warningMsg() {

	std::string  temp;
	char         buffer[longConversionDigits+1];

	temp  = "There are ";
	ultos(numberOfSignatures, buffer, 10, longConversionDigits);
	temp += buffer;
	temp += " signatures which are accepted belonging to the prelut!!!\n";

	printMsg(temp);

}


/****************************************************************
 * CLASS numberOfGoodLutSignatureWarningMsg						*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

numberOfGoodLutSignatureWarningMsg::numberOfGoodLutSignatureWarningMsg() : analysisWarningMsg() {

	numberOfSignatures = 0;

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

numberOfGoodLutSignatureWarningMsg::numberOfGoodLutSignatureWarningMsg(unsigned long numberOfSignatures) : analysisWarningMsg() {

	this->numberOfSignatures = numberOfSignatures;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

numberOfGoodLutSignatureWarningMsg::~numberOfGoodLutSignatureWarningMsg() {

}

/****************************************************************
 * This method displayDirectories a warning message.	 		*
 ****************************************************************/

void numberOfGoodLutSignatureWarningMsg::warningMsg() {

	std::string  temp;
	char         buffer[longConversionDigits+1];

	temp  = "There are ";
	ultos(numberOfSignatures, buffer, 10, longConversionDigits);
	temp += buffer;
	temp += " signatures which are acceptaed belonging to the lut!!!\n";

	printMsg(temp);

}


/****************************************************************
 * CLASS goodPrelutSignatureWarningMsg							*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

goodPrelutSignatureWarningMsg::goodPrelutSignatureWarningMsg() : analysisWarningMsg() {

	stringTable.clear();

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

goodPrelutSignatureWarningMsg::goodPrelutSignatureWarningMsg(std::string stringTable) : analysisWarningMsg() {

	this->stringTable = stringTable;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

goodPrelutSignatureWarningMsg::~goodPrelutSignatureWarningMsg() {

}

/****************************************************************
 * This method displayDirectories a warning message.	 		*
 ****************************************************************/

void goodPrelutSignatureWarningMsg::warningMsg() {

	std::string  temp;

	temp  = "The good signatures belonging to the prelut are:\n";
	temp += stringTable;

	printMsg(temp);

}


/****************************************************************
 * CLASS goodLutSignatureWarningMsg								*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

goodLutSignatureWarningMsg::goodLutSignatureWarningMsg() : analysisWarningMsg() {

	stringTable.clear();

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

goodLutSignatureWarningMsg::goodLutSignatureWarningMsg(std::string stringTable) : analysisWarningMsg() {

	this->stringTable = stringTable;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

goodLutSignatureWarningMsg::~goodLutSignatureWarningMsg() {

}

/****************************************************************
 * This method displayDirectories a warning message.	 		*
 ****************************************************************/

void goodLutSignatureWarningMsg::warningMsg() {

	std::string  temp;

	temp  = "The good signatures belonging to the lut are:\n";
	temp += stringTable;

	printMsg(temp);

}


/****************************************************************
 * CLASS usedTablesWarningMsg									*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

usedTablesWarningMsg::usedTablesWarningMsg() : analysisWarningMsg() {

	stringTable.clear();

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

usedTablesWarningMsg::usedTablesWarningMsg(std::string stringTable) : analysisWarningMsg() {

	this->stringTable = stringTable;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

usedTablesWarningMsg::~usedTablesWarningMsg() {

}

/****************************************************************
 * This method displayDirectories a warning message.	 		*
 ****************************************************************/

void usedTablesWarningMsg::warningMsg() {

	std::string  temp;

	temp  = "The used tables are:\n";
	temp += stringTable;

	printMsg(temp);

}


/****************************************************************
 * CLASS trackWithSameCoordinateDistributionWarningMsg			*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

trackWithSameCoordinateDistributionWarningMsg::trackWithSameCoordinateDistributionWarningMsg() : analysisWarningMsg() {

	distribution    = NULL;
	numberOfEntries = 0;

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

trackWithSameCoordinateDistributionWarningMsg::trackWithSameCoordinateDistributionWarningMsg(unsigned int* distribution, unsigned int numberOfEntries) : analysisWarningMsg() {

	this->distribution    = distribution;
	this->numberOfEntries = numberOfEntries;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

trackWithSameCoordinateDistributionWarningMsg::~trackWithSameCoordinateDistributionWarningMsg() {

}

/****************************************************************
 * This method displayDirectories a warning message.	 		*
 ****************************************************************/

void trackWithSameCoordinateDistributionWarningMsg::warningMsg() {

	bool         nothingDetected;
	std::string  temp;
	char         buffer[intConversionDigits+1];

	nothingDetected  = true;
	temp             = "Distribution of the tracks which have the same coordinate:\n";
	for (unsigned int i = 0; i < numberOfEntries; i++) {

		if (distribution != NULL) {

			if (distribution[i] > 1) {

				nothingDetected = false;

				uitos(i, buffer, 10, intConversionDigits);
				temp += buffer;
				temp += "\t=> #Tracks: ";

				uitos(distribution[i], buffer, 10, intConversionDigits);
				temp += buffer;
				temp += "\n";

			}

		}

	}

	if (nothingDetected) {

		temp += "No coordinate consisting of more than one track found!!!";

	}

	printMsg(temp);

}


/****************************************************************
 * CLASS volumeNotFoundWarningMsg				 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

volumeNotFoundWarningMsg::volumeNotFoundWarningMsg() : analysisWarningMsg() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

volumeNotFoundWarningMsg::~volumeNotFoundWarningMsg() {

}

/****************************************************************
 * This method displayDirectories a warning message.	 		*
 ****************************************************************/

void volumeNotFoundWarningMsg::warningMsg() {

	printMsg("The volume in the TGeoManager is not found!!!");

}


/****************************************************************
 * CLASS listOfVolumesNotFoundWarningMsg		 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

listOfVolumesNotFoundWarningMsg::listOfVolumesNotFoundWarningMsg() : analysisWarningMsg() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

listOfVolumesNotFoundWarningMsg::~listOfVolumesNotFoundWarningMsg() {

}

/****************************************************************
 * This method displayDirectories a warning message.	 		*
 ****************************************************************/

void listOfVolumesNotFoundWarningMsg::warningMsg() {

	printMsg("The list of the volumes in the TGeoManager is not found!!!");

}


/****************************************************************
 * CLASS listOfTracksNotFoundWarningMsg		 					*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

listOfTracksNotFoundWarningMsg::listOfTracksNotFoundWarningMsg() : analysisWarningMsg() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

listOfTracksNotFoundWarningMsg::~listOfTracksNotFoundWarningMsg() {

}

/****************************************************************
 * This method displayDirectories a warning message.	 		*
 ****************************************************************/

void listOfTracksNotFoundWarningMsg::warningMsg() {

	printMsg("The list of the tracks in the TGeoManager is not found!!!");

}


/****************************************************************
 * CLASS currentTrackNotFoundWarningMsg		 					*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

currentTrackNotFoundWarningMsg::currentTrackNotFoundWarningMsg() : analysisWarningMsg() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

currentTrackNotFoundWarningMsg::~currentTrackNotFoundWarningMsg() {

}

/****************************************************************
 * This method displayDirectories a warning message.	 		*
 ****************************************************************/

void currentTrackNotFoundWarningMsg::warningMsg() {

	printMsg("The list of the tracks in the TGeoManager is not found!!!");

}


/****************************************************************
 * CLASS timeViolationWarningMsg		 						*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

timeViolationWarningMsg::timeViolationWarningMsg() : analysisWarningMsg() {

	time1 = 0;
	time2 = 0;

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

timeViolationWarningMsg::timeViolationWarningMsg(double time1, double time2) : analysisWarningMsg() {

	this->time1 = time1;
	this->time2 = time2;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

timeViolationWarningMsg::~timeViolationWarningMsg() {

}

/****************************************************************
 * This method displayDirectories a warning message.	 		*
 ****************************************************************/

void timeViolationWarningMsg::warningMsg() {

	std::string temp;
	char        buffer[doubleConversion+1];

	temp  = "The time for the added point is not acceptable because it is not rising. The violating time is ";
	dtos(time2, buffer, doubleConversionDigits);
	temp += buffer;
	temp += " while the last correct time was ";
	dtos(time1, buffer, doubleConversionDigits);
	temp += buffer;
	temp += "!!!";
	
	printMsg(temp);

}


/****************************************************************
 * CLASS numberOfTracksInAllLayersWarningMsg					*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

numberOfTracksInAllLayersWarningMsg::numberOfTracksInAllLayersWarningMsg() : analysisWarningMsg() {

	distribution                  = NULL;
	numberOfEntries               = 0;
	numberOfSummedEventsToAverage = 0;

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

numberOfTracksInAllLayersWarningMsg::numberOfTracksInAllLayersWarningMsg(unsigned int* distribution, unsigned int numberOfEntries, unsigned int numberOfSummedEventsToAverage) : analysisWarningMsg() {

	this->distribution                  = distribution;
	this->numberOfEntries               = numberOfEntries;
	this->numberOfSummedEventsToAverage = numberOfSummedEventsToAverage;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

numberOfTracksInAllLayersWarningMsg::~numberOfTracksInAllLayersWarningMsg() {

}

/****************************************************************
 * This method displayDirectories a warning message.	 		*
 ****************************************************************/

void numberOfTracksInAllLayersWarningMsg::warningMsg() {

	bool         nothingDetected;
	std::string  temp;
	char         buffer[intConversionDigits+1];

	/* This is just to avoid the division with zero */
	if (numberOfSummedEventsToAverage == 0)
		numberOfSummedEventsToAverage = 1;

	nothingDetected  = true;
	temp             = "Distribution of the found tracks per histogram layer:\n";
	for (unsigned int i = 0; i < numberOfEntries; i++) {

		if (distribution != NULL) {

			if (distribution[i] > 0) {

				nothingDetected = false;

				uitos(i, buffer, 10, intConversionDigits);
				temp += buffer;
				temp += "\t=> #Tracks: ";

				uitos((unsigned int)((double)distribution[i] / (double)numberOfSummedEventsToAverage), buffer, 10, intConversionDigits);
				temp += buffer;
				temp += "\n";

			}

		}

	}

	if (nothingDetected) {

		temp += "No track is found!!!";

	}

	printMsg(temp);

}


/****************************************************************
 * CLASS numberOfTrackDensitiesInAllLayersWarningMsg			*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

numberOfTrackDensitiesInAllLayersWarningMsg::numberOfTrackDensitiesInAllLayersWarningMsg() : analysisWarningMsg() {

	distribution                  = NULL;
	numberOfEntries               = 0;
	numberOfSummedEventsToAverage = 0;

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

numberOfTrackDensitiesInAllLayersWarningMsg::numberOfTrackDensitiesInAllLayersWarningMsg(int* distribution, unsigned int numberOfEntries, unsigned int numberOfSummedEventsToAverage) : analysisWarningMsg() {

	this->distribution                  = distribution;
	this->numberOfEntries               = numberOfEntries;
	this->numberOfSummedEventsToAverage = numberOfSummedEventsToAverage;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

numberOfTrackDensitiesInAllLayersWarningMsg::~numberOfTrackDensitiesInAllLayersWarningMsg() {

}

/****************************************************************
 * This method displayDirectories a warning message.	 		*
 ****************************************************************/

void numberOfTrackDensitiesInAllLayersWarningMsg::warningMsg() {

	bool         nothingDetected;
	std::string  temp;
	char         buffer[intConversionDigits+1];

	/* This is just to avoid the division with zero */
	if (numberOfSummedEventsToAverage == 0)
		numberOfSummedEventsToAverage = 1;

	nothingDetected  = true;
	temp             = "Distribution of the found track densities per histogram layer:\n";
	for (unsigned int i = 0; i < numberOfEntries; i++) {

		if (distribution != NULL) {

			if (distribution[i] != 0) {

				nothingDetected = false;

				uitos(i, buffer, 10, intConversionDigits);
				temp += buffer;
				temp += "\t=> #Tracks: ";

				itos((int)((double)distribution[i] / (double)numberOfSummedEventsToAverage), buffer, 10, intConversionDigits);
				temp += buffer;
				temp += "\n";

			}

		}

	}

	if (nothingDetected) {

		temp += "The track density is equal for each layer!!!";

	}

	printMsg(temp);

}


/****************************************************************
 * CLASS numberOfMinimalTracksInAllColumnsWarningMsg			*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

numberOfMinimalTracksInAllColumnsWarningMsg::numberOfMinimalTracksInAllColumnsWarningMsg() : analysisWarningMsg() {

	distribution    = NULL;
	numberOfEntries = 0;

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

numberOfMinimalTracksInAllColumnsWarningMsg::numberOfMinimalTracksInAllColumnsWarningMsg(unsigned int* distribution, unsigned int numberOfEntries) : analysisWarningMsg() {

	this->distribution    = distribution;
	this->numberOfEntries = numberOfEntries;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

numberOfMinimalTracksInAllColumnsWarningMsg::~numberOfMinimalTracksInAllColumnsWarningMsg() {

}

/****************************************************************
 * This method displayDirectories a warning message.	 		*
 ****************************************************************/

void numberOfMinimalTracksInAllColumnsWarningMsg::warningMsg() {

	bool         nothingDetected;
	std::string  temp;
	unsigned int sum;
	char         buffer[intConversionDigits+1];

	nothingDetected  = true;
	temp             = "Distribution of the minimal number of found tracks per histogram column in all layers:\n";
	sum              = 0;
	for (unsigned int i = 0; i < numberOfEntries; i++) {

		if (distribution != NULL) {

			if (distribution[i] > 0) {

			sum      += distribution[i];

				nothingDetected = false;

				uitos(i, buffer, 10, intConversionDigits);
				temp += buffer;
				temp += "\t=> #Tracks: ";

				uitos(distribution[i], buffer, 10, intConversionDigits);
				temp += buffer;
				temp += "\n";

			}

		}

	}

	if (nothingDetected) {

		temp += "No track is found!!!";

	}
	else {

		temp += "There are ";
		uitos(sum, buffer, 10, intConversionDigits);
		temp += buffer;
		temp += " tracks found in the worst case in one layer.";

	}

	printMsg(temp);

}


/****************************************************************
 * CLASS numberOfAverageTracksInAllColumnsWarningMsg			*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

numberOfAverageTracksInAllColumnsWarningMsg::numberOfAverageTracksInAllColumnsWarningMsg() : analysisWarningMsg() {

	distribution                  = NULL;
	numberOfEntries               = 0;
	numberOfSummedEventsToAverage = 0;

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

numberOfAverageTracksInAllColumnsWarningMsg::numberOfAverageTracksInAllColumnsWarningMsg(double* distribution, unsigned int numberOfEntries, unsigned int numberOfSummedEventsToAverage) : analysisWarningMsg() {

	this->distribution                  = distribution;
	this->numberOfEntries               = numberOfEntries;
	this->numberOfSummedEventsToAverage = numberOfSummedEventsToAverage;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

numberOfAverageTracksInAllColumnsWarningMsg::~numberOfAverageTracksInAllColumnsWarningMsg() {

}

/****************************************************************
 * This method displayDirectories a warning message.	 		*
 ****************************************************************/

void numberOfAverageTracksInAllColumnsWarningMsg::warningMsg() {

	unsigned int i;
	bool         nothingDetected;
	std::string  temp;
	double       numberOfTracks;
	char         buffer[doubleConversion+1];

	nothingDetected  = true;
	temp             = "Distribution of the average number of found tracks per histogram column in all layers:\n";

	if (distribution != NULL) {

		numberOfTracks = 0;
		for (i = 0; i < numberOfEntries; i++)
			if (distribution[i] > 0)
				numberOfTracks += distribution[i];

		/* This is just to avoid the division with zero */
		if (numberOfTracks == 0)
			numberOfTracks = 1;

		for (i = 0; i < numberOfEntries; i++) {

			if (distribution[i] > 0) {

				nothingDetected = false;

				uitos(i, buffer, 10, intConversionDigits);
				temp += buffer;
				temp += "\t=> #Tracks: ";

				dtos((distribution[i] * 100) / numberOfTracks, buffer, doubleConversionDigits);
				temp += buffer;
				temp += "\n";

			}

		}

	}

	if (nothingDetected) {

		temp += "No track is found!!!";

	}
	else {

		/* This is just to avoid the division with zero */
		if (numberOfSummedEventsToAverage == 0)
			numberOfSummedEventsToAverage = 1;

		temp += "percentage of tracks [%] (100%/Lage=";
		ustos((unsigned short)((numberOfTracks / (double)numberOfSummedEventsToAverage) + 0.5), buffer, 10, shortConversionDigits);
		temp += buffer;
		temp += ")";

	}

	printMsg(temp);

}


/****************************************************************
 * CLASS numberOfMaximalTracksInAllColumnsWarningMsg			*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

numberOfMaximalTracksInAllColumnsWarningMsg::numberOfMaximalTracksInAllColumnsWarningMsg() : analysisWarningMsg() {

	distribution    = NULL;
	numberOfEntries = 0;

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

numberOfMaximalTracksInAllColumnsWarningMsg::numberOfMaximalTracksInAllColumnsWarningMsg(unsigned int* distribution, unsigned int numberOfEntries) : analysisWarningMsg() {

	this->distribution    = distribution;
	this->numberOfEntries = numberOfEntries;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

numberOfMaximalTracksInAllColumnsWarningMsg::~numberOfMaximalTracksInAllColumnsWarningMsg() {

}

/****************************************************************
 * This method displayDirectories a warning message.	 		*
 ****************************************************************/

void numberOfMaximalTracksInAllColumnsWarningMsg::warningMsg() {

	bool         nothingDetected;
	std::string  temp;
	unsigned int sum;
	char         buffer[intConversionDigits+1];

	nothingDetected  = true;
	temp             = "Distribution of the maximal number of found tracks per histogram column in all layers:\n";
	sum              = 0;
	for (unsigned int i = 0; i < numberOfEntries; i++) {

		if (distribution != NULL) {

			sum      += distribution[i];

			if (distribution[i] > 0) {

				nothingDetected = false;

				uitos(i, buffer, 10, intConversionDigits);
				temp += buffer;
				temp += "\t=> #Tracks: ";

				uitos(distribution[i], buffer, 10, intConversionDigits);
				temp += buffer;
				temp += "\n";

			}

		}

	}

	if (nothingDetected) {

		temp += "No track is found!!!";

	}
	else {

		temp += "There are ";
		uitos(sum, buffer, 10, intConversionDigits);
		temp += buffer;
		temp += " tracks found in the worst case in one layer.";

	}

	printMsg(temp);

}


/****************************************************************
 * CLASS sizeOfSeparatorFifosWarningMsg							*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

sizeOfSeparatorFifosWarningMsg::sizeOfSeparatorFifosWarningMsg() : analysisWarningMsg() {

	distribution    = NULL;
	numberOfEntries = 0;

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

sizeOfSeparatorFifosWarningMsg::sizeOfSeparatorFifosWarningMsg(unsigned int* distribution, unsigned int numberOfEntries) : analysisWarningMsg() {

	this->distribution    = distribution;
	this->numberOfEntries = numberOfEntries;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

sizeOfSeparatorFifosWarningMsg::~sizeOfSeparatorFifosWarningMsg() {

}

/****************************************************************
 * This method displayDirectories a warning message.	 		*
 ****************************************************************/

void sizeOfSeparatorFifosWarningMsg::warningMsg() {

	bool         nothingDetected;
	std::string  temp;
	unsigned int value;
	unsigned int counter;
	unsigned int sum;
	char         buffer[intConversionDigits+1];

	nothingDetected  = true;
	temp             = "Distribution of the assumed histogram column FIFO sizes for the separator of the found tracks:\n";
	value            = 0;
	counter          = 0;
	sum              = 0;
	for (unsigned int i = 0; i < numberOfEntries; i++) {

		if (distribution != NULL) {

			value    = std::min(counter, distribution[i]);

			counter += distribution[i];

			sum     += value;

			if (value > 0) {

				nothingDetected = false;

				uitos(i, buffer, 10, intConversionDigits);
				temp += buffer;
				temp += "\t=> Fifo-size: ";

				uitos(value, buffer, 10, intConversionDigits);
				temp += buffer;
				temp += "\n";

			}

		}

	}

	if (nothingDetected) {

		temp += "No Fifo-size can be evaluated!!!";

	}
	else {

		temp += "There are ";
		uitos(sum, buffer, 10, intConversionDigits);
		temp += buffer;
		temp += " FIFOs needed at all.";

	}

	printMsg(temp);

}


/****************************************************************
 * CLASS youCannotEnableTrackWithSameMomentaAndCoordinateDistributionWarningMsg*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

youCannotEnableTrackWithSameMomentaAndCoordinateDistributionWarningMsg::youCannotEnableTrackWithSameMomentaAndCoordinateDistributionWarningMsg() : analysisWarningMsg() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

youCannotEnableTrackWithSameMomentaAndCoordinateDistributionWarningMsg::~youCannotEnableTrackWithSameMomentaAndCoordinateDistributionWarningMsg() {

}

/****************************************************************
 * This method displays a warning message.		 				*
 ****************************************************************/

void youCannotEnableTrackWithSameMomentaAndCoordinateDistributionWarningMsg::warningMsg() {

	printMsg("The distribution with the tracks with the same momentas cannot be enabled in combination with the distribution with the same coordinates!!!");

}
