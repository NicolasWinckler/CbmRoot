/*
 * CbmMCStage.cpp
 *
 *  Created on: Dec 1, 2009
 *      Author: stockman
 */

#include "CbmMCStage.h"

ClassImp(CbmMCStage);

CbmMCStage::CbmMCStage():fWeight(1.0), fLoaded(kFALSE), fFill(kFALSE) {
}

CbmMCStage::~CbmMCStage() {
}

CbmMCStage::CbmMCStage(fDetectorType id, std::string fileName, std::string branchName, Double_t weight)
: CbmMCObject(id), fFileName(fileName), fBranchName(branchName), fWeight(weight), fLoaded(kFALSE), fFill(kFALSE)
{}

