/** @file CbmStsModule.h
 ** @author Volker Friese <v.friese@gsi.de>
 ** @date 14.05.2013
 **/

#ifndef CBMSTSMODULE_H
#define CBMSTSMODULE_H 1


#include <map>
#include <vector>
#include "CbmStsElement.h"
#include "CbmStsSenzor.h"



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
class CbmStsModule : public CbmStsElement
{
  public:

    /** Default constructor **/
    CbmStsModule();


    /** Standard constructor
     ** @param name   Name
     ** @param title  Title
     ** @param node   Pointer to relevant TGeoPhysicalNode
     */
    CbmStsModule(const char* name, const char* title,
                 TGeoPhysicalNode* node = NULL);


    /** Destructor **/
    virtual ~CbmStsModule();


    /** Add an analogue signal to the buffer
     *
     * @param channel        channel number
     * @param time           time of signal [ns]
     * @param charge         analogue charge [e]
     *
     * The signal will be added to the buffer. Interference with
     * previous signals within the same channels is checked and the
     * proper action is executed.
     */
    void AddSignal(Int_t channel, Double_t time, Double_t charge);


    /** Clean the buffer
     ** @param time  Read-out time [ns]
     **
     ** All analogue signals in the buffer with time prior to the specified
     ** read-out time are digitised and sent to the digitiser / DAQ.
     **/
     void CleanBuffer(Double_t time);


     /** Set the digitisation parameters
      ** @param dynRagne   Dynamic range [e]
      ** @param threshold  Threshold [e]
      ** @param nAdc       Number of ADC channels
      **/
     void SetParameters(Double_t dynRange, Double_t threshold, Int_t nAdc) {
    	 fDynRange       = dynRange;
    	 fThreshold      = threshold;
    	 fNofAdcChannels = nAdc;
    	 fIsSet = kTRUE;
     }


  private:

    Double_t fDynRange;        // dynamic range [e]
    Double_t fThreshold;       // threshold [e]
    Int_t    fNofAdcChannels;  // Number of ADC channels
    Bool_t   fIsSet;           // Flag whether parameters are set


    /** Buffer for the analogue signals
     ** This is a std::map from channel number to a pair
     ** of time and analogue charge
     **/
    map<Int_t, pair<Double_t, Double_t> > fBuffer;


    /** Digitise an analog charge signal
     ** @param channel  Module readout channel number
     ** @param charge   Analog charge [e]
     ** @param time     Absolute signal time [ns]
     ** @return  Pointer to new digi
     **/
    void Digitize(Int_t channel, Double_t charge, Double_t time);


    /** Prevent usage of copy constructor and assignment operator **/
    CbmStsModule(const CbmStsModule&);
    CbmStsModule& operator=(const CbmStsModule&);
    

    ClassDef(CbmStsModule,1);

};

#endif /* CBMSTSMODULE_H */
