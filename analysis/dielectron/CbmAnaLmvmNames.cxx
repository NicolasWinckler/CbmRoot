/**
 * \file CbmAnaLmvmNames.cxx
 * \brief Helper class for histogram names.
 * \date 2012
 */
#include "CbmAnaLmvmNames.h"
#include "TColor.h"
#include <boost/assign/list_of.hpp>

using boost::assign::list_of;

const vector<string> CbmAnaLmvmNames::fSourceTypes =
      list_of("signal")("bg")("pi0")("gamma");
const vector<string> CbmAnaLmvmNames::fSourceTypesLatex =
      list_of("S")("BG")("#pi^{0}")("#gamma");
const vector<int> CbmAnaLmvmNames::fSourceTypesColor =
      list_of(kRed)(kBlue)(kGreen)(kOrange);
const vector<string> CbmAnaLmvmNames::fAnaSteps =
      list_of("mc")("acc")("reco")("chi2prim")("elid")("gammacut")("mvd1cut")("mvd2cut")("stcut")("ttcut")("ptcut");
const vector<string> CbmAnaLmvmNames::fAnaStepsLatex =
      list_of("MC")("ACC")("REC")("#chi^{2}_{prim}")("ID")("m_{#gamma}")("mvd1")("mvd2")("ST")("TT")("P_{t}");
const vector<int> CbmAnaLmvmNames::fAnaStepsColor =
      list_of(kGreen+3)(kOrange+3)(kBlack)(kOrange+7)(kRed)(kPink-6)(kGreen)(kOrange-3)(kViolet+10)(kMagenta)(kYellow+1);
const vector<string> CbmAnaLmvmNames::fSignalNames =
      list_of("rho0")("omega")("phi")("omega_dalitz");
const vector<string> CbmAnaLmvmNames::fBgPairSourceLatex =
      list_of("#gamma-#gamma")("#pi^{0}-#pi^{0}")("o.-o.")("#gamma-#pi^{0}")("#gamma-o.")("#pi^{0}-o.");
