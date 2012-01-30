/**
 * \file CbmLitMCPoint.h
 *
 * \brief Monte-Carlo point.
 *
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2011
 **/
#ifndef CBMLITMCPOINT_H_
#define CBMLITMCPOINT_H_

#include "std/base/CbmLitFloat.h"
#include <string>
#include <sstream>
#include <cmath>

/**
 * \class CbmLitMCPoint
 *
 * \brief Monte-Carlo point.
 *
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2011
 **/
class CbmLitMCPoint {
public:
   /*
    * \brief Constructor.
    */
   CbmLitMCPoint():
      fX(0.), fY(0.), fZ(0.),
      fPx(0.), fPy(0.), fPz(0.),
      fRefId(-1), fStationId(-1), fQ(1.) {}

   /*
    * \brief Destructor.
    */
   virtual ~CbmLitMCPoint() {}

   /* Getters */
   litfloat GetX() const { return fX; }
   litfloat GetY() const { return fY; }
   litfloat GetZ() const { return fZ; }
   litfloat GetPx() const { return fPx; }
   litfloat GetPy() const { return fPy; }
   litfloat GetPz() const { return fPz; }
   int GetRefId() const { return fRefId; }
   int GetStationId() const { return fStationId; }
   litfloat GetTx() const { return fPx / fPz; }
   litfloat GetTy() const { return fPy / fPz; }
   litfloat GetP() const { return std::sqrt(fPx*fPx + fPy*fPy + fPz*fPz);}
   litfloat GetQp() const {
      litfloat p = GetP();
      litfloat res = (p != 0.) ? fQ/p : 0.;
      return res;
   }


   /* Setters */
   void SetX(litfloat x) { fX = x; }
   void SetY(litfloat y) { fY = y; }
   void SetZ(litfloat z) { fZ = z; }
   void SetPx(litfloat px) { fPx = px; }
   void SetPy(litfloat py) { fPy = py; }
   void SetPz(litfloat pz) { fPz = pz; }
   void SetQ(litfloat q) { fQ = q; }
   void SetRefId(int refId) { fRefId = refId; }
   void SetStationId(int stationId) { fStationId = stationId; }

   /**
    * \brief Returns std::string representation of the class.
    * \return Class representation as std::string.
    */
   virtual std::string ToString() const {
      std::stringstream ss;
      ss << "MCPoint: pos=(" << GetX() << "," << GetY() << "," << GetZ() << ") "
         << "mom=(" << GetPx() << "," << GetPy() << "," << GetPz() << ") "
         << "refId=" << GetRefId() << " stationId=" << GetStationId() << std::endl;
      return ss.str();
   }

   /**
    * \brief Operator << for convenient output to std::ostream.
    * \return Insertion stream in order to be able to call a succession of insertion operations.
    */
   friend std::ostream& operator<<(std::ostream& strm, const CbmLitMCPoint& point) {
      strm << point.ToString();
      return strm;
   }

private:
   litfloat fX, fY, fZ; // Space coordinates [cm].
   litfloat fPx, fPy, fPz; // Momentum components.
   int fRefId; // Reference index.
   int fStationId; // Station index.
   litfloat fQ; // +/-1 - negative or positive charge
};

#endif /* CBMLITMCPOINT_H_ */
