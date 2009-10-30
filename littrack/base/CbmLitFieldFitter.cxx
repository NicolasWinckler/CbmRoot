/** CbmLitFieldFitter.cxx
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2009
 * @version 1.0
 *
 **/
#include "CbmLitFieldFitter.h"
#include "CbmLitEnvironment.h"
#include "FairField.h"

#include "TFitterMinuit.h"

#include <cmath>

class FCNPolynom : public ROOT::Minuit2::FCNBase {
public:
    FCNPolynom(
    		const std::vector<double>& x,
            const std::vector<double>& y,
            const std::vector<double>& z) : fX(x), fY(y), fZ(z) {}

    ~FCNPolynom() {}

    virtual double Up() const {return 1.;}

    virtual double operator()(
    		const std::vector<double>& par) const {
		double r = 0.;
		for(unsigned int i = 0; i < fX.size(); i++) {
			double ri = calcE(fX[i], fY[i], fZ[i] , par);
			r +=   ri * ri;
		}
		return r;
    }

    double calcE(
    		double x,
    		double y,
    		double z,
    		const std::vector<double>& par) const {
        return z - Func(x, y, par);
    }

    double Func(
    		double x,
    		double y,
    		const std::vector<double>& par) const {
		double x2 = x*x;
		double y2 = y*y;
		double xy = x*y;
		double x3 = x2*x;
		double y3 = y2*y;
		double xy2 = x*y2;
		double x2y = x2*y;

		return par[0] + par[1]*x + par[2]*y + par[3]*x2 + par[4]*xy + par[5]*y2 + par[6]*x3 + par[7]*x2y + par[8]*xy2 + par[9]*y3;
    }

private:
    std::vector<double> fX;
    std::vector<double> fY;
    std::vector<double> fZ;
};


CbmLitFieldFitter::CbmLitFieldFitter():
	fXangle(25.),
	fYangle(25.),
	fNofBinsX(30),
	fNofBinsY(30),
	fUseEllipseAcc(false)
{
	CbmLitEnvironment* env = CbmLitEnvironment::Instance();
	fField = env->GetField();
}

CbmLitFieldFitter::~CbmLitFieldFitter()
{

}

void CbmLitFieldFitter::FitSlice(
		double Z,
		std::vector<double>& parBx,
		std::vector<double>& parBy,
		std::vector<double>& parBz)
{
	double tanXangle = std::tan(fXangle*3.14159265/180); //
	double tanYangle = std::tan(fYangle*3.14159265/180); //

	double Xmax = Z * tanXangle;
	double Ymax = Z * tanYangle;

	double HX = 2 * Xmax / fNofBinsX; // step size for X position
	double HY = 2 * Ymax / fNofBinsY; // step size for Y position
	std::vector<double> x;
	std::vector<double> y;
	std::vector<double> Bx;
	std::vector<double> By;
	std::vector<double> Bz;
	for (int j = 0; j < fNofBinsX; j++) { // loop over x position
		double X = -Xmax + j * HX;
		for (int k = 0; k < fNofBinsY; k++) { // loop over y position
			double Y = -Ymax + k * HY;

			//check the acceptance
			if (fUseEllipseAcc) {
				double el = (X*X)/(Xmax*Xmax) + (Y*Y)/(Ymax*Ymax);
				if (el > 1.) continue;
			}

			// get field value
			double pos[3] = {X, Y, Z};
			double B[3];
			fField->GetFieldValue(pos, B);

			x.push_back(X);
			y.push_back(Y);
			Bx.push_back(B[0]);
			By.push_back(B[1]);
			Bz.push_back(B[2]);
		}
	}
	FitSlice(x, y, Bx, parBx);
	FitSlice(x, y, By, parBy);
	FitSlice(x, y, Bz, parBz);
}

void CbmLitFieldFitter::FitSlice(
		const std::vector<double>& x,
		const std::vector<double>& y,
		const std::vector<double>& z,
		std::vector<double>& par)
{
	FCNPolynom *theFCN = new FCNPolynom(x, y, z);

	TFitterMinuit theMinuit;
	theMinuit.SetPrintLevel(-1);
	theMinuit.SetMinuitFCN(theFCN);
	theMinuit.SetParameter(0, "c0", 0., 0.1, 1., -1.);
	theMinuit.SetParameter(1, "c1", 0., 0.1, 1., -1.);
	theMinuit.SetParameter(2, "c2", 0., 0.1, 1., -1.);
	theMinuit.SetParameter(3, "c3", 0., 0.1, 1., -1.);
	theMinuit.SetParameter(4, "c4", 0., 0.1, 1., -1.);
	theMinuit.SetParameter(5, "c5", 0., 0.1, 1., -1.);
	theMinuit.SetParameter(6, "c6", 0., 0.1, 1., -1.);
	theMinuit.SetParameter(7, "c7", 0., 0.1, 1., -1.);
	theMinuit.SetParameter(8, "c8", 0., 0.1, 1., -1.);
	theMinuit.SetParameter(9, "c9", 0., 0.1, 1., -1.);
	theMinuit.CreateMinimizer();
	theMinuit.Minimize();

	par.clear();
	for(int i = 0; i < 10; i++) {
		par.push_back(theMinuit.GetParameter(i));
	}
}

