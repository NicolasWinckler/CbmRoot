/** CbmTrdClusterFinder.h
 *@author Florian Uhlig <f.uhlig@gsi.de>
 **
 ** Task to find digis/pads which are not separated but
 ** touch each other. Create as an output an array of
 ** the digis belonging to the cluster.
 ** First sort the digis according to the unique sector 
 ** number. This will result in as many arrays as sectors
 ** for one detector module.
 ** Then look for each module in all sectors for clusters.
 ** If a cluster was found at the sector
 ** boundaries check if there is another cluster in the
 ** next sector which has an overlap with these cluster.
 ** If there is an overlap than merge the two clusters.
 **
 **/

#ifndef CBMTRDCLUSTERFINDER_H
#define CBMTRDCLUSTERFINDER_H 

#include "FairTask.h"
#include "CbmTrdDetectorId.h"
#include <map>
#include <set>

class CbmTrdDigiPar;
class CbmTrdModule;
class TClonesArray;

class CbmTrdClusterFinder : public FairTask
{
  
 public:
  
  /**
   * Default constructor.
   */
  CbmTrdClusterFinder();
  
  /**
   * Default destructor.
   */
  ~CbmTrdClusterFinder();
 
  /** Initialisation **/
  virtual InitStatus ReInit();
  virtual InitStatus Init();
  virtual void SetParContainers();
  
  /** Executed task **/
  virtual void Exec(Option_t * option);
  
  /** Finish task **/
  virtual void Finish();

  /** Set simple clustering on/off **/
  void SetSimpleClustering(Bool_t use) {fUseSimpleClustering=use;}

 private:

  void SortDigis();
  void RealClustering();
  void SimpleClustering();

  Bool_t fUseSimpleClustering; //! Use simple or real clustering

  TClonesArray*     fDigis;       /** Input array of CbmTrdDigi **/
  TClonesArray*     fClusters;    /** Output array of CbmTrdCluster **/

  CbmTrdDigiPar *fDigiPar;   //!
  CbmTrdModule  *fModuleInfo; //!
  
  CbmTrdDetectorId fTrdId; //!
  
  std::map<Int_t, std::set<Int_t> > fDigiMap;//! /** sector digis **/
  std::map<Int_t, std::set<Int_t> > fModuleMap;//! /** sector id per module **/
  
  ClassDef(CbmTrdClusterFinder,1);
  
};
#endif
