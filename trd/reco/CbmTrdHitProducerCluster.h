#ifndef CBMTRDHITPRODUCERCLUSTER_H
#define CBMTRDHITPRODUCERCLUSTER_H 

#include "FairTask.h"

class CbmTrdDigiPar;
class TClonesArray;

class CbmTrdHitProducerCluster : public FairTask
{
public:

   CbmTrdHitProducerCluster();

   virtual ~CbmTrdHitProducerCluster();

   virtual InitStatus Init();

   virtual void SetParContainers();

   virtual void Exec(Option_t * option);

private:

   void CenterOfGravity(
         Int_t clusterId);

   TClonesArray* fDigis; /** Input array of CbmTrdDigi **/
   TClonesArray* fClusters; /** Input array of CbmTrdCluster **/
   TClonesArray* fHits; /** Output array of CbmTrdHit **/

   CbmTrdDigiPar* fDigiPar;

   CbmTrdHitProducerCluster(const CbmTrdHitProducerCluster&);
   CbmTrdHitProducerCluster& operator=(const CbmTrdHitProducerCluster&);

   ClassDef(CbmTrdHitProducerCluster,2);
};
#endif
