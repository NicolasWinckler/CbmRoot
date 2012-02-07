/**
* \file CbmRichRingFitterEllipse.h
*
* \brief This is the implementation of ellipse fitting
*    using MINUIT.
* \author Semen Lebedev <s.lebedev@gsi.de>
* \date 2011
**/

#ifndef CBMRICHRINGFITTERELLIPSEH
#define CBMRICHRINGFITTERELLIPSEH

#include "CbmRichRingFitterEllipseBase.h"
#include "TFitterMinuit.h"
//#include "TH2D.h"

#include <vector>

using std::vector;

/**
* \class FCNEllipse
*
* \brief FCN for Minuit.
* \author Semen Lebedev <s.lebedev@gsi.de>
* \date 2011
**/
class FCNEllipse : public ROOT::Minuit2::FCNBase {
public:
   /**
    * \brief Default constructor.
    */
   FCNEllipse(
         const std::vector<Double_t>& x,
         const std::vector<Double_t>& y) :
            fX(x),
            fY(y),
            fErrorDef(1.)
   {

   }

   /**
    * \brief Default destructor.
    */
   ~FCNEllipse()
   {

   }

   /**
    * \brief Inherited from ROOT::Minuit2::FCNBase.
    */
   virtual Double_t Up() const
   {
      return fErrorDef;
   }

   /**
    * \brief Inherited from ROOT::Minuit2::FCNBase.
    */
   virtual Double_t operator()(
         const vector<Double_t>& par) const
   {
      Double_t r = 0.;
      for(UInt_t i = 0; i < fX.size(); i++) {
         Double_t ri = calcE(i, par);
         r +=   ri * ri;
      }
      return r;
   }

   /**
    * \brief Calculate E for certain hit.
    * \param[in] i Hit index.
    * \param[in] par Ellipse parameters.
    */
   Double_t calcE(
         Int_t i,
         const vector<Double_t>& par) const
   {
      Double_t d1 = sqrt( (fX[i] - par[0])*(fX[i] - par[0]) +
                          (fY[i] - par[1])*(fY[i] - par[1])  );
      Double_t d2 = sqrt( (fX[i] - par[2])*(fX[i] - par[2]) +
                          (fY[i] - par[3])*(fY[i] - par[3])  );
      Double_t ri = d1 + d2 - 2 * par[4];
      return ri;
    }


   vector<Double_t> X() const {return fX;}

   vector<Double_t> Y() const {return fY;}

   void SetErrorDef(Double_t def) {fErrorDef = def;}

private:
    vector<Double_t> fX; // vector of X coordinates
    vector<Double_t> fY; // vector of Y coordinates
    Double_t fErrorDef;
};

/**
* \class CbmRichRingFitterEllipse
*
* \brief This is the implementation of ellipse fitting
*    using MINUIT.
* \author Semen Lebedev <s.lebedev@gsi.de>
* \date 2011
**/
class CbmRichRingFitterEllipse : public CbmRichRingFitterEllipseBase
{
public:
   /**
    * \brief Default constructor.
    */
   CbmRichRingFitterEllipse();

   /**
    * \brief Standard destructor.
    */
   virtual ~CbmRichRingFitterEllipse();

   /**
    * \brief Fit ring using hit coordinates from vectors.
    * \param[in] ring RICH ring to be fitted.
    * \param[in] hitX Vector of x coordinates of hits.
    * \param[in] hitY Vector of y coordinates of hits.
    */
   void DoFit(
         CbmRichRing *ring,
         const vector<Double_t>& hitX,
         const vector<Double_t>& hitY);

   /**
    * \brief Fit ring.
    * \param[in,out] ring RICH ring to be fitted.
    */
   void DoFit(
         CbmRichRing *ring);

   /**
    * \brief Execute ring fitting algorithm.
    * \param[in] x X coordinates of hits.
    * \param[in] y Y coordinates of hit.
    */
   vector<Double_t> DoFit(
         const vector<Double_t>& x,
         const vector<Double_t>& y);

protected:
   /**
    * \brief Transform obtained parameters from MINUIT to CbmRichRing.
    * \param[out] x pointer to the RICH ring.
    * \param[in] par Parameters obtained from MINUIT.
    */
   void TransformToRichRing(
         CbmRichRing* ring,
         const vector<Double_t>& par);

   ClassDef(CbmRichRingFitterEllipse,1);
};

#endif
