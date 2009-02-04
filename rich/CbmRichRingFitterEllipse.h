/*  CbmRichRingFitterEllipse
    Description: This is the implementation of ellipse fitting
    using MINUIT.

    Author : Simeon Lebedev and Gennady Ososkov
    E-mail : S.Lebedev@gsi.de
*/


#ifndef CBM_RICH_RING_FITTER_ELLIPSE_H
#define CBM_RICH_RING_FITTER_ELLIPSE_H 1

#include "CbmRichRingFitterEllipseBase.h"
#include "TFitterMinuit.h"
#include "TH2D.h"

class FCNEllipse : public ROOT::Minuit2::FCNBase {
public:
    FCNEllipse(const std::vector<Double_t>& x,
               const std::vector<Double_t>& y) :
                            fX(x),
                            fY(y),
                            fErrorDef(1.) {
               }

    ~FCNEllipse() {}

    virtual Double_t Up() const {return fErrorDef;}

    virtual Double_t operator()(const std::vector<Double_t>& par) const{
     Double_t r = 0.;
        for(UInt_t i = 0; i < fX.size(); i++) {
            Double_t ri = calcE(i, par);
            r +=   ri * ri;
        }
        return r;
    }

    Double_t calcE(Int_t i, const std::vector<Double_t>& par) const{
        Double_t d1 = sqrt( (fX[i] - par[0])*(fX[i] - par[0]) +
                          (fY[i] - par[1])*(fY[i] - par[1])  );
        Double_t d2 = sqrt( (fX[i] - par[2])*(fX[i] - par[2]) +
                          (fY[i] - par[3])*(fY[i] - par[3])  );
        Double_t ri = d1 + d2 - 2 * par[4];
        return ri;
    }

    std::vector<Double_t> X() const {return fX;}
    std::vector<Double_t> Y() const {return fY;}
    void SetErrorDef(Double_t def) {fErrorDef = def;}
private:

    std::vector<Double_t> fX;
    std::vector<Double_t> fY;
    Double_t fErrorDef;
};

class CbmRichRingFitterEllipse : public CbmRichRingFitterEllipseBase
{
public:

   /** Default constructor **/
   CbmRichRingFitterEllipse();

   /** Standard constructor **/
   CbmRichRingFitterEllipse(Int_t verbose,
                            Int_t correction,
                            TString fieldName);


   virtual ~CbmRichRingFitterEllipse();

   void DoFit1(CbmRichRing *pRing, std::vector<Double_t> x, std::vector<Double_t> y);

   void DoFit(CbmRichRing *pRing);

   /** Ring Fitting algorithm **/
   std::vector<Double_t> DoFit1(std::vector<Double_t> x,
               std::vector<Double_t> y);

protected:
   void TransformToRichRing(CbmRichRing* ring, std::vector<Double_t> par);

   ClassDef(CbmRichRingFitterEllipse,1);

};

#endif
