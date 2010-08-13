// -------------------------------------------------------------------------
// -----                 CbmTrdGeoHandler header file                  -----
// -----                 Created 13/08/10  by F. Uhlig                 -----
// -------------------------------------------------------------------------


/** CbmTrdGeoHandler.h
 ** Helper class to extract information from the GeoManager which is
 ** needed in many other TRD classes. This helper class should be a
 ** single place to hold all these functions.
 ** @author F. Uhlig <f.uhlig@gsi.de>
 **/

#ifndef CBMTRDGEOHANDLER_H
#define CBMTRDGEOHANDLER_H 1

#include "TObject.h"

class CbmTrdGeoHandler : public TObject {
 public:

  /** Constructor **/
  CbmTrdGeoHandler();


  /** Destructor **/
  ~CbmTrdGeoHandler() {};

  /** Get Layer Information from gGeoManager **/
  Bool_t GetLayerInfo(std::vector<Int_t> &layersBeforStation);

 private:

  Bool_t GetLayerInfoFromOldGeometry(std::vector<Int_t> &layersBeforStation);
  Bool_t GetLayerInfoFromNewGeometry(std::vector<Int_t> &layersBeforStation);


  ClassDef(CbmTrdGeoHandler,1) 

};


#endif //CBMTRDGEOHANDLER_H

