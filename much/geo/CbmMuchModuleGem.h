/** CbmMuchModuleGem.h
 *
 *@author  M.Ryzhinskiy <m.ryzhinskiy@gsi.de>
 *@version 1.0
 *@since   11.02.08
 *
 * This class holds geometry parameters of much modules
 *
 * The channel ID consists of:
 *   sector number  (0-2047),       bits 0-10
 *   channel number (0-1024),       bits 11-20
 *
 */

#ifndef CBMMUCHMODULEGEM_H
#define CBMMUCHMODULEGEM_H 1

#include "CbmMuchGeoScheme.h"
#include "CbmMuchModule.h"

#include "TObjArray.h"
#include "TVector3.h"

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

class CbmMuchSector;
class CbmMuchPad;

class CbmMuchModuleGem : public CbmMuchModule{
 public:
  /** Default constructor **/
  CbmMuchModuleGem();

  /** Standard constructor
   *@param iStation  Station index
   *@param iLayer    Layer index
   *@param iSide     Defines side of the layer (0 - Front, 1 - Back)
   *@param iModule   Module index
   *@param position  Location of the module center in global c.s. (all dimensions in [cm])
   *@param cutRadius Radius of the cut (if any, otherwise = -1.) [cm]
   **/
  CbmMuchModuleGem(Int_t iStation, Int_t iLayer, Bool_t iSide, Int_t iModule, TVector3 position, TVector3 size, Double_t cutRadius);

  /** Destructor **/
  virtual ~CbmMuchModuleGem(){}

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
  CbmMuchSector* GetSector(Long64_t channelId);
  
  CbmMuchSector* GetSectorByIndex(Int_t iSector) { return fSectors[iSector]; }
  
  
  /** Gets pad by the given Id. */
  CbmMuchPad* GetPad(Long64_t channelId);
  
  /** */
  Int_t GetNSectors() const { return fSectors.size(); }
  
  /** Gets array of pads for this module. */
  vector<CbmMuchPad*> GetPads();
  
  /** */
  Int_t GetNPads();
  
  /** Adds a given sector to the array.
   *@param  sector CbmMuchSector which should be added to the array.**/
  void AddSector(CbmMuchSector* sector) { fSectors.push_back(sector); }
  
  /** */
  virtual Bool_t InitModule(){}
  
  /** */
  void DrawModule(Color_t color);
  
  void DrawPads();
  void SetPadFired(Long64_t channelId,Int_t digiIndex,Int_t adcCharge);
  
 protected:
  vector<CbmMuchSector*> fSectors; // Array of sectors within this module

  ClassDef(CbmMuchModuleGem,2);
};
#endif


