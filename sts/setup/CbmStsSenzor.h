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

#include "CbmStsAddress.h"
#include "CbmStsElement.h"
#include "CbmStsSetup.h"

class CbmStsModule;


class CbmStsPoint;
class CbmStsSensorType;


/** @class CbmStsSenzor
 ** @brief Class representing an instance of a sensor in the CBM-STS.
 ** @author V.Friese <v.friese@gsi.de>
 ** @version 2.0
 **
 **/



class CbmStsSenzor : public CbmStsElement
{

  public:

    /** Constructor  **/
    CbmStsSenzor();


    /** Standard constructor
     ** @param name   Name
     ** @param title  Title
     ** @param node   Pointer to relevant TGeoPhysicalNode
     */
    CbmStsSenzor(const char* name, const char* title,
                 TGeoPhysicalNode* node = NULL);


    /** Destructor  **/
    virtual ~CbmStsSenzor() { };


    /** Get mother module **/
    CbmStsModule* GetModule() const;


    /** Get the sensor Id within the module  **/
    Int_t GetSensorId() const {
      return CbmStsAddress::GetElementId(fAddress, kStsSensor);
    }


    /** Pointer to sensor type **/
    CbmStsSensorType* GetType() const { return fType; }


    /** Produce charge in the sensor and send it to the module.
     **
     ** Perform the appropriate action for a particle trajectory in the
     ** sensor characterised by the CbmStsPoint object
     **/
    void ProcessPoint(CbmStsPoint* point) const;


    /** Set the sensor address
     ** @param address  Sensor address
     **/
    void SetAddress(UInt_t address) { fAddress = address; }


    /** Set a pointer to the mother module
     ** @param  module   Pointer to mother module
     **/
    //void SetModule(CbmStsModule* module) { fModule = module; }


  private:

    CbmStsSensorType* fType;    ///< Pointer to sensor type
    //CbmStsModule* fModule;      ///< Pointer to mother module


    CbmStsSenzor(const CbmStsSenzor&);
    CbmStsSenzor& operator=(const CbmStsSenzor&);

    ClassDef(CbmStsSenzor,1);

};




#endif
