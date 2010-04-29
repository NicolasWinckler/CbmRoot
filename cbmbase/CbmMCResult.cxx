/*
 * CbmMCResult.cpp
 *
 *  Created on: Dec 1, 2009
 *      Author: stockman
 */

#include "CbmMCResult.h"

ClassImp(CbmMCResult);

CbmMCResult::CbmMCResult() {
}

CbmMCResult::~CbmMCResult() {
}

CbmMCResult::CbmMCResult(fDetectorType start, fDetectorType stop)
: fStartType(start), fStopType(stop)
{}

