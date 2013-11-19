/**
 * \file CbmMuchFindHitsStraws.h
 **/


#ifndef CBMMUCHFINDHITSSTRAWS_H
#define CBMMUCHFINDHITSSTRAWS_H 1

#include "CbmMuchGeoScheme.h"
#include "FairTask.h"
#include "TString.h"

class TClonesArray;

class CbmMuchFindHitsStraws : public FairTask
{
public:

   /** Constructor with task name **/
   CbmMuchFindHitsStraws(const char* digiFileName);

   /** Destructor **/
   virtual ~CbmMuchFindHitsStraws();

   /** Execution **/
   virtual void Exec(Option_t* opt);

   /** Initialization **/
   virtual InitStatus Init();

   Int_t GetEff() const { return fEffic; }
   Int_t GetMerge() const { return fMerge; }
   Int_t GetMirror() const { return fMirror; }
   Double_t GetPhi(Int_t i) const { return fPhis[i]; }
   void SetEff(Int_t eff) { fEffic = eff; }
   void SetMerge(Int_t merge) { fMerge = merge; }
   void SetMirror(Int_t mirror) { fMirror = mirror; }
   void SetBinary() { fBinary = 1; fMirror = 0; }
   void SetPhi(Int_t i, Double_t phi) { fPhis[i] = phi; } // set doublets rotation angles (degs)
   void SetPhis(Double_t ph0, Double_t ph1, Double_t ph2) { fPhis[0] = ph0; fPhis[1] = ph1; fPhis[2] = ph2;} // set doublets rotation angles (degs)

private:

   void Effic(Double_t *diam); // apply inefficiency (currently inside straw tube walls)
   void Merge(); // merge hits inside the same straw tube
   void Mirror(); // add mirror hits (left/right ambiguity)

   CbmMuchGeoScheme*                fGeoScheme;         // Geometry scheme
   TString                          fDigiFile;          // Digitization file
   TClonesArray*                    fDigis;             // Input array of CbmMuchDigi
   TClonesArray*                    fDigiMatches;       // Input/output array of CbmMuchDigiMatch
   TClonesArray*                    fHits;              // Output array of CbmMuchHit
   Int_t                            fEffic;             // Efficiency correction flag
   Int_t                            fMerge;             // merging flag
   Int_t                            fMirror;            // mirror flag (left-right ambiguity)
   Int_t                            fBinary;            // binary flag (binary info - tube center as coord.)
   Double_t                         fPhis[3];           // rotation angles of doublets

   CbmMuchFindHitsStraws(const CbmMuchFindHitsStraws&);
   CbmMuchFindHitsStraws& operator=(const CbmMuchFindHitsStraws&);

   ClassDef(CbmMuchFindHitsStraws,1);
};

#endif
