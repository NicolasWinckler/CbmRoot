/* $Id: CbmRichRing.h,v 1.8 2006/09/13 14:51:43 hoehne Exp $*/

/* History of CVS commits:
 *
 * $Log: CbmRichRing.h,v $
 * Revision 1.8  2006/09/13 14:51:43  hoehne
 * two variables (Selection2D, SelectionNN) added in which values between 0 and 1 are stored for fake rejection
 * ReconstructedFlag removed
 *
 * Revision 1.7  2006/08/11 14:03:57  hoehne
 * move SetUncertainty and GetUncertainty to SetChi2 and GetChi2
 *
 * Revision 1.6  2006/07/12 06:27:54  hoehne
 * new functions: SetDistance and GetDistance added which set/ give the distance between ring center and track attached to this
 * ring
 *
 * Revision 1.5  2006/02/23 11:24:10  hoehne
 * RecFlag added (Simeon Lebedev)
 *
 * Revision 1.4  2006/01/23 11:40:13  hoehne
 * MCMotherID added
 *
 * Revision 1.3  2006/01/19 10:36:37  hoehne
 * restructured according to new RingFinder Class
 * array added which contains hits attached to a ring
 *
 *
 */


// -------------------------------------------------------------------------
// -----                     CbmRichRing header file                  -----
// -----   Created 05/07/04  by A. Soloviev <solovjev@cv.jinr.ru>     -----
// -------------------------------------------------------------------------



/**  CbmRichRing.h
 *@author A. Soloviev
 **
 ** Rings in Rich Photodetector
 **/


#ifndef CBM_RICH_RING_H
#define CBM_RICH_RING_H

#include "CbmRichHit.h"

#include "TObject.h"

#include <vector>

class CbmRichRing : public TObject {

public:

  /** Default constructor **/
  CbmRichRing();

  /** Constructor with arguments
   *@param x    x Position of ring center [cm]
   *@param y    y Position of ring center [cm]
   *@param r    radius of ring [cm]
   **/
  CbmRichRing ( Double_t x, Double_t y, Double_t r );

  /** Destructor **/
  virtual ~CbmRichRing();

  /** to attach the rich hit to the ring */
  void AddHit(Int_t pHit)  {fHitCollection.push_back(pHit);}
  /** remove hit from ring 
   * hitId is the index in the RICH hit array 
   * return false if hit is not found in ring*/  
  Bool_t RemoveHit(Int_t hitId);
  /** to obtain the number of hits associated to the ring */
  Int_t GetNofHits() const { return fHitCollection.size(); }
  /** to obtain the rich hit at a particular index */
  Int_t GetHit(Int_t i) {return fHitCollection[i];}
  /** to print ring parameters **/
  void Print();
  

  /** Modifiers**/
  inline void SetCenterX(Double_t x)                 {fCenterX = x;}
  inline void SetCenterY(Double_t y)                 {fCenterY = y;}
  inline void SetRadius(Double_t r)                  {fRadius = r;}
  inline void SetAaxis(Double_t a)                   {fAaxis = a;}
  inline void SetBaxis(Double_t b)                   {fBaxis = b;}
  inline void SetAaxisCor(Double_t a)                {fAaxisCor = a;}
  inline void SetBaxisCor(Double_t b)                {fBaxisCor = b;} 
         void SetXYABPhi(Double_t x, Double_t y,
                         Double_t a, Double_t b,
                         Double_t phi);
  inline void SetPhi(Double_t phi)                   {fPhi = phi;}  
  inline void SetTrackID(Int_t track)                {fTrackID = track;}
  inline void SetMCMotherID(Int_t track)             {fMCMotherID = track;}
  inline void SetDistance(Double_t d)                {fDistance = d;}
  inline void SetChi2(Double_t chi2)                 {fChi2 = chi2;}
  inline void SetRecFlag ( Int_t recflag )           {fRecFlag = recflag;}
  inline void SetAngle( Double_t angle)              {fAngle = angle;}
  inline void SetNofHitsOnRing(Int_t onring)         {fNofHitsOnRing = onring;}    
  /** either -1 and 1: -1 = fake ring, 1 = good ring (selection by 2-dimensional cuts)*/
  inline void SetSelection2D (Double_t selection2D ) {fSelection2D = selection2D;}  
  /** number between -1 and 1: -1 = fake ring, 1 = good ring (selection by neural net)*/
  inline void SetSelectionNN (Double_t selectionNN ) {fSelectionNN = selectionNN;}
  
  /** Accessors **/
  inline Double_t GetCenterX()            const {return fCenterX;}
  inline Double_t GetCenterY()            const {return fCenterY;}
  inline Double_t GetRadius()             const {return fRadius;}
  inline Double_t GetAaxis()              const {return fAaxis;}
  inline Double_t GetBaxis()              const {return fBaxis;}
  inline Double_t GetAaxisCor()           const {return fAaxisCor;}
  inline Double_t GetBaxisCor()           const {return fBaxisCor;}
  inline Double_t GetPhi()                const {return fPhi;}  
  Double_t GetXF1() const;
  Double_t GetYF1() const;  
  Double_t GetXF2() const;
  Double_t GetYF2() const;
  
  inline Int_t GetTrackID()               const {return fTrackID;}
  inline Int_t GetMCMotherID()            const {return fMCMotherID;}
  
  inline Double_t GetSelection2D()        const {return fSelection2D;}
  inline Double_t GetSelectionNN()        const {return fSelectionNN;}
  inline Double_t GetDistance()           const {return fDistance;}
  inline Double_t GetChi2()               const {return fChi2;}
  Double_t GetRadialPosition()            const;  
  inline Double_t GetAngle()              const {return fAngle;}
  inline Int_t GetNofHitsOnRing()         const {return fNofHitsOnRing;}
  Double_t GetRadialAngle()               const;  
  inline Int_t GetRecFlag()               const {return fRecFlag;}
  
  
private:

  std::vector<Int_t> fHitCollection; /** STL container to hold the hits */

protected: 

  Double_t fCenterX;
  Double_t fCenterY;
  Double_t fRadius;
  
  Double_t fAaxis; ///major axis of ellipse
  Double_t fBaxis; ///minor axes of the ellipse
  Double_t fAaxisCor; ///major axis of ellipse after corection
  Double_t fBaxisCor; ///minor axes of the ellipse after correctiom
  
  Double_t fPhi; /// rotation angle

  Int_t fTrackID; /** ID of the matched track, set after track matching */
  Int_t fMCMotherID; /** ID of the MC track */
  
  Double_t fDistance;   /** Distance to track matched to this ring */ 
  Double_t fChi2;
  Double_t fAngle;
  Int_t fNofHitsOnRing;

  Double_t fSelection2D;  /** value for selection high quality rings */
  Double_t fSelectionNN;  /** value for selection high quality rings */

  Int_t fRecFlag;
  
  ClassDef(CbmRichRing,2)
};

#endif // CBM_RICH_RING_H
