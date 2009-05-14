/** CbmMuchModuleGem.h
 *
 *@author  M.Ryzhinskiy <m.ryzhinskiy@gsi.de>
 *@version 1.0
 *@since   11.02.08
 **
 ** This class holds geometry parameters of much modules
 **
 **/

#ifndef CBMMUCHMODULEGEM_H
#define CBMMUCHMODULEGEM_H 1

#include "CbmMuchGeoScheme.h"
#include "CbmMuchModule.h"

#include "TObject.h"
#include "TObjArray.h"
#include "TVector3.h"
#include "TPolyLine.h"
#include "TClonesArray.h"
#include "TPave.h"

#include <vector>

using std::vector;

class CbmMuchSector;

class CbmMuchModuleGem : public CbmMuchModule
{

public:

    /** Default constructor **/
    CbmMuchModuleGem();

    /** Standard constructor
     *@param detId     Detector ID
     *@param position  Location of the module center in global c.s. (all dimensions in [cm])
     *@param size      Size of the module (all dimensions in [cm])
     *@param cutRadius Radius of the cut (if any, otherwise = -1.) [cm]
     **/
    CbmMuchModuleGem(Int_t detId, TVector3 position, TVector3 size, Double_t cutRadius);

    /** Standard constructor
     *@param iStation  Station index
     *@param iLayer    Layer index
     *@param iSide     Defines side of the layer (0 - Front, 1 - Back)
     *@param iModule   Module index
     *@param position  Location of the module center in global c.s. (all dimensions in [cm])
     *@param size      Size of the module (all dimensions in [cm])
     *@param cutRadius Radius of the cut (if any, otherwise = -1.) [cm]
     **/
    CbmMuchModuleGem(Int_t iStation, Int_t iLayer, Bool_t iSide, Int_t iModule, TVector3 position, TVector3 size, Double_t cutRadius);

    /** Destructor **/
    virtual ~CbmMuchModuleGem();

    /** Accessors **/
    Int_t    GetNSectors()    const { return fSectors.GetEntriesFast(); }

    /** Gets sector by the given index. **/
    CbmMuchSector* GetSector(Int_t iSector) const { return (CbmMuchSector*)fSectors.At(iSector); }
    /** Gets sector by the given numbers of column and row in the grid. **/
    CbmMuchSector* GetSector(Int_t iGridColumn, Int_t iGridRow);
    /** Gets sector by the given coordinates in global c.s. */
    CbmMuchSector* GetSector(Double_t x, Double_t y);

    /** Adds a given sector to the array.
     *@param  sector   CbmMuchSector which should be added to the array.**/
    void AddSector(CbmMuchSector* sector);


    TClonesArray* GetPoints()   const { return fPoints;     }
    TClonesArray* GetHits()     const { return fHits;       }
    TClonesArray* GetClusters() const { return fClusters;   }
    void SetPoints(TClonesArray* points)     { fPoints   = points;   }
    void SetHits  (TClonesArray* hits)       { fHits     = hits;     }
    void SetClusters(TClonesArray* clusters) { fClusters = clusters; }

    /** Grid initialization. **/
    Bool_t InitGrid(Bool_t useModuleDesign);
    /** Adds to each sector info about neighbour sectors **/
    void InitNeighbourSectors();
    /** Adds to each pad info about neighbour pads **/
    void InitNeighbourPads();

    virtual Bool_t InitModule();
    virtual void DrawModule(Color_t color);

private:
    Bool_t                 fUseModuleDesign;       // Whether to use module or monolithic design
    TObjArray              fSectors;               // Array of sectors within this module
    Double_t               fGridDx, fGridDy;       // Size of the grid cell (for fast search purpose)
    Int_t                  fGridCols;              // Number of columns in the grid
    Int_t                  fGridRows;              // Number of rows in the grid
    vector<vector<Int_t> > fGridIndices;           // 2D-vector of sector numbers
    TClonesArray*          fPoints;                //!
    TClonesArray*          fHits;                  //!
    TClonesArray*          fClusters;              //!


    Double_t GetGridCellY(Int_t iRow);
    Double_t GetGridCellX(Int_t iCol);
    Double_t GetInitX(CbmMuchSector* sector);
    Double_t GetInitY(CbmMuchSector* sector);

    ClassDef(CbmMuchModuleGem,1);
};
#endif


