/** CbmMuchDigitizeAdvancedGem.h
 *@author Evgeny Kryshen <e.kryshen@gsi.de>
 *@since 01.05.11
 *@version 2.0
 *@author Mikhail Ryzhinskiy <m.ryzhinskiy@gsi.de>
 *@since 19.03.07
 *@version 1.0
 **
 ** CBM task class for digitizing MUCH
 ** Task level RECO
 ** Produces objects of type CbmMuchDigi out of CbmMuchPoint.
 **/

#ifndef CBMMUCHDIGITIZEADVANCEDGEM_H
#define CBMMUCHDIGITIZEADVANCEDGEM_H 1

#include "FairTask.h"

#include "TStopwatch.h"
#include "TPolyLine.h"
#include "TMath.h"
#include "TClonesArray.h"
#include "TString.h"

#include "CbmMuchDigi.h"
#include "CbmMuchDigiMatch.h"
#include "CbmMuchGeoScheme.h"
#include "TF1.h"
#include <map>


class CbmMuchSector;
class CbmMuchPoint;
class CbmMCEpoch;
class CbmMuchPad;
class CbmMuchRadialPad;
class CbmMuchRadialSector;
class TChain;

enum DetectorType {kGEM, kMICROMEGAS};

static double sigma_e[]  = { 4.06815, -0.225699, 0.464502, -0.141208, 0.0226821,-0.00195697, 6.87497e-05 };
static double sigma_mu[] = { 74.5272, -49.7648, 14.4886, -2.23059, 0.188254,-0.00792744, 0.00011976 };
static double sigma_p[]  = { 175.879, -15.016, -34.6513, 13.346, -2.08732, 0.153678,-0.00440115 };
static double mpv_e[]    = { 14.654, -0.786582, 2.32435, -0.875594, 0.167237,-0.0162335, 0.000616855 };
static double mpv_mu[]   = { 660.746, -609.335, 249.011, -55.6658, 7.04607, -0.472135, 0.0129834 };
static double mpv_p[]    = { 4152.73, -3123.98, 1010.85, -178.092, 17.8764, -0.963169, 0.0216643 };
static double min_logT_e  = -3.21888; 
static double min_logT_mu = -0.916291; 
static double min_logT_p  =  1.0986; 

class CbmMuchDigitizeAdvancedGem : public FairTask
{

  public:

    /** Default constructor. */
    CbmMuchDigitizeAdvancedGem();

    /** Standard constructor. */
    CbmMuchDigitizeAdvancedGem(Int_t iVerbose);

    /** Constructor with name. */
    CbmMuchDigitizeAdvancedGem(const char* name, const char* digiFileName, Int_t iVerbose);

    /** Destructor. */
    virtual ~CbmMuchDigitizeAdvancedGem();

    /** Execution. */
    virtual void Exec(Option_t* opt);

    /**
     * Sets radius of a spot from secondary electrons.
     * @param spotRadius Spot radius [cm] (0.03 cm by default).
     */
    void SetSpotRadius(Double_t spotRadius = 0.05) { fSpotRadius = spotRadius; }

    /**
     * Sets mean gas gain value.
     * @param gasGain Mean gas gain value [electrons] (10000 by default).
     */
    void SetMeanGasGain(Double_t gasGain)   { fMeanGasGain = gasGain; }

    /**
     * Sets maximal charge which a pad able to collect.
     * @param qMax Charge value [electrons] (440000 by default).
     */
    void SetQMaximum(UInt_t qMax) { fQMax = qMax; }

    /**
     * Sets charge threshold [ADC channels] for pads.
     * @param qThreshold Charge threshold value [ADC channels] (3 by default).
     */
    void SetQThreshold(UInt_t qThreshold) { fQThreshold = qThreshold*fQMax/fNADCChannels; }

    /**
     * Sets number of ADC channels.
     * @param nADCChannels  Number of ADC channels (256 by default).
     */
    void SetNADCChannels(UInt_t nADCChannels) { fNADCChannels = nADCChannels; }

    /**
     * Sets mean electronics noise value.
     * @param meanNoise Mean noise value [electrons] (0 by default).
     */
    void SetMeanNoise(UInt_t meanNoise) { fMeanNoise = meanNoise; }

    /**
     * Sets the probability (normalized to unity) to find a dead pad.
     * @param deadPadsFrac Probability to find a dead pad (0-1).
     */
    void SetDeadPadsFrac(Double_t deadPadsFrac) { fDeadPadsFrac = deadPadsFrac; }

    /** Sets the time resolution.
     * @param dTime Time resolution [ns].
     */
    void SetDTime(Double_t dTime) { fDTime = dTime; }

    /** Gets time resolution [ns]. **/
    Double_t GetDTime() { return fDTime; }

    /** Parameterization of sigma for electrons for Landau distribution.
     *@param logT Logarithm of electron kinetic energy (energy in [MeV]).
     *@param mass Mass of the particle [MeV/c^2].
     **/
    static Double_t Sigma_n_e(Double_t Tkin, Double_t mass);

    /** Parameterization of most probable value for electrons for Landau distribution.
     *@param logT  Logarithm of electron kinetic energy (energy in [MeV]).
     *@param mass Mass of the particle [MeV/c^2].
     **/
    static Double_t MPV_n_e(Double_t Tkin, Double_t mass);
    
    void SetEpoch(Bool_t epoch) {fEpoch=epoch;}
    void SetMcChain(TChain* mcChain) {fMcChain=mcChain;}
    void SetDeadTime(Double_t deadTime) {fDeadTime = deadTime; } 
    void SetDriftVelocity(Double_t velocity) {fDriftVelocity = velocity; }
    void SetPeakingTime(Double_t peakingTime) {fPeakingTime = peakingTime; }
    void SetRemainderTime(Double_t remainderTime) {fRemainderTime = remainderTime; }
    void SetTimeBinWidth(Double_t timeBinWidth) {fTimeBinWidth = timeBinWidth; }
  private:
    CbmMuchGeoScheme*  fGeoScheme;     // Main object responsible for geometry
    TString            fDigiFile;      // Digitization file
    TClonesArray*      fPoints;        // Input array of CbmMuchPoint
    TClonesArray*      fMCTracks;      // Input array of MCTrack
    TClonesArray*      fDigis;         // Output array of CbmMuchDigi
    TClonesArray*      fDigiMatches;   // Output array of CbmMuchDigiMatches
    CbmMCEpoch*        fMcEpoch;      // Output array of CbmMuchDigiMatches
    Int_t              fNFailed;       // Total number of points which digitization has failed
    Int_t              fNOutside;      // Total number of points which was found outside a detector
    Int_t              fNMulti;        // Total number of channels that was hitby several points
    UInt_t             fNADCChannels;  // Number of ADC channels
    UInt_t             fQMax;          // Maximum charge that a pad can collect [electrons]
    UInt_t             fQThreshold;    // Charge threshold [electrons]
    UInt_t             fMeanNoise;     // Mean electronics noise value [electrons]
    Double_t           fSpotRadius;    // Spot radius from secondary electrons [cm]
    Double_t           fMeanGasGain;   // Mean gas gain value (1e4 by default)
    Double_t           fDTime;         // Time resolution [ns]
    Int_t              fEvent;         // Event counter
    Double_t           fDeadPadsFrac;  // Probability to find a dead pad
    TStopwatch         fTimer;         // Timer
    Bool_t             fEpoch;         // Epoch digitizer fEpoch=1. Default fEpoch=0 
    TChain*            fMcChain;       // Chain of McFiles with McTrack info    
    Double_t           fDeadTime;      // Channel dead time [ns]
    Double_t           fDriftVelocity; // Drift Velocity [um/ns]
    Double_t           fPeakingTime;   // Peaking time [ns]
    Double_t           fRemainderTime; // Remainder time = t_r [ns]: remainder is simulated as exp(-t/t_r)
    Double_t           fTimeBinWidth;  // Width of the bin for signal shape simulation
    Int_t              fChainEventId;  // Temporary solution for accessing MC chain in epoch approach
//    static TF1*        fPol6;          // 6-order polynomial for Landau sigma and MPV
    
    /** Finish. **/
    virtual void FinishTask();

    /** Get parameter containers. **/
    virtual void SetParContainers();

    /** Initialization. **/
    virtual InitStatus Init();

    /** Re-initialization. **/
    virtual InitStatus ReInit();

    /** Reset eventwise counters. **/
    void Reset();

    /** Advanced digis production using avalanches. **/
    Bool_t ExecAdvanced(CbmMuchPoint* point, Int_t iPoint);

    /** Builds a TPolyLine from given rectangle parameters (no rotation angle).
     *@param x0     X of the center.
     *@param y0     Y of the center.
     *@param width  Width.
     *@param height Height.
     **/

    Bool_t AddDigi(CbmMuchPad* pad);
    Bool_t AddDigi(CbmMuchRadialPad* pad);
    inline Int_t GasGain();

    Int_t GetNPrimaryElectrons(CbmMuchPoint* point);
    Bool_t AddCharge(CbmMuchRadialPad* pad, UInt_t charge, Int_t iPoint, Double_t time, Double_t aL);
    Bool_t AddCharge(CbmMuchRadialSector* s,UInt_t ne, Int_t iPoint, Double_t time, Double_t aL,Double_t phi1, Double_t phi2);

    Double_t fTotalDriftTime;
    
    ClassDef(CbmMuchDigitizeAdvancedGem,1)
};
#endif
