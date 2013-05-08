/** @file CbmStsSensor.h
 ** @author Volker Friese <v.friese@gsi.de>
 ** @date 03.05.2013
 **
 ** This class is to replace the CbmStsSensor, at which point it will be
 ** renamed to spell correctly.
 **/

#ifndef CBMSTSSENZOR_H
#define CBMSTSSENZOR_H 1


#include "TGeoPhysicalNode.h"
#include "TNamed.h"


class CbmStsPoint;
class CbmStsSensorType;


/** @class CbmStsSenzor
 ** @brief Class representing an instance of a sensor in the CBM-STS.
 ** @author V.Friese <v.friese@gsi.de>
 ** @version 2.0
 **
 **/



class CbmStsSenzor : public TNamed
{

  public:

    /** Constructor  **/
    CbmStsSenzor();


    /** Destructor  **/
    virtual ~CbmStsSenzor() { };


    /** Produce charge in the sensor and send it to the module.
     **
     ** Perform the appropriate action for a particle trajectory in the
     ** sensor characterised by the CbmStsPoint object
     **/
    void ProcessPoint(CbmStsPoint* point) const;


  private:

    Int_t fAddress;             ///< Unique identifier
    TGeoPhysicalNode*  fNode;   ///< Pointer to node in the geometry
    CbmStsSensorType* fType;    ///< Pointer to sensor type




  ClassDef(CbmStsSenzor,1);

};




#endif
