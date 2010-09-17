#ifndef CBML1RICHRINGQA_H
#define CBML1RICHRINGQA_H

#include "FairTask.h"

#include "TH1D.h"
#include "TH2D.h"

#include <map>
#include <vector>
#include <list>

class CbmL1RichRingQa : public FairTask{
  private:
    CbmL1RichRingQa(const CbmL1RichRingQa&);
    CbmL1RichRingQa operator=(const CbmL1RichRingQa&);
  struct MCRing
  {
    MCRing():MCTrackID(-1),primary(0),P(0),PDG(-1),NHits(0),Reconstructed(0),kind(0),x(0),y(0),r(0),Hits(){};
    Int_t MCTrackID;
    bool primary;
    Double_t P;
    Int_t PDG;
    Int_t NHits;
    Int_t Reconstructed;
    Int_t kind;
    Double_t x, y, r;
    std::vector<int> Hits;
  };

  struct PerfHit
  {
    Double_t x, y;
    Int_t MCTrackID;
    Int_t index;
    Bool_t found;  
    Bool_t on_ring;
  };

  TClonesArray* fRingArray;        // Array of CbmRichRings
  TClonesArray* fMCPointArray;       // Array of FairMCPoints
  TClonesArray* fMCTrackArray;       // Array of CbmMCTracks
  TClonesArray* fHitArray;         // Array of CbmRichHits

  public:

  /** Default constructor **/

  CbmL1RichRingQa(const char *name="CbmL1RichRingQa", const char *title="CbmL1RichRingQa", Int_t verbose=1);

  /** Destructor **/
  ~CbmL1RichRingQa();

  /** Initialization of the task **/
  InitStatus Init();

  /** Executed task **/
  void Exec(Option_t* option);

  /** Finish task **/
  void Finish();

  void CirFit( std::list<std::pair<Double_t,Double_t> > &P, Double_t *X, Double_t *Y, Double_t *R );

  ClassDef(CbmL1RichRingQa,1)

};

#endif
