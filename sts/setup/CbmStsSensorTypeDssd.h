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
    virtual void ProcessPoint(CbmStsSensorPoint*);


    /** Set the parameters
     ** @param dx,dy,dz          Size in x,y,z [cm]
     ** @param pitchF,pitchB     Strip pitch foint and back side [cm]
     ** @param stereoF,stereoB   Strip stereo angle fron and back side [degrees]
     **/
    void SetParameters(Double_t dx, Double_t dy, Double_t dz,
                       Double_t pitchF, Double_t pitchB,
                       Double_t stereoF, Double_t stereoB);


  private:

    Double_t fDx;       ///< Dimension in x [cm]
    Double_t fDy;       ///< Dimension in y [cm]
    Double_t fDz;       ///< Thickness in z [cm]
    Double_t fPitchF;   ///< Strip pitch front side [cm]
    Double_t fPitchB;   ///< Strip pitch back side [cm]
    Double_t fStereoF;  ///< Stereo angle front side [degrees]
    Double_t fStereoB;  ///< Stereo angle back side [degrees]
    Bool_t   fIsSet;    ///< Flag whether parameters are set


    /** Temporary variables to avoid frequent calculations **/
    Double_t fNofStripsF;   //! Number of strips on front side
    Double_t fNofStripsB;   //! Number of strips on back side
    Double_t fCosStereoF;   //! cos of stereo angle front side
    Double_t fSinStereoF;   //! sin if stereo angle front side
    Double_t fCosStereoB;   //! cos of stereo angle back side
    Double_t fSinStereoB;   //! sin of stereo angle back side


    /** Produce charge on front or back side from a CbmStsPoint
     ** @param side   0 = front, 1 = back side
     **/
    void ProduceCharge(CbmStsSensorPoint* point, Int_t side) const;


    /** Register produced charge in one strip
     ** @param side  0 = front, 1 = back
     ** @param strip strip number
     ** @param charge  charge in strip [e]
     ** @param time    time of registration [ns]
     **/
    void RegisterCharge(Int_t side, Int_t strip,
                        Double_t charge, Double_t time) const;






  ClassDef(CbmStsSensorTypeDssd,1);

};




#endif
