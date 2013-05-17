/** @file CbmStsModule.h
 ** @author Volker Friese <v.friese@gsi.de>
 ** @date 14.05.2013
 **/

#ifndef CBMSTSMODULE_H
#define CBMSTSMODULE_H 1


#include <map>
#include <vector>
#include "TNamed.h"
#include "CbmStsSenzor.h"


class TGeoPhysicalNode;


using namespace std;


/** @class CbmStsModule
 ** @brief Class representing an instance of a readout unit in the CBM-STS.
 ** @author V.Friese <v.friese@gsi.de>
 ** @version 1.0
 **
 ** The StsModule is the read-out unit in the CBM STS. It consists of one
 ** sensor or two or more daisy-chained sensors (CbmStsSensor), the analogue
 ** cable and the read-out electronics.
 **
 ** The module receives and stores the analogue signals from the sensor(s)
 ** in a buffer. It takes care of interference of signals in one and the
 ** same channel (two signals arriving within a given dead time).
 ** The module digitises the analogue signals and sends them to the
 ** CbmDaq when appropriate.
 **/


class CbmStsModule : public TNamed
{
  public:

    /** Default constructor **/
    CbmStsModule();


    /** Destructor **/
    virtual ~CbmStsModule();


    /** Add a sensor to the module
     ** @param sensor   Pointer to sensor object
     **/
    void AddSensor(CbmStsSenzor* sensor);


    /** Add an analogue signal to the buffer
     *
     * @param sensorChannel  channel number within sensor
     * @param time           time of signal [ns]
     * @param charge         analogue charge [e]
     *
     * The signal will be added to the buffer. Interference with
     * previous signals within the same channels is checked and the
     * proper action is executed.
     */
    void AddSignal(CbmStsSenzor* sensor, Int_t side, Int_t strip,
                   Double_t time, Double_t charge);


  private:

    Int_t fAddress;                       ///< Unique element identifier

    vector<CbmStsSenzor*> fSensors;       ///< Array of sensors

    TGeoPhysicalNode* fNode;               ///< Pointer to geometry

    /** Buffer for the analogue signals **/
    map<Int_t, pair<Double_t, Double_t> > fBuffer;


    CbmStsModule(const CbmStsModule&);
    CbmStsModule& operator=(const CbmStsModule&);
    
    ClassDef(CbmStsModule,1);

};

#endif /* CBMSTSMODULE_H */
