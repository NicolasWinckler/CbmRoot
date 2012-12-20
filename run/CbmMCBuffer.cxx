/** @file CbmMCBuffer.cxx
 ** @author Volker Friese <v.friese@gsi.de>
 ** @date 8 February 2012
 **/


#include "FairLogger.h"

#include "CbmMCBuffer.h"



using namespace std;


CbmMCBuffer* CbmMCBuffer::fgInstance = 0;



// -----   Default constructor   ---------------------------------------------
CbmMCBuffer::CbmMCBuffer() 
  : fTime(0.),
    fEventId(0)
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
    default:    nPoints = 0;
    }
  }
  return nPoints;

}
// ---------------------------------------------------------------------------



// -----   Get next point   --------------------------------------------------
const FairMCPoint* CbmMCBuffer::GetNextPoint(DetectorId det) {

  const FairMCPoint* nextPoint = NULL;

  switch (det) {
  case kMVD:  nextPoint = fMvdBuffer.GetNextPoint(fTime);  break;
  case kSTS:  nextPoint = fStsBuffer.GetNextPoint(fTime);  break;
  case kRICH: nextPoint = fRichBuffer.GetNextPoint(fTime); break;
  case kMUCH: nextPoint = fMuchBuffer.GetNextPoint(fTime); break;
  case kTRD:  nextPoint = fTrdBuffer.GetNextPoint(fTime);  break;
  case kTOF:  nextPoint = fTofBuffer.GetNextPoint(fTime);  break;
  case kECAL: nextPoint = fEcalBuffer.GetNextPoint(fTime); break;
  case kPSD:  nextPoint = fPsdBuffer.GetNextPoint(fTime);  break;
  default:    nextPoint = NULL;
  }

  return nextPoint;
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
void CbmMCBuffer::Print() {

  FairLogger* logger = FairLogger::GetLogger();

  logger->Info(MESSAGE_ORIGIN, "Status of MCBuffer: ");
  logger->Info(MESSAGE_ORIGIN, "Current time: %f ns", fTime);
  fMvdBuffer.Print(kMVD, logger);
  fStsBuffer.Print(kSTS, logger);
  fRichBuffer.Print(kRICH, logger);
  fMuchBuffer.Print(kMUCH, logger);
  fTrdBuffer.Print(kTRD, logger);
  fTofBuffer.Print(kTOF, logger);
  fEcalBuffer.Print(kECAL, logger);
  fPsdBuffer.Print(kPSD, logger);
  logger->Info(MESSAGE_ORIGIN, "Total buffer size: %8.2f MB", GetSize());

}
// ---------------------------------------------------------------------------



