/** @file CbmMCBuffer.cxx
 ** @author Volker Friese <v.friese@gsi.de>
 ** @date 8 February 2012
 **/

#include <iomanip>

#include "TMath.h"

#include "FairLogger.h"

#include "CbmMCBuffer.h"



using namespace std;


CbmMCBuffer* CbmMCBuffer::fgInstance = 0;



// -----   Default constructor   ---------------------------------------------
CbmMCBuffer::CbmMCBuffer() 
  : fMvdBuffer("MVD"),
    fStsBuffer("STS"),
    fRichBuffer("RICH"),
    fMuchBuffer("MUCH"),
    fTrdBuffer("TRD)"),
    fTofBuffer("TOF"),
    fEcalBuffer("ECAL"),
    fPsdBuffer("PSD"),
    fTime(0.),
    fEventId(0),
    fEndOfRun(kFALSE)
{ 
}
// ---------------------------------------------------------------------------



// -----   Destructor   ------------------------------------------------------
CbmMCBuffer::~CbmMCBuffer() { }
// ---------------------------------------------------------------------------



// -----   Finish   ----------------------------------------------------------
void CbmMCBuffer::Clear() {
  fMvdBuffer.Clear();
  fStsBuffer.Clear();
  fRichBuffer.Clear();
  fMuchBuffer.Clear();
  fTrdBuffer.Clear();
  fTofBuffer.Clear();
  fEcalBuffer.Clear();
  fPsdBuffer.Clear();
}
// ---------------------------------------------------------------------------
    


// -----   Fill buffer   -----------------------------------------------------
Int_t CbmMCBuffer::Fill(TClonesArray* points, DetectorId det,
                        Int_t eventId, Double_t eventTime) {
  Int_t iDet = det;
  return Fill(points, iDet, eventId, eventTime);
}
// ---------------------------------------------------------------------------



// -----   Fill buffer   -----------------------------------------------------
Int_t CbmMCBuffer::Fill(TClonesArray* points, Int_t det,
                        Int_t eventId, Double_t eventTime) {

  fTime    = eventTime;
  fEventId = eventId;

  Int_t nPoints = 0;
  if ( points ) {
    switch (det) {
    case kMVD:  nPoints = fMvdBuffer.Fill(points, eventTime, eventId);  break;
    case kSTS:  nPoints = fStsBuffer.Fill(points, eventTime, eventId);  break;
    case kRICH: nPoints = fRichBuffer.Fill(points, eventTime, eventId); break;
    case kMUCH: nPoints = fMuchBuffer.Fill(points, eventTime, eventId); break;
    case kTRD:  nPoints = fTrdBuffer.Fill(points, eventTime, eventId);  break;
    case kTOF:  nPoints = fTofBuffer.Fill(points, eventTime, eventId);  break;
    case kECAL: nPoints = fEcalBuffer.Fill(points, eventTime, eventId); break;
    case kPSD:  nPoints = fPsdBuffer.Fill(points, eventTime, eventId);  break;
    default:    nPoints = 0; break;
    }
  }

  return nPoints;
}
// ---------------------------------------------------------------------------



// -----   Get time of last data   ------------------------------------------
Double_t CbmMCBuffer::GetMaxTime() const {
  Double_t tMax = fMvdBuffer.GetMaxTime();
  tMax = TMath::Max( tMax, fStsBuffer.GetMaxTime() );
  tMax = TMath::Max( tMax, fRichBuffer.GetMaxTime() );
  tMax = TMath::Max( tMax, fMuchBuffer.GetMaxTime() );
  tMax = TMath::Max( tMax, fTrdBuffer.GetMaxTime() );
  tMax = TMath::Max( tMax, fTofBuffer.GetMaxTime() );
  tMax = TMath::Max( tMax, fEcalBuffer.GetMaxTime() );
  tMax = TMath::Max( tMax, fPsdBuffer.GetMaxTime() );
  return tMax;
}
// ---------------------------------------------------------------------------



// -----   Get time of first data   ------------------------------------------
Double_t CbmMCBuffer::GetMinTime() const {
  Double_t tMin = GetMaxTime();
  if ( fMvdBuffer.GetMinTime() > 0.)
    tMin = TMath::Min( tMin, fMvdBuffer.GetMinTime() );
  if ( fStsBuffer.GetMinTime() > 0.)
    tMin = TMath::Min( tMin, fStsBuffer.GetMinTime() );
  if ( fRichBuffer.GetMinTime() > 0.)
    tMin = TMath::Min( tMin, fRichBuffer.GetMinTime() );
  if ( fMuchBuffer.GetMinTime() > 0.)
    tMin = TMath::Min( tMin, fMuchBuffer.GetMinTime() );
  if ( fTrdBuffer.GetMinTime() > 0.)
    tMin = TMath::Min( tMin, fTrdBuffer.GetMinTime() );
  if ( fTofBuffer.GetMinTime() > 0.)
    tMin = TMath::Min( tMin, fTofBuffer.GetMinTime() );
  if ( fEcalBuffer.GetMinTime() > 0.)
    tMin = TMath::Min( tMin, fEcalBuffer.GetMinTime() );
  if ( fPsdBuffer.GetMinTime() > 0.)
    tMin = TMath::Min( tMin, fPsdBuffer.GetMinTime() );
  return tMin;
}
// ---------------------------------------------------------------------------



// -----   Get next point   --------------------------------------------------
const FairMCPoint* CbmMCBuffer::GetNextPoint(DetectorId det) {

  const FairMCPoint* nextPoint = NULL;

  if ( ! fEndOfRun ) {
    switch (det) {
      case kMVD:  nextPoint = fMvdBuffer.GetNextPoint(fTime);  break;
      case kSTS:  nextPoint = fStsBuffer.GetNextPoint(fTime);  break;
      case kRICH: nextPoint = fRichBuffer.GetNextPoint(fTime); break;
      case kMUCH: nextPoint = fMuchBuffer.GetNextPoint(fTime); break;
      case kTRD:  nextPoint = fTrdBuffer.GetNextPoint(fTime);  break;
      case kTOF:  nextPoint = fTofBuffer.GetNextPoint(fTime);  break;
      case kECAL: nextPoint = fEcalBuffer.GetNextPoint(fTime); break;
      case kPSD:  nextPoint = fPsdBuffer.GetNextPoint(fTime);  break;
      default:    nextPoint = NULL; break;
    }
  }
  else {
    switch (det) {
      case kMVD:  nextPoint = fMvdBuffer.GetNextPoint();  break;
      case kSTS:  nextPoint = fStsBuffer.GetNextPoint();  break;
      case kRICH: nextPoint = fRichBuffer.GetNextPoint(); break;
      case kMUCH: nextPoint = fMuchBuffer.GetNextPoint(); break;
      case kTRD:  nextPoint = fTrdBuffer.GetNextPoint();  break;
      case kTOF:  nextPoint = fTofBuffer.GetNextPoint();  break;
      case kECAL: nextPoint = fEcalBuffer.GetNextPoint(); break;
      case kPSD:  nextPoint = fPsdBuffer.GetNextPoint();  break;
      default:    nextPoint = NULL; break;
    }
  }

  return nextPoint;
}
// ---------------------------------------------------------------------------



// -----   Get number of points   ---------------------------------------------
Int_t CbmMCBuffer::GetNofEntries() const {
  Int_t nEntries = fMvdBuffer.GetNofEntries();
  nEntries += fStsBuffer.GetNofEntries();
  nEntries += fRichBuffer.GetNofEntries();
  nEntries += fMuchBuffer.GetNofEntries();
  nEntries += fTrdBuffer.GetNofEntries();
  nEntries += fTofBuffer.GetNofEntries();
  nEntries += fEcalBuffer.GetNofEntries();
  nEntries += fPsdBuffer.GetNofEntries();
  return nEntries;
}
// ---------------------------------------------------------------------------



// -----   Get buffer size   -------------------------------------------------
Double_t CbmMCBuffer::GetSize() const {

  Double_t size = 0.;
  size += fMvdBuffer.GetSize();
  size += fStsBuffer.GetSize();
  size += fRichBuffer.GetSize();
  size += fMuchBuffer.GetSize();
  size += fTrdBuffer.GetSize();
  size += fTofBuffer.GetSize();
  size += fEcalBuffer.GetSize();
  size += fPsdBuffer.GetSize();

  return size;
}
// ---------------------------------------------------------------------------



// -----   Instance   --------------------------------------------------------
CbmMCBuffer* CbmMCBuffer::Instance() {
  if ( ! fgInstance ) fgInstance = new CbmMCBuffer();
  return fgInstance;
}
// ---------------------------------------------------------------------------
    


// -----   Print   -----------------------------------------------------------
void CbmMCBuffer::Print(const char* option) const {

  LOG(INFO) << "MCBuffer: Last event " << fEventId << " at "
            << fixed << setprecision(3) << fTime << " ns, "
            << GetNofEntries() << " points from " << GetMinTime()
            << " ns to " << GetMaxTime() << " ns, size " << GetSize()
            << " MB" << FairLogger::endl;

  if ( fMvdBuffer.GetSize() )  fMvdBuffer.Print();
  if ( fStsBuffer.GetSize() )  fStsBuffer.Print();
  if ( fRichBuffer.GetSize() ) fRichBuffer.Print();
  if ( fMuchBuffer.GetSize() ) fMuchBuffer.Print();
  if ( fTrdBuffer.GetSize() )  fTrdBuffer.Print();
  if ( fTofBuffer.GetSize() )  fTofBuffer.Print();
  if ( fEcalBuffer.GetSize() ) fEcalBuffer.Print();
  if ( fPsdBuffer.GetSize() )  fPsdBuffer.Print();

}
// ---------------------------------------------------------------------------





