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

#include "CbmTask.h"

#include "TStopwatch.h"
#include "TPolyLine.h"
#include "TMath.h"
#include "TLine.h"
#include "TRandom3.h"


#include <set>
#include <map>

class TClonesArray;
class CbmGeoMuchPar;
class CbmMuchDigiPar;
class CbmMuchStation;
class CbmMuchPoint;
class CbmMuchDigiScheme;
class Point;
class Segment;
class TFile;


enum DetectorType {kGEM, kMICROMEGAS};

class CbmMuchDigitize : public CbmTask
{

 public:

  /** Default constructor **/
  CbmMuchDigitize();

  /** Standard constructor **/
  CbmMuchDigitize(Int_t iVerbose);

  /** Constructor with name **/
  CbmMuchDigitize(const char* name, Int_t iVerbose);

  /** Destructor **/
  virtual ~CbmMuchDigitize();

  /** Execution **/
  virtual void Exec(Option_t* opt);

  /** Sets switch whether to use (=1) avalanche or not (=0). 0 by default. **/
  inline void SetUseAvalanche(Int_t useAvalanche) { fUseAvalanche = useAvalanche; }

  /** Sets radius of a spot from secondary electrons [cm] (0.03 cm by default). **/
  inline void SetSpotRadius(Double_t spotRadius) { fSpotRadius = spotRadius; }

  /** Sets detector type used in MUCH. **/
  void SetDetectorType(DetectorType type);

  /** Sets mean gas gain value. **/
  inline void SetMeanGasGain(Double_t gasGain)   { fMeanGasGain = gasGain; }

  /** Sets maximal charge [electrons] which a pad able to collect. **/
  inline void SetQMaximum(UInt_t qMax) { fQMax = qMax; }

  /** Sets charge threshold [ADC channels] for pads. **/
  inline void SetQThreshold(UInt_t qThreshold) { fQThreshold = qThreshold; }

  /** Sets the time resolution. **/
  inline void SetDTime(Double_t dTime) { fDTime = dTime; }

  /** Gets time resolution **/
  inline Double_t GetDTime() { return fDTime; }

    /** Parametrization of sigma for electrons for Lndau distribution 
   *@param logT  Logarithm of electron kinetic energy (energy in [MeV])
   **/
  static Double_t Sigma_n_e(Double_t Tkin, Double_t mass);

  /** Parametrization of most probable value for electrons for Lndau distribution 
   *@param logT  Logarithm of electron kinetic energy (energy in [MeV])
   **/
  static Double_t MPV_n_e(Double_t Tkin, Double_t mass);

 private:

  CbmGeoMuchPar*     fGeoPar;        // Geometry parameter container
  CbmMuchDigiPar*    fDigiPar;       // Digitization parameter container
  CbmMuchDigiScheme* fDigiScheme;    // Digitization scheme
  TClonesArray*      fPoints;        // Input array of CbmMuchPoint
  TClonesArray*      fMCTracks;      // Input array of MCTrack
  TClonesArray*      fDigis;         // Output array of CbmMuchDigi
  TClonesArray*      fDigiMatches;   // Output array of CbmMuchDigiMatches
  Int_t              fNPoints;       // Total number of points
  Int_t              fNFailed;       // Total number of points which digitization has failed
  Int_t              fNOutside;      // Total number of points which was found outside a detector
  Int_t              fNMulti;        // Total number of channels that was hitby several points
  Int_t              fNDigis;        // Total number of created Digis
  Int_t              fUseAvalanche;  // Switch whether to use (=1) avalanches or not (=0). 0 by default.
  UInt_t             fQMax;          // Maximum charge that a pad can collect [electrons]
  UInt_t             fQThreshold;    // Charge threshold [channels of ADC]
  Double_t           fSpotRadius;    // Spot radius from secondary electrons [cm]
  Double_t           fMeanGasGain;   // Mean gas gain value (1e4 by default)
  Double_t           fDTime;         // Time resolution [ns]
  Int_t              fEvent;         // Event counter
  TRandom3*          fRnd;           // Random generator
  TRandom*           fLandauRnd;
  TStopwatch         fTimer;         // Timer

  /** Map of active channels (pair detectorId, channel number) 
   * to index of MuchDigi 
   **/
  std::map<std::pair<Int_t, Int_t>, Int_t> fChannelMap; 
  /** Map of channels (pair detectorId, channel number), wounded by 
   * secondary electrons, to CbmMuchDigi.
   **/
  std::map<std::pair<Int_t, Int_t>, CbmMuchDigi*> fChargedPads;   
  /** Map of DigiMatches (pair detectorId, channel number), which matches
   * pads wounded by secondary electrons.
   **/
  std::map<std::pair<Int_t, Int_t>, CbmMuchDigiMatch*> fChargedMatches;

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
  Bool_t ExecAdvanced(CbmMuchStation* station, CbmMuchPoint* point, Int_t iPoint);

  /** Simple digis production without avalanches **/
  Bool_t ExecSimple(CbmMuchStation* station, CbmMuchPoint* point, Int_t iPoint, Int_t& inc);

  /** Simple digis production without avalanches **/
  Bool_t ExecStraws(CbmMuchStation* station, CbmMuchPoint* point, Int_t iPoint, Int_t& inc);

  /** Fills output arrays with digis, which have charge above the threshold **/
  void FirePads();

  /** Builds a TPolyLine from given rectangle parameters 
   *@param x0     X of the center
   *@param y0     Y of the center
   *@param width  Width
   *@param height Height
   *@param sinrot Sin of rotation angle
   *@param cosrot Cos of rotation angle
   **/
  TPolyLine GetPolygon(Double_t x0, Double_t y0, 
		       Double_t width, Double_t height, 
		       Double_t sinrot, Double_t cosrot);

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




#ifndef POINT_H
#define POINT_H 1
/** A 2D point inherited from TObject **/
class Point : public TObject 
{
 public:
  Point() {}
  Point(Double_t x, Double_t y) {
    fX = x; fY = y;
  }
  Double_t GetX() { return fX; }
  Double_t GetY() { return fY; }
  void SetX(Double_t x) { fX = x; }
  void SetY(Double_t y) { fY = y; }  

 private:
  Double_t fX;
  Double_t fY;
  ClassDef(Point,1)


};
#endif


//#ifndef SEGMENT_H
//#define SEGMENT_H 1
///** Holds a segment object **/
//class Segment : public TLine 
//{
// public:
//  
//  Segment(Double_t x1, Double_t y1, Double_t x2, Double_t y2)
//    :TLine(x1,y1,x2,y2){
//    
//    // Get a, b, c
//    a = y1 - y2;
//    b = x2 - x1;
//    c = x1*y2 - x2*y1;
//  }
//  
//  Bool_t IsHorizontal(){
//    if(TMath::Abs(fY1 - fY2) <=1e-5) return kTRUE;
//    return kFALSE;
//  }
//  
//  Bool_t IsVertical(){
//    if(TMath::Abs(fX1 - fX2) <= 1e-5) return kTRUE;
//    return kFALSE;
//  }
//  
//  // Checks whether the segment contains the given point
//  Bool_t ContainsPoint(Double_t x, Double_t y){
//    if(TMath::Abs(a*x + b*y + c) > 1e-5) 
//      return kFALSE;
//    
//    if(IsVertical()){
//      if(fY1 < fY2) return (y>=fY1 && y<=fY2) ? kTRUE : kFALSE;
//      else return (y>=fY2 && y<=fY1) ? kTRUE : kFALSE;
//    }
//    else if(IsHorizontal()){
//      if(fX1 < fX2) return (x>=fX1 && x<=fX2) ? kTRUE : kFALSE;
//      else return (x>=fX2 && x<=fX1) ? kTRUE : kFALSE;
//    }
//    else{
//      if(fX1 < fX2) return (x>=fX1 && x<=fX2) ? kTRUE : kFALSE;
//      else return (x>=fX2 && x<=fX1) ? kTRUE : kFALSE;
//    }
//    return kFALSE;
//    
//  }
//
//  Double_t GetA() { return a; }
//  Double_t GetB() { return b; }
//  Double_t GetC() { return c; }
//
//
// private:
//  Double_t a;
//  Double_t b;
//  Double_t c;  
//
//  ClassDef(Segment,1)
//
//};
//#endif



