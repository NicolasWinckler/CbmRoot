#ifndef CBM_ANA_LMVM_NAMES_H
#define CBM_ANA_LMVM_NAMES_H

#include <vector>
#include <string>

using namespace std;

/*
 * \brief Enumeration for different sources.
 */
enum SourceTypes {
  kSignal = 0,
  kBg = 1,
  kPi0 = 2,
  kGamma = 3
};

/*
 * \brief Enumeration for analysis steps.
 */
enum AnalysisSteps {
  kMc = 0,
  kAcc = 1,
  kReco = 2,
  kChi2Prim = 3,
  kElId = 4,
  kGammaCut = 5,
  kMvd1Cut = 6,
  kMvd2Cut = 7,
  kStCut = 8,
  kTtCut = 9,
  kPtCut = 10
};

/*
 * \brief Enumeration for different sources of BG pairs
 */
enum BgPairSource {
  kGG = 0,
  kPP = 1,
  kOO = 2,
  kGP = 3,
  kGO = 4,
  kPO = 5
};

/*
 * \brief Enumeration for signal names.
 */
/*enum SignalNames {
  kRho0 = 0,
  kOmega = 1,
  kPhi = 2,
  kOmegaDalitz = 3
};*/

class CbmAnaLmvmNames
{
public:
   const static int fNofSourceTypes = 4;
   const static vector<string> fSourceTypes;
   const static vector<string> fSourceTypesLatex;
   const static vector<int> fSourceTypesColor;

   const static int fNofAnaSteps = 11;
   const static vector<string> fAnaSteps;
   const static vector<string> fAnaStepsLatex;
   const static vector<int> fAnaStepsColor;

   const static vector<string> fSignalNames;

   const static int fNofBgPairSources = 6;
   const static vector<string> fBgPairSourceLatex;
};

#endif

