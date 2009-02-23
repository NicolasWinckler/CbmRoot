/** CbmMuchDigitize.h
 *@author Mikhail Ryzhinskiy <m.ryzhinskiy@gsi.de>
 *@since 19.03.07
 *@version 1.0
 **
 ** CBM task class for digitizing MUCH
 ** Task level RECO
 ** Produces objects of type CbmMuchDigi out of CbmMuchPoint.
 **/

#ifndef CBMMUCHDIGITIZE_H
#define CBMMUCHDIGITIZE_H 1

#include "TH1.h"
#include "TH2.h"

#include "FairTask.h"

#include "TStopwatch.h"
#include "TPolyLine.h"
#include "TMath.h"
#include "TLine.h"
#include "TRandom3.h"
#include "TRandom.h"
#include "TClonesArray.h"
#include "TFile.h"

#include "CbmMuchDigi.h"
#include "CbmMuchDigiMatch.h"
#include "CbmMuchGeoScheme.h"

#include <set>
#include <map>

class CbmMuchSector;
class CbmMuchPoint;
class TFile;

enum DetectorType {kGEM, kMICROMEGAS};

class CbmMuchDigitize : public FairTask
{

 public:

  /** Default constructor **/
  CbmMuchDigitize();

  /** Standard constructor **/
  CbmMuchDigitize(Int_t iVerbose);

  /** Constructor with name **/
  CbmMuchDigitize(const char* name, const char* digiFileName, Int_t iVerbose);

  /** Destructor **/
  virtual ~CbmMuchDigitize();

  /** Execution **/
  virtual void Exec(Option_t* opt);

  /** Sets switch whether to use (=1) avalanche or not (=0). 0 by default. **/
  void SetUseAvalanche(Int_t useAvalanche) { fUseAvalanche = useAvalanche; }

  /** Sets radius of a spot from secondary electrons [cm] (0.03 cm by default). **/
  void SetSpotRadius(Double_t spotRadius) { fSpotRadius = spotRadius; }

  /** Sets detector type used in MUCH. **/
  void SetDetectorType(DetectorType type);

  /** Sets mean gas gain value. **/
  void SetMeanGasGain(Double_t gasGain)   { fMeanGasGain = gasGain; }

  /** Sets maximal charge [electrons] which a pad able to collect. **/
  void SetQMaximum(UInt_t qMax) { fQMax = qMax; }

  /** Sets charge threshold [ADC channels] for pads. **/
  void SetQThreshold(UInt_t qThreshold) { fQThreshold = qThreshold*fQMax/fNADCChannels; }

  /** Sets number of ADC channels. **/
  void SetNADCChannels(UInt_t nADCChannels) { fNADCChannels = nADCChannels; }

  /** Sets mean electronics value [electrons]. **/
  void SetMeanNoise(UInt_t meanNoise) { fMeanNoise = meanNoise; }

  /** Sets the time resolution. **/
  void SetDTime(Double_t dTime) { fDTime = dTime; }

  /** Gets time resolution **/
  Double_t GetDTime() { return fDTime; }

  /** Parameterization of sigma for electrons for Landau distribution
   *@param logT  Logarithm of electron kinetic energy (energy in [MeV])
   **/
  static Double_t Sigma_n_e(Double_t Tkin, Double_t mass);

  /** Parameterization of most probable value for electrons for Landau distribution
   *@param logT  Logarithm of electron kinetic energy (energy in [MeV])
   **/
  static Double_t MPV_n_e(Double_t Tkin, Double_t mass);

 private:

  CbmMuchGeoScheme*  fGeoScheme;     // TODO add comments
  TFile*             fDigiFile;      // Digitization file

  TClonesArray*      fPoints;        // Input array of CbmMuchPoint
  TClonesArray*      fMCTracks;      // Input array of MCTrack
  TClonesArray*      fDigis;         // Output array of CbmMuchDigi
  TClonesArray*      fDigiMatches;   // Output array of CbmMuchDigiMatches
  Int_t              fNFailed;       // Total number of points which digitization has failed
  Int_t              fNOutside;      // Total number of points which was found outside a detector
  Int_t              fNMulti;        // Total number of channels that was hitby several points
  Int_t              fUseAvalanche;  // Switch whether to use (=1) avalanches or not (=0). 0 by default.
  UInt_t             fNADCChannels;  // Number of ADC channels
  UInt_t             fQMax;          // Maximum charge that a pad can collect [electrons]
  UInt_t             fQThreshold;    // Charge threshold [electrons]
  UInt_t             fMeanNoise;     // Mean electronics noise value [electrons]
  Double_t           fSpotRadius;    // Spot radius from secondary electrons [cm]
  Double_t           fMeanGasGain;   // Mean gas gain value (1e4 by default)
  Double_t           fDTime;         // Time resolution [ns]
  Int_t              fEvent;         // Event counter
  TRandom3*          fRnd;           // Random generator
  TRandom*           fLandauRnd;     // Random Landau distr. generator
  TStopwatch         fTimer;         // Timer

  /** Map of active channels to index of MuchDigi. **/
  std::map<Long64_t, Int_t> fChannelMap;

  /** Map of channels, wounded by secondary electrons, to CbmMuchDigi.  **/
  std::map<Long64_t, CbmMuchDigi*> fChargedPads;

  /** Map of DigiMatches, which matches pads wounded by secondary electrons.  **/
  std::map<Long64_t, CbmMuchDigiMatch*> fChargedMatches;

  /** Finish **/
  virtual void Finish();

  /** Get parameter containers **/
  virtual void SetParContainers();

  /** Intialisation **/
  virtual InitStatus Init();

  /** Reinitialisation **/
  virtual InitStatus ReInit();

  /** Reset eventwise counters **/
  void Reset();

  /** Advanced digis production using avalanches **/
  Bool_t ExecAdvanced(CbmMuchPoint* point, Int_t iPoint);

  /** Simple digis production without avalanches **/
  Bool_t ExecSimple(CbmMuchPoint* point, Int_t iPoint);

  /** Fills output arrays with digis, which have charge above the threshold **/
  void FirePads();

  /** Add electronics noise. **/
  void AddNoise();

  /** Add electronics noise to the given pad. **/
  void AddNoise(CbmMuchPad* pad);

  /** Builds a TPolyLine from given rectangle parameters (no rotation angle)
   *@param x0     X of the center
   *@param y0     Y of the center
   *@param width  Width
   *@param height Height
   **/
  TPolyLine GetPolygon(Double_t x0, Double_t y0,
		       Double_t width, Double_t height);

  /** Verifies whether projections with given coordinates are intersected
   *@param x11     coordinate of one end of the first projection
   *@param x12     coordinate of another end of the first projection
   *@param x21     coordinate of one end of the second projection
   *@param x22     coordinate of another end of the second projection
   *@param length  length of intresection
   **/
  Bool_t ProjectionsIntersect(Double_t x11, Double_t x12, Double_t x21, Double_t x22, Double_t& length);

  /** Determines whether the two given polygons intersect and calculates the
   *  area of intersection.
   *@param polygon1  Fisrt polygon
   *@param polygon2  Second polygon
   *@param area      Intersection area
   **/
  Bool_t PolygonsIntersect(CbmMuchSector* sector, TPolyLine polygon1, TPolyLine polygon2,
			   Double_t& area);

  /*****************************************************************
   * Function returns a random number distributed according
   * exponential law, which reproduces the gas gain fluctuation
   *@author Volodia Nikulin
   *@since 14/04/2007
   ****************************************************************/
  inline Int_t GasGain();

  static Double_t e_sigma_n_e(Double_t &logT);
  static Double_t e_MPV_n_e(Double_t &logT);
  static Double_t mu_sigma_n_e(Double_t &logT);
  static Double_t mu_MPV_n_e(Double_t &logT);
  static Double_t p_sigma_n_e(Double_t &logT);
  static Double_t p_MPV_n_e(Double_t &logT);

  ClassDef(CbmMuchDigitize,1)
};
#endif
