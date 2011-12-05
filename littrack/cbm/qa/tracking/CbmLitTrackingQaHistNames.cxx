/**
 * \file CbmLitTrackingQaHistNames.cxx
 * \brief Helper class for histogram names.
 * \author Semen Lebedev <s.lebedev@gsi.de>
 * \date 2011
 */
#include "CbmLitTrackingQaHistNames.h"
#include <boost/assign/list_of.hpp>
using boost::assign::list_of;

const vector<string> CbmLitTrackingQaHistNames::fTypes = list_of("Acc")("Rec")("Eff");
const vector<string> CbmLitTrackingQaHistNames::fTypeDetAcc = list_of("Mc")("Acc")("Eff");

const vector<string> CbmLitTrackingQaHistNames::fCatTracking = list_of("All")("Ref")("Prim")("Sec")("Mu")("El");
const vector<string> CbmLitTrackingQaHistNames::fCatRich = list_of("All")("AllRef")("El")("ElRef")("Pi")("PiRef");
const vector<string> CbmLitTrackingQaHistNames::fCatElid = list_of("ElId")("PiSupp");
const vector<string> CbmLitTrackingQaHistNames::fCatDetAcc = list_of("El");
