/** CbmMuchStationGem.h
 *@author  E.Kryshen <e.kryshen@gsi.de>
 *@version 1.0
 *@since   18.03.09
 **
 ** This class holds the transport geometry parameters
 ** of one MuCh GEM/micromegas tracking station.
 ** Derived from base CbmMuchStation class
 **/


#ifndef CBMMUCHSTATIONGEM_H
#define CBMMUCHSTATIONGEM_H 1

#include "CbmMuchStation.h"

class CbmMuchStationGem : public CbmMuchStation{
 public:

  /** Default constructor **/
  CbmMuchStationGem();


  /** Standard constructor
  *@param iStation  Station index
  *@param z         z position of station centre [cm]
  **/
  CbmMuchStationGem(Int_t iStation, Double_t z);


  /** Destructor **/
  virtual ~CbmMuchStationGem();

 protected:
  ClassDef(CbmMuchStationGem,1);
};
#endif
