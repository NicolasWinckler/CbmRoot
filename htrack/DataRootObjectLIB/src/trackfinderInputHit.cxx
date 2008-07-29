/* *******************************************************************
// (C)opyright 2004
// 
// Institute of Computer Science V
// Prof. Männer
// University of Mannheim, Germany
// 
// *******************************************************************
// 
// Designer(s):   Steinle / Gläß
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
//     - consists of the information for the hits
//
// *******************************************************************
//
// $Author: csteinle $
// $Date: 2007-10-19 14:33:11 $
// $Revision: 1.6 $
//
// *******************************************************************/


#include "../../MiscLIB/include/defs.h"
#include "../../MiscLIB/include/errorHandling.h"
#ifdef CBMROOTFRAMEWORK
#ifdef CBMROOTFRAMEWORKHITCOMPATIBILITY
	#include "CbmStsMapsHit.h"
	#include "CbmStsStripHit.h"
	#include "CbmStsHybridHit.h"
#else
	#include "CbmStsHit.h"
#endif
#else
	#include "../../RootFrameworkLIB/include/CbmStsMapsHit.h"
	#include "../../RootFrameworkLIB/include/CbmStsStripHit.h"
	#include "../../RootFrameworkLIB/include/CbmStsHybridHit.h"
	#include "../../RootFrameworkLIB/include/CbmStsHit.h"
#endif
#include "../include/dataRootObjectError.h"
#include "../include/dataRootObjectWarningMsg.h"
#include "../include/trackfinderInputHit.h"


/****************************************************************
 * Default constructor											*
 ****************************************************************/

trackfinderInputHit::trackfinderInputHit() {

	hit       = NULL;
	point     = NULL;
	track     = NULL;
	station   = NULL;
	isPointer = true;
	hitIndex  = 0;

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

trackfinderInputHit::trackfinderInputHit(const trackfinderInputHit& value) {

	if (value.isPointer)
		this->hit   = value.hit;
	else {

#ifdef CBMROOTFRAMEWORKHITCOMPATIBILITY

		if (value.station->isMapsType())
			this->hit   = new CbmStsMapsHit(*((CbmStsMapsHit*)value.hit));
		else if (value.station->isStripType())
			this->hit   = new CbmStsStripHit(*((CbmStsStripHit*)value.hit));
		else if (value.station->isHybridType())
			this->hit   = new CbmStsHybridHit(*((CbmStsHybridHit*)value.hit));
		else if (value.station->isNoType())
			this->hit   = new CbmStsHit(*((CbmStsHit*)value.hit));

#else

		this->hit   = new CbmStsHit(*((CbmStsHit*)value.hit));

#endif

		if (hit == NULL)
			throw cannotAccessHitsOrTracksError(DATAROOTOBJECTLIB);
	}
	this->point     = value.point;
	this->track     = value.track;
	this->station   = value.station;
	this->isPointer = value.isPointer;
	this->hitIndex  = value.hitIndex;

}
trackfinderInputHit::trackfinderInputHit(CbmHit* hitPointer, int index) {

	hit       = hitPointer;

	if (hit == NULL)
		throw cannotAccessHitsOrTracksError(DATAROOTOBJECTLIB);

	point     = NULL;
	track     = NULL;
	station   = NULL;
	isPointer = true;
	hitIndex  = index;

}
trackfinderInputHit::trackfinderInputHit(int detectorId, double posX, double posY, double posZ, double xError, double yError, int index, int pointIndex, bool maps, bool strip, bool hybrid) {

	TVector3 position(posX, posY, posZ);
	TVector3 positionError(xError, yError, 0);

	if (maps ^ strip ^ hybrid) {

#ifdef CBMROOTFRAMEWORKHITCOMPATIBILITY

		if (maps)
			hit   = new CbmStsMapsHit(detectorId, position, positionError, pointIndex, -1);
		else if (strip)
			hit   = new CbmStsStripHit(detectorId, 0, position, positionError, pointIndex, -1, -1, -1, -1, -1, -1, -1, -1, -1);
		else if (hybrid)
			hit   = new CbmStsHybridHit(detectorId, position, positionError, pointIndex, -1, -1);

#else

		throw functionIsDeprecatedError(DATAROOTOBJECTLIB);

#endif

		if (hit == NULL)
			throw cannotAccessHitsOrTracksError(DATAROOTOBJECTLIB);

		point     = NULL;
		track     = NULL;
		station   = NULL;
		isPointer = false;
		hitIndex  = index;
	
	}
	else {

		wrongHitDefinitionWarningMsg* wrongHitDefinition = new wrongHitDefinitionWarningMsg();
		wrongHitDefinition->warningMsg();
		if(wrongHitDefinition != NULL) {
			delete wrongHitDefinition;
			wrongHitDefinition = NULL;
		}

	}

}
trackfinderInputHit::trackfinderInputHit(int detectorId, double posX, double posY, double posZ, double xError, double yError, int index, int pointIndex) {

	TVector3 position(posX, posY, posZ);
	TVector3 positionError(xError, yError, 0);

#ifndef CBMROOTFRAMEWORKHITCOMPATIBILITY

	hit   = new CbmStsHit(detectorId, position, positionError, -1, -1, -1);
	hit->SetRefIndex(pointIndex);

#else

	throw functionIsDeprecatedError(DATAROOTOBJECTLIB);

#endif

	if (hit == NULL)
		throw cannotAccessHitsOrTracksError(DATAROOTOBJECTLIB);

	point     = NULL;
	track     = NULL;
	station   = NULL;
	isPointer = false;
	hitIndex  = index;

}
/****************************************************************
 * Destructor													*
 ****************************************************************/

trackfinderInputHit::~trackfinderInputHit() {

	if (!isPointer)
		if (hit != NULL)
			delete hit;
	hit = NULL;

}

/****************************************************************
 * operator = (trackfinderInputHit)								*
 ****************************************************************/

const trackfinderInputHit& trackfinderInputHit::operator = (const trackfinderInputHit& value) {

	if (!this->isPointer)
		if (this->hit != NULL)
			delete this->hit;
	if (value.isPointer)
		this->hit   = value.hit;
	else {

#ifdef CBMROOTFRAMEWORKHITCOMPATIBILITY

		if (value.station->isMapsType())
			this->hit   = new CbmStsMapsHit(*((CbmStsMapsHit*)value.hit));
		else if (value.station->isStripType())
			this->hit   = new CbmStsStripHit(*((CbmStsStripHit*)value.hit));
		else if (value.station->isHybridType())
			this->hit   = new CbmStsHybridHit(*((CbmStsHybridHit*)value.hit));
		else if (value.station->isNoType())
			this->hit   = new CbmStsHit(*((CbmStsHit*)value.hit));

#else

		this->hit   = new CbmStsHit(*((CbmStsHit*)value.hit));

#endif

		if (hit == NULL)
			throw cannotAccessHitsOrTracksError(DATAROOTOBJECTLIB);
	}
	this->point     = value.point;
	this->track     = value.track;
	this->station   = value.station;
	this->isPointer = value.isPointer;
	this->hitIndex  = value.hitIndex;

	return *this;

}

/****************************************************************
 * method returns the detectorId								*
 ****************************************************************/

int trackfinderInputHit::getStationId() {

	int returnValue = 0;
	
	if (hit != NULL)
		returnValue = station->getId();
	else
		throw cannotAccessHitsOrTracksError(DATAROOTOBJECTLIB);
	
	return returnValue;

}

/****************************************************************
 * method returns the x position								*
 ****************************************************************/

double trackfinderInputHit::getPosX() {

	double returnValue = 0;
	
	if (hit != NULL)
		returnValue = hit->GetX();
	else
		throw cannotAccessHitsOrTracksError(DATAROOTOBJECTLIB);
	
	return returnValue;

}

/****************************************************************
 * method returns the y position								*
 ****************************************************************/

double trackfinderInputHit::getPosY() {

	double returnValue = 0;
	
	if (hit != NULL)
		returnValue = hit->GetY();
	else
		throw cannotAccessHitsOrTracksError(DATAROOTOBJECTLIB);
	
	return returnValue;

}

/****************************************************************
 * method returns the z position								*
 ****************************************************************/

double trackfinderInputHit::getPosZ() {

	double returnValue = 0;
	
	if (hit != NULL)
		returnValue = hit->GetZ();
	else
		throw cannotAccessHitsOrTracksError(DATAROOTOBJECTLIB);
	
	return returnValue;

}

/****************************************************************
 * method gets the original hit-object							*
 ****************************************************************/

CbmHit* trackfinderInputHit::getHit() {

	CbmHit* returnValue = NULL;
	
	if (hit != NULL)
		returnValue = hit;
	else
		throw cannotAccessHitsOrTracksError(DATAROOTOBJECTLIB);
	
	return returnValue;

}

/****************************************************************
 * method gets the original point-object from hit				*
 ****************************************************************/

CbmMCPoint* trackfinderInputHit::getPoint() {

	return point;

}

/****************************************************************
 * method gets the track-object from hit						*
 ****************************************************************/

trackfinderInputTrack* trackfinderInputHit::getTrack() {

	return track;

}

/****************************************************************
 * method gets the station-object from hit						*
 ****************************************************************/

trackfinderInputStation* trackfinderInputHit::getStation() {

	return station;

}

/****************************************************************
 * method returns true if the original hit-object is from a		*
 * maps detector.												*
 ****************************************************************/

bool trackfinderInputHit::isMapsHit() {

	return station->isMapsType();

}

/****************************************************************
 * method returns true if the original hit-object is from a		*
 * strip detector.												*
 ****************************************************************/

bool trackfinderInputHit::isStripHit() {

	return station->isStripType();

}

/****************************************************************
 * method returns true if the original hit-object is from a		*
 * hybrid detector.												*
 ****************************************************************/

bool trackfinderInputHit::isHybridHit() {

	return station->isHybridType();

}

/****************************************************************
 * method gets the id of the original hit-object				*
 ****************************************************************/

int trackfinderInputHit::getHitIndex() {

	return hitIndex;

}

/****************************************************************
 * method sets the original point-object from hit				*
 ****************************************************************/

void trackfinderInputHit::setPoint(CbmMCPoint* actualPoint) {

	point = actualPoint;

}

/****************************************************************
 * method sets the track-object from hit						*
 ****************************************************************/

void trackfinderInputHit::setTrack(trackfinderInputTrack* actualTrack) {

	track = actualTrack;

}

/****************************************************************
 * method sets the track-object from hit						*
 ****************************************************************/

void trackfinderInputHit::setStation(trackfinderInputStation* actualStation) {

	station = actualStation;

}

/****************************************************************
 * This method returns the size of the reserved memory for		*
 * the source data.												*
 ****************************************************************/

double trackfinderInputHit::getReservedSizeOfData(unsigned short dimension) {

	double returnValue;

	returnValue  = sizeof(hit);
	returnValue += sizeof(point);
	returnValue += sizeof(track);
	returnValue += sizeof(station);
	returnValue += sizeof(isPointer);
	returnValue += sizeof(hitIndex);

	returnValue  = (returnValue / (1 << (10 * dimension)));

	return returnValue;

}

/****************************************************************
 * This method returns the size of the allocated memory for		*
 * the source data.												*
 ****************************************************************/

double trackfinderInputHit::getAllocatedSizeOfData(unsigned short dimension) {

	double returnValue;

	if (hit != NULL)
		returnValue  = sizeof(*hit);
	if (point != NULL)
		returnValue += sizeof(*point);

	returnValue  = (returnValue / (1 << (10 * dimension)));

	return returnValue;

}

/****************************************************************
 * This method returns the size of the used memory for			*
 * the source data.												*
 ****************************************************************/

double trackfinderInputHit::getUsedSizeOfData(unsigned short dimension) {

	double returnValue;

	returnValue  = sizeof(hit);
	if (hit != NULL)
		returnValue  = sizeof(*hit);
	returnValue += sizeof(point);
	if (point != NULL)
		returnValue += sizeof(*point);
	returnValue += sizeof(track);
	returnValue += sizeof(station);
	returnValue += sizeof(isPointer);
	returnValue += sizeof(hitIndex);

	returnValue  = (returnValue / (1 << (10 * dimension)));

	return returnValue;

}

/****************************************************************
 * This method returns the size of the reserved memory for		*
 * the source data.												*
 ****************************************************************/

double trackfinderInputHit::getReservedSizeOfAddOnData(unsigned short dimension) {

	double returnValue;

	returnValue  = sizeof(hit);
	returnValue += sizeof(point);
	returnValue += sizeof(track);
	returnValue += sizeof(station);
	returnValue += sizeof(isPointer);
	returnValue += sizeof(hitIndex);

	returnValue  = (returnValue / (1 << (10 * dimension)));

	return returnValue;

}

/****************************************************************
 * This method returns the size of the allocated memory for		*
 * the source data.												*
 ****************************************************************/

double trackfinderInputHit::getAllocatedSizeOfAddOnData(unsigned short dimension) {

	double returnValue;

	returnValue  = 0;
	returnValue  = (returnValue / (1 << (10 * dimension)));

	return returnValue;

}

/****************************************************************
 * This method returns the size of the used memory for			*
 * the source data.												*
 ****************************************************************/

double trackfinderInputHit::getUsedSizeOfAddOnData(unsigned short dimension) {

	double returnValue;

	returnValue  = sizeof(hit);
	returnValue += sizeof(point);
	returnValue += sizeof(track);
	returnValue += sizeof(station);
	returnValue += sizeof(isPointer);
	returnValue += sizeof(hitIndex);

	returnValue  = (returnValue / (1 << (10 * dimension)));

	return returnValue;

}

/****************************************************************
 * Class implementation for the dictionary						*
 ****************************************************************/

ClassImp(trackfinderInputHit)
