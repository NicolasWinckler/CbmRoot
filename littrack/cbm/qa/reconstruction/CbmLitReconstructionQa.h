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

   /* Sets momentum range for efficiency calculation */
   void SetMomentumRange(Double_t minMom, Double_t maxMom);

   /* Set number of bins for efficiency v. momentum histograms */
   void SetNofBinsMom(Int_t nofBins);

   /* Sets rapidity range for efficiency calculation */
   void SetYRange(Double_t minY, Double_t maxY);

   /* Set number of bins for efficiency v. rapidity histograms */
   void SetNofBinsY(Int_t nofBins);

   /* Sets Pt range for efficiency calculation */
   void SetPtRange(Double_t minPt, Double_t maxPt);

   /* Set number of bins for efficiency v. Pt histograms */
   void SetNofBinsPt(Int_t nofBins);

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

   /**
    * \brief Return TRUE if detector presents in the setup.
    * \param[in] detId Id of the detector kMVD, kSTS...
    * \return TRUE if detector is in the setup.
    */
   bool GetDetectorPresence(
         DetectorId detId) const;

   /**
    * \brief Set explicitly electron setup of the detector.
    * \param[in] isElectronSetup true if electron setup
    */
   void SetIsElectronSetup(bool isElectronSetup);

   /**
    * \brief Return TRUE if electron setup of CBM is detected.
    * \return Return TRUE if electron setup of CBM is detected.
    */
   bool IsElectronSetup() const;

   /**
    * \brief Set if consecute MC points are used for efficiency normalization.
    * \param[in] param quota Quota value.
    */
   void SetUseConsecutivePointsInSts(Bool_t useConsecutivePointsInSts);

   static void PrintLatexTable(
         const std::vector<std::string>& files);

   void CreateStudyHTML(
         const std::string& title,
         const std::vector<std::string>& results,
         const std::vector<std::string>& names);

private:
   /* Derived from FairTask. Executed after all events are processed. */
   virtual void Finish();

   CbmLitReconstructionQaImpl* fImpl;

   ClassDef(CbmLitReconstructionQa, 1);
};

#endif /* CBMLITRECONSTRUCTIONQA_H_ */