/*
 * CbmLitQaHistNames.cxx
 *
 *  Created on: 02.11.2011
 *      Author: slebedev
 */

#include "CbmLitQaHistNames.h"

const vector<string> CbmLitQaHistNames::fTypes = list_of("Acc")("Rec")("Eff");
const vector<string> CbmLitQaHistNames::fTypeDetAcc = list_of("Mc")("Acc")("Eff");

const vector<string> CbmLitQaHistNames::fCatTracking = list_of("All")("Ref")("Prim")("Sec")("Mu")("El");
const vector<string> CbmLitQaHistNames::fCatRich = list_of("All")("AllRef")("El")("ElRef")("Pi")("PiRef");
const vector<string> CbmLitQaHistNames::fCatElid = list_of("ElId")("PiSupp");
const vector<string> CbmLitQaHistNames::fCatDetAcc = list_of("El");
