/**
 * \file CbmLitRadLengthQa.h
 * \brief FairTask for estimation of radiation length in the detector.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2013
 */

#ifndef CBMLITRADLENGTHQA_H_
#define CBMLITRADLENGTHQA_H_

#include "FairTask.h"
#include <string>
#include <map>
using std::string;
using std::map;
class CbmHistManager;
class TClonesArray;

class CbmLitRadLengthQa : public FairTask
{
public:
   /**
    * \brief Constructor.
    */
   CbmLitRadLengthQa();

   /**
    * \brief Destructor.
    */
   virtual ~CbmLitRadLengthQa();

   /**
     * \brief Derived from FairTask.
     */
    virtual InitStatus Init();

    /**
     * \brief Derived from FairTask.
     */
    virtual void Exec(
       Option_t* opt);

    /**
     * \brief Derived from FairTask.
     */
    virtual void Finish();

    /** Setters */
    void SetOutputDir(const string& outputDir) { fOutputDir = outputDir; }

private:
    /**
     * \brief Read data branches.
     */
    void ReadDataBranches();

    /**
     * \brief Create histograms.
     */
    void CreateHistograms();

    /**
     * \brief Exec total radiation thickness calculation.
     */
    void ExecTotal();

    /**
     * \brief Exec specific to TRD radiation thickness calculation.
     */
    void ExecTrd();

    void FillHistosTrd(
          const map<Int_t, map<Int_t, map<Int_t, Double_t> > >& map,
          const string& histName,
          Double_t x,
          Double_t y);

    /**
     * \brief Draw histograms.
     */
    void Draw();

    /**
     * \brief Draw TProfile2D by pattern name.
     */
    void DrawP2ByPattern(
          const string& pattern);

    /**
     * \brief Draw TH1 by pattern name.
     */
    void DrawH1ByPattern(
          const string& pattern);

    CbmHistManager* fHM; // Histogram manager
    string fOutputDir; // Output directory for results

    // Pointers to data arrays
    TClonesArray* fRadLen; // RadLen array

    ClassDef(CbmLitRadLengthQa, 1);
};

#endif /* CBMLITRADLENGTHQA_H_ */
