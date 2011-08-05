/** CbmLitReconstructionQa.h
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2007
 * @version 3.0
 **
 ** Task for global track reconstruction performance calculation.
 ** Produces efficiency histograms. Calculates different statistics.
 **/

#ifndef CBMLITRECONSTRUCTIONQA_H_
#define CBMLITRECONSTRUCTIONQA_H_

#include "FairTask.h"
#include "CbmDetectorList.h"


class CbmLitReconstructionQaImpl;

class CbmLitReconstructionQa : public FairTask
{
public:
   /* Constructor */
   CbmLitReconstructionQa();

   /* Destructor */
   virtual ~CbmLitReconstructionQa();

   /* Derived from FairTask. Executed before starting event-by-event execution. */
   virtual InitStatus Init();

   /* Derived from FairTask. Executed on each event.
     * @param opt Options */
   virtual void Exec(
      Option_t* opt);

   /* Sets the minimum number of MC points in STS.
    * @param minNofPointsSts Minimum number of MC points in STS. */
   void SetMinNofPointsSts(Int_t minNofPointsSts);

   /* Sets the minimum number of MC points in TRD.
     * @param minNofPointsTrd Minimum number of MC points in TRD. */
   void SetMinNofPointsTrd(Int_t minNofPointsTrd);

   /* Sets the minimum number of MC points in MUCH.
    * @param minNofPointsMuch Minimum number of MC points in MUCH. */
   void SetMinNofPointsMuch(Int_t minNofPointsMuch);

   /* Sets the minimum number of MC points in TOF.
    * @param minNofPointsTof Minimum number of MC points in TOF. */
   void SetMinNofPointsTof(Int_t minNofPointsTof);

   /* Sets the quota value, which is true/all hits for track to
    * be considered correctly reconstructed.
    * @param quota Quota value. */
   void SetQuota(Double_t quota);

   /* Sets the minimum number of hits in MC RICH ring.
    * @param minNofHits Minimum number of hits in MC RICH ring. */
   void SetMinNofHitsRich(Int_t minNofHits);

   /* Sets the quota value for RICH
    * @param quota Quota value. */
   void SetQuotaRich(Double_t quota);

   /* Sets the minimum number of hits in TRD track.
     * @param minNofPointsTrd Minimum number of hits in TRD. */
   void SetMinNofHitsTrd(Int_t minNofHitsTrd);

   /* Sets the minimum number of hits in MUCH track.
    * @param minNofHitsMuch Minimum number of hits in MUCH track. */
   void SetMinNofHitsMuch(Int_t minNofHitsMuch);

   /* Sets the verbose level for printout.
    * @param verbose Verbose value. */
   void SetVerbose(Int_t verbose);

   /* Sets the output directory for images.
    * @param dir Directory name. */
   void SetOutputDir(const std::string& dir);

   /* Sets the output json file name.
    * @param dir path. */
   void SetOutputJsonFileName(const std::string& path);

   /* Sets momentum range for efficiency calculation */
   void SetMomentumRange(Double_t minMom, Double_t maxMom);

   /* Set number of bins for efficiency v. momentum histograms */
   void SetNofBinsMom(Int_t nofBins);

   /* Implement functionality of drawing histograms in the macro
    * from the specified file
    * @param fileName Name of the file */
   void DrawHistosFromFile(const std::string& fileName);

   /* Explicitly sets detectors in the setup
    * detId Id of the detector kMVD, kSTS...
    * isDet true if detector is in the setup */
   void SetDetectorPresence(
		   DetectorId detId,
		   bool isDet);

   /* Set explicitly electron setup of the detector
    * isElectronSetup true if electron setup*/
   void SetIsElectronSetup(bool isElectronSetup);

   static void PrintLatexTable();

private:
   /* Derived from FairTask. Executed after all events are processed. */
   virtual void Finish();

   CbmLitReconstructionQaImpl* fImpl;


   ClassDef(CbmLitReconstructionQa, 1);
};

#endif /* CBMLITRECONSTRUCTIONQA_H_ */
