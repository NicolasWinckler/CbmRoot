/**
* \file CbmRichRingFitterEllipseBase.h
*
* \brief Base class for concrete ellipse fitting algorithms.
*  Each derived class must implement the method DoFit.
*
* \author Semen Lebedev <s.lebedev@gsi.de>
* \date 2011
**/

#ifndef CBMRICHRINGFITTERELLIPSEBASEH
#define CBMRICHRINGFITTERELLIPSEBASEH

#include "CbmRichRingFitterImpl.h"

class CbmRichRing;
class TH2D;
class CbmRichHit;

/**
* \class CbmRichRingFitterEllipseBase
*
* \brief Base class for concrete ellipse fitting algorithms.
*  Each derived class must implement the method DoFit.
*
* \author Semen Lebedev <s.lebedev@gsi.de>
* \date 2011
**/
class CbmRichRingFitterEllipseBase: public CbmRichRingFitterImpl{
public:
   /**
    * \brief Default constructor.
    */
   CbmRichRingFitterEllipseBase(){ };

   /**
    * \brief Destructor.
    */
   virtual ~CbmRichRingFitterEllipseBase(){ };

protected:
   /**
    * \brief Calculate chi2.
    * \param[in,out] ring RICH ring.
    */
   virtual void CalcChi2(
         CbmRichRing* ring);

   /**
    * \brief Calculate chi2 using parameters of 2D curve.
    * \param[in] A A parameter of curve.
    * \param[in] B B parameter of curve.
    * \param[in] C C parameter of curve.
    * \param[in] D D parameter of curve.
    * \param[in] E E parameter of curve.
    * \param[in] F F parameter of curve.
    * \param[in] ring RICH ring.
    */
   virtual void CalcChi2(
         Double_t A,
         Double_t B,
         Double_t C,
         Double_t D,
         Double_t E,
         Double_t F,
         CbmRichRing* ring);

   /**
    * \brief Perform radius correction procedure.
    * Corrected parameters are store in fAaxisCor and fBaxisCor variables.
    * \param[in,out] ring RICH ring.
    */
   virtual void PerformRadiusCorrection(
         CbmRichRing* ring);

   /**
    * \brief Initialize histograms for radius correction procedure.
    */
   virtual void InitHistForRadiusCorrection();

   TH2D* fh_mapaxisAXY; // correction map for A axis
   TH2D* fh_mapaxisBXY; // correction map for B axis

   ClassDef(CbmRichRingFitterEllipseBase,1);
};

#endif /* CBMRICHRINGFITTERELLIPSEBASEH */
