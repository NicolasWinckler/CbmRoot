/** CbmLitReconstructionQaImpl.cxx
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2007
 * @version 2.0
 **/

#include "qa/CbmLitReconstructionQa.h"
#include "qa/CbmLitReconstructionQaImpl.h"
#include "qa/CbmLitReconstructionQaPrint.h"

CbmLitReconstructionQa::CbmLitReconstructionQa():
   FairTask("LitReconstructionQA", 1)
{
   fImpl = new CbmLitReconstructionQaImpl();
}

CbmLitReconstructionQa::~CbmLitReconstructionQa()
{

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


void CbmLitReconstructionQa::SetOutputJsonFileName(const std::string& path)
{
   fImpl->SetOutputJsonFileName(path);
}

void CbmLitReconstructionQa::SetMomentumRange(Double_t minMom, Double_t maxMom)
{
   fImpl->SetMomentumRange(minMom, maxMom);
}

void CbmLitReconstructionQa::SetNofBinsMom(Int_t nofBins)
{
   fImpl->SetNofBinsMom(nofBins);
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

void CbmLitReconstructionQa::SetIsElectronSetup(bool isElectronSetup)
{
   fImpl->SetIsElectronSetup(isElectronSetup);
}

void CbmLitReconstructionQa::PrintLatexTable()
{
   CbmLitReconstructionQaPrint::PrintLatexTable();
}

ClassImp(CbmLitReconstructionQa);
