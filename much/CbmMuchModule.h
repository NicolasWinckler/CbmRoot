/** CbmMuchModule.h
 * 
 *@author  M.Ryzhinskiy <m.ryzhinskiy@gsi.de>
 *@version 1.0
 *@since   11.02.08
 **
 ** This class holds geometry parameters of much modules 
 **
 **/

#ifndef CBMMUCHMODULE_H
#define CBMMUCHMODULE_H 1

#include "CbmMuchGeoScheme.h"

#include "TObject.h"
#include "TObjArray.h"
#include "TVector3.h"
#include "TPolyLine.h"
#include "TClonesArray.h"
#include "TPave.h"

#include <vector>

using std::vector;

class CbmMuchSector;

class CbmMuchModule :
  public TPave
//  public TPolyLine
{

 public:

  /** Default constructor **/
  CbmMuchModule();

  /** Standard constructor
   *@param detId     Unique detector identifier
   *@param position  Location of the module center in global c.s. (all dimensions in [cm])
   *@param size      Size of the module (all dimensions in [cm])
   *@param cutRadius Radius of the cut (if any, otherwise = -1.) [cm]
   **/
  CbmMuchModule(Long64_t detId, TVector3 position, TVector3 size, Double_t cutRadius);

  /** Standard constructor
   *@param iStation  Station ID
   *@param iLayer    Layer ID
   *@param iSide     Defines side of the layer (0 - Front, 1 - Back)
   *@param iModule   Module ID
   *@param position  Location of the module center in global c.s. (all dimensions in [cm])
   *@param size      Size of the module (all dimensions in [cm])
   *@param cutRadius Radius of the cut (if any, otherwise = -1.) [cm]
   **/
  CbmMuchModule(Int_t iStation, Int_t iLayer, Bool_t iSide, Int_t iModule, TVector3 position, TVector3 size, Double_t cutRadius);

  /** Destructor **/
  virtual ~CbmMuchModule();

  /** Accessors **/
  Long64_t GetDetectorId()  const { return fDetectorId; }
  Int_t    GetNSectors()    const { return fSectors.GetEntriesFast(); }
  Double_t GetCutRadius()   const { return fCutRadius; }
  Double_t GetGridDx()      const { return fGridDx; }
  Double_t GetGridDy()      const { return fGridDy; }
  Int_t    GetGridCols()    const { return fGridCols; }
  Int_t    GetGridRows()    const { return fGridRows; }
  TVector3 GetSize()        const { return fSize; }
  TVector3 GetPosition()    const { return fPosition; }

  /** Gets sector by the given index. **/
  CbmMuchSector* GetSector(Int_t iSector) const { return (CbmMuchSector*)fSectors.At(iSector); }  
  /** Gets sector by the given numbers of column and row in the grid. **/
  CbmMuchSector* GetSector(Int_t iGridColumn, Int_t iGridRow);

  /** Adds a given sector to the array. 
  *@param  sector   CbmMuchSector which should be added to the array.**/
  void AddSector(CbmMuchSector* sector);

  /** Grid intitialization. **/
  Bool_t InitGrid();
  /** Adds to each sector info about neighbour sectors **/
  void InitNeighbourSectors();
  /** Adds to each pad info about neighbour pads **/
  void InitNeighbourPads();
 
  TClonesArray* GetPoints()   const { return fPoints;     }
  TClonesArray* GetHits()     const { return fHits;       }
  TClonesArray* GetClusters() const { return fClusters;   }
  void SetPoints(TClonesArray* points)     { fPoints   = points;   }
  void SetHits  (TClonesArray* hits)       { fHits     = hits;     }
  void SetClusters(TClonesArray* clusters) { fClusters = clusters; }  
  void DrawModule();

 protected:
  Long64_t               fDetectorId;            // Unique detector ID
  Double_t               fCutRadius;             // Radius of the cut (if any, otherwise = -1.) [cm]
  TVector3               fSize;                  // Size vector of the module (all dimensions in [cm])
  TVector3               fPosition;              // Location vector of the module center in global c.s. (all dimensions in [cm])
  TObjArray              fSectors;               // Array of sectors within this module
  Double_t               fGridDx, fGridDy;       // Size of the grid cell (for fast search purpose)
  Int_t                  fGridCols;              // Number of columns in the grid
  Int_t                  fGridRows;              // Number of rows in the grid
  vector<vector<Int_t> > fGridIndices;           // 2D-vector of sector numbers
  TClonesArray*          fPoints;                //!
  TClonesArray*          fHits;                  //!
  TClonesArray*          fClusters;              //!

/*   TH2I                   fGridIndices; */
/*   vector<vector<Int_t> > GetGridIndices(); */

  ClassDef(CbmMuchModule,1);
};
#endif


