/** @file CbmStsSensor.h
 ** @author Volker Friese <v.friese@gsi.de>
 ** @date 03.05.2013
 **
 ** This class is to replace the CbmStsSensor, at which point it will be
 ** renamed to spell correctly.
 **/

#ifndef CBMSTSSENZOR_H
#define CBMSTSSENZOR_H 1


#include "CbmStsAddress.h"
#include "setup/CbmStsElement.h"
#include "setup/CbmStsSensorType.h"

class TGeoPhysicalNode;
class CbmStsModule;
class CbmStsPoint;
class CbmStsSensorType;


/** @class CbmStsSenzor
 ** @brief Class representing an instance of a sensor in the CBM-STS.
 ** @author V.Friese <v.friese@gsi.de>
 ** @version 2.0
 **
 ** The sensor is the smallest geometric element in the STS setup.
 ** It is the daughter node of a module, which may contain one sensor
 ** or several daisy-chained ones. The sensor class represents
 ** the physical node through its member fNode, but also performs detector
 ** response simulation through its member fType.
 ** After being instantiated by the initialisation of CbmStsSetup,
 ** the sensor type must be assigned to the sensor. This is currently done
 ** by the digitiser task.
 ** The sensor class performs the coordinate transformation from the
 ** global system to the sensor system, having the sensor midpoint as origin.
 ** The analog response is then modelled by the CbmStsSensorType object.
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


    /** Process one MC Point
     ** @param point  Pointer to CbmStsPoint object
     ** @return  Status variable, depends on sensor type
     **
     ** Perform the appropriate action for a particle trajectory in the
     ** sensor characterised by the CbmStsPoint object
     **/
    Int_t ProcessPoint(CbmStsPoint* point) const;


    /** Set the sensor address
     ** @param address  Sensor address
     **/
    void SetAddress(UInt_t address) { fAddress = address; }


    /** Set the sensor type
     ** @param Pointer to sensor type object
     **/
    void SetType(CbmStsSensorType* type) { fType = type; }



  private:

    CbmStsSensorType* fType;    ///< Pointer to sensor type

    /** Prevent usage of copy constructor and assignment operator **/
    CbmStsSenzor(const CbmStsSenzor&);
    CbmStsSenzor& operator=(const CbmStsSenzor&);


    ClassDef(CbmStsSenzor,1);

};


#endif
