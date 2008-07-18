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
//     - implements methods to visualize tracks
//
// *******************************************************************
//
// $Author: csteinle $
// $Date: 2007-04-26 12:50:08 $
// $Revision: 1.3 $
//
// *******************************************************************/


#include "../../MiscLIB/include/globalWarningMsg.h"
#include "../include/analysisWarningMsg.h"
#include "../include/visualAnalysis.h"
#include "TObjArray.h"
#include "TGeoVolume.h"


/* **************************************************************
 * CLASS visualAnalysis					 						*
 * **************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

visualAnalysis::visualAnalysis() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

visualAnalysis::~visualAnalysis() {

}

/****************************************************************
 * method sets the visibility for the TGeoManager volumes to	*
 * display														*
 ****************************************************************/

void visualAnalysis::setupDisplay(std::string name1, std::string name2, std::string name3, std::string name4) {

	TObjArray*  listOfVolumes;
	TGeoVolume* actualVolume;
	std::string actualVolumeName;

	if (gGeoManager != NULL) {

		listOfVolumes = gGeoManager->GetListOfVolumes();

		if (listOfVolumes != NULL) {

			for (int i = 0; i < listOfVolumes->GetEntries(); i++) {

				actualVolume = (TGeoVolume*)listOfVolumes->At(i);
				if (actualVolume != NULL) {

					actualVolumeName = actualVolume->GetName();

					if (((actualVolumeName.find(name1, 0) == std::basic_string<char>::npos) || name1.empty()) &&
						((actualVolumeName.find(name2, 0) == std::basic_string<char>::npos) || name2.empty()) &&
						((actualVolumeName.find(name3, 0) == std::basic_string<char>::npos) || name3.empty()) &&
						((actualVolumeName.find(name4, 0) == std::basic_string<char>::npos) || name4.empty()))
						actualVolume->InvisibleAll(true);
					else
						actualVolume->InvisibleAll(false);

				}
				else {

					volumeNotFoundWarningMsg* volumeNotFound = new volumeNotFoundWarningMsg();
					volumeNotFound->warningMsg();
					if(volumeNotFound != NULL) {
						delete volumeNotFound;
						volumeNotFound = NULL;
					}

				}

			}

		}
		else {

			listOfVolumesNotFoundWarningMsg* listOfVolumesNotFound = new listOfVolumesNotFoundWarningMsg();
			listOfVolumesNotFound->warningMsg();
			if(listOfVolumesNotFound != NULL) {
				delete listOfVolumesNotFound;
				listOfVolumesNotFound = NULL;
			}

		}

	}
	else {

		geoManagerNotFoundWarningMsg* geoManagerNotFound = new geoManagerNotFoundWarningMsg();
		geoManagerNotFound->warningMsg();
		if(geoManagerNotFound != NULL) {
			delete geoManagerNotFound;
			geoManagerNotFound = NULL;
		}

	}

}

/****************************************************************
 * method removes all tracks from the TGeoManager				*
 ****************************************************************/

void visualAnalysis::removeTracks() {

	TObjArray* listOfTracks;

	if (gGeoManager != NULL) {

		listOfTracks = gGeoManager->GetListOfTracks();
		if (listOfTracks != NULL)
			listOfTracks->Clear();
		else {

			listOfTracksNotFoundWarningMsg* listOfTracksNotFound = new listOfTracksNotFoundWarningMsg();
			listOfTracksNotFound->warningMsg();
			if(listOfTracksNotFound != NULL) {
				delete listOfTracksNotFound;
				listOfTracksNotFound = NULL;
			}

		}

	}
	else {

		geoManagerNotFoundWarningMsg* geoManagerNotFound = new geoManagerNotFoundWarningMsg();
		geoManagerNotFound->warningMsg();
		if(geoManagerNotFound != NULL) {
			delete geoManagerNotFound;
			geoManagerNotFound = NULL;
		}

	}

}

/****************************************************************
 * method adds the track to the TGeoManager						*
 ****************************************************************/

void visualAnalysis::addCurrentTrack(int pdgCode, int trackIndex) {

	TObjArray* listOfTracks;
	int        index;

	if (gGeoManager != NULL) {

		if (trackIndex < 0) {

			listOfTracks = gGeoManager->GetListOfTracks();
			if (listOfTracks != NULL)
				index = gGeoManager->AddTrack(listOfTracks->GetEntries(), pdgCode);
			else {

				listOfTracksNotFoundWarningMsg* listOfTracksNotFound = new listOfTracksNotFoundWarningMsg();
				listOfTracksNotFound->warningMsg();
				if(listOfTracksNotFound != NULL) {
					delete listOfTracksNotFound;
					listOfTracksNotFound = NULL;
				}

				index = gGeoManager->AddTrack(0, pdgCode);

			}

		}
		else
			index = gGeoManager->AddTrack(trackIndex, pdgCode);

		gGeoManager->SetCurrentTrack(index);

	}
	else {

		geoManagerNotFoundWarningMsg* geoManagerNotFound = new geoManagerNotFoundWarningMsg();
		geoManagerNotFound->warningMsg();
		if(geoManagerNotFound != NULL) {
			delete geoManagerNotFound;
			geoManagerNotFound = NULL;
		}

	}

}

/****************************************************************
 * method adds a point to the current track in the TGeoManager	*
 ****************************************************************/

void visualAnalysis::addPointToCurrentTrack(double x, double y, double z, double time) {

	TVirtualGeoTrack* track;
	double            xLast;
	double            yLast;
	double            zLast;
	double            timeLast;

	if (gGeoManager != NULL) {

		track = gGeoManager->GetCurrentTrack();
		if (track != NULL) {

			if (track->GetNpoints() > 0)
				track->GetLastPoint(xLast, yLast, zLast, timeLast);
			else
				timeLast = 0;

			if (time < 0)
				track->AddPoint(x, y, z, timeLast + 1);
			else {

				if (time > timeLast)
					track->AddPoint(x, y, z, time);
				else {

					timeViolationWarningMsg* timeViolation = new timeViolationWarningMsg(timeLast, time);
					timeViolation->warningMsg();
					if(timeViolation != NULL) {
						delete timeViolation;
						timeViolation = NULL;
					}

				}

			}

		}
		else {

			currentTrackNotFoundWarningMsg* currentTrackNotFound = new currentTrackNotFoundWarningMsg();
			currentTrackNotFound->warningMsg();
			if(currentTrackNotFound != NULL) {
				delete currentTrackNotFound;
				currentTrackNotFound = NULL;
			}

		}

	}
	else {

		geoManagerNotFoundWarningMsg* geoManagerNotFound = new geoManagerNotFoundWarningMsg();
		geoManagerNotFound->warningMsg();
		if(geoManagerNotFound != NULL) {
			delete geoManagerNotFound;
			geoManagerNotFound = NULL;
		}

	}

}

/****************************************************************
 * method draws all tracks from the TGeoManager					*
 ****************************************************************/

void visualAnalysis::drawTracks() {

	if (gGeoManager != NULL)
		gGeoManager->DrawTracks();
	else {

		geoManagerNotFoundWarningMsg* geoManagerNotFound = new geoManagerNotFoundWarningMsg();
		geoManagerNotFound->warningMsg();
		if(geoManagerNotFound != NULL) {
			delete geoManagerNotFound;
			geoManagerNotFound = NULL;
		}

	}

}
