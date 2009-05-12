/** CbmMuchFindHitsAdvancedGem.h
 *@author Mikhail Ryzhinskiy <m.ryzhinskiy@gsi.de>
 *@since 31.01.08
 *@version 1.0
 **
 ** CBM task class for finding clusters in the MUCH
 ** Task level RECO
 ** Produces objects of type CbmMuchCluster.
 **/

#ifndef CBMMUCHFINDCLUSTERS_H
#define CBMMUCHFINDCLUSTERS_H 1


#include "TStopwatch.h"
#include "TClonesArray.h"
#include "TFile.h"
#include "CbmMuchGeoScheme.h"
#include "FairTask.h"

#include <map>
#include <vector>
#include <set>

using std::map;
using std::pair;
using std::vector;
using std::set;

class CbmMuchPad;
class CbmMuchCluster;

class CbmMuchFindHitsAdvancedGem : public FairTask
{

 public :

  /** Default constructor. */
  CbmMuchFindHitsAdvancedGem();

  /**
   * Standard constructor.
   * @param iVerbose   Verbosity level.
   */
  CbmMuchFindHitsAdvancedGem(Int_t iVerbose);

  /**
   * Constructor with task name.
   * @param name      Task name.
   * @param digiFile  Input digitization scheme file.
   * @param iVerbose  Verbosity level.
   */
  CbmMuchFindHitsAdvancedGem(const char* name, const char* digiFileName, Int_t iVerbose);

  /** Destructor. */
  virtual ~CbmMuchFindHitsAdvancedGem();

  /** Execution. */
  virtual void Exec(Option_t* opt);

 private:

  CbmMuchGeoScheme*                fGeoScheme;         // Geometry scheme
  TFile*                           fDigiFile;          // Digitization file
  TClonesArray*                    fHits;              // Output array of CbmMuchHit
  TClonesArray*                    fDigis;             // Input array of CbmMuchDigi
  TClonesArray*                    fDigiMatches;       // Input array of CbmMuchDigiMatch
  TClonesArray*                    fPrimaryClusters;   // Output array of primary CbmMuchCluster objects
  map<pair<Int_t, Int_t>, Int_t>   fChannelDigiMap;    // Correspondence between unique channel id and digi index
  set<Int_t>                       fSelectedDigis;     // Digis already included in clusters
  TStopwatch                       fTimer;             // Timer

  /** Get parameter containers. */
  virtual void SetParContainers();
  /** Initialization. */
  virtual InitStatus Init();
  /** Reinitialization. */
  virtual InitStatus ReInit();


  /** Fills fPrimaryClusters array with found clusters. **/
  void FindClusters();

  /** Creates correspondence between channel number and digi index.
   *  Fills fChannelDigiMap. **/
  void FillChannelDigiMap();

  /**  **/
  void CreateCluster(Int_t iDigi, vector<Int_t> &digiIndices, Int_t &sumCharge, Int_t &qMax, Int_t qThreshold = 0);

  /** **/
  void SetDigiClusterMatch(vector<Int_t> digiIndices, Int_t clusterIndex);

  void ApplyThreshold(CbmMuchCluster* cluster, vector<CbmMuchCluster*> &newClusters);
  /** **/
  CbmMuchPad* GetPadByDigi(Int_t digiIndex, Int_t &charge);

  ClassDef(CbmMuchFindHitsAdvancedGem,1);
};

#endif
