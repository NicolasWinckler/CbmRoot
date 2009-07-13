/** CbmMuchSegmentManual.h
 *@author Mikhail Ryzhinskiy <m.ryzhinskiy@gsi.de>
 *@since 02.04.09
 *@version 1.0
 *
 * class for making parameter file for MUCH digitizer
 *
 */

#ifndef CBMMUCHSEGMENTMANUAL_H
#define CBMMUCHSEGMENTMANUAL_H 1

#include "FairTask.h"

#include <vector>
#include <map>

using std::map;
using std::vector;

class CbmMuchModuleGem;
class CbmMuchSector;
class CbmMuchLayerSide;
class CbmGeoMuchPar;
class TObjArray;

class CbmMuchSegmentManual : public FairTask {

  public:
    /** Default constructor */
    CbmMuchSegmentManual();

    /** Destructor */
    virtual ~CbmMuchSegmentManual();

    /** Standard constructor
     *@param digiFileName  Name of the output file with segmentation parameters
     */
    CbmMuchSegmentManual(char* digiFileName);

    virtual InitStatus Init();

    /** Performs the segmentation of all stations */
    void SegmentMuch();

    /** Sets number of stations
     *@param nStations  Number of stations
     */
    void SetNStations(Int_t nStations) {fNStations = nStations;}

    /** Sets number of circled regions for the given station
     * @param iStation  Station index
     * @param nRegions  Number of regions in the station
     */
    void SetNRegions(Int_t iStation, Int_t nRegions);
    void SetNRegions(Int_t nRegions[]);

    /** Sets the radius for the given circled region in the given station
     * @param iStation Station index
     * @param iRegion  Region index
     * @param radius   Radius of the region
     */
    void SetRegionRadius(Int_t iStation, Int_t iRegion, Double_t radius);
    void SetRegionRadii(Int_t iStation, Double_t radii[]);

    /** Sets number of channels per sector for the given station
     * @param iStation   Station index
     * @param nChannels  Number of channels
     */
    void SetNChannels(Int_t iStation, Int_t nChannels);
    void SetNChannels(Int_t nChannels[]);

    /** Sets resolution for the central region of the given station.
     * @param iStation Station index
     * @param sigmaX   Resolution in X direction
     * @param sigmaY   Resolution in Y direction
     */
    void SetMinSigma(Int_t iStation, Double_t sigmaX, Double_t sigmaY);
    void SetMinSigma(Double_t sigmaX[], Double_t sigmaY[]);
    void SetSigma(Int_t iStation, Int_t iRegion, Double_t sigmaX, Double_t sigmaY);
    void SetSigmas(Int_t iStation, Double_t sigmaX[], Double_t sigmaY[]);

    /** Sets pad size for the central region of the given station.
     * @param iStation Station index
     * @param padLx    Pad width
     * @param padLy    Pad length
     */
    void SetMinPadSize(Int_t iStation, Double_t padLx, Double_t padLy);
    void SetMinPadSize(Double_t padLx[], Double_t padLy[]);
    void SetPadSize(Int_t iStation, Int_t iRegion, Double_t padLx, Double_t padLy);
    void SetPadSizes(Int_t iStation, Double_t padLx[], Double_t padLy[]);

  private:
    CbmGeoMuchPar*                fGeoPar;         // Geometry parameters container
    Int_t                         fNStations;      // Number of stations
    TObjArray*                    fStations;       // Array of stations
    Char_t*                       fDigiFileName;   // Name of the file with segmentation parameters
    map<Int_t, Int_t>             fNRegions;       // Map from a station index to a number of circled regions in the station
    map<Int_t, vector<Double_t> > fRadii;          // Map from a station index to a vector of circled regions radii
    map<Int_t, vector<Double_t> > fSecLx;          // Map from a station index to a vector of sector widths for each region
    map<Int_t, vector<Double_t> > fSecLy;          // Map from a station index to a vector of sector lengths for each region
    map<Int_t, Double_t>          fSecMinLx;       // Map from a station index to a minimum width of a sector
    map<Int_t, Double_t>          fSecMinLy;       // Map from a station index to a minimum length of a sector
    map<Int_t, Int_t>             fNChannels;      // Map from a station index to a number of channels per sector
    map<Int_t, Int_t>             fNCols;          // Map from a station index to a number of columns in a sector
    map<Int_t, Int_t>             fNRows;          // Map from a station index to a number of rows in a sector

    /** Get parameter containers **/
    virtual void SetParContainers();

    /**
     * Checks whether the given module is intersected by the circle of the given radius
     * @param module Module to verify
     * @param radius Radius of the circle
     * @return       0 if the module is outside the circle, 1 if it is intersected by
     *               the circle, 2 if it is contained in the circle
     */
    Int_t IntersectsRad(CbmMuchModuleGem* module, Double_t radius);

    /**
     * Checks whether the given sector is intersected by the circle of the given radius
     * @param sector Sector to verify
     * @param radius Radius of the circle
     * @return       0 if the sector is outside the circle, 1 if it is intersected by
     *               the circle, 2 if it is contained in the circle
     */
    Int_t IntersectsRad(CbmMuchSector* sector, Double_t radius);

    /**
     * Performs segmentation of the given layer side
     * @param layerSide layer side to segment
     */
    void SegmentLayerSide(CbmMuchLayerSide* layerSide);

    /**
     * Performs segmentation of the given module
     * @param module          Module to segment
     * @param useModuleDesign Whether module design is used
     */
    void SegmentModule(CbmMuchModuleGem* module, Bool_t useModuleDesign);

    /**
     * Performs segmentation of the given sector in the module
     * @param module Module which contains the given sector
     * @param sector Sector to segment
     */
    void SegmentSector(CbmMuchModuleGem* module, CbmMuchSector* sector);

    /**
     * Gets maximum sector size for the given module
     * @param module Module to process
     * @param side   Defines which side of a sector should be returned
     * @return       maximum sector length, if side="Length", maximum
     *               sector width, if side="Width"
     */
    Double_t GetSectorMaxSize(CbmMuchModuleGem* module, const TString side);

    /**
     * Gets maximum pad size for the given module
     * @param module Module to process
     * @param side   Defines which side of a pad should be returned
     * @return       maximum pad length, if side="Length", maximum
     *               pad width, if side="Width"
     */
    Double_t GetPadMaxSize(CbmMuchModuleGem* module, const TString side);

    /**
     * Checks whether the given sector should be segmented in the given direction
     * @param sector    Sector to verify
     * @param direction Direction to segment in ("X" or "Y")
     * @return          true if segmentation is required, false otherwise
     */
    Bool_t ShouldSegment(CbmMuchSector* sector, const TString direction);

    /** Draws segmented stations. */
    void DrawSegmentation();

    /** Prints summary */
    void Print();

    ClassDef(CbmMuchSegmentManual,1)
};

#endif
