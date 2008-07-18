/* *******************************************************************
// (C)opyright 2004
// 
// Institute of Computer Science V
// Prof. M�nner
// University of Mannheim, Germany
// 
// *******************************************************************
// 
// Designer(s):   Steinle / Gl��
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
//     - consists of the information for one detector station
//
// *******************************************************************
//
// $Author: csteinle $
// $Date: 2007-05-14 15:44:00 $
// $Revision: 1.2 $
//
// *******************************************************************/


#include "../include/dataRootObjectWarningMsg.h"
#include "../include/trackfinderInputStation.h"


/****************************************************************
 * CLASS trackfinderInputStation				 				*
 ****************************************************************/


/****************************************************************
 * Default constructor											*
 ****************************************************************/

trackfinderInputStation::trackfinderInputStation() {

	id       = 0;
	distance = 0;
	removeType();
	masked   = false;
	index    = 0;

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

trackfinderInputStation::trackfinderInputStation(const trackfinderInputStation& value) {

	this->id       = value.id;
	this->distance = value.distance;
	this->type     = value.type;
	this->masked   = value.masked;
	this->index    = value.index;

}
trackfinderInputStation::trackfinderInputStation(double stationDistance,
												  int stationId,
												  bool stationMaps,
												  bool stationHybrid,
												  bool stationStrip,
												  bool mask) {

	distance = stationDistance;
	id       = stationId;
	removeType();
	setMapsType(stationMaps);
	setHybridType(stationHybrid);
	setStripType(stationStrip);
	masked   = mask;
	index    = 0;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

trackfinderInputStation::~trackfinderInputStation() {

}

/****************************************************************
 * operator = ()												*
 ****************************************************************/

const trackfinderInputStation& trackfinderInputStation::operator = (const trackfinderInputStation& value) {

	this->id       = value.id;
	this->distance = value.distance;
	this->type     = value.type;
	this->masked   = value.masked;
	this->index    = value.index;

	return *this;

}

/****************************************************************
 * This method returns the id of the station.					*
 ****************************************************************/

int trackfinderInputStation::getId() {

	return id;

}

/****************************************************************
 * This method returns the distance of the station.				*
 ****************************************************************/

double trackfinderInputStation::getDistance() {

	return distance;

}

/****************************************************************
 * This method returns if the station is of no type.			*
 ****************************************************************/

bool trackfinderInputStation::isNoType() {

	return (type == NOSTATIONTYPE);

}

/****************************************************************
 * This method returns if the station is of type MAPS.			*
 ****************************************************************/

bool trackfinderInputStation::isMapsType() {

	return (type == MAPSSTATIONTYPE);

}

/****************************************************************
 * This method returns if the station is of type HYBRID.		*
 ****************************************************************/

bool trackfinderInputStation::isHybridType() {

	return (type == HYBRIDSTATIONTYPE);

}

/****************************************************************
 * This method returns if the station is of type STRIP.			*
 ****************************************************************/

bool trackfinderInputStation::isStripType() {

	return (type == STRIPSTATIONTYPE);

}

/****************************************************************
 * This method returns if the station is masked.				*
 ****************************************************************/

bool trackfinderInputStation::isMasked() {

	return masked;

}

/****************************************************************
 * This method returns the index of the station.				*
 ****************************************************************/

unsigned short trackfinderInputStation::getIndex() {

	if (masked) {
		indexOfMaskedStationWarningMsg* indexOfMaskedStation = new indexOfMaskedStationWarningMsg();
		indexOfMaskedStation->warningMsg();
		if(indexOfMaskedStation != NULL) {
			delete indexOfMaskedStation;
			indexOfMaskedStation = NULL;
		}
	}

	return index;

}

/****************************************************************
 * This method sets the distance of the station.				*
 ****************************************************************/

void trackfinderInputStation::setDistance(double stationDistance) {

	distance = stationDistance;

}

/****************************************************************
 * This method sets the id of the station.						*
 ****************************************************************/

void trackfinderInputStation::setId(int stationId) {

	id = stationId;

}

/****************************************************************
 * This method sets if the station is of no type.				*
 ****************************************************************/

void trackfinderInputStation::removeType() {

	type = NOSTATIONTYPE;

}

/****************************************************************
 * This method sets if the station is of type MAPS.				*
 ****************************************************************/

void trackfinderInputStation::setMapsType(bool stationMaps) {

	if ((stationMaps) && (type != MAPSSTATIONTYPE)) {
		if (type == NOSTATIONTYPE)
			type = MAPSSTATIONTYPE;
		else {
			stationSetMapsTypeWarningMsg* stationSetMapsType = new stationSetMapsTypeWarningMsg(id, (type == HYBRIDSTATIONTYPE), (type == STRIPSTATIONTYPE));
			stationSetMapsType->warningMsg();
			if(stationSetMapsType != NULL) {
				delete stationSetMapsType;
				stationSetMapsType = NULL;
			}
		}
	}

}

/****************************************************************
 * This method sets if the station is of type HYBRID.			*
 ****************************************************************/

void trackfinderInputStation::setHybridType(bool stationHybrid) {

	if ((stationHybrid) && (type != HYBRIDSTATIONTYPE)) {
		if (type == NOSTATIONTYPE)
			type = HYBRIDSTATIONTYPE;
		else {
			stationSetHybridTypeWarningMsg* stationSetHybridType = new stationSetHybridTypeWarningMsg(id, (type == MAPSSTATIONTYPE), (type == STRIPSTATIONTYPE));
			stationSetHybridType->warningMsg();
			if(stationSetHybridType != NULL) {
				delete stationSetHybridType;
				stationSetHybridType = NULL;
			}
		}
	}

}

/****************************************************************
 * This method sets if the station is of type STRIP.			*
 ****************************************************************/

void trackfinderInputStation::setStripType(bool stationStrip) {

	if ((stationStrip) && (type != STRIPSTATIONTYPE)) {
		if (type == NOSTATIONTYPE)
			type = STRIPSTATIONTYPE;
		else {
			stationSetStripTypeWarningMsg* stationSetStripType = new stationSetStripTypeWarningMsg(id, (type == MAPSSTATIONTYPE), (type == HYBRIDSTATIONTYPE));
			stationSetStripType->warningMsg();
			if(stationSetStripType != NULL) {
				delete stationSetStripType;
				stationSetStripType = NULL;
			}
		}
	}

}

/****************************************************************
 * This method sets the mask of the station.					*
 ****************************************************************/

void trackfinderInputStation::setMask(bool stationMask) {

	masked = stationMask;

}

/****************************************************************
 * This method sets the index of the station.					*
 ****************************************************************/

void trackfinderInputStation::setIndex(unsigned short stationIndex) {

	index = stationIndex;

}
