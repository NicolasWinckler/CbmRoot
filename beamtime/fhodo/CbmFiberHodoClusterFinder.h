/** CbmFiberHodoClusterFinder.h
 *@author Florian Uhlig <f.uhlig@gsi.de>
 **
 ** Task to find neighboring fibres which are above
 ** the base line.
 ** Create as an output an array of
 ** the digis belonging to the cluster.
 **/

#ifndef CBMFIBERHODOCLUSTERFINDER_H
#define CBMFIBERHODOCLUSTERFINDER_H 

#include "FairTask.h"
#include "CbmFiberHodoAddress.h"
#include "CbmFiberHodoDigi.h"

#include <map>
#include <set>

//don't know why forward declaration doesn't work here
//class TClonesArray;
#include "TClonesArray.h"



struct classcomp {
  bool operator() (const CbmFiberHodoDigi* lhs, const CbmFiberHodoDigi* rhs) const
    {return CbmFiberHodoAddress::GetStripId(lhs->GetAddress()) < CbmFiberHodoAddress::GetStripId(rhs->GetAddress());}
};
    
class CbmFiberHodoClusterFinder : public FairTask
{
  
 public:
  
  /**
   * Default constructor.
   */
  CbmFiberHodoClusterFinder();
  
  /**
   * Default destructor.
   */
  ~CbmFiberHodoClusterFinder();
 
  /** Initialisation **/
  virtual InitStatus ReInit();
  virtual InitStatus Init();
  virtual void SetParContainers();
  
  /** Executed task **/
  virtual void Exec(Option_t * option);
  
  /** Finish task **/
  virtual void Finish();

 private:

  TClonesArray*     fDigis;       /** Input array of CbmFiberHodoDigi **/
  TClonesArray*     fClusters;    /** Output array of CbmFiberHodoCluster **/

  std::map<Int_t, std::set<CbmFiberHodoDigi*, classcomp> > fDigiMap;   /** digis per hodo layer **/ 

  CbmFiberHodoClusterFinder(const CbmFiberHodoClusterFinder&);
  CbmFiberHodoClusterFinder& operator=(const CbmFiberHodoClusterFinder&);

  ClassDef(CbmFiberHodoClusterFinder,1);
  
};
#endif
