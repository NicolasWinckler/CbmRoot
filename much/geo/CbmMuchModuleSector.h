/** CbmMuchModuleSector.h
 *
 *@author  E.Kryshen <e.kryshen@gsi.de>
 *@version 1.0
 *@since   07.12.11
 *
 * This class holds geometry parameters of much sector-type modules 
 *
 * The channel ID consists of:
 *   sector number  (0-2047),       bits 0-10
 *   channel number (0-1024),       bits 11-20
 *
 */

#ifndef CBMMUCHMODULESECTOR_H
#define CBMMUCHMODULESECTOR_H 1

#include "CbmMuchGeoScheme.h"
#include "CbmMuchModule.h"

#include "TObject.h"
#include "TObjArray.h"
#include "TVector3.h"
#include "TPolyLine.h"
#include "TClonesArray.h"
#include "TPave.h"

class CbmMuchRadialSector;
class CbmMuchRadialPad;

#include <vector>
#include <map>

using std::vector;
using std::multimap;
using std::pair;

// Length of the index of the corresponding volume
#define WL_SECTOR 2147483647LL //2047
#define WL_CHANNEL 1023LL

// Number of a start bit for each volume
#define SB_SECTOR 0LL
#define SB_CHANNEL 32LL //11


class CbmMuchRadialSector;

class CbmMuchModuleSector : public CbmMuchModule
{

  public:

    /** Default constructor **/
    CbmMuchModuleSector();

    CbmMuchModuleSector(Int_t iStation, Int_t iLayer, Bool_t iSide, Int_t iModule, TVector3 position,TVector3 size,
        Double_t dx1, Double_t dx2, Double_t dy, Double_t dz, Double_t cutRadius);
    
    /** Standard constructor
     *@param detId     Detector ID
     *@param position  Location of the module center in global c.s. (all dimensions in [cm])
     *@param size      Size of the module (all dimensions in [cm])
     *@param cutRadius Radius of the cut (if any, otherwise = -1.) [cm]
     **/
    CbmMuchModuleSector(Int_t detId, TVector3 position, TVector3 size, Double_t cutRadius);

    /** Standard constructor
     *@param iStation  Station index
     *@param iLayer    Layer index
     *@param iSide     Defines side of the layer (0 - Front, 1 - Back)
     *@param iModule   Module index
     *@param position  Location of the module center in global c.s. (all dimensions in [cm])
     *@param size      Size of the module (all dimensions in [cm])
     *@param cutRadius Radius of the cut (if any, otherwise = -1.) [cm]
     **/
    CbmMuchModuleSector(Int_t iStation, Int_t iLayer, Bool_t iSide, Int_t iModule, TVector3 position, TVector3 size, Double_t cutRadius);

    /** Destructor **/
    virtual ~CbmMuchModuleSector();

    /**
     * Gets sector index for the given channel Id.
     * @param channelId   Channel Id.
     * @return            Sector index within the module.
     */
    static Int_t GetSectorIndex(Long64_t channelId) {
      return (channelId & (WL_SECTOR << SB_SECTOR)) >> SB_SECTOR;
    }
    /**
     * Gets channel index for the given channel Id.
     * @param channelId   Channel Id.
     * @return            Channel index within the sector.
     */
    static Int_t GetChannelIndex(Long64_t channelId) {
      return (channelId & (WL_CHANNEL << SB_CHANNEL)) >> SB_CHANNEL;
    }
    /**
     * Gets channel Id by sector index and channel index.
     * @param iSector    Sector index within the module.
     * @param iChannel   Channel index within the sector.
     * @return           Detector Id.
     */
    static Long64_t GetChannelId(Int_t iSector, Int_t iChannel) {
      return ((Long64_t)iSector << SB_SECTOR) | ((Long64_t)iChannel << SB_CHANNEL);
    }

    /** Gets sector by the given channel Id. **/
    CbmMuchRadialSector* GetSector(Long64_t channelId);
    /** Gets sector by the given coordinates in global c.s. */
    CbmMuchRadialSector* GetSectorByRadius(Double_t r);
    /** Gets pad by the given Id. */
    CbmMuchRadialPad* GetPad(Long64_t channelId);
    /** Gets array of pads for this module. */
    vector<CbmMuchPad*> GetPads();
    Int_t GetNPads();

    CbmMuchRadialSector* GetSectorByIndex(Int_t iSector) {return (CbmMuchRadialSector*) fSectors.At(iSector); }

    
    /** Adds a given sector to the array.
     *@param  sector   CbmMuchRadialSector which should be added to the array.**/
    void AddSector(CbmMuchRadialSector* sector);

//    virtual Bool_t InitModule();
    virtual void DrawModule(Color_t color);

    Int_t GetNSectorChannels() const { return fNSectorChannels; }
    void SetNSectorChannels(Int_t nSectorChannels) { fNSectorChannels = nSectorChannels; }

    void ClearDigis() { fDigis.clear(); }
    void AddDigi(Double_t time,Int_t id) { fDigis.insert(pair<Double_t,Int_t>(time,id)); }
    multimap<Double_t,Int_t> GetDigis() { return fDigis; } 

    Double_t GetDx1() { return fDx1; }
    Double_t GetDx2() { return fDx2; }
    Double_t GetDy()  { return fDy;  }
    Double_t GetDz()  { return fDz;  }

    virtual Bool_t InitModule();
  private:
    Bool_t                 fUseModuleDesign;       // Whether to use module or monolithic design
    Int_t                  fNSectorChannels;       // Number of channels per "complete" sector
    vector<Double_t>       fSectorRadii;           // vector of radii
    multimap<Double_t,Int_t>  fDigis;              //!
    Double_t fDx1;
    Double_t fDx2;
    Double_t fDy;
    Double_t fDz;
    
    ClassDef(CbmMuchModuleSector,2);
};
#endif


