#include <string.h>
#include "TChain.h"
#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TTree.h"
#include "TKey.h"
#include "Riostream.h"
#include "TString.h"
#include "TDirectory.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "Riostream.h"
#include "TRint.h"
#include "TROOT.h"
#include "TStyle.h"
#include <map>

TFile *Target;

std::map<TString, TH1*> fHistoMap;
std::map<TString, TH1*>::iterator fHistoMapIt;

void FindHistos(TDirectory *target);

void plotResults(){
  gROOT->Reset(); 
  gStyle->SetPalette(1,0);
  gROOT->SetStyle("Plain");
  gStyle->SetPadTickX(1);                        
  gStyle->SetPadTickY(1);  
  gStyle->SetOptStat(kFALSE);
  gStyle->SetOptTitle(kFALSE);
  Target = TFile::Open("result.root", "READ");
  FindHistos(Target);
  TCanvas *c = new TCanvas("c","c",800,600);
  c->Divide(1,1);
  c->cd(1);
  TLegend *leg = new TLegend(0.6,0.7,0.85,0.85);
  leg->SetLineColor(0);
  leg->SetLineStyle(0);
  leg->SetFillStyle(0);
  leg->SetTextSize(0.03);
  
  c->cd(1)->SetLogx(0);
  c->cd(1)->SetLogy(0);
  c->cd(1)->SetLogz(0);
  /*
  fBirthGamma->Draw("");
  TString name = fBirthGamma->GetName();
  c->SaveAs(TString("pics/" + name + ".pdf"));
  c->SaveAs(TString("pics/" + name + ".png"));
  fBirthPi0->Draw("");
  name = fBirthPi0->GetName();
  c->SaveAs(TString("pics/" + name + ".pdf"));
  c->SaveAs(TString("pics/" + name + ".png"));
  fBirthPair->Draw("");
  name = fBirthPair->GetName();
  c->SaveAs(TString("pics/" + name + ".pdf"));
  c->SaveAs(TString("pics/" + name + ".png"));
  */
  leg->SetHeader("MC-#gamma pairs from MC-e^{+}e^{-} pairs");
  leg->AddEntry(fHistoMap["InvMassSpectrumGammaEPPairsInMagnet"],"with vertex in magnet volume","L");
  leg->AddEntry(fHistoMap["InvMassSpectrumGammaEPPairsInTarget"],"with vertex in target volume","L");
  leg->AddEntry(fHistoMap["InvMassSpectrumGammaEPPairsOpenAngle"],"with vertex in target volume #Theta < 1.0","L");
  fHistoMap["InvMassSpectrumGammaEPPairsInMagnet"]->Draw();
  fHistoMap["InvMassSpectrumGammaEPPairsInTarget"]->Draw("same");
  fHistoMap["InvMassSpectrumGammaEPPairsOpenAngle"]->Draw("same");
  leg->Draw("same");
  c->SaveAs("pics/MC_InvM_fromGamma_fromEPPair.pdf");
  c->SaveAs("pics/MC_InvM_fromGamma_fromEPPair.png");
  leg->Clear();
  fHistoMap["InvMassSpectrumGammaAllPairs"]->Draw();
  c->SaveAs("pics/MC_InvM_fromGamma.pdf");
  c->SaveAs("pics/MC_InvM_fromGamma.png");

  //fHistoMap["InvMassSpectrumGammaGTCandPairs"]->Draw();
  //fHistoMap["InvMassSpectrumGammaCandPairs"]->Draw("same");
  leg->SetHeader("GT-#gamma pairs from GT-e^{+}e^{-} pairs");
  leg->AddEntry(fHistoMap["InvMassSpectrumGammaCandPairs"],"without charge info.","L");
  leg->AddEntry(fHistoMap["InvMassSpectrumGammaEPCandPairs"],"with charge info.","L");
  leg->AddEntry(fHistoMap["InvMassSpectrumGammaEPCandPairsOpenAngle"],"with charge info. #Theta < 1.0","L");
  fHistoMap["InvMassSpectrumGammaCandPairs"]->Draw();
  fHistoMap["InvMassSpectrumGammaEPCandPairs"]->Draw("same"); 
  fHistoMap["InvMassSpectrumGammaEPCandPairsOpenAngle"]->Draw("same"); 
  leg->Draw("same");
  c->SaveAs("pics/KF_InvM_fromGamma_fromEPPair.pdf");
  c->SaveAs("pics/KF_InvM_fromGamma_fromEPPair.png");
  leg->Clear();
  /*
  CalcDeltaInvMassSpectrum("InvMassSpectrumGammaGTCandPairs");
  CalcDeltaInvMassSpectrum("InvMassSpectrumGammaCandPairs");
  CalcDeltaInvMassSpectrum("InvMassSpectrumGammaEPCandPairs");
  CalcDeltaInvMassSpectrum("InvMassSpectrumGammaEPCandPairsOpenAngle");
  leg->SetHeader("GT-#gamma pairs from GT-e^{+}e^{-} pairs");
  leg->AddEntry(fHistoMap["DeltaInvMassSpectrumGammaGTCandPairs"],"all GT","L");
  leg->AddEntry(fHistoMap["DeltaInvMassSpectrumGammaCandPairs"],"without charge info.","L");
  leg->AddEntry(fHistoMap["DeltaInvMassSpectrumGammaEPCandPairs"],"with charge info.","L");
  leg->AddEntry(fHistoMap["DeltaInvMassSpectrumGammaEPCandPairsOpenAngle"],"with charge info. #Theta < 1.0","L");
  fHistoMap["DeltaInvMassSpectrumGammaGTCandPairs"]->Draw();
  fHistoMap["DeltaInvMassSpectrumGammaCandPairs"]->Draw("same");
  fHistoMap["DeltaInvMassSpectrumGammaEPCandPairs"]->Draw("same"); 
  fHistoMap["DeltaInvMassSpectrumGammaEPCandPairsOpenAngle"]->Draw("same");
  leg->Draw("same");
  c->SaveAs("pics/KF_DeltaInvM_fromGamma_fromEPPair.pdf");
  c->SaveAs("pics/KF_DeltaInvM_fromGamma_fromEPPair.png");
  leg->Clear();
  */
  fHistoMap["gammaAndGammaMother"]->Draw();
  c->SaveAs("pics/MC_MotherPID_gamma_and_gamma.pdf");
  c->SaveAs("pics/MC_MotherPID_gamma_and_gamma.png");

  fHistoMap["gammaMother"]->Draw();
  c->SaveAs("pics/MC_MotherPID_gamma.pdf");
  c->SaveAs("pics/MC_MotherPID_gamma.png");




  c->cd(1)->SetLogx(0);
  c->cd(1)->SetLogy(1);
  c->cd(1)->SetLogz(0);
  leg->AddEntry(fHistoMap["EPPairFromPi0DetectionEfficiencyAll"],"all reco. global tracks","LEP");
  leg->AddEntry(fHistoMap["EPPairFromPi0DetectionEfficiency"],"reco. global tracks after PID cut","LEP");
  fHistoMap["EPPairFromPi0DetectionEfficiency"]->Draw("PE");
  fHistoMap["EPPairFromPi0DetectionEfficiencyAll"]->Draw("PE,same");
  leg->Draw("same");
  c->SaveAs("pics/EPPairFromPi0DetectionEfficiency.pdf");
  c->SaveAs("pics/EPPairFromPi0DetectionEfficiency.png");
  leg->Clear();
  fHistoMap["GT_MC_PID"]->Draw("PE");
  c->SaveAs("pics/GT_MC_PID.pdf");
  c->SaveAs("pics/GT_MC_PID.png");
  fHistoMap["gammaAndGammaMother"]->SetMarkerStyle(20);
  fHistoMap["gammaAndGammaMother"]->SetLineColor(2);
  fHistoMap["gammaAndGammaMother"]->SetMarkerColor(2);
  leg->SetHeader("");
  leg->AddEntry(fHistoMap["gammaAndGammaMother"],"#gamma pairs","LEP");
  leg->AddEntry(fHistoMap["gammaMother"],"single #gamma","LEP");
  fHistoMap["gammaAndGammaMother"]->Draw("PE");
  fHistoMap["gammaMother"]->Draw("PE,same");
  leg->Draw("same");
  c->SaveAs("pics/MC_MotherPID_gamma_andOr_gamma.pdf");
  c->SaveAs("pics/MC_MotherPID_gamma_andOr_gamma.png");
  leg->Clear();

  fHistoMap["ePlusMinusMother"]->Draw("PE");
  fHistoMap["ePlusAndMinusMother"]->SetMarkerStyle(20);
  fHistoMap["ePlusAndMinusMother"]->SetLineColor(2);
  fHistoMap["ePlusAndMinusMother"]->SetMarkerColor(2);
  leg->SetHeader();
  leg->AddEntry(fHistoMap["ePlusMinusMother"],"single e^{+} or e^{-}","LEP");
  leg->AddEntry(fHistoMap["ePlusAndMinusMother"],"e^{+}e^{-} pairs","LEP");
  fHistoMap["ePlusAndMinusMother"]->Draw("PE,same");
  leg->Draw("same");
  c->SaveAs("pics/MC_MotherPID_elec_andOr_posi.pdf");
  c->SaveAs("pics/MC_MotherPID_elec_andOr_posi.png");
  leg->Clear();

  fHistoMap["ePlusMinusMother"]->Draw();
  c->SaveAs("pics/MC_MotherPID_elec_or_posi.pdf");
  c->SaveAs("pics/MC_MotherPID_elec_or_posi.png");
  fHistoMap["ePlusAndMinusMother"]->Draw();
  c->SaveAs("pics/MC_MotherPID_elec_and_posi.pdf");
  c->SaveAs("pics/MC_MotherPID_elec_and_posi.png");

  leg->AddEntry(fHistoMap["MCPid_global"],"global","LEP");
  leg->AddEntry(fHistoMap["MCPid_inMagnet"],"vertex within magnet volume","LEP");
  leg->AddEntry(fHistoMap["MCPid_inTarget"],"vertex within target volume","LEP");
  fHistoMap["MCPid_global"]->Draw("PE");
  fHistoMap["MCPid_inMagnet"]->Draw("PE,same");
  fHistoMap["MCPid_inTarget"]->Draw("PE,same");
  leg->Draw("same");
  c->SaveAs("pics/MC_PID.pdf");
  c->SaveAs("pics/MC_PID.png");
  leg->Clear();

  leg->SetHeader("vertex of e^{+}e^{-} from");
  leg->AddEntry(fHistoMap["PairGammaVertex_z"],"#gamma","LEP");
  leg->AddEntry(fHistoMap["PairPi0Vertex_z"],"#pi^{0}","LEP");
  fHistoMap["PairGammaVertex_z"]->Draw();
  fHistoMap["PairPi0Vertex_z"]->Draw("same");
  leg->Draw("same");
  c->SaveAs("pics/MC_zBirth_ep_from_gamma_pi0.pdf");
  c->SaveAs("pics/MC_zBirth_ep_from_gamma_pi0.png");
  leg->Clear();

  leg->AddEntry(fHistoMap["EPPairOpeningAngle"],"mixed MC-e^{+}e^{-} pairs","L");
  leg->AddEntry(fHistoMap["EPPairOpeningAnglePi0"],"MC-e^{+}e^{-} pairs from #pi^{0}","L");
  leg->AddEntry(fHistoMap["EPPairOpeningAngleGamma"],"MC-e^{+}e^{-} pairs from #gamma","L");
  fHistoMap["EPPairOpeningAngle"]->Draw();
  fHistoMap["EPPairOpeningAnglePi0"]->Draw("same");
  fHistoMap["EPPairOpeningAngleGamma"]->Draw("same");
  leg->Draw("same");
  c->SaveAs("pics/MC_openingAngle_sameMother_Gamma_Pi0.pdf");
  c->SaveAs("pics/MC_openingAngle_sameMother_Gamma_Pi0.png");
  leg->Clear();

  c->cd(1)->SetLogx(1);
  c->cd(1)->SetLogy(1);
  c->cd(1)->SetLogz(0);
  fHistoMap["ZBirthEPfromGamma"]->Draw();
  c->SaveAs("pics/MC_zBirth_epPairs_motherGamma.pdf");
  c->SaveAs("pics/MC_zBirth_epPairs_motherGamma.png");

  c->cd(1)->SetLogx(0);
  c->cd(1)->SetLogy(0);
  c->cd(1)->SetLogz(1);

  fHistoMap["Pt_global"]->Draw("colz");
  c->SaveAs("pics/MC_PT_PID_global.pdf");
  c->SaveAs("pics/MC_PT_PID_global.png");
  fHistoMap["P_global"]->Draw("colz");
  c->SaveAs("pics/MC_P_PID_global.pdf");
  c->SaveAs("pics/MC_P_PID_global.png");
  fHistoMap["Pt_inMagnet"]->Draw("colz");
  c->SaveAs("pics/MC_PT_PID_Magnet.pdf");
  c->SaveAs("pics/MC_PT_PID_Magnet.png");
  fHistoMap["P_inMagnet"]->Draw("colz");
  c->SaveAs("pics/MC_P_PID_Magnet.pdf");
  c->SaveAs("pics/MC_P_PID_Magnet.png");
  fHistoMap["Pt_inTarget"]->Draw("colz");
  c->SaveAs("pics/MC_PT_PID_target.pdf");
  c->SaveAs("pics/MC_PT_PID_target.png");
  fHistoMap["P_inTarget"]->Draw("colz");
  c->SaveAs("pics/MC_P_PID_target.pdf");
  c->SaveAs("pics/MC_P_PID_target.png");
  fHistoMap["MotherDaughter_global"]->Draw("colz");
  c->SaveAs("pics/MC_MotherPID_DaughterPID.pdf");
  c->SaveAs("pics/MC_MotherPID_DaughterPID.png");
  fHistoMap["MotherDaughter_inMagnet"]->Draw("colz");
  c->SaveAs("pics/MC_MotherPID_DaughterPID_magnet.pdf");
  c->SaveAs("pics/MC_MotherPID_DaughterPID_magnet.png");
  fHistoMap["MotherDaughter_inTarget"]->Draw("colz");
  c->SaveAs("pics/MC_MotherPID_DaughterPID_target.pdf");
  c->SaveAs("pics/MC_MotherPID_DaughterPID_target.png");
  fHistoMap["ZBirthAll"]->Draw("colz");
  c->SaveAs("pics/MC_zBirth_PID.pdf");
  c->SaveAs("pics/MC_zBirth_PID.png");

  fHistoMap["InvMPairMother"]->Draw("colz");
  c->SaveAs("pics/MC_invM_elec_posi_MotherPID.pdf");
  c->SaveAs("pics/MC_invM_elec_posi_MotherPID.png");
  fHistoMap["PtPairMother"]->Draw("colz");
  c->SaveAs("pics/MC_PT_elec_posi_MotherPID.pdf");
  c->SaveAs("pics/MC_PT_elec_posi_MotherPID.png");
  fHistoMap["PPairMother"]->Draw("colz");
  c->SaveAs("pics/MC_P_elec_posi_MotherPID.pdf");
  c->SaveAs("pics/MC_P_elec_posi_MotherPID.png");
  fHistoMap["OpenAnglePairMother"]->Draw("colz");
  c->SaveAs("pics/MC_openingAngle_elec_posi_MotherPID.pdf");
  c->SaveAs("pics/MC_openingAngle_elec_posi_MotherPID.png");

  fHistoMap["TRD_GT_dEdx_KF_P"]->Draw("colz");
  c->SaveAs("pics/TRD_GT_dEdx_KF_P.pdf");
  c->SaveAs("pics/TRD_GT_dEdx_KF_P.png");
  fHistoMap["RICH_GT_radiusA_KF_P"]->Draw("colz");
  c->SaveAs("pics/RICH_GT_radiusA_KF_P.pdf");
  c->SaveAs("pics/RICH_GT_radiusA_KF_P.png");
  fHistoMap["RICH_GT_radiusB_KF_P"]->Draw("colz");
  c->SaveAs("pics/RICH_GT_radiusB_KF_P.pdf");
  c->SaveAs("pics/RICH_GT_radiusB_KF_P.png");
  fHistoMap["RICH_GT_radius_KF_P"]->Draw("colz");
  c->SaveAs("pics/RICH_GT_radius_KF_P.pdf");
  c->SaveAs("pics/RICH_GT_radius_KF_P.png");
  fHistoMap["TOF_GT_time_KF_P"]->Draw("colz");
  c->SaveAs("pics/TOF_GT_time_KF_P.pdf");
  c->SaveAs("pics/TOF_GT_time_KF_P.png");

  fHistoMap["TRD_GT_dEdx_KF_P_true"]->Draw("colz");
  c->SaveAs("pics/TRD_GT_dEdx_KF_P_true.pdf");
  c->SaveAs("pics/TRD_GT_dEdx_KF_P_true.png");
  fHistoMap["RICH_GT_radiusA_KF_P_true"]->Draw("colz");
  c->SaveAs("pics/RICH_GT_radiusA_KF_P_true.pdf");
  c->SaveAs("pics/RICH_GT_radiusA_KF_P_true.png");
  fHistoMap["RICH_GT_radiusB_KF_P_true"]->Draw("colz");
  c->SaveAs("pics/RICH_GT_radiusB_KF_P_true.pdf");
  c->SaveAs("pics/RICH_GT_radiusB_KF_P_true.png");
  fHistoMap["RICH_GT_radius_KF_P_true"]->Draw("colz");
  c->SaveAs("pics/RICH_GT_radius_KF_P_true.pdf");
  c->SaveAs("pics/RICH_GT_radius_KF_P_true.png");
  fHistoMap["TOF_GT_time_KF_P_true"]->Draw("colz");
  c->SaveAs("pics/TOF_GT_time_KF_P_true.pdf");
  c->SaveAs("pics/TOF_GT_time_KF_P_true.png");

  fHistoMap["DeltaP"]->Draw("colz");
  c->SaveAs("pics/DeltaP.pdf");
  c->SaveAs("pics/DeltaP.png");
  fHistoMap["DeltaPt"] ->Draw("colz");
  c->SaveAs("pics/DeltaPt.pdf");
  c->SaveAs("pics/DeltaPt.png");
  fHistoMap["DeltaPGT_PKF"]->Draw("colz");
  c->SaveAs("pics/DeltaPGT_PKF.pdf");
  c->SaveAs("pics/DeltaPGT_PKF.png");
  fHistoMap["KF_PID_MC_PID"]->Draw("colz");
  c->SaveAs("pics/KF_PID_MC_PID.pdf");
  c->SaveAs("pics/KF_PID_MC_PID.png");
  c->Close();
}

void FindHistos(TDirectory *target) {
  cout << "Target path: " << target->GetPath() << endl;
  TString path( (char*)strstr( target->GetPath(), ":" ) );
  path.Remove( 0, 2 );
  TDirectory *current_sourcedir = gDirectory;
  Bool_t status = TH1::AddDirectoryStatus();
  TH1::AddDirectory(kFALSE);
  TChain *globChain = 0;
  TIter nextkey( current_sourcedir->GetListOfKeys() );
  TKey *key, *oldkey=0;
  while ( (key = (TKey*)nextkey())) {
    if (oldkey && !strcmp(oldkey->GetName(),key->GetName())) continue;
    TObject *obj = key->ReadObj();
    if ( obj->IsA()->InheritsFrom( TH1::Class() ) ) {
      fHistoMap[TString(obj->GetName())] = (TH1*)obj;
      if ( obj->IsA()->InheritsFrom( TH2::Class() ) ) {
	fHistoMap[TString(obj->GetName())]->SetContour(99);
      }
    } else if ( obj->IsA()->InheritsFrom( TDirectory::Class() ) ) {
      cout << "Found subdirectory " << obj->GetName() << endl;
      target->cd(obj->GetName());
      TDirectory *newdir = target->CurrentDirectory();
      FindHistos(newdir);
    }
  }
  TH1::AddDirectory(status);
}
