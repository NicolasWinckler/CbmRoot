/**
* \file CbmRichProjectionProducer.h
*
* \brief Project track by straight line from imaginary plane
* to the mirror and reflect it to the photodetector plane.
*
* \author P.Stolpovsky
* \date 2005
**/

#ifndef CBM_RICH_PROJECTION_PRODUCER
#define CBM_RICH_PROJECTION_PRODUCER

class TClonesArray;
class CbmGeoRichPar;
class TObjArray;

/**
* \class CbmRichProjectionProducer
*
* \brief Project track by straight line from imaginary plane
* to the mirror and reflect it to the photodetector plane.
*
* \author P.Stolpovsky
* \date 2005
**/
class CbmRichProjectionProducer
{
public:

   /**
    * \brief Standard constructor.
    * \param[in] zflag Flag whether to use point in imaginary plane (zflag=1) or mirror point (zflag=2) for extrapolation.
    */
   CbmRichProjectionProducer(
         int zflag);

   /**
    * \brief Destructor.
    */
   virtual ~CbmRichProjectionProducer();

   /**
    * \brief Initialization of the task.
    */
   virtual void Init();

   /**
    * \brief Initialization  of Parameter Containers.
    */
   virtual void SetParContainers();

   /**
    * \brief Executed task.
    */
   virtual void DoProjection();

   /**
    * \brief Set flag whether to use point in imaginary plane (zflag=1)
    * or mirror point (zflag=2) for extrapolation.
    * \param[in] flag Flag.
    */
   void SetZFlag(int flag) {fZflag = flag;}

protected:
   int fZflag; //Flag whether to use point in imaginary plane (zflag=1) or mirror point (zflag=2) for extrapolation.

private:
   TClonesArray* fListRICHImPlanePoint; // Starting points&directions
   TClonesArray* fProjectionTrackParam; // RICH projections as FairTrackParam

   int fNHits; // Number of hits
   int fEvent; // number of events

   double fDetX; // X-coordinate of photodetector
   double fDetY; // Y-coordinate of photodetector
   double fDetZ; // Z-coordinate of photodetector
   double fDetWidthX; // width of photodetector in x
   double fDetWidthY; // width of photodetector in y
   double fThetaDet; // tilting angle of photodetector (around x-axis)
   double fPhiDet; // tilting angle of photodetector (around y-axis)
  
   double fDetX_transf; // X-coordinate of photodetector (transformed system)
   double fDetY_transf; // Y-coordinate of photodetector (transformed system)
   double fDetZ_transf; // Z-coordinate of photodetector (transformed system)

   double fZm; // Z-coordinate of mirror center
   double fYm; // Y-coordinate of mirror center
   double fXm; // X-coordinate of mirror center
   double fR; // mirror radius
   double fThetaM; // mirror tilting angle
   double fSPHE_theta; // theta angle for SPHE
   double fSPHE_phi; // phi angle for SPHE
  
   double fmax_x; // reasonable max x value for track extrapolation
   double fmax_y; // reasonable max y value for track extrapolation

   TObjArray* fSensNodes;
   TObjArray* fPassNodes;
   CbmGeoRichPar* fPar;

  /**
   * \brief Copy constructor.
   */
  CbmRichProjectionProducer(const CbmRichProjectionProducer&);

  /**
   * \brief Assignment operator.
   */
  CbmRichProjectionProducer& operator=(const CbmRichProjectionProducer&);
};

#endif
