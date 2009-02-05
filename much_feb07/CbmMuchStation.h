/** CbmMuchStation.h
 *@author  M.Ryzhinskiy <m.ryzhinskiy@gsi.de>
 *@version 1.0
 *@since   15.03.07
 **
 ** This class holds the transport geometry parameters 
 ** of one MuCh tracking station necessary for digitization.
 **/


#ifndef CBMMUCHSTATION_H
#define CBMMUCHSTATION_H 1


#include "TObjArray.h"
#include "TNamed.h"
#include "TString.h"

#include <map>
#include <vector>
#include <iostream>

using std::cout;
using std::endl;

class CbmMuchSector;
class CbmMuchSectorDigiPar;


class CbmMuchStation : public TNamed
{

 public:

  /** Default constructor **/
  CbmMuchStation();


  /** Standard constructor 
  *@param name      Volume name of station
  *@param iStation  Station number 
  *@param z         z position of station centre [cm]
  *@param d         thickness of station [cm]
  *@param rl        radiation length of material [cm]
  *@param rmin      Inner radius [cm]
  *@param rmax      Outer radius [cm]
  *@param rotation  Rotation angle in global c.s. [rad]
  **/
  CbmMuchStation(const char* name, Int_t detId, Double_t z, Double_t d,
		Double_t rl, Double_t rmin, Double_t rmax, 
		Double_t rotation);


  /** Destructor **/
  virtual ~CbmMuchStation();


  /** Accessors **/
  inline Int_t    GetDetectorId() const { return fDetectorId; }
  inline Int_t    GetSystemId()   const { 
    return ( ( fDetectorId & (15<<24) ) >> 24); }
  inline Int_t    GetStationNr()  const { 
    return ( ( fDetectorId & (255<<16) ) >> 16 ); }
  inline Double_t GetZ()          const { return fZ; }
  inline Double_t GetD()          const { return fD; }
  inline Double_t GetRotation()   const { return fRotation; }
  inline Double_t GetSinRot()     const { return fSinRot; }
  inline Double_t GetCosRot()     const { return fCosRot; }
  inline Double_t GetRadLength()  const { return fRadLength; }
  inline Double_t GetRmin()       const { return fRmin; }
  inline Double_t GetRmax()       const { return fRmax; }
  inline Int_t    GetNSectors()   const { return fSectors->GetEntriesFast(); }
  Int_t    GetNChannels();
  inline TObjArray* GetSectors() { return fSectors; }
  inline CbmMuchSector* GetSector(Int_t iSector) { 
    return (CbmMuchSector*) fSectors->At(iSector); }
  CbmMuchSector* GetSectorByNr(Int_t sectorNr);

  // Access grid parameters
  /** Gets number of a sector corresponding to grid cell
   * (its row and column number).
   **/
  inline Int_t GetSectorNr(Int_t iRow, Int_t iCol) const { 
    if(iRow < 0) iRow = 0;
    if(iRow > fGridRows - 1) iRow = fGridRows - 1;
    if(iCol < 0) iCol = 0;
    if(iCol > fGridCols - 1) iCol = fGridCols - 1;
    return fGridIndices[iRow][iCol]; }
  CbmMuchSector* GetSector(Int_t iRow, Int_t iCol);
  inline Double_t GetGridDx()   const { return fGridDx; }
  inline Double_t GetGridDy()   const { return fGridDy; }
  inline Double_t GetGridLx()   const { return fGridLx; }
  inline Double_t GetGridLy()   const { return fGridLy; }
  inline Int_t    GetGridCols() const { return fGridCols; }
  inline Int_t    GetGridRows() const { return fGridRows; }


  /** Add one sector to the array **/
  void AddSector(CbmMuchSectorDigiPar* sectorPar);
  void AddSector(CbmMuchSector* sector);


  /** Reset all eventwise counters **/
  void Reset();


  /** Output to screen **/
  virtual void Print();

  /** Initializes grid for the station in order to make fast search in the future **/
  void InitGrid();
  /** Adds to each sector info about neighbour sectors **/
  void InitNeighbourSectors();
  /** Adds to each pad info about neighbour pads **/
  void InitNeighbourPads();


 protected:

  TString    fName;             // Station name
  Int_t      fDetectorId;       // Unique detector ID
  Double32_t fZ;                // z position of station centre (midplane) [cm]
  Double32_t fD;                // Thickness of station
  Double32_t fRadLength;        // Radiation length of material [cm]
  Double32_t fRmin;             // Inner radius [cm]
  Double32_t fRmax;             // Outer radius [cm]
  Double32_t fRotation;         // Rotation angle in global c.s [rad]
  Double32_t fSinRot;           // Sin of fRotation
  Double32_t fCosRot;           // Cos of fRotation
  TObjArray* fSectors;          // Array of CbmMuchSectors
  std::map<Int_t, Int_t> fSectorMap; //! Map from sector number to index
  

 private:
  
  // Grid (makes searching sectors fast)
  Double_t                         fGridDx;      // width of the grid cell
  Double_t                         fGridDy;      // height of the grid cell
  Double_t                         fGridLx;      // grid width
  Double_t                         fGridLy;      // grid length
  Int_t                            fGridCols;    // number of columns in the grid
  Int_t                            fGridRows;    // number of rows in the grid
  std::vector<std::vector<Int_t> > fGridIndices; // 2D-vector of sector numbers

  ClassDef(CbmMuchStation,1);

};



#endif
