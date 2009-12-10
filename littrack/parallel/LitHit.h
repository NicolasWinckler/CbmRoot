/** LitHit.h
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2009
 * @version 1.0
 *
 * Hit data classes for fast parallel version of the Littrack tracking.
 **/

#ifndef LITHIT_H_
#define LITHIT_H_

#include "LitTypes.h"

template<class T>
class LitStripHit
{
public:
	T phiCos;
	T phiSin;
	T U;
	T Du;

	friend std::ostream & operator<<(std::ostream &strm, const LitStripHit &hit){
		strm << "LitStripHit: " << "phiCos=" << hit.phiCos << " phiSin=" << hit.phiSin
			<< " U=" << hit.U << " Du=" << hit.Du
//			<< " planeId=" << (int)hit.planeId
			<< std::endl;
		return strm;
	}
} _fvecalignment;


template<class T>
class LitPixelHit
{
public:
	T X, Y;
	T Dx, Dy;
	T Dxy;

	friend std::ostream & operator<<(std::ostream &strm, const LitPixelHit &hit){
		strm << "LitPixelHit: " << "X=" << hit.X << " Y=" << hit.Y
			<< " Dx=" << hit.Dx << " Dy=" << hit.Dy << " Dxy=" << hit.Dxy
//			<< " planeId=" << (int)hit.planeId
			<< std::endl;
		return strm;
	}
} _fvecalignment;



class LitScalStripHit
{
public:
	fscal phiCos;
	fscal phiSin;
	fscal U;
	fscal Du;
	unsigned char planeId;
	unsigned short refId;
	fscal Z;

	friend std::ostream & operator<<(std::ostream &strm, const LitScalStripHit &hit){
		strm << "LitScalStripHit: " << "phiCos=" << hit.phiCos << " phiSin=" << hit.phiSin
			<< " U=" << hit.U << " Du=" << hit.Du << " planeId=" << (int)hit.planeId
			<< " refId=" << hit.refId << " Z=" << hit.Z << std::endl;
		return strm;
	}
};// _fvecalignment;



class LitScalPixelHit
{
public:
	fscal X, Y;
	fscal Dx, Dy;
	fscal Dxy;
	unsigned char planeId;
	unsigned short refId;
	fscal Z;

	friend std::ostream & operator<<(std::ostream &strm, const LitScalPixelHit &hit){
		strm << "LitScalPixelHit: " << "X=" << hit.X << " Y=" << hit.Y
			<< " Dx=" << hit.Dx << " Dy=" << hit.Dy << " Dxy=" << hit.Dxy
			<< " planeId=" << (int)hit.planeId << " refId=" << hit.refId << " Z=" << hit.Z << std::endl;
		return strm;
	}
};// _fvecalignment;


#endif /* LITHIT_H_ */
