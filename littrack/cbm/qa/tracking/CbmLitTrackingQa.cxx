/** CbmLitTrackingQa.cxx
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2007
 * @version 3.0
 **/

#include "qa/tracking/CbmLitTrackingQa.h"
#include "qa/tracking/CbmLitTrackingQaImpl.h"
#include "qa/tracking/CbmLitTrackingQaStudyReport.h"
#include <fstream>

#include "TSystem.h"

#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

CbmLitTrackingQa::CbmLitTrackingQa():
   FairTask("LitTrackingQA", 1)
{
   fImpl = new CbmLitTrackingQaImpl();
}

CbmLitTrackingQa::~CbmLitTrackingQa()
{
   if (fImpl) delete fImpl;
}

InitStatus CbmLitTrackingQa::Init()
{
   return fImpl->Init();
}

void CbmLitTrackingQa::Exec(
   Option_t* opt)
{
   fImpl->Exec(opt);
}

void CbmLitTrackingQa::Finish()
{
   fImpl->Finish();
}

void CbmLitTrackingQa::SetMinNofPointsSts(Int_t minNofPointsSts)
{
   fImpl->SetMinNofPointsSts(minNofPointsSts);
}

void CbmLitTrackingQa::SetMinNofPointsTrd(Int_t minNofPointsTrd)
{
   fImpl->SetMinNofPointsTrd(minNofPointsTrd);
}

void CbmLitTrackingQa::SetMinNofPointsMuch(Int_t minNofPointsMuch)
{
   fImpl->SetMinNofPointsMuch(minNofPointsMuch);
}

void CbmLitTrackingQa::SetMinNofPointsTof(Int_t minNofPointsTof)
{
   fImpl->SetMinNofPointsTof(minNofPointsTof);
}

void CbmLitTrackingQa::SetQuota(Double_t quota)
{
   fImpl->SetQuota(quota);
}

void CbmLitTrackingQa::SetMinNofHitsRich(Int_t minNofHits)
{
   fImpl->SetMinNofHitsRich(minNofHits);
}

void CbmLitTrackingQa::SetQuotaRich(Double_t quota)
{
   fImpl->SetQuotaRich(quota);
}

void CbmLitTrackingQa::SetMinNofHitsTrd(Int_t minNofHitsTrd)
{
   fImpl->SetMinNofHitsTrd(minNofHitsTrd);
}

void CbmLitTrackingQa::SetMinNofHitsMuch(Int_t minNofHitsMuch)
{
   fImpl->SetMinNofHitsMuch(minNofHitsMuch);
}

void CbmLitTrackingQa::SetVerbose(Int_t verbose)
{
   fImpl->SetVerbose(verbose);
}

void CbmLitTrackingQa::SetOutputDir(const std::string& dir)
{
   fImpl->SetOutputDir(dir);
}

void CbmLitTrackingQa::DrawHistosFromFile(const std::string& fileName)
{
   fImpl->DrawHistosFromFile(fileName);
}

void CbmLitTrackingQa::SetDetectorPresence(
      DetectorId detId,
      bool isDet){
   fImpl->SetDetectorPresence(detId, isDet);
}

bool CbmLitTrackingQa::GetDetectorPresence(
      DetectorId detId) const
{
   return fImpl->GetDetectorPresence(detId);
}

void CbmLitTrackingQa::SetIsElectronSetup(bool isElectronSetup)
{
   fImpl->SetIsElectronSetup(isElectronSetup);
}

bool CbmLitTrackingQa::IsElectronSetup() const
{
   return fImpl->IsElectronSetup();
}

void CbmLitTrackingQa::SetUseConsecutivePointsInSts(Bool_t useConsecutivePointsInSts)
{
   fImpl->SetUseConsecutivePointsInSts(useConsecutivePointsInSts);
}

void CbmLitTrackingQa::CreateReport(
      const string& title,
      const vector<string>& results,
      const vector<string>& names)
{
   CbmLitTrackingQaStudyReport report(kLitHtml);
   report.SetIsElectronSetup(IsElectronSetup());
   report.SetDetectorPresence(kMVD, GetDetectorPresence(kMVD));
   report.SetDetectorPresence(kSTS, GetDetectorPresence(kSTS));
   report.SetDetectorPresence(kRICH, GetDetectorPresence(kRICH));
   report.SetDetectorPresence(kTRD, GetDetectorPresence(kTRD));
   report.SetDetectorPresence(kMUCH, GetDetectorPresence(kMUCH));
   report.SetDetectorPresence(kTOF, GetDetectorPresence(kTOF));

   string fileType = ".html";
//   if (reportType == kLitHtml) fileType = ".html";
//   if (reportType == kLitText) fileType = ".txt";
   ofstream fout(string(fImpl->GetOutputDir() + "/rec_qa_study" + fileType).c_str());

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

ClassImp(CbmLitTrackingQa);
