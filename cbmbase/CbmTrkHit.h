// -------------------------------------------------------------------------
// -----                     CbmTrkHit header file                     -----
// -----                 Created 16/11/07  by V. Friese                -----
// -------------------------------------------------------------------------

#ifndef CBMTRKHIT_H
#define CBMTRKHIT_H


#include "CbmHit.h"


/**
 ** Abstract base class for hits used for tracking in CBM.
 ** Derives from CbmHit. 
 ** Additional member is the covariance of x and y.
 ** Derived classes have to implement the pure virtual method GetStationNr()
 **
 **@author V.Friese <v.friese@gsi.de>
 **@since 16/11/07
 */

class CbmTrkHit : public CbmHit 
{

 public:

  /** Default constructor **/
  CbmTrkHit();


  /** Constructor with hit parameters 
  *@param detId  Unique detector identifier
  *@param pos    Position vector [cm]
  *@param dpos   Error of position vector [cm]
  *@param covXY  Covariance of x and y [cm**2]
  *@param index  Reference index
  **/
  CbmTrkHit(Int_t detID, TVector3& pos, TVector3& dpos, 
	    Double_t covXY, Int_t index);


  /** Destructor **/
  virtual ~CbmTrkHit();


  /** Accessors **/
  Double_t GetCovXY() const { return fCovXY; }
  virtual Int_t GetStationNr() const = 0;


  /** Output to screen **/
  virtual void Print(const Option_t* opt = 0) const;



 protected:

  Double32_t fCovXY;          // Covariance of x and y coordinates



ClassDef(CbmTrkHit,1);

};



#endif
