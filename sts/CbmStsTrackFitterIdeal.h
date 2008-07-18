// -------------------------------------------------------------------------
// -----                CbmStsTrackFitterIdeal header file             -----
// -----                  Created 12/05/06  by D. Kresan               -----
// -------------------------------------------------------------------------


/** CbmStsTrackFitterIdeal
 *@author D.Kresan <D.Kresan@gsi.de>
 **/

#ifndef CBMSTSTRACKFITTERIDEAL
#define CBMSTSTRACKFITTERIDEAL

#include "CbmStsTrackFitter.h"

#include "TDatabasePDG.h"

class CbmStsPoint;
class CbmTrackParam;
class CbmStsTrack;
class TClonesArray;


class CbmStsTrackFitterIdeal : public CbmStsTrackFitter
{
 public:
  /** Default constructor **/
  CbmStsTrackFitterIdeal();

  /** Destructor **/
  virtual ~CbmStsTrackFitterIdeal();

  /** Initialisation. **/
  void Init();

  /** Fitting algorithm **/
  Int_t DoFit(CbmStsTrack* pTrack, Int_t pidHypo = 211);

  void Extrapolate( CbmStsTrack* track, Double_t z,
		   CbmTrackParam *param ) {}

  /** Set tracks parameters from the MC point **/
  void SetTrackParam(CbmStsPoint* point, CbmTrackParam* trackParam, Int_t out = 0);

private:

  /** Data branches **/
  TClonesArray* fArrayMCTrack;
  TClonesArray* fArrayStsPoint;
  TClonesArray* fArrayStsHit;

  TDatabasePDG* dbPDG;

  ClassDef(CbmStsTrackFitterIdeal,1);
};

#endif
