/** CbmMuchFindClusters.h
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


#include "CbmMuchTask.h"
#include "TStopwatch.h"
#include "TClonesArray.h"
#include "TFile.h"
#include "CbmMuchGeoScheme.h"

#include <map>
#include <vector>
#include <set>

using std::map;
using std::pair;
using std::vector;
using std::set;

class CbmMuchPad;
class CbmMuchCluster;

class CbmMuchFindClusters : public CbmMuchTask
{

 public :

  /** Default constructor. */
  CbmMuchFindClusters();

  /**
   * Standard constructor.
   * @param iVerbose   Verbosity level.
   */
  CbmMuchFindClusters(Int_t iVerbose);

  /**
   * Constructor with task name.
   * @param name      Task name.
   * @param digiFile  Input digitization scheme file.
   * @param iVerbose  Verbosity level.
   */
  CbmMuchFindClusters(const char* name, TFile* digiFile, Int_t iVerbose);

  /** Destructor. */
  virtual ~CbmMuchFindClusters();

  /** Execution. */
  virtual void Exec(Option_t* opt);

  /** Get parameter containers. */
  virtual void SetParContainers();

  /** Initialization. */
  virtual InitStatus Init();

  /** Reinitialization. */
  virtual InitStatus ReInit();

 private:

  TFile*                           fDigiFile;          // Digitization file
  TClonesArray*                    fDigis;             // Input array of CbmMuchDigi
  TClonesArray*                    fDigiMatches;       // Input array of CbmMuchDigiMatch
  TClonesArray*                    fPrimaryClusters;   // Output array of primary CbmMuchCluster objects
  map<pair<Int_t, Int_t>, Int_t>   fChannelDigiMap;    // Correspondence between unique channel id and digi index
  set<Int_t>                       fSelectedDigis;     // Digis already included in clusters


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

  ClassDef(CbmMuchFindClusters,1);
};

#endif
