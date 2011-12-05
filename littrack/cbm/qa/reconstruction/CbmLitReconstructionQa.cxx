/** CbmLitReconstructionQa.cxx
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2007
 * @version 3.0
 **/

#include "qa/reconstruction/CbmLitReconstructionQa.h"
#include "qa/reconstruction/CbmLitReconstructionQaImpl.h"
#include "qa/reconstruction/CbmLitQaReconstructionReportStudy.h"
#include <fstream>

#include "TSystem.h"

#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

CbmLitReconstructionQa::CbmLitReconstructionQa():
   FairTask("LitReconstructionQA", 1)
{
   fImpl = new CbmLitReconstructionQaImpl();
}

CbmLitReconstructionQa::~CbmLitReconstructionQa()
{
   if (fImpl) delete fImpl;
}

InitStatus CbmLitReconstructionQa::Init()
{
   return fImpl->Init();
}

void CbmLitReconstructionQa::Exec(
   Option_t* opt)
{
   fImpl->Exec(opt);
}

void CbmLitReconstructionQa::Finish()
{
   fImpl->Finish();
}

void CbmLitReconstructionQa::SetMinNofPointsSts(Int_t minNofPointsSts)
{
   fImpl->SetMinNofPointsSts(minNofPointsSts);
}

void CbmLitReconstructionQa::SetMinNofPointsTrd(Int_t minNofPointsTrd)
{
   fImpl->SetMinNofPointsTrd(minNofPointsTrd);
}

void CbmLitReconstructionQa::SetMinNofPointsMuch(Int_t minNofPointsMuch)
{
   fImpl->SetMinNofPointsMuch(minNofPointsMuch);
}

void CbmLitReconstructionQa::SetMinNofPointsTof(Int_t minNofPointsTof)
{
   fImpl->SetMinNofPointsTof(minNofPointsTof);
}

void CbmLitReconstructionQa::SetQuota(Double_t quota)
{
   fImpl->SetQuota(quota);
}

void CbmLitReconstructionQa::SetMinNofHitsRich(Int_t minNofHits)
{
   fImpl->SetMinNofHitsRich(minNofHits);
}

void CbmLitReconstructionQa::SetQuotaRich(Double_t quota)
{
   fImpl->SetQuotaRich(quota);
}

void CbmLitReconstructionQa::SetMinNofHitsTrd(Int_t minNofHitsTrd)
{
   fImpl->SetMinNofHitsTrd(minNofHitsTrd);
}

void CbmLitReconstructionQa::SetMinNofHitsMuch(Int_t minNofHitsMuch)
{
   fImpl->SetMinNofHitsMuch(minNofHitsMuch);
}

void CbmLitReconstructionQa::SetVerbose(Int_t verbose)
{
   fImpl->SetVerbose(verbose);
}

void CbmLitReconstructionQa::SetOutputDir(const std::string& dir)
{
   fImpl->SetOutputDir(dir);
}


void CbmLitReconstructionQa::SetMomentumRange(Double_t minMom, Double_t maxMom)
{
   fImpl->SetMomentumRange(minMom, maxMom);
}

void CbmLitReconstructionQa::SetNofBinsMom(Int_t nofBins)
{
   fImpl->SetNofBinsMom(nofBins);
}

void CbmLitReconstructionQa::SetYRange(Double_t minY, Double_t maxY)
{
   fImpl->SetYRange(minY, maxY);
}

void CbmLitReconstructionQa::SetNofBinsY(Int_t nofBins)
{
   fImpl->SetNofBinsY(nofBins);
}

void CbmLitReconstructionQa::SetPtRange(Double_t minPt, Double_t maxPt)
{
   fImpl->SetPtRange(minPt, maxPt);
}

void CbmLitReconstructionQa::SetNofBinsPt(Int_t nofBins)
{
   fImpl->SetNofBinsPt(nofBins);
}

void CbmLitReconstructionQa::DrawHistosFromFile(const std::string& fileName)
{
   fImpl->DrawHistosFromFile(fileName);
}

void CbmLitReconstructionQa::SetDetectorPresence(
      DetectorId detId,
      bool isDet){
   fImpl->SetDetectorPresence(detId, isDet);
}

bool CbmLitReconstructionQa::GetDetectorPresence(
      DetectorId detId) const
{
   return fImpl->GetDetectorPresence(detId);
}

void CbmLitReconstructionQa::SetIsElectronSetup(bool isElectronSetup)
{
   fImpl->SetIsElectronSetup(isElectronSetup);
}

bool CbmLitReconstructionQa::IsElectronSetup() const
{
   return fImpl->IsElectronSetup();
}

void CbmLitReconstructionQa::SetUseConsecutivePointsInSts(Bool_t useConsecutivePointsInSts)
{
   fImpl->SetUseConsecutivePointsInSts(useConsecutivePointsInSts);
}

void CbmLitReconstructionQa::CreateReport(
      LitReportType reportType,
      const string& title,
      const vector<string>& results,
      const vector<string>& names)
{
   CbmLitQaReconstructionReportStudy report(reportType);
   report.SetIsElectronSetup(IsElectronSetup());
   report.SetDetectorPresence(kMVD, GetDetectorPresence(kMVD));
   report.SetDetectorPresence(kSTS, GetDetectorPresence(kSTS));
   report.SetDetectorPresence(kRICH, GetDetectorPresence(kRICH));
   report.SetDetectorPresence(kTRD, GetDetectorPresence(kTRD));
   report.SetDetectorPresence(kMUCH, GetDetectorPresence(kMUCH));
   report.SetDetectorPresence(kTOF, GetDetectorPresence(kTOF));

   string fileType = ".tex";
   if (reportType == kLitHtml) fileType = ".html";
   if (reportType == kLitText) fileType = ".txt";
   ofstream fout(string(fImpl->GetOutputDir() + "/rec_qa_study"+fileType).c_str());

   vector<boost::property_tree::ptree*> qa;
   vector<boost::property_tree::ptree*> check;
   qa.resize(results.size());
   check.resize(results.size());

   boost::property_tree::ptree ideal;
   string qaIdealFile = string(gSystem->Getenv("VMCWORKDIR")) + ("/littrack/cbm/qa/rec_qa_ideal.json");
   read_json(qaIdealFile.c_str(), ideal);

   for (int i = 0; i < results.size(); i++) {
      qa[i] = new boost::property_tree::ptree;
      check[i] = new boost::property_tree::ptree;
      read_json(results[i] + "/rec_qa.json", *(qa[i]));
      read_json(results[i] + "/rec_qa_check.json", *(check[i]));
   }

   report.Create(fout, names, qa, &ideal, check);

   for (int i = 0; i < results.size(); i++) {
      delete qa[i];
      delete check[i];
   }
}

ClassImp(CbmLitReconstructionQa);
