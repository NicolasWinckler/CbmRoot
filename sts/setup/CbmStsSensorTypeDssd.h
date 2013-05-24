/** @file CbmStsSensorTypeDssd.h
 ** @author Volker Friese <v.friese@gsi.de>
 ** @date 02.05.2013
 **/

#ifndef CBMSTSSENSORTYPEDSSD_H
#define CBMSTSSENSORTYPEDSSD_H 1


#include "CbmStsSensorType.h"


/** @class CbmStsSensorTypeDssd
 ** @brief Class describing double-sided silicon strip sensors.
 ** @author V.Friese <v.friese@gsi.de>
 ** @version 1.0
 **
 ** This class describes the response of double-sided silicon
 ** strip sensors in the STS. Its free parameters are the dimensions
 ** of the sensor, the strip pitches and stereo angles on front
 ** and back side, and the width of the inactive area at the
 ** sensor borders.
 **
 ** Charge is created uniformly along the particle trajectory
 ** in the sensor and is projected to the read-out edge (top edge).
 ** Strips not reaching the top edge are cross-connected
 ** horizontally (double metal layer or cable) to a
 ** corresponding strip.
 **/



class CbmStsSensorTypeDssd : public CbmStsSensorType
{

  public:

    /** Constructor  **/
    CbmStsSensorTypeDssd();


    /** Destructor  **/
    virtual ~CbmStsSensorTypeDssd() { };


   /** Print parameters **/
    virtual void Print(Option_t* opt = "") const;


    /** Process a point **/
    virtual void ProcessPoint(CbmStsSensorPoint* point,
                              const CbmStsSenzor* sensor) const;



    /** Set the parameters
     ** @param dx,dy,dz          Size in x,y,z [cm]
     ** @param pitchF,pitchB     Strip pitch foint and back side [cm]
     ** @param stereoF,stereoB   Strip stereo angle fron and back side [degrees]
     **/
    void SetParameters(Double_t dx, Double_t dy, Double_t dz,
                       Double_t pitchF, Double_t pitchB,
                       Double_t stereoF, Double_t stereoB);


  private:

    Double_t fDx;        ///< Dimension in x [cm]
    Double_t fDy;        ///< Dimension in y [cm]
    Double_t fDz;        ///< Thickness in z [cm]
    Double_t fPitch[2];  ///< Strip pitch front/back side [cm]
    Double_t fStereo[2]; ///< Stereo angle front/back side [degrees]
    Bool_t   fIsSet;     ///< Flag whether parameters are set


    /** Temporary variables to avoid frequent calculations **/
    Double_t fNofStrips[2]; //! Number of strips on front/back side
    Double_t fCosStereo[2]; //! cos of stereo angle front/back side
    Double_t fSinStereo[2]; //! sin if stereo angle front/back side
    Int_t   fStripShift[2]; //! Shift in number of strips from bottom to top


    /** Get the readout channel in the module for a given strip and side
     ** @param strip     Strip number
     ** @param side      Side (0 = front, 1 = back)
     ** @param sensorId  Index of sensor within module
     ** @return  Channel number in module
     **/
    Int_t GetModuleChannel(Int_t strip, Int_t side, Int_t sensorId) const;


    /** Get strip and side from module channel.
     ** @param[in] channel   Channel number in module
     ** @param[in] sensorId  Sensor index in module
     ** @param[out]  strip   Strip number in sensor
     ** @param[out]  side    Sensor side [0 = front, 1 = back]
     **/
    void GetStrip(Int_t channel, Int_t sensorId, Int_t& strip, Int_t& side);


    /** Produce charge on front or back side from a CbmStsPoint
     ** @param side   0 = front, 1 = back side
     **/
    void ProduceCharge(CbmStsSensorPoint* point, Int_t side,
                       const CbmStsSenzor* sensor) const;


    /** Register produced charge in one strip
     ** @param side  0 = front, 1 = back
     ** @param strip strip number
     ** @param charge  charge in strip [e]
     ** @param time    time of registration [ns]
     **/
    void RegisterCharge(const CbmStsSenzor* sensor, Int_t side, Int_t strip,
                        Double_t charge, Double_t time) const;


    /** Test the consistent implementation of GetModuleChannel and
     ** GetStrip. The latter should be the reverse of the former.
     ** @return kTRUE if successful
     **/
    Bool_t SelfTest();



    ClassDef(CbmStsSensorTypeDssd,1);

};


#endif
