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
      fXIn(0.), fYIn(0.), fZIn(0.),
      fPxIn(0.), fPyIn(0.), fPzIn(0.),
      fXOut(0.), fYOut(0.), fZOut(0.),
      fPxOut(0.), fPyOut(0.), fPzOut(0.),
      fRefId(-1), fStationId(-1), fQ(1.) {}

   /*
    * \brief Destructor.
    */
   virtual ~CbmLitMCPoint() {}

   /* Getters */
   litfloat GetX() const { return (fXIn + fXOut) / 2.; }
   litfloat GetY() const { return (fYIn + fYOut) / 2.; }
   litfloat GetZ() const { return (fZIn + fZOut) / 2.; }
   litfloat GetPx() const { return (fPxIn + fPxOut) / 2.; }
   litfloat GetPy() const { return (fPyIn + fPyOut) / 2.; }
   litfloat GetPz() const { return (fPzIn + fPzOut) / 2.; }
   litfloat GetTx() const { return GetPx() / GetPz(); }
   litfloat GetTy() const { return GetPy() / GetPz(); }
   litfloat GetP() const { return std::sqrt(GetPx() * GetPx() + GetPy() * GetPy() + GetPz() * GetPz()); }
   litfloat GetQp() const {
      litfloat p = GetP();
      litfloat res = (p != 0.) ? fQ / p : 0.;
      return res;
   }
   litfloat GetXIn() const { return fXIn; }
   litfloat GetYIn() const { return fYIn; }
   litfloat GetZIn() const { return fZIn; }
   litfloat GetPxIn() const { return fPxIn; }
   litfloat GetPyIn() const { return fPyIn; }
   litfloat GetPzIn() const { return fPzIn; }
   litfloat GetTxIn() const { return fPxIn / fPzIn; }
   litfloat GetTyIn() const { return fPyIn / fPzIn; }
   litfloat GetPIn() const { return std::sqrt(fPxIn * fPxIn + fPyIn * fPyIn + fPzIn * fPzIn); }
   litfloat GetQpIn() const {
      litfloat p = GetPIn();
      litfloat res = (p != 0.) ? fQ / p : 0.;
      return res;
   }
   litfloat GetXOut() const { return fXOut; }
   litfloat GetYOut() const { return fYOut; }
   litfloat GetZOut() const { return fZOut; }
   litfloat GetPxOut() const { return fPxOut; }
   litfloat GetPyOut() const { return fPyOut; }
   litfloat GetPzOut() const { return fPzOut; }
   litfloat GetTxOut() const { return fPxOut / fPzOut; }
   litfloat GetTyOut() const { return fPyOut / fPzOut; }
   litfloat GetPOut() const { return std::sqrt(fPxOut * fPxOut + fPyOut * fPyOut + fPzOut * fPzOut); }
   litfloat GetQpOut() const {
      litfloat p = GetPOut();
      litfloat res = (p != 0.) ? fQ / p : 0.;
      return res;
   }
   int GetRefId() const { return fRefId; }
   int GetStationId() const { return fStationId; }



   /* Setters */
   void SetXIn(litfloat x) { fXIn = x; }
   void SetYIn(litfloat y) { fYIn = y; }
   void SetZIn(litfloat z) { fZIn = z; }
   void SetPxIn(litfloat px) { fPxIn = px; }
   void SetPyIn(litfloat py) { fPyIn = py; }
   void SetPzIn(litfloat pz) { fPzIn = pz; }
   void SetXOut(litfloat x) { fXOut = x; }
   void SetYOut(litfloat y) { fYOut = y; }
   void SetZOut(litfloat z) { fZOut = z; }
   void SetPxOut(litfloat px) { fPxOut = px; }
   void SetPyOut(litfloat py) { fPyOut = py; }
   void SetPzOut(litfloat pz) { fPzOut = pz; }
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
   litfloat fXIn, fYIn, fZIn; // Space coordinates on entrance plane [cm].
   litfloat fPxIn, fPyIn, fPzIn; // Momentum components on entrance plane. [GeV/c]
   litfloat fXOut, fYOut, fZOut; // Space coordinates on exit plane [cm].
   litfloat fPxOut, fPyOut, fPzOut; // Momentum components on exit plane. [GeV/c]
   int fRefId; // Reference index.
   int fStationId; // Station index.
   litfloat fQ; // +/-1 - negative or positive charge
};

#endif /* CBMLITMCPOINT_H_ */
