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
// $Date: 2008-10-24 16:41:19 $
// $Revision: 1.4 $
//
/////////////////////////////////////////////////////////////////////


#include "../include/prelutMath.h"
#include "../include/prelutAccess.h"
#include "../include/lutMath.h"
#include "../include/lutAccess.h"
#include "../include/lutGeneratorError.h"
#include "../include/lutGeneratorWarningMsg.h"
#include "../include/lutImplementation.h"


/****************************************************************
 * deletes allocated and initialized memory						*
 ****************************************************************/

void lutImplementation::deleteSpace() {

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

}
void lutImplementation::deleteLuts(identifiedPrelut* first, identifiedLut* second) {

	if (first != NULL) {
		if (first->lutObject != NULL) {
			switch(first->type) {
				case ANALYTICFORMULALUT:
					delete ((prelutMath*)first->lutObject);
					break;
				case FILELUT:
					delete ((prelutAccess*)first->lutObject);
					break;
				default:
					delete first->lutObject;
					break;
			}
			first->lutObject = NULL;
			first->type      = NOLUT;
		}
	}
	if (second != NULL) {
		if (second->lutObject != NULL) {
			switch(second->type) {
				case ANALYTICFORMULALUT:
					delete ((lutMath*)second->lutObject);
					break;
				case FILELUT:
					delete ((lutAccess*)second->lutObject);
					break;
				default:
					delete second->lutObject;
					break;
			}
			second->lutObject = NULL;
			second->type      = NOLUT;
		}
	}

}
void lutImplementation::deleteMemory() {

	deleteSpace();
	deleteLuts(&firstLut, &secondLut);

}

/****************************************************************
 * allocates memory for the histogramSpace object				*
 ****************************************************************/

void lutImplementation::allocateSpace(double dim1Min, double dim1Max, int dim1Step, double dim2Min, double dim2Max, int dim2Step, double dim3Min, double dim3Max, int dim3Step) {

	deleteSpace();

	space        = (histogramSpace**)calloc(1, sizeof(histogramSpace*));
	if (space == NULL)
		throw memoryAllocationError(LUTGENERATORLIB);

	(*space)     = new histogramSpace();
	if (*space == NULL)
		throw memoryAllocationError(LUTGENERATORLIB);

	(*space)->setMin(dim1Min, DIM1);
	(*space)->setMin(dim2Min, DIM2);
	(*space)->setMin(dim3Min, DIM3);
	(*space)->setMax(dim1Max, DIM1);
	(*space)->setMax(dim2Max, DIM2);
	(*space)->setMax(dim3Max, DIM3);
	(*space)->setStep(dim1Step, DIM1);
	(*space)->setStep(dim2Step, DIM2);
	(*space)->setStep(dim3Step, DIM3);

	isSpaceLocal = true;

}
void lutImplementation::allocateSpace(histogramSpace** space) {

	deleteSpace();

	this->space  = space;

	isSpaceLocal = false;

}

/****************************************************************
 * allocates memory for the look-up-table object				*
 ****************************************************************/

void lutImplementation::allocateMathLuts(identifiedPrelut* first, identifiedLut* second, double dim3StartEntry, double dim3StopEntry, trackfinderInputMagneticField* magneticField) {

	deleteLuts(first, second);

	if ((this->space == NULL) || (*(this->space) == NULL))
		throw cannotAccessHistogramSpaceError(LUTGENERATORLIB);

	if (first != NULL) {

		first->lutObject  = new prelutMath((*space)->getMin(DIM3), (*space)->getMax(DIM3), (*space)->getStep(DIM3), dim3StartEntry, dim3StopEntry);
		if (first->lutObject == NULL)
			throw memoryAllocationError(LUTGENERATORLIB);
		first->type       = ANALYTICFORMULALUT;

	}

	if (second != NULL) {

		second->lutObject = new lutMath((*space)->getMin(DIM1), (*space)->getMax(DIM1), (*space)->getStep(DIM1), (*space)->getMin(DIM2), (*space)->getMax(DIM2), (*space)->getStep(DIM2));
		if (second->lutObject == NULL)
			throw memoryAllocationError(LUTGENERATORLIB);
		second->type      = ANALYTICFORMULALUT;

		if (magneticField != NULL)
			second->lutObject->setMagneticField(magneticField);

	}

}
void lutImplementation::allocateFileLuts(identifiedPrelut* first, identifiedLut* second, double dim3StartEntry, double dim3StopEntry) {

	deleteLuts(first, second);

	if ((this->space == NULL) || (*(this->space) == NULL))
		throw cannotAccessHistogramSpaceError(LUTGENERATORLIB);

	if (first != NULL) {

		first->lutObject  = new prelutAccess((*space)->getMin(DIM3), (*space)->getMax(DIM3), (*space)->getStep(DIM3), dim3StartEntry, dim3StopEntry);
		if (first->lutObject == NULL)
			throw memoryAllocationError(LUTGENERATORLIB);
		first->type       = FILELUT;

	}

	if (second != NULL) {

		second->lutObject = new lutAccess((*space)->getMin(DIM1), (*space)->getMax(DIM1), (*space)->getStep(DIM1), (*space)->getMin(DIM2), (*space)->getMax(DIM2), (*space)->getStep(DIM2));
		if (second->lutObject == NULL)
			throw memoryAllocationError(LUTGENERATORLIB);
		second->type      = FILELUT;

	}

}

/****************************************************************
 * Method reads the file look-up-tables.						*
 ****************************************************************/

void lutImplementation::readFileLuts(std::string prelutFileName, std::string lutFileName, double dim3StartEntry, double dim3StopEntry, std::streambuf* terminal) {

	allocateFileLuts(&firstLut, &secondLut, dim3StartEntry, dim3StopEntry);

	if ((firstLut.type != FILELUT) ||(secondLut.type != FILELUT))
		throw wrongLutTypeForUsedFunctionalityDetectedError();

	if (!prelutFileName.empty())
		((prelutAccess*)(firstLut.lutObject))->read(prelutFileName, terminal);
	if (!lutFileName.empty())
		((lutAccess*)(secondLut.lutObject))->read(lutFileName, terminal);

}

/****************************************************************
 * Method generates the analytic formula file look-up-tables.	*
 ****************************************************************/

void lutImplementation::generateAnalyticFormulaFileLuts(std::string prelutFileName, std::string prelutName, std::string lutFileName, std::string lutName, double dim3StartEntry, double dim3StopEntry, trackfinderInputMagneticField* magneticField, std::streambuf* terminal) {

	identifiedPrelut    firstLutMath;
	identifiedLut       secondLutMath;
	digitalHit          digitalHitValue;
	trackfinderInputHit analogHitValue;
	prelutHoughBorder   actualPrelutBorder;
	lutHoughBorder      actualLutBorder;

	firstLutMath.lutObject  = NULL;
	firstLutMath.type       = NOLUT;
	secondLutMath.lutObject = NULL;
	secondLutMath.type      = NOLUT;

	allocateMathLuts(&firstLutMath, &secondLutMath, dim3StartEntry, dim3StopEntry, magneticField);

	allocateFileLuts(&firstLut, &secondLut, dim3StartEntry, dim3StopEntry);

	if ((firstLutMath.type != ANALYTICFORMULALUT) || (secondLutMath.type != ANALYTICFORMULALUT) || (firstLut.type != FILELUT) ||(secondLut.type != FILELUT))
		throw wrongLutTypeForUsedFunctionalityDetectedError();

	((prelutAccess*)(firstLut.lutObject))->clear();
	((lutAccess*)(secondLut.lutObject))->clear();
	for (unsigned long i = 0; i < digitalHitValue.getMaxNumberOfDigitalHitData(); i++) {

		digitalHitValue.setData(i);
		analogHitValue = digitalHitValue.getHit();
		
		((prelutMath*)(firstLutMath.lutObject))->evaluate(&analogHitValue, &actualPrelutBorder);
		((lutMath*)(secondLutMath.lutObject))->evaluate(&analogHitValue, &actualLutBorder);

		((prelutAccess*)(firstLut.lutObject))->addEntry(actualPrelutBorder, digitalHitValue);
		((lutAccess*)(secondLut.lutObject))->addEntry(actualLutBorder, digitalHitValue);

	}

	if (!prelutFileName.empty())
		((prelutAccess*)(firstLut.lutObject))->write(prelutFileName, prelutName, terminal);
	if (!lutFileName.empty())
		((lutAccess*)(secondLut.lutObject))->write(lutFileName, lutName, HARDWAREFORMAT, terminal);

	deleteLuts(&firstLutMath, &secondLutMath);

}

/****************************************************************
 * Method generates the Runge-Kutta file look-up-tables.		*
 ****************************************************************/

void lutImplementation::generateRungeKuttaFileLuts(std::string prelutFileName, std::string prelutName, std::string lutFileName, std::string lutName, double dim3StartEntry, double dim3StopEntry, trackfinderInputMagneticField* magneticField, std::streambuf* terminal) {

/**/

}

/****************************************************************
 * Default constructor											*
 ****************************************************************/

lutImplementation::lutImplementation() {

	space               = NULL;
	isSpaceLocal        = false;
	firstLut.lutObject  = NULL;
	firstLut.type       = NOLUT;
	secondLut.lutObject = NULL;
	secondLut.type      = NOLUT;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

lutImplementation::~lutImplementation() {

	deleteMemory();

}

/****************************************************************
 * Method initializes the object.								*
 ****************************************************************/

void lutImplementation::initAnalyticFormulaLuts(double dim1Min, double dim1Max, int dim1Step, double dim2Min, double dim2Max, int dim2Step, double dim3Min, double dim3Max, int dim3Step, double dim3StartEntry, double dim3StopEntry, trackfinderInputMagneticField* magneticField) {

	allocateSpace(dim1Min, dim1Max, dim1Step, dim2Min, dim2Max, dim2Step, dim3Min, dim3Max, dim3Step);

	allocateMathLuts(&firstLut, &secondLut, dim3StartEntry, dim3StopEntry, magneticField);

}
void lutImplementation::initAnalyticFormulaLuts(histogramSpace** space, double dim3StartEntry, double dim3StopEntry, trackfinderInputMagneticField* magneticField) {

	allocateSpace(space);

	allocateMathLuts(&firstLut, &secondLut, dim3StartEntry, dim3StopEntry, magneticField);

}
void lutImplementation::initFileLuts(double dim1Min, double dim1Max, int dim1Step, double dim2Min, double dim2Max, int dim2Step, double dim3Min, double dim3Max, int dim3Step, double dim3StartEntry, double dim3StopEntry, std::string prelutFileName, std::string lutFileName, std::streambuf* terminal) {

	allocateSpace(dim1Min, dim1Max, dim1Step, dim2Min, dim2Max, dim2Step, dim3Min, dim3Max, dim3Step);

	readFileLuts(prelutFileName, lutFileName, dim3StartEntry, dim3StopEntry, terminal);

}
void lutImplementation::initFileLuts(histogramSpace** space, double dim3StartEntry, double dim3StopEntry, std::string prelutFileName, std::string lutFileName, std::streambuf* terminal) {

	allocateSpace(space);

	readFileLuts(prelutFileName, lutFileName, dim3StartEntry, dim3StopEntry, terminal);

}
void lutImplementation::initAnalyticFormulaFileLuts(double dim1Min, double dim1Max, int dim1Step, double dim2Min, double dim2Max, int dim2Step, double dim3Min, double dim3Max, int dim3Step, double dim3StartEntry, double dim3StopEntry, std::string prelutFileName, std::string lutFileName, trackfinderInputMagneticField* magneticField, std::streambuf* terminal) {

	allocateSpace(dim1Min, dim1Max, dim1Step, dim2Min, dim2Max, dim2Step, dim3Min, dim3Max, dim3Step);

	generateAnalyticFormulaFileLuts(prelutFileName, "Prelut generated by the analytic formula", lutFileName, "Lut generated by the analytic formula", dim3StartEntry, dim3StopEntry, magneticField, terminal);

}
void lutImplementation::initAnalyticFormulaFileLuts(histogramSpace** space, double dim3StartEntry, double dim3StopEntry, std::string prelutFileName, std::string lutFileName, trackfinderInputMagneticField* magneticField, std::streambuf* terminal) {

	allocateSpace(space);

	generateAnalyticFormulaFileLuts(prelutFileName, "Prelut generated by the analytic formula", lutFileName, "Lut generated by the analytic formula", dim3StartEntry, dim3StopEntry, magneticField, terminal);

}
void lutImplementation::initRungeKuttaFileLuts(double dim1Min, double dim1Max, int dim1Step, double dim2Min, double dim2Max, int dim2Step, double dim3Min, double dim3Max, int dim3Step, double dim3StartEntry, double dim3StopEntry, std::string prelutFileName, std::string lutFileName, trackfinderInputMagneticField* magneticField, std::streambuf* terminal) {

	allocateSpace(dim1Min, dim1Max, dim1Step, dim2Min, dim2Max, dim2Step, dim3Min, dim3Max, dim3Step);

	generateRungeKuttaFileLuts(prelutFileName, "Prelut generated by the Runge-Kutta approach", lutFileName, "Lut generated by the Runge-Kutta approach", dim3StartEntry, dim3StopEntry, magneticField, terminal);

}
void lutImplementation::initRungeKuttaFileLuts(histogramSpace** space, double dim3StartEntry, double dim3StopEntry, std::string prelutFileName, std::string lutFileName, trackfinderInputMagneticField* magneticField, std::streambuf* terminal) {

	allocateSpace(space);

	generateRungeKuttaFileLuts(prelutFileName, "Prelut generated by the Runge-Kutta approach", lutFileName, "Lut generated by the Runge-Kutta approach", dim3StartEntry, dim3StopEntry, magneticField, terminal);

}

/****************************************************************
 * This method returns true if the magnetism					*
 * functions can be used with the actual LUT type.				*
 ****************************************************************/

bool lutImplementation::typeUsesMagnetism() {

	return (secondLut.type == ANALYTICFORMULALUT);

}

/****************************************************************
 * This method returns true if the correction					*
 * functions can be used with the actual LUT type.				*
 ****************************************************************/

bool lutImplementation::typeUsesCorrections() {

	return (secondLut.type == ANALYTICFORMULALUT);

}

/****************************************************************
 * This method returns the double pointer to the				*
 * histogramSpace object.										*
 ****************************************************************/

histogramSpace** lutImplementation::getSpacePointer() {

	return space;

}

/****************************************************************
 * This method returns the Definition of the prelut object.		*
 ****************************************************************/

prelutDefinition lutImplementation::getPrelutDefinition() {

	prelutDefinition returnValue;

	if (firstLut.lutObject != NULL)
		returnValue = firstLut.lutObject->getPrelutDefinition();
	else
		throw cannotGetDefinitionOfUndefinedObjectError();

	return returnValue;

}

/****************************************************************
 * This method returns the Definition of the lut object.		*
 ****************************************************************/

lutDefinition lutImplementation::getLutDefinition() {

	lutDefinition returnValue;

	if (secondLut.lutObject != NULL)
		returnValue = secondLut.lutObject->getLutDefinition();
	else
		throw cannotGetDefinitionOfUndefinedObjectError();

	return returnValue;

}

/****************************************************************
 * This method returns the prelut border.						*
 ****************************************************************/

prelutHoughBorder lutImplementation::getPrelutBorder() {

	prelutHoughBorder returnValue;

	if (firstLut.lutObject != NULL)
		returnValue = firstLut.lutObject->getBorder();
	else
		throw cannotGetBorderOfUndefinedObjectError();

	return returnValue;

}

/****************************************************************
 * This method returns the lut border.							*
 ****************************************************************/

lutHoughBorder lutImplementation::getLutBorder() {

	lutHoughBorder returnValue;

	if (secondLut.lutObject != NULL)
		returnValue = secondLut.lutObject->getBorder();
	else
		throw cannotGetBorderOfUndefinedObjectError();

	return returnValue;

}

/****************************************************************
 * This method returns both borders.							*
 ****************************************************************/

lutBorder lutImplementation::getBorder() {

	prelutHoughBorder actualPrelutBorder;
	lutHoughBorder    actualLutBorder;
	lutBorder         returnValue;

	actualPrelutBorder = getPrelutBorder();
	actualLutBorder    = getLutBorder();

	returnValue.setPrelutHoughBorder(actualPrelutBorder);
	returnValue.setLutHoughBorder(actualLutBorder);

	return returnValue;

}

/****************************************************************
 * This method clears the border.								*
 ****************************************************************/

void lutImplementation::clear(lutHoughBorder* borderPointer) {

	if (secondLut.lutObject != NULL)
		secondLut.lutObject->clear(borderPointer);
	else
		throw cannotClearBorderOfUndefinedObjectError();

}

/****************************************************************
 * This method sets the prelut range to use.					*
 * Caution: This function here has no useful					*
 * implementations in some subclasses. So be					*
 * careful when using.											*
 ****************************************************************/

void lutImplementation::setPrelutRange(double rangeStart, double rangeStop) {

	if (firstLut.lutObject != NULL)
		firstLut.lutObject->set(rangeStart, rangeStop);
	else
		throw cannotSetPrelutRangeOfUndefinedObjectError();

}

/****************************************************************
 * This method returns the magnetic field to use.				*
 * Caution: This function here has no useful					*
 * implementations in some subclasses. So be					*
 * careful when using.											*
 ****************************************************************/

trackfinderInputMagneticField* lutImplementation::getMagneticField() {

	trackfinderInputMagneticField* returnValue = NULL;

	if (typeUsesMagnetism()) {

		if (secondLut.lutObject != NULL)
			returnValue = secondLut.lutObject->getMagneticField();
		else
			throw cannotGetMagneticFieldOfUndefinedObjectError();

	}
	else {

		tryToAccessMagneticFieldOfFileObjectWarningMsg* tryToAccessMagneticFieldOfFileObject = new tryToAccessMagneticFieldOfFileObjectWarningMsg();
		tryToAccessMagneticFieldOfFileObject->warningMsg();
		if(tryToAccessMagneticFieldOfFileObject != NULL) {
			delete tryToAccessMagneticFieldOfFileObject;
			tryToAccessMagneticFieldOfFileObject = NULL;
		}

	}

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

	if (typeUsesMagnetism()) {

		if (secondLut.lutObject != NULL)
			returnValue = secondLut.lutObject->getMagneticFieldFactor();
		else
			throw cannotGetMagneticFieldFactorOfUndefinedObjectError();

	}
	else {

		tryToAccessMagneticFieldFactorOfFileObjectWarningMsg* tryToAccessMagneticFieldFactorOfFileObject = new tryToAccessMagneticFieldFactorOfFileObjectWarningMsg();
		tryToAccessMagneticFieldFactorOfFileObject->warningMsg();
		if(tryToAccessMagneticFieldFactorOfFileObject != NULL) {
			delete tryToAccessMagneticFieldFactorOfFileObject;
			tryToAccessMagneticFieldFactorOfFileObject = NULL;
		}

	}

	return returnValue;

}

/****************************************************************
 * This method sets the magnetic field to use.					*
 * Caution: This function here has no useful					*
 * implementations in some subclasses. So be					*
 * careful when using.											*
 ****************************************************************/

void lutImplementation::setMagneticField(trackfinderInputMagneticField* magneticField) {

	if (typeUsesMagnetism()) {

		if (secondLut.lutObject != NULL)
			secondLut.lutObject->setMagneticField(magneticField);
		else
			throw cannotSetMagneticFieldOfUndefinedObjectError();

	}
	else {

		tryToAccessMagneticFieldOfFileObjectWarningMsg* tryToAccessMagneticFieldOfFileObject = new tryToAccessMagneticFieldOfFileObjectWarningMsg();
		tryToAccessMagneticFieldOfFileObject->warningMsg();
		if(tryToAccessMagneticFieldOfFileObject != NULL) {
			delete tryToAccessMagneticFieldOfFileObject;
			tryToAccessMagneticFieldOfFileObject = NULL;
		}

	}

}

/****************************************************************
 * This method sets the magnetic field factor to use instead of	*
 * the magneticField.											*
 * Caution: This function here has no useful					*
 * implementations in some subclasses. So be					*
 * careful when using.											*
 ****************************************************************/

void lutImplementation::setMagneticFieldFactor(double magneticFieldFactor) {

	if (typeUsesMagnetism()) {

		if (secondLut.lutObject != NULL)
			secondLut.lutObject->setMagneticFieldFactor(magneticFieldFactor);
		else
			throw cannotSetMagneticFieldFactorOfUndefinedObjectError();

	}
	else {

		tryToAccessMagneticFieldFactorOfFileObjectWarningMsg* tryToAccessMagneticFieldFactorOfFileObject = new tryToAccessMagneticFieldFactorOfFileObjectWarningMsg();
		tryToAccessMagneticFieldFactorOfFileObject->warningMsg();
		if(tryToAccessMagneticFieldFactorOfFileObject != NULL) {
			delete tryToAccessMagneticFieldFactorOfFileObject;
			tryToAccessMagneticFieldFactorOfFileObject = NULL;
		}

	}

}

/****************************************************************
 * This method returns the number of corrections.				*
 * Caution: This function here has no useful					*
 * implementations in some subclasses. So be					*
 * careful when using.											*
 ****************************************************************/

unsigned long lutImplementation::getNumberOfCorrections() {

	unsigned long returnValue;

	if (typeUsesCorrections()) {

		if (secondLut.lutObject != NULL)
			returnValue = secondLut.lutObject->getNumberOfCorrections();
		else
			throw cannotAccessCorrectionsOfUndefinedObjectError();

	}
	else {

		tryToAccessLutCorrectionsOfFileObjectWarningMsg* tryToAccessLutCorrectionsOfFileObject = new tryToAccessLutCorrectionsOfFileObjectWarningMsg();
		tryToAccessLutCorrectionsOfFileObject->warningMsg();
		if(tryToAccessLutCorrectionsOfFileObject != NULL) {
			delete tryToAccessLutCorrectionsOfFileObject;
			tryToAccessLutCorrectionsOfFileObject = NULL;
		}

	}

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

	if (typeUsesCorrections()) {

		if (secondLut.lutObject != NULL)
			returnValue = secondLut.lutObject->getNumberOfCoordCorrections();
		else
			throw cannotAccessCorrectionsOfUndefinedObjectError();

	}
	else {

		tryToAccessLutCorrectionsOfFileObjectWarningMsg* tryToAccessLutCorrectionsOfFileObject = new tryToAccessLutCorrectionsOfFileObjectWarningMsg();
		tryToAccessLutCorrectionsOfFileObject->warningMsg();
		if(tryToAccessLutCorrectionsOfFileObject != NULL) {
			delete tryToAccessLutCorrectionsOfFileObject;
			tryToAccessLutCorrectionsOfFileObject = NULL;
		}

	}

	return returnValue;

}

/****************************************************************
 * This method resets both correction counters.					*
 * Caution: This function here has no useful					*
 * implementations in some subclasses. So be					*
 * careful when using.											*
 ****************************************************************/

void lutImplementation::resetCorrectionCounter() {

	if (typeUsesCorrections()) {

		if (secondLut.lutObject != NULL)
			secondLut.lutObject->resetCorrectionCounter();
		else
			throw cannotAccessCorrectionsOfUndefinedObjectError();

	}
	else {

		tryToAccessLutCorrectionsOfFileObjectWarningMsg* tryToAccessLutCorrectionsOfFileObject = new tryToAccessLutCorrectionsOfFileObjectWarningMsg();
		tryToAccessLutCorrectionsOfFileObject->warningMsg();
		if(tryToAccessLutCorrectionsOfFileObject != NULL) {
			delete tryToAccessLutCorrectionsOfFileObject;
			tryToAccessLutCorrectionsOfFileObject = NULL;
		}

	}

}

/****************************************************************
 * This method evaluates the prelut table.						*
 ****************************************************************/

void lutImplementation::evaluatePrelut(trackfinderInputHit* hit, prelutHoughBorder* borderPointer) {

	if (firstLut.lutObject != NULL)
		firstLut.lutObject->evaluate(hit, borderPointer);
	else
		throw cannotEvaluateBorderOfUndefinedObjectError();

}

/****************************************************************
 * This method evaluates the lut table.							*
 ****************************************************************/

void lutImplementation::evaluateLut(trackfinderInputHit* hit, lutHoughBorder* borderPointer) {

	if (secondLut.lutObject != NULL)
		secondLut.lutObject->evaluate(hit, borderPointer);
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
