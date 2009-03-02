/** CbmMuchHit.h
 *@author Mikhail Ryzhinskiy <m.ryzhinskiy@gsi.de>
 *@version 1.0
 *@since 22.03.07
 **
 ** Data class for a reconstructed hit in the MUCH
 ** Data level RECO
 ** Derived from FairHit.
 ** Additional members: Channel numbers and covariance in x and y
 **/



#ifndef CBMMUCHHIT_H
#define CBMMUCHHIT_H 1



#include "TVector3.h"
#include "CbmHit.h"

#include <iostream>

class CbmMuchHit : public CbmHit
{

 public:

  /** Default constructor (not for use) **/
  CbmMuchHit();

  CbmMuchHit(Int_t detId, TVector3& pos, TVector3& dPos,
	     Double_t dxy);


  /** Standard constructor
  *@param detId     Unique detector ID
  *@param pos       Position in global c.s. [cm]
  *@param dPos      Errors of position in global c.s. [cm]
  *@param dxy       Covariance of x and y
  *@param iDigi     Digi index
  *@param times      Time since event start [ns]
  *@param dTime     Time resolution [ns]
  **/
  CbmMuchHit(Int_t detId, TVector3& pos, TVector3& dPos,
	     Double_t dxy, Int_t iDigi, Double_t* times,
	     Double_t dTime);

  /** Standard constructor (clustering)
  *@param detId     Unique detector ID
  *@param pos       Position in global c.s. [cm]
  *@param dPos      Errors of position in global c.s. [cm]
  *@param dxy       Covariance of x and y
  *@param iCluster  Parent cluster index
  **/
  CbmMuchHit(Int_t detId, TVector3& pos, TVector3& dPos,
	     Double_t dxy, Int_t iCluster);


  /** Destructor **/
  virtual ~CbmMuchHit();

  /** Screen output **/
  virtual void Print(const Option_t* opt) const;


  /** Accessors **/

  /** Gets system ID **/
  inline Int_t    GetSystemId()
    const { return ( fDetectorID & (15<<24) ) >> 24; }

  /** Gets station number within the system **/
  virtual Int_t    GetStationNr() const {
	  Int_t stationNr = ((fDetectorID & (255<<16) ) >> 16);
	  std::cout << "fDetectorId=" << fDetectorID << " staNr=" << stationNr << std::endl;
	  return ( fDetectorID & (255<<16) ) >> 16;
  }

  /** Gets sector number within the station **/
  inline Int_t    GetSectorNr()
    const { return ( fDetectorID & (32767<<1) ) >> 1; }

  /** Gets covariance in X and Y **/
  inline Double_t GetCovXY() const  { return fCovXY; }

  /** Gets index of the Digi corresponding to this hit **/
  inline Int_t    GetDigi()  const  {  return fRefIndex; }

  /** Gets time since event start **/
  inline Double_t GetTime(Int_t i)  { return fTime[i]; }
  inline Double_t* GetTimes()       { return fTime;}
  /** Gets time resolution **/
  inline Double_t GetDTime()        { return fDTime; }

  inline Int_t GetCluster() const {return fCluster;}

  void SetCluster(Int_t clus) { fCluster = clus; } //AZ

 private:

  Double_t fTime[3];    // Time since event start [ns]
  Double_t fDTime;      // Time resolution [ns]
  Int_t fCluster;       // Index of the parent cluster for the hit

  ClassDef(CbmMuchHit,1);
};


#endif
