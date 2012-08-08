/* CbmMuchFindHitsGem.h
 *
 *@author  E.Kryshen <e.kryshen@gsi.de>
 *@version 2.0
 *@since   31.07.12
 *
 *@author  M.Ryzhinskiy <m.ryzhinskiy@gsi.de>
 *@version 1.0
 *@since   11.02.08
 *
 *  Produces hits in GEM-like modules using different
 *  clustering algorithms. Clustering algorithms can be chosen
 *  by the switch SetAlgorithm(Int_t iAlgorithm)
 *  Several clustering algorithms are supported
 *  0 - one hit per fired pad
 *  1 - one hit per cluster
 *  2 - simple cluster deconvolution with charge threshold
 */

#ifndef CBMMUCHFINDHITSGEM_H
#define CBMMUCHFINDHITSGEM_H 1

#define MAXCLUSTERSIZE 40
#include "FairTask.h"
#include "TString.h"
#include <map>
#include <vector>
using std::map;
using std::pair;
using std::vector;

class CbmMuchPad;
class CbmMuchCluster;
class TClonesArray;
class CbmMuchGeoScheme;

class CbmMuchFindHitsGem: public FairTask {
  public:
    CbmMuchFindHitsGem();
    CbmMuchFindHitsGem(const char* name, const char* digiFileName,Int_t iVerbose);
    virtual ~CbmMuchFindHitsGem() {
      fDigiIndices.clear();
      fFiredPads.clear();
    }
    virtual void Exec(Option_t* opt);
    void SetAlgorithm(Int_t iAlgorithm)             { fAlgorithm = iAlgorithm;}
    void SetThresholdRatio(Double_t thresholdRatio) {fThresholdRatio = thresholdRatio; }
    void SetClusterSeparationTime(Double_t time)    { fClusterSeparationTime = time; }
  private:
    virtual InitStatus Init();
    virtual InitStatus ReInit(){ return kSUCCESS;}
    virtual void SetParContainers() {}
    virtual void FinishTask() {}
    void FindClusters();
    void CreateCluster(CbmMuchPad* pad);
    void ExecClusteringSimple(CbmMuchCluster* cluster, vector<CbmMuchCluster*> &clusters);
    void ExecClusteringPeaks(CbmMuchCluster* cluster,Int_t iCluster);
    void CreateHits(vector<CbmMuchCluster*> clusters, Int_t iCluster);
    void CreateHits(CbmMuchCluster* cluster,Int_t iCluster);
    Int_t FindLocalMaxima();
    void FlagLocalMax(Int_t i,Int_t j);
    Bool_t CreateClusterMap(CbmMuchCluster* cluster);
    TString  fDigiFile;                    // Digitization file
    Int_t    fAlgorithm;                   // Defines which algorithm to use
    Double_t fClusterSeparationTime;       // Minimum required time between two clusters
    Double_t fThresholdRatio;              // Charge threshold ratio relative to max cluster charge
    Int_t fEvent;                          // Event counter
    TClonesArray* fDigis;                  // Input array of CbmMuchDigi
    Int_t fDigiIndexMap[MAXCLUSTERSIZE][MAXCLUSTERSIZE]; //
    Int_t fClusterMap[MAXCLUSTERSIZE][MAXCLUSTERSIZE];   //
    Int_t fLocalMax[MAXCLUSTERSIZE][MAXCLUSTERSIZE];     //
    Int_t fClusterNx;
    Int_t fClusterNy;

    TClonesArray* fClusters;               // Output array of CbmMuchCluster objects
    TClonesArray* fHits;                   // Output array of CbmMuchHit
    CbmMuchGeoScheme* fGeoScheme;          // Geometry scheme
    // auxiliary maps and vectors
    vector<Int_t> fDigiIndices;            //!
    vector<CbmMuchPad*> fFiredPads;        //!
    
    ClassDef(CbmMuchFindHitsGem,1);
};

#endif
