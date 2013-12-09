#ifndef CBMTRDASIC_H
#define CBMTRDASIC_H 1

#include "TNamed.h"
#include "TArrayD.h"
#include <vector>

class CbmTrdPoint;
class TVector3;

class CbmTrdAsic : public TNamed
{
 public:

  /**
   * \brief Default constructor.
   **/
  CbmTrdAsic();

  /**
   * \brief Constructor with parameters.
   */
  CbmTrdAsic(
	     Int_t address, Int_t orientation, Int_t nofChannels, Int_t FebGrouping, 
	     Double_t x, Double_t y, Double_t z,
	     Double_t sizex, Double_t sizey, Double_t sizez);

  /**
   * \brief Destructor.
   **/
  virtual ~CbmTrdAsic();

  /** Accessors **/
  Int_t GetAsicAddress() const { return fAsicAddress; }
  Int_t GetOrientation() const { return fOrientation; }
  Double_t GetX() const { return fX; }
  Double_t GetY() const { return fY; }
  Double_t GetZ() const { return fZ; }
  Double_t GetSizeX() const { return fSizeX; }
  Double_t GetSizeY() const { return fSizeY; }
  Double_t GetSizeZ() const { return fSizeZ; }
  Int_t GetnofChannels() const {return fnofChannels; }
  Int_t GetFebGrouping() const {return fFebGrouping; }
  std::vector<Int_t> GetChannelAddresses() const {return fChannelAddresses; }
  void SetChannelAddresses( std::vector<Int_t> addresses );

 private:

  Int_t fAsicAddress; // unique detector ID
  Int_t fOrientation; // angle between long pad axis and y-axis in steps of 90 deg [0..3]
  Double_t fX; // center of asic in global c.s. [cm]
  Double_t fY; // center of asic in global c.s. [cm]
  Double_t fZ; // center of asic in global c.s. [cm]
  Double_t fSizeX; // asic half size in x [cm]
  Double_t fSizeY; // asic half size in y [cm]
  Double_t fSizeZ; // asic half size in z [cm]
  Int_t fnofChannels;
  Int_t fFebGrouping;
  std::vector<Int_t> fChannelAddresses;

  ClassDef(CbmTrdAsic, 3);
};

#endif
