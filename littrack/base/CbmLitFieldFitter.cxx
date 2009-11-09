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

/* 0 degree polynom.
 * 1 coefficient.
 */
class CbmLitPolynom0 : public CbmLitPolynom
{
public:
	double Calculate(
			double x,
			double y,
			double c[]) const {
		return c[0];
	}
	unsigned int GetNofCoefficients() const {return 1;}
};

/* 1st degree polynom
 * 3 coefficients.
 */
class CbmLitPolynom1 : public CbmLitPolynom
{
public:
	double Calculate(
			double x,
			double y,
			double c[]) const {
		return c[0] + c[1]*x + c[2]*y;
	}
	unsigned int GetNofCoefficients() const {return 3;}
};


/* 2nd degree polynom
 * 6 coefficients.
 */
class CbmLitPolynom2 : public CbmLitPolynom
{
public:
	double Calculate(
			double x,
			double y,
			double c[]) const {
		return c[0] + c[1]*x + c[2]*y + c[3]*x*y + c[4]*x*x + c[5]*y*y;
	}
	unsigned int GetNofCoefficients() const {return 6;}
};

/* 3rd degree polynom
 * 10 coefficients.
 */
class CbmLitPolynom3 : public CbmLitPolynom
{
public:
	double Calculate(
			double x,
			double y,
			double c[]) const {
		double x2 = x*x;
		double y2 = y*y;
		double yx2 = y*x2;
		double xy2 = x*y2;
		double x3 = x2*x;
		double y3 = y2*y;
		return c[0] + c[1]*x + c[2]*y + c[3]*x*y + c[4]*x2 + c[5]*y2 + c[6]*xy2 + c[7]*yx2 + c[8]*x3 + c[9]*y3;
	}
	unsigned int GetNofCoefficients() const {return 10;}
};

/* 4th degree polynom
 * 15 coefficients.
 */
class CbmLitPolynom4 : public CbmLitPolynom
{
public:
	double Calculate(
			double x,
			double y,
			double c[])	const {
		double x2 = x*x;
		double y2 = y*y;
		double yx2 = y*x2;
		double xy2 = x*y2;
		double x3 = x2*x;
		double y3 = y2*y;
		double x4 = x2*x2;
		double y4 = y2*y2;
		return c[0] + c[1]*x + c[2]*y + c[3]*x*y + c[4]*x2 + c[5]*y2 + c[6]*xy2 + c[7]*yx2 + c[8]*x3 + c[9]*y3 +
			   + c[10]*x2*y2 + c[11]*x*y3 + c[12]*y*x3 + c[13]*x4 + c[14]*y4;
	}
	unsigned int GetNofCoefficients() const {return 15;}
};


/* 5th degree polynom
 * 22 coefficients.
 */
class CbmLitPolynom5 : public CbmLitPolynom
{
public:
	double Calculate(
			double x,
			double y,
			double c[]) const {
		double x2 = x*x;
		double y2 = y*y;
		double yx2 = y*x2;
		double xy2 = x*y2;
		double x3 = x2*x;
		double y3 = y2*y;
		double x4 = x2*x2;
		double y4 = y2*y2;
		double x5 = x4*x;
		double y5 = y4*y;
		return c[0] + c[1]*x + c[2]*y + c[3]*x*y + c[4]*x2 + c[5]*y2 + c[6]*xy2 + c[7]*yx2 + c[8]*x3 + c[9]*y3 +
			   c[10]*x2*y2 + c[11]*x*y3 + c[12]*y*x3 + c[13]*x4 + c[14]*y4 + c[15]*x2*y3 + c[16]*y2*x3 +
			   c[17]*x*y4 + c[18]*y*x4 + c[19]*x5 + c[20]*y5;
	}
	unsigned int GetNofCoefficients() const {return 22;}
};


/* 6th degree polynom
 * 28 coefficients.
 */
class CbmLitPolynom6 : public CbmLitPolynom
{
public:
	double Calculate(
			double x,
			double y,
			double c[]) const {
		double x2 = x*x;
		double y2 = y*y;
		double yx2 = y*x2;
		double xy2 = x*y2;
		double x3 = x2*x;
		double y3 = y2*y;
		double x4 = x2*x2;
		double y4 = y2*y2;
		double x5 = x4*x;
		double y5 = y4*y;
		double x6 = x4*x2;
		double y6 = y4*y2;
		return c[0] + c[1]*x + c[2]*y + c[3]*x*y2 + c[4]*x*y + c[5]*x2 + c[6]*y2 + c[7]*x3 + c[8]*y3 + c[9]*y*x2 +
			   c[10]*x*y3 + c[11]*y*x3 + c[12]*x2*y2 + c[13]*x4 + c[14]*y4 + c[15]*x6 + c[16]*y6 + c[17]*x2*y4 +
			   c[18]*y2*x4 + c[19]*x*y5 + c[20]*y*x5 + c[21]*x3*y3 + c[22]*x2*y3 + c[23]*y2*x3 + c[24]*x*y4 +
			   c[25]*y*x4 + c[26]*y5 + c[27]*x5;
	}
	unsigned int GetNofCoefficients() const {return 28;}
};


/* 7th degree polynom
 * 36 coefficients.
 */
class CbmLitPolynom7 : public CbmLitPolynom
{
public:
	double Calculate(
			double x,
			double y,
			double c[]) const {
		double x2 = x*x;
		double y2 = y*y;
		double yx2 = y*x2;
		double xy2 = x*y2;
		double x3 = x2*x;
		double y3 = y2*y;
		double x4 = x2*x2;
		double y4 = y2*y2;
		double x5 = x4*x;
		double y5 = y4*y;
		double x6 = x4*x2;
		double y6 = y4*y2;
		double x7 = x4*x3;
		double y7 = y4*y3;
		return c[0] + c[1]*x + c[2]*y + c[3]*x*y2 + c[4]*x*y + c[5]*x2 + c[6]*y2 + c[7]*x3 + c[8]*y3 + c[9]*y*x2 +
			   c[10]*x*y3 + c[11]*y*x3 + c[12]*x2*y2 + c[13]*x4 + c[14]*y4 + c[15]*x6 + c[16]*y6 + c[17]*x2*y4 +
			   c[18]*y2*x4 + c[19]*x*y5 + c[20]*y*x5 + c[21]*x3*y3 + c[22]*x7 + c[23]*y7 + c[24]*y3*x4 + c[25]*x2*y5 +
			   c[26]*y2*x5 + c[27]*x*y6 + c[28]*y*x6 + c[29]*x2*y3 + c[30]*y2*x3 + c[31]*x*y4 + c[32]*y*x4 +
			   c[33]*x3*y4 + c[34]*y5 + c[35]*x5;
	}
	unsigned int GetNofCoefficients() const {return 36;}
};


/* 8th degree polynom
 * 45 coefficients.
 */
class CbmLitPolynom8 : public CbmLitPolynom
{
public:
	double Calculate(
			double x,
			double y,
			double c[]) const {
		double x2 = x*x;
		double y2 = y*y;
		double yx2 = y*x2;
		double xy2 = x*y2;
		double x3 = x2*x;
		double y3 = y2*y;
		double x4 = x2*x2;
		double y4 = y2*y2;
		double x5 = x4*x;
		double y5 = y4*y;
		double x6 = x4*x2;
		double y6 = y4*y2;
		double x7 = x4*x3;
		double y7 = y4*y3;
		double x8 = x4*x4;
		double y8 = y4*y4;

		return c[0] + c[1]*x + c[2]*y + c[3]*x*y2 + c[4]*x*y + c[5]*x2 + c[6]*y2 + c[7]*x3 + c[8]*y3 + c[9]*y*x2 +
			   c[10]*y*x7 + c[11]*y2*x6 + c[12]*x*y7 + c[13]*x2*y6 + c[14]*x*y3 + c[15]*y*x3 + c[16]*x2*y2 + c[17]*x4 +
			   c[18]*y4 + c[19]*x6 + c[20]*y6 + c[21]*x4*y4 + c[22]*x2*y4 + c[23]*y2*x4 + c[24]*x*y5 + c[25]*y*x5 +
			   c[26]*x3*y3 + c[27]*x7 + c[28]*y7 + c[29]*x3*y5 + c[30]*y3*x4 + c[31]*x2*y5 + c[32]*y2*x5 + c[33]*x*y6 +
			   c[34]*y*x6 + c[35]*x2*y3 + c[36]*y2*x3 + c[37]*x*y4 + c[38]*y*x4 + c[39]*x3*y4 + c[40]*y5 + c[41]*x5 +
			   c[42]*y3*x5 + c[43]*x8 + c[44]*y8;
	}
	unsigned int GetNofCoefficients() const {return 45;}
};

/* 9th degree polynom
 * 55 coefficients.
 */
class CbmLitPolynom9 : public CbmLitPolynom
{
public:
	double Calculate(
			double x,
			double y,
			double c[]) const {
		double x2 = x*x;
		double y2 = y*y;
		double yx2 = y*x2;
		double xy2 = x*y2;
		double x3 = x2*x;
		double y3 = y2*y;
		double x4 = x2*x2;
		double y4 = y2*y2;
		double x5 = x4*x;
		double y5 = y4*y;
		double x6 = x4*x2;
		double y6 = y4*y2;
		double x7 = x4*x3;
		double y7 = y4*y3;
		double x8 = x4*x4;
		double y8 = y4*y4;
		double x9 = x5*x4;
		double y9 = y5*y4;

		return c[0] + c[1]*x + c[2]*y + c[3]*xy2 + c[4]*x*y + c[5]*x2 + c[6]*y2 + c[7]*x3 + c[8]*y3 + c[9]*yx2 + c[10]*y*x7 +
			   c[11]*y2*x6 + c[12]*x*y7 + c[13]*x2*y6 + c[14]*x2*y7 + c[15]*x9 + c[16]*y9 + c[17]*x*y3 + c[18]*y*x3 + c[19]*x2*y2 +
			   c[20]*x4 + c[21]*y4 + c[22]*x6 + c[23]*y6 + c[24]*x4*y4 + c[25]*x2*y4 + c[26]*y2*x4 + c[27]*x*y5 + c[28]*y*x5 +
			   c[29]*x3*y3 + c[30]*x7 + c[31]*y7 + c[32]*x3*y5 + c[33]*y3*x4 + c[34]*x2*y5 + c[35]*y2*x5 + c[36]*x*y6 + c[37]*y*x6 +
			   c[38]*x2*y3 + c[39]*y2*x3 + c[40]*x*y4 + c[41]*y*x4 + c[42]*x3*y4 + c[43]*y5 + c[44]*x5 + c[45]*y3*x5 + c[46]*x*y8 +
			   c[47]*y3*x6 + c[48]*x4*y5 + c[49]*y4*x5 + c[50]*x3*y6 + c[51]*y2*x7 + c[52]*y*x8 + c[53]*x8 + c[54]*y8;
	}
	unsigned int GetNofCoefficients() const {return 55;}
};


class FCNPolynom : public ROOT::Minuit2::FCNBase {
public:
    FCNPolynom(
    		const std::vector<double>& x,
            const std::vector<double>& y,
            const std::vector<double>& z,
            CbmLitPolynom* polynom) :
            	fX(x),
            	fY(y),
            	fZ(z),
            	fPolynom(polynom) {}

    ~FCNPolynom() {}

    virtual double Up() const {return 1.;}

    virtual double operator()(
    		const std::vector<double>& par) const {
    	double* p = const_cast<double*>(&par[0]);
		double r = 0.;
		for(unsigned int i = 0; i < fX.size(); i++) {
			double ri = fZ[i] - fPolynom->Calculate(fX[i], fY[i], p);
			r +=   ri * ri;
		}
		return r;
    }

    const CbmLitPolynom* GetPolynom() const {return fPolynom;}

private:
    std::vector<double> fX;
    std::vector<double> fY;
    std::vector<double> fZ;
    CbmLitPolynom* fPolynom;
};


CbmLitFieldFitter::CbmLitFieldFitter(
		unsigned int polynomDegree):
	fXangle(25.),
	fYangle(25.),
	fNofBinsX(30),
	fNofBinsY(30),
	fUseEllipseAcc(false),
	fPolynomDegree(polynomDegree)
{
	CbmLitEnvironment* env = CbmLitEnvironment::Instance();
	fField = env->GetField();

	switch (polynomDegree){
	case 0:
		fPolynom = new CbmLitPolynom0();
	    break;
	case 1:
		fPolynom = new CbmLitPolynom1();
		break;
	case 2:
		fPolynom = new CbmLitPolynom2();
		break;
	case 3:
		fPolynom = new CbmLitPolynom3();
		break;
	case 4:
		fPolynom = new CbmLitPolynom4();
		break;
	case 5:
		fPolynom = new CbmLitPolynom5();
		break;
	case 6:
		fPolynom = new CbmLitPolynom6();
		break;
	case 7:
		fPolynom = new CbmLitPolynom7();
		break;
	case 8:
		fPolynom = new CbmLitPolynom8();
		break;
	case 9:
		fPolynom = new CbmLitPolynom9();
		break;
	default:
		fPolynom = NULL;
	}
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
	FCNPolynom *theFCN = new FCNPolynom(x, y, z, fPolynom);

	TFitterMinuit theMinuit;
	theMinuit.SetPrintLevel(-1);
	theMinuit.SetMinuitFCN(theFCN);
	int nofParams = theFCN->GetPolynom()->GetNofCoefficients();

	for (int i = 0; i < nofParams; i++) {
		std::stringstream ss;
		ss << "c" << i;
		theMinuit.SetParameter(i, ss.str().c_str(), 0., 0.1, 1., -1.);
	}
	theMinuit.CreateMinimizer();
	theMinuit.Minimize();

	par.clear();
	for(int i = 0; i < nofParams; i++) {
		par.push_back(theMinuit.GetParameter(i));
	}
//	delete theFCN;
}

