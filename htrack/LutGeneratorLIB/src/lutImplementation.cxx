/////////////////////////////////////////////////////////////////////
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
//     - class for the implementation of both look-up-tables
//
// *******************************************************************
//
// $Author: csteinle $
// $Date: 2007-05-21 12:18:13 $
// $Revision: 1.2 $
//
/////////////////////////////////////////////////////////////////////


#include "../include/prelutMath.h"
#include "../include/lutMath.h"
#include "../include/lutGeneratorError.h"
#include "../include/lutImplementation.h"


/****************************************************************
 * Default constructor											*
 ****************************************************************/

lutImplementation::lutImplementation() {

	space        = NULL;
	isSpaceLocal = false;
	firstLut     = NULL;
	secondLut    = NULL;

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

lutImplementation::lutImplementation(double dim1Min, double dim1Max, int dim1Step, double dim2Min, double dim2Max, int dim2Step) {

	space        = (histogramSpace**)calloc(1, sizeof(histogramSpace*));
	if (space == NULL)
		throw memoryAllocationError(LUTGENERATORLIB);
	(*space)     = new histogramSpace();
	if (*space == NULL)
		throw memoryAllocationError(LUTGENERATORLIB);
	isSpaceLocal = true;
	(*space)->setMin(dim1Min, DIM1);
	(*space)->setMin(dim2Min, DIM2);
	(*space)->setMax(dim1Max, DIM1);
	(*space)->setMax(dim2Max, DIM2);
	(*space)->setStep(dim1Step, DIM1);
	(*space)->setStep(dim2Step, DIM2);

	firstLut     = NULL;
	secondLut    = new lutMath(dim1Min, dim1Max, dim1Step, dim2Min, dim2Max, dim2Step);;

}
lutImplementation::lutImplementation(double dim3Min, double dim3Max, int dim3Step, double dim3StartEntry, double dim3StopEntry) {

	space        = (histogramSpace**)calloc(1, sizeof(histogramSpace*));
	if (space == NULL)
		throw memoryAllocationError(LUTGENERATORLIB);
	(*space)     = new histogramSpace();
	if (*space == NULL)
		throw memoryAllocationError(LUTGENERATORLIB);
	isSpaceLocal = true;
	(*space)->setMin(dim3Min, DIM3);
	(*space)->setMax(dim3Max, DIM3);
	(*space)->setStep(dim3Step, DIM3);

	firstLut     = new prelutMath(dim3Min, dim3Max, dim3Step, dim3StartEntry, dim3StopEntry);
	secondLut    = NULL;

}
lutImplementation::lutImplementation(double dim1Min, double dim1Max, int dim1Step, double dim2Min, double dim2Max, int dim2Step, double dim3Min, double dim3Max, int dim3Step, double dim3StartEntry, double dim3StopEntry) {

	space        = (histogramSpace**)calloc(1, sizeof(histogramSpace*));
	if (space == NULL)
		throw memoryAllocationError(LUTGENERATORLIB);
	(*space)     = new histogramSpace();
	if (*space == NULL)
		throw memoryAllocationError(LUTGENERATORLIB);
	isSpaceLocal = true;
	(*space)->setMin(dim1Min, DIM1);
	(*space)->setMin(dim2Min, DIM2);
	(*space)->setMin(dim3Min, DIM3);
	(*space)->setMax(dim1Max, DIM1);
	(*space)->setMax(dim2Max, DIM2);
	(*space)->setMax(dim3Max, DIM3);
	(*space)->setStep(dim1Step, DIM1);
	(*space)->setStep(dim2Step, DIM2);
	(*space)->setStep(dim3Step, DIM3);

	firstLut     = new prelutMath(dim3Min, dim3Max, dim3Step, dim3StartEntry, dim3StopEntry);
	secondLut    = new lutMath(dim1Min, dim1Max, dim1Step, dim2Min, dim2Max, dim2Step);

}
lutImplementation::lutImplementation(histogramSpace** space, double dim3StartEntry, double dim3StopEntry) {

	this->space  = space;
	isSpaceLocal = false;

	if ((this->space == NULL) || (*(this->space) == NULL))
		throw cannotAccessHistogramSpaceError(LUTGENERATORLIB);

	firstLut     = new prelutMath((*space)->getMin(DIM3), (*space)->getMax(DIM3), (*space)->getStep(DIM3), dim3StartEntry, dim3StopEntry);
	secondLut    = new lutMath((*space)->getMin(DIM1), (*space)->getMax(DIM1), (*space)->getStep(DIM1), (*space)->getMin(DIM2), (*space)->getMax(DIM2), (*space)->getStep(DIM2));

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

lutImplementation::~lutImplementation() {

	if (isSpaceLocal) {
		if (space != NULL) {
			if ((*space) != NULL) {
				delete (*space);
				(*space) = NULL;
			}
			free(space);
			space = NULL;
		}
	}
	if (firstLut != NULL) {
		delete firstLut;
		firstLut = NULL;
	}
	if (secondLut != NULL) {
		delete secondLut;
		secondLut = NULL;
	}

}

/****************************************************************
 * Method initializes the object.								*
 ****************************************************************/

void lutImplementation::init(double dim1Min, double dim1Max, int dim1Step, double dim2Min, double dim2Max, int dim2Step) {

	if (isSpaceLocal) {
		if (space != NULL) {
			if ((*space) != NULL) {
				delete (*space);
				(*space) = NULL;
			}
			free(space);
			space = NULL;
		}
	}
	if (firstLut != NULL) {
		delete firstLut;
		firstLut = NULL;
	}
	if (secondLut != NULL) {
		delete secondLut;
		secondLut = NULL;
	}

	space        = (histogramSpace**)calloc(1, sizeof(histogramSpace*));
	if (space == NULL)
		throw memoryAllocationError(LUTGENERATORLIB);
	(*space)     = new histogramSpace();
	if (*space == NULL)
		throw memoryAllocationError(LUTGENERATORLIB);
	isSpaceLocal = true;
	(*space)->setMin(dim1Min, DIM1);
	(*space)->setMin(dim2Min, DIM2);
	(*space)->setMax(dim1Max, DIM1);
	(*space)->setMax(dim2Max, DIM2);
	(*space)->setStep(dim1Step, DIM1);
	(*space)->setStep(dim2Step, DIM2);

	firstLut     = NULL;
	secondLut    = new lutMath(dim1Min, dim1Max, dim1Step, dim2Min, dim2Max, dim2Step);

}
void lutImplementation::init(double dim3Min, double dim3Max, int dim3Step, double dim3StartEntry, double dim3StopEntry) {

	if (isSpaceLocal) {
		if (space != NULL) {
			if ((*space) != NULL) {
				delete (*space);
				(*space) = NULL;
			}
			free(space);
			space = NULL;
		}
	}
	if (firstLut != NULL) {
		delete firstLut;
		firstLut = NULL;
	}
	if (secondLut != NULL) {
		delete secondLut;
		secondLut = NULL;
	}

	space        = (histogramSpace**)calloc(1, sizeof(histogramSpace*));
	if (space == NULL)
		throw memoryAllocationError(LUTGENERATORLIB);
	(*space)     = new histogramSpace();
	if (*space == NULL)
		throw memoryAllocationError(LUTGENERATORLIB);
	isSpaceLocal = true;
	(*space)->setMin(dim3Min, DIM3);
	(*space)->setMax(dim3Max, DIM3);
	(*space)->setStep(dim3Step, DIM3);

	firstLut     = new prelutMath(dim3Min, dim3Max, dim3Step, dim3StartEntry, dim3StopEntry);
	secondLut    = NULL;

}
void lutImplementation::init(double dim1Min, double dim1Max, int dim1Step, double dim2Min, double dim2Max, int dim2Step, double dim3Min, double dim3Max, int dim3Step, double dim3StartEntry, double dim3StopEntry) {

	if (isSpaceLocal) {
		if (space != NULL) {
			if ((*space) != NULL) {
				delete (*space);
				(*space) = NULL;
			}
			free(space);
			space = NULL;
		}
	}
	if (firstLut != NULL) {
		delete firstLut;
		firstLut = NULL;
	}
	if (secondLut != NULL) {
		delete secondLut;
		secondLut = NULL;
	}

	space        = (histogramSpace**)calloc(1, sizeof(histogramSpace*));
	if (space == NULL)
		throw memoryAllocationError(LUTGENERATORLIB);
	(*space)     = new histogramSpace();
	if (*space == NULL)
		throw memoryAllocationError(LUTGENERATORLIB);
	isSpaceLocal = true;
	(*space)->setMin(dim1Min, DIM1);
	(*space)->setMin(dim2Min, DIM2);
	(*space)->setMin(dim3Min, DIM3);
	(*space)->setMax(dim1Max, DIM1);
	(*space)->setMax(dim2Max, DIM2);
	(*space)->setMax(dim3Max, DIM3);
	(*space)->setStep(dim1Step, DIM1);
	(*space)->setStep(dim2Step, DIM2);
	(*space)->setStep(dim3Step, DIM3);

	firstLut     = new prelutMath(dim3Min, dim3Max, dim3Step, dim3StartEntry, dim3StopEntry);
	secondLut    = new lutMath(dim1Min, dim1Max, dim1Step, dim2Min, dim2Max, dim2Step);

}
void lutImplementation::init(histogramSpace** space, double dim3StartEntry, double dim3StopEntry) {

	if (isSpaceLocal) {
		if (space != NULL) {
			if ((*space) != NULL) {
				delete (*space);
				(*space) = NULL;
			}
			free(space);
			space = NULL;
		}
	}
	if (firstLut != NULL) {
		delete firstLut;
		firstLut = NULL;
	}
	if (secondLut != NULL) {
		delete secondLut;
		secondLut = NULL;
	}

	this->space  = space;
	isSpaceLocal = false;

	if ((this->space == NULL) || (*(this->space) == NULL))
		throw cannotAccessHistogramSpaceError(LUTGENERATORLIB);

	firstLut     = new prelutMath((*space)->getMin(DIM3), (*space)->getMax(DIM3), (*space)->getStep(DIM3), dim3StartEntry, dim3StopEntry);
	secondLut    = new lutMath((*space)->getMin(DIM1), (*space)->getMax(DIM1), (*space)->getStep(DIM1), (*space)->getMin(DIM2), (*space)->getMax(DIM2), (*space)->getStep(DIM2));

}

/****************************************************************
 * This method returns the double pointer to the				*
 * histogramSpace object.										*
 ****************************************************************/

histogramSpace** lutImplementation::getSpacePointer() {

	return space;

}

/****************************************************************
 * This method returns the Definition of the lut object.		*
 ****************************************************************/

lutDefinition lutImplementation::getLutDefinition() {

	lutDefinition returnValue;

	if (secondLut != NULL)
		returnValue = secondLut->getLutDefinition();
	else
		throw cannotGetDefinitionOfUndefinedObjectError();

	return returnValue;

}

/****************************************************************
 * This method returns the Definition of the prelut object.		*
 ****************************************************************/

prelutDefinition lutImplementation::getPrelutDefinition() {

	prelutDefinition returnValue;

	if (firstLut != NULL)
		returnValue = firstLut->getPrelutDefinition();
	else
		throw cannotGetDefinitionOfUndefinedObjectError();

	return returnValue;

}

/****************************************************************
 * This method returns the lut border.							*
 ****************************************************************/

lutHoughBorder lutImplementation::getLutBorder() {

	lutHoughBorder returnValue;

	if (secondLut != NULL)
		returnValue = secondLut->getBorder();
	else
		throw cannotGetBorderOfUndefinedObjectError();

	return returnValue;

}

/****************************************************************
 * This method returns the prelut border.						*
 ****************************************************************/

prelutHoughBorder lutImplementation::getPrelutBorder() {

	prelutHoughBorder returnValue;

	if (firstLut != NULL)
		returnValue = firstLut->getBorder();
	else
		throw cannotGetBorderOfUndefinedObjectError();

	return returnValue;

}

/****************************************************************
 * This method returns both borders.							*
 ****************************************************************/

lutBorder lutImplementation::getBorder() {

	prelutHoughBorder prelut;
	lutHoughBorder    lut;
	lutBorder         returnValue;

	prelut = getPrelutBorder();
	lut    = getLutBorder();

	returnValue.setPrelutHoughBorder(prelut);
	returnValue.setLutHoughBorder(lut);

	return returnValue;

}

/****************************************************************
 * This method clears the border.								*
 ****************************************************************/

void lutImplementation::clear(lutHoughBorder* borderPointer) {

	if (secondLut != NULL)
		secondLut->clear(borderPointer);
	else
		throw cannotClearBorderOfUndefinedObjectError();

}

/****************************************************************
 * This method returns the magnetic field to use.				*
 * Caution: This function here has no useful					*
 * implementations in some subclasses. So be					*
 * careful when using.											*
 ****************************************************************/

trackfinderInputMagneticField* lutImplementation::getMagneticField() {

	trackfinderInputMagneticField* returnValue = NULL;

	if (secondLut != NULL)
		returnValue = secondLut->getMagneticField();
	else
		throw cannotGetMagneticFieldOfUndefinedObjectError();

	return returnValue;

}

/****************************************************************
 * This method returns the magnetic field factor to use instead	*
 * of the magneticField.										*
 * Caution: This function here has no useful					*
 * implementations in some subclasses. So be					*
 * careful when using.											*
 ****************************************************************/

double lutImplementation::getMagneticFieldFactor() {

	double returnValue = 0.0;

	if (secondLut != NULL)
		returnValue = secondLut->getMagneticFieldFactor();
	else
		throw cannotGetMagneticFieldFactorOfUndefinedObjectError();

	return returnValue;

}

/****************************************************************
 * This method sets the magnetic field to use.					*
 * Caution: This function here has no useful					*
 * implementations in some subclasses. So be					*
 * careful when using.											*
 ****************************************************************/

void lutImplementation::setMagneticField(trackfinderInputMagneticField* magneticField) {

	if (secondLut != NULL)
		secondLut->setMagneticField(magneticField);
	else
		throw cannotSetMagneticFieldOfUndefinedObjectError();

}

/****************************************************************
 * This method sets the magnetic field factor to use instead of	*
 * the magneticField.											*
 * Caution: This function here has no useful					*
 * implementations in some subclasses. So be					*
 * careful when using.											*
 ****************************************************************/

void lutImplementation::setMagneticFieldFactor(double magneticFieldFactor) {

	if (secondLut != NULL)
		secondLut->setMagneticFieldFactor(magneticFieldFactor);
	else
		throw cannotSetMagneticFieldFactorOfUndefinedObjectError();

}

/****************************************************************
 * This method returns the number of corrections.				*
 * Caution: This function here has no useful					*
 * implementations in some subclasses. So be					*
 * careful when using.											*
 ****************************************************************/

unsigned long lutImplementation::getNumberOfCorrections() {

	unsigned long returnValue;

	if (secondLut != NULL)
		returnValue = secondLut->getNumberOfCorrections();
	else
		throw cannotAccessCorrectionsOfUndefinedObjectError();

	return returnValue;

}

/****************************************************************
 * This method returns the number of coord corrections.			*
 * Caution: This function here has no useful					*
 * implementations in some subclasses. So be					*
 * careful when using.											*
 ****************************************************************/

unsigned long lutImplementation::getNumberOfCoordCorrections() {

	unsigned long returnValue;

	if (secondLut != NULL)
		returnValue = secondLut->getNumberOfCoordCorrections();
	else
		throw cannotAccessCorrectionsOfUndefinedObjectError();

	return returnValue;

}

/****************************************************************
 * This method resets both correction counters.					*
 * Caution: This function here has no useful					*
 * implementations in some subclasses. So be					*
 * careful when using.											*
 ****************************************************************/

void lutImplementation::resetCorrectionCounter() {

	if (secondLut != NULL)
		secondLut->resetCorrectionCounter();
	else
		throw cannotAccessCorrectionsOfUndefinedObjectError();

}

/****************************************************************
 * This method evaluates the lut table.							*
 ****************************************************************/

void lutImplementation::evaluateLut(trackfinderInputHit* hit, lutHoughBorder* borderPointer) {

	if (secondLut != NULL)
		secondLut->evaluate(hit, borderPointer);
	else
		throw cannotEvaluateBorderOfUndefinedObjectError();

}

/****************************************************************
 * This method evaluates the prelut table.						*
 ****************************************************************/

void lutImplementation::evaluatePrelut(trackfinderInputHit* hit, prelutHoughBorder* borderPointer) {

	if (firstLut != NULL)
		firstLut->evaluate(hit, borderPointer);
	else
		throw cannotEvaluateBorderOfUndefinedObjectError();

}

/****************************************************************
 * This method evaluates the prelut table.						*
 ****************************************************************/

void lutImplementation::evaluate(trackfinderInputHit* hit, lutBorder* borderPointer) {

	if (borderPointer != NULL) {

		evaluatePrelut(hit, borderPointer->getPrelutHoughBorderPointer());
		evaluateLut(hit, borderPointer->getLutHoughBorderPointer());

	}
	else {

		evaluatePrelut(hit, NULL);
		evaluateLut(hit, NULL);

	}

}
