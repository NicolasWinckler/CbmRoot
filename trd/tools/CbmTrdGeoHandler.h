/**
 * \file CbmTrdGeoHandler.h
 * \brief Helper class to extract information from the GeoManager.
 * \author Florian Uhlig <f.uhlig@gsi.de>
 * \date 13/08/2010
 *
 * Updated 20/05/2013 by Andrey Lebedev <andrey.lebedev@gsi.de>
 *
 * Helper class to extract information from the GeoManager which is
 * needed in many other TRD classes. This helper class should be a
 * single place to hold all these functions.
 */

#ifndef CBMTRDGEOHANDLER_H_
#define CBMTRDGEOHANDLER_H_ 1

#include "TObject.h"

class TGeoBBox;
class TGeoVolume;
class TGeoHMatrix;

class CbmTrdGeoHandler : public TObject
{
public:

   /**
   * \brief Constructor.
   **/
   CbmTrdGeoHandler();

   /**
   * \brief Destructor.
   **/
   ~CbmTrdGeoHandler();

   void Init(Bool_t isSimulation = kFALSE);

   /**
   * \brief Return module address calculated based on the current node in the TGeoManager.
   */
   Int_t GetModuleAddress();

   /**
   * \brief Navigate to node and return module address.
   */
   Int_t GetModuleAddress(const TString& path);
          
   /**
   * \brief Return pad orientation of the current node in the TGeoManager.
   */
   Int_t GetPadOrientation();

   /**
   * \brief Navigate to node and return pad orientation.
   */
   Int_t GetPadOrientation(const TString& path);

   // Path has to contain information down to module part, 
   // otherwise retrieved information can be wrong.
   // Path example:
   // /cave_1/trd_v13x_1/layer01_101/module1_10100101/gas_1

   Double_t GetSizeX(const TString& path);
   Double_t GetSizeY(const TString& path);
   Double_t GetSizeZ(const TString& path);
   Double_t GetX(const TString& path);
   Double_t GetY(const TString& path);
   Double_t GetZ(const TString& path);
   Int_t GetModuleType(const TString& path);

private:

   void NavigateTo(const TString& path);

   Bool_t fIsSimulation; //!

   UInt_t fGeoPathHash;        //!
   TGeoVolume* fCurrentVolume; //!
   TGeoBBox* fVolumeShape;     //!
   Double_t fGlobal[3];        //! Global center of volume
   TGeoHMatrix* fGlobalMatrix; //!
   Int_t fModuleType; //!

   CbmTrdGeoHandler(const CbmTrdGeoHandler&);
   CbmTrdGeoHandler operator=(const CbmTrdGeoHandler&);

   ClassDef(CbmTrdGeoHandler, 4)
};

#endif //CBMTRDGEOHANDLER_H

