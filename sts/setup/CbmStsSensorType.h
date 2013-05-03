/** @file CbmStsSensorType.h
 ** @author Volker Friese <v.friese@gsi.de>
 ** @date 02.05.2013
 **/

#ifndef CBMSTSSENSORTYPE_H
#define CBMSTSSENSORTYPE_H 1


#include "TNamed.h"

class CbmStsPoint;


/** @class CbmStsSensorType
 ** @brief Base class for description of the functional behavior of
 ** sensors in the STS.
 ** @author V.Friese <v.friese@gsi.de>
 ** @version 1.0
 **
 ** Concrete classes deriving from CbmStsSensorType describe the
 ** production of charge by a CbmStsPoint (charged particle
 ** traversing the sensor). The must implement the abstract
 ** method ProduceCharge.
 **/



class CbmStsSensorType : public TNamed
{

  public:

    /** Constructor  **/
    CbmStsSensorType(const char* name = "");


    /** Destructor  **/
    virtual ~CbmStsSensorType() { };


    /** Produce charge in the sensor
     **
     ** @param x,y,z:  coordinates in the internal coordinate system [cm]
     ** @param tx,ty:  direction of particle (dx/dz, dy/dz) in internal
     **                coordinate system
     ** @param eloss:  total energy loss of particle in the sensor [GeV]
     **
     ** Perform the appropriate action for a particle trajectory in the
     ** sensor characterised by the arguments.
     **/
    virtual void ProcessPoint(CbmStsPoint* point) = 0;


  private:

    Int_t fTypeId;  ///< Unique type identifier



  ClassDef(CbmStsSensorType,1);

};




#endif
