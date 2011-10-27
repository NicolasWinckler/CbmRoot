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
      fRefId(-1), fStationId(-1) {}

   /*
    * \brief Destructor.
    */
   virtual ~CbmLitMCPoint() {}

   /* Getters */
   myf GetX() const { return fX; }
   myf GetY() const { return fY; }
   myf GetZ() const { return fZ; }
   myf GetPx() const { return fPx; }
   myf GetPy() const { return fPy; }
   myf GetPz() const { return fPz; }
   int GetRefId() const { return fRefId; }
   int GetStationId() const { return fStationId; }
   myf GetTx() const { return fPx / fPz; }
   myf GetTy() const { return fPy / fPz; }
   myf GetP() const { return std::sqrt(fPx*fPx + fPy*fPy + fPz*fPz);}

   /* Setters */
   void SetX(myf x) { fX = x; }
   void SetY(myf y) { fY = y; }
   void SetZ(myf z) { fZ = z; }
   void SetPx(myf px) { fPx = px; }
   void SetPy(myf py) { fPy = py; }
   void SetPz(myf pz) { fPz = pz; }
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
   myf fX, fY, fZ; // Space coordinates [cm].
   myf fPx, fPy, fPz; // Momentum components.
   int fRefId; // Reference index.
   int fStationId; // Station index.
};

#endif /* CBMLITMCPOINT_H_ */
