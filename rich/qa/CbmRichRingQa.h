/** CbmRichRingQa.h
 * @author Semen Lebedev <s.lebedev@gsi.de>
 * @since 2005
 * @version 3.0
 **/

#ifndef CBMRICHRINGQA_H
#define CBMRICHRINGQA_H

#include "CbmRichRing.h"
#include "FairTask.h"

class TH1D;
class CbmRichRing;
class CbmRichRingFitterCOP;
class CbmRichRingSelectImpl;

#include <map>
#include <fstream>


class CbmRichRingQa : public FairTask
{
private:
    TClonesArray* fRings; // Array of CbmRichRings
    TClonesArray* fPoints; // Array of FairMCPoints
    TClonesArray* fTracks; // Array of CbmMCTracks
    TClonesArray* fHits; // Array of CbmRichHits
    TClonesArray* fMatches; // Array of CbmRichRingMatch
    TClonesArray* fProj; // Array of CbmRichRingMatch
    TClonesArray* fTrackMatch; //Array of STSTrackMatch
    TClonesArray* gTrackArray; // Array of Global Tracks

    std::map<Int_t, Int_t> fRingMap;
    Int_t fEventNumber;
    Int_t fNofHitsInRingCut;
    Double_t fQuota;

public:

    /** Default constructor **/
    CbmRichRingQa();

    /** Standard Constructor with name, title, and verbosity level
        *@param verbose      verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
    */
    CbmRichRingQa(
          const char *name,
          const char *title,
          Int_t verbose);

    /** Destructor **/
    virtual ~CbmRichRingQa();

    /** Initialization of the task **/
    virtual InitStatus Init();

    /** Executed task **/
    virtual void Exec(
          Option_t* option);

    /** Finish task **/
    virtual void FinishTask();

    void EfficiencyCalc();

    void DiffFakeTrueCircle();

    Double_t GetStsMomentum(
          CbmRichRing * ring);

    Int_t fNofAccElRings;
    Int_t fNofRecElRings;
    Int_t fNofCloneRings;
    Int_t fNofFakeRings;

/// Difference Fake and True rings histograms BEGIN
    TH1D* fh_FakeNofHits;
    TH1D* fh_TrueElNofHits;

    TH1D* fh_FakeAngle;
    TH1D* fh_TrueElAngle;

    TH1D* fh_FakeNofHitsOnRing;
    TH1D* fh_TrueElNofHitsOnRing;

    TH1D* fh_FakeChi2;
    TH1D* fh_TrueElChi2;

    TH1D* fh_FakeRadPos;
    TH1D* fh_TrueElRadPos;

    TH1D* fh_FakeRadius;
    TH1D* fh_TrueElRadius;
/// Difference Fake and True rings histograms END

    std::ofstream  foutFakeAndTrue;

    CbmRichRingFitterCOP* fFitCOP;
    CbmRichRingSelectImpl* fSelectImpl;

    ClassDef(CbmRichRingQa,1)

};

#endif
