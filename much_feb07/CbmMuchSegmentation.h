/** CbmMuchSegmentation.h
 *@author Mikhail Ryzhinskiy <m.ryzhinskiy@gsi.de>
 *@since 20.06.07
 *@version 1.0
 **
 ** class for making parameter file for MUCH digitizer
 ** 
 **/

#ifndef CBMMUCHSEGMENTATION_H
#define CBMMUCHSEGMENTATION_H 1

#include "TObject.h"
#include "TPave.h"
#include "TVector2.h"
#include "TArrayD.h"

#include "CbmTask.h"
#include "CbmGeoNode.h"

#include <vector>
#include <map>

class CbmGeoMuchPar;
class TClonesArray;

class CbmMuchSegmentation : public CbmTask
{

 public:

  /** Default constructor **/
  CbmMuchSegmentation();

  /** Standard constructor 
   *@param digiFileName    Output digitizer file name
   **/
  CbmMuchSegmentation(char* digiFileName);

  /** Destructor **/
  virtual ~CbmMuchSegmentation();

  /** Runs segmentation procedure for each station **/
  void SegmentMuch();

  /** Sets the name of the digitizer parameter file 
   *@param name  Name of the digitizer parameter file 
  **/
  void SetDigiFileName(char* name) { fDigiFileName = name; }

  /** Sets minimal resolution in X for the given station
   *@param iStation  Number of a station (1..fNStations) 
   *@param sigmaX    Value of minimal resolution in X
  **/
  void SetMinSigmaX(Int_t iStation, Double_t sigmaX);

  /** Sets minimal resolution in Y for the given station 
   *@param iStation  Number of a station (1..fNStations) 
   *@param sigmaY    Value of minimal resolution in Y
  **/
  void SetMinSigmaY(Int_t iStation, Double_t sigmaY);

  /** Sets number of stations in the MUCH system **/
  void SetNStations(Int_t nStations) {
     fNStations = nStations;
     fAngle.resize(fNStations);
     fMinSigmaX.resize(fNStations);
     fMinSigmaY.resize(fNStations);
     fMinPadWidth.resize(fNStations);
     fMinPadLength.resize(fNStations);
     fMinSectorWidth.resize(fNStations);
     fMinSectorLength.resize(fNStations);
  }

  /** Sets rotation angle for the given station 
   *@param iStation  Number of a station (1..fNStations) 
  **/
  void SetAngle(Int_t iStation, Double_t angle);

  /** Gets the name of the digitizer parameter file **/
  inline char* GetDigiFileName() const { return fDigiFileName; }

  /** Gets minimal pad width for the given station [cm] 
   *@param iStation  Number of a station (1..fNStations) 
  **/
  inline Double_t GetMinPadWidth(Int_t iStation) const { return fMinPadWidth[iStation-1]; }

  /** Gets minimal pad length for the given station [cm] 
   *@param iStation  Number of a station (1..fNStations) 
  **/
  inline Double_t GetMinPadLength(Int_t iStation) const { return fMinPadLength[iStation-1]; }

  /** Gets minimal sector width for the given station [cm] 
   *@param iStation  Number of a station (1..fNStations) 
  **/
  inline Double_t GetMinSectorWidth(Int_t iStation) const { return fMinSectorWidth[iStation-1]; }

  /** Gets minimal sector length for the given station [cm] 
   *@param iStation  Number of a station (1..fNStations) 
  **/
  inline Double_t GetMinSectorLength(Int_t iStation) const { return fMinSectorLength[iStation-1]; }

  /** Gets minimal resolution in X for the given station [microns] 
   *@param iStation  Number of a station (1..fNStations) 
  **/
  inline Double_t GetMinSigmaX(Int_t iStation) const { return fMinSigmaX[iStation-1]; }

  /** Gets minimal resolution in Y for the given station [microns] 
   *@param iStation  Number of a station (1..fNStations) 
  **/
  inline Double_t GetMinSigmaY(Int_t iStation) const { return fMinSigmaY[iStation-1]; }

  /** Gets number of stations in the MUCH system **/
  inline Int_t GetNStations() const { return fNStations; }


 private:

  enum PlainPart   {kMain, kGaps};               
  enum GeoNodeType {kPassive, kSensitive};       // Type of CbmGeoNode enum
  CbmGeoMuchPar*         fGeoPar;                // Geometry parameters container
  char*                  fDigiFileName;          // Name of the digitizer parameter file
  Int_t                  fNStations;             // Total number of stations
  std::vector<Double_t>  fMinPadWidth;           // Minimal pad width [cm]
  std::vector<Double_t>  fMinPadLength;          // Minimal pad length [cm]
  std::vector<Double_t>  fMinSectorWidth;        // Minimal sector width [cm]
  std::vector<Double_t>  fMinSectorLength;       // Minimal sector length [cm]
  std::vector<Double_t>  fMinSigmaX;             // Minimal resolution on X [microns]
  std::vector<Double_t>  fMinSigmaY;             // Minimal resolution on Y [microns]
  std::vector<Double_t>  fAngle;                 // Rotation angle of each station [degrees]
  std::vector<std::vector<Double_t> > fRadia;    // Radia of regions for each station
  std::vector<std::vector<Double_t> > fRadiaInt; // Radia of regions for each station (excluding internal and external station radia)

  std::vector<Double_t>  fHitDensities;          // Hit density boundaries
  std::vector<Double_t>  fSigmasX;               // Resolutions in X corresponding to hit density boundaries
  std::vector<Double_t>  fSigmasY;               // Resolutions in Y corresponding to hit density boundaries
  std::vector<Int_t>     fHDIndices;             // Indices in fSigmasX, fSigmasY vectors for HD for each gap

  /** Get parameter containers **/
  virtual void SetParContainers();

  /** Intialisation **/
  virtual InitStatus Init();
  
  /** Calculates regions radia where hit density decreases twicely for all stations. **/
  void FillRadia(std::map<Double_t, std::pair<GeoNodeType, CbmGeoNode*> > geoNodesMap);
  /** Calculates internal radia for regions where the hit density reduces twicely
      from the given histogram. **/
  std::vector<Double_t> CalculateInternalRadia(TH1D* hist);
  /** Makes segmentation for a given station **/
  void MakeSegmentation(Int_t iStation);
  /** Defines sector size for each region **/
  void SetSectorSizes(int nRegions, double minSecLx, double minSecLy, std::vector<double>& secLx, std::vector<double>& secLy);
  /** Determines whether a TPave with the given coordinates of its vertices intersects the given TPave. **/
  Bool_t IntersectsPave(TPave* pave, TVector2 p1, TVector2 p2, TVector2 p3, TVector2 p4);
  /** Determines whether a TPave with the given coordinates of its vertices intersects existing TPaves. **/
  Bool_t IntersectsPaves(std::vector<TPave*> paves, TVector2 p1, TVector2 p2, TVector2 p3, TVector2 p4);
  /** Determines whether the fraction of the intersection area of a TPave (with the given coordinates of its vertices) with
      the circle of a given radius is more than the given fraction.  **/
  Bool_t IntersectsFraction(Double_t iRadius, Double_t x1, Double_t y1, Double_t x2, Double_t y2, Double_t fraction);
  /** Draws TPave with the given coordinates and fills it with a given color.**/
  void DrawPave(Double_t x1, Double_t y1, Double_t x2, Double_t y2, std::vector<TPave*>& paves, Int_t iColor);
  /** Draws TPaves of the given width and height within the ring af the given radia. **/
  void DrawPaves(Double_t rad1, Double_t rad2, Double_t width, Double_t height, 
		 std::vector<TPave*>& paves, Int_t iColor, PlainPart iPart, Double_t fraction);
  /** Calculates minimal sizes of pads for the given station. **/
  void CalculateMinimalSizes(int iStation);
  /** Creates and fills digi file with appropriate data. **/
  void CreateDigiFile(int iStation, int iType, std::vector<TPave*> paves);

  ClassDef(CbmMuchSegmentation,1)
};

#endif
