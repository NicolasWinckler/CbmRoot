/** CbmMuchStationStraw.h
 *@author  E.Kryshen <e.kryshen@gsi.de>
 *@version 1.0
 *@since   18.03.09
 **
 ** This class holds the transport geometry parameters
 ** of one MuCh GEM/micromegas tracking station.
 ** Derived from base CbmMuchStation class
 **/


#ifndef CBMMUCHSTATIONSTRAW_H
#define CBMMUCHSTATIONSTRAW_H 1

#include "CbmMuchStation.h"

class CbmMuchStationStraw : public CbmMuchStation{
 public:

  /** Default constructor **/
  CbmMuchStationStraw();


  /** Standard constructor
  *@param iStation  Station index
  *@param z         z position of station centre [cm]
  **/
  CbmMuchStationStraw(Int_t iStation, Double_t z);


  /** Destructor **/
  virtual ~CbmMuchStationStraw();

 protected:
  ClassDef(CbmMuchStationStraw,1);
};
#endif
