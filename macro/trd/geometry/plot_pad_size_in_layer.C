//
//  plot pad size of layers in cm2 in png files
//  Nov 2012
//
//  run as follows:
//  root -l -b -q plot_pad_size_in_layer.C++\(\"trd.v13/trd_v13b.digi.par\"\)
//  display *.png
//

#include "Riostream.h"
#include "TRint.h"
#include "TROOT.h"
#include "TStyle.h"
#include "TH2.h"
#include "TCanvas.h"
#include "TPaveText.h"
#include "TString.h"
#include <iostream>
#include <fstream>
#include <cmath>
#include <map>

void plot_pad_size_in_layer(TString digiPar="trd.v13/trd_v13g.digi.par") {

  gStyle->SetPalette(1,0);
  gROOT->SetStyle("Plain");
  gStyle->SetPadTickX(1);                        
  gStyle->SetPadTickY(1); 
  gStyle->SetOptStat(kFALSE);
  gStyle->SetOptTitle(kFALSE);

  Bool_t read = false;
  TH2I *fLayerDummy = new TH2I("LayerDummy","",1200,-600,600,1000,-500,500);
  fLayerDummy->SetXTitle("x-coordinate [cm]");
  fLayerDummy->SetYTitle("y-coordinate [cm]");
  fLayerDummy->GetXaxis()->SetLabelSize(0.02);
  fLayerDummy->GetYaxis()->SetLabelSize(0.02);
  fLayerDummy->GetZaxis()->SetLabelSize(0.02);
  fLayerDummy->GetXaxis()->SetTitleSize(0.02);
  fLayerDummy->GetXaxis()->SetTitleOffset(1.5);
  fLayerDummy->GetYaxis()->SetTitleSize(0.02);
  fLayerDummy->GetYaxis()->SetTitleOffset(2);
  fLayerDummy->GetZaxis()->SetTitleSize(0.02);
  fLayerDummy->GetZaxis()->SetTitleOffset(-2);

  TString title;
  TString buffer;
  TString firstModule = "";
  Int_t blockCounter(0), startCounter(0); // , stopCounter(0);
  Double_t msX(0), msY(0), mpX(0), mpY(0), mpZ(0), psX(0), psY(0);
  std::map<float, TCanvas*> layerView;// map key is z-position of modules
  std::map<float, TCanvas*>::iterator it;
  ifstream digipar;
  digipar.open(digiPar.Data(), ifstream::in);
  while (digipar.good()) {
    digipar >> buffer;
    //cout << "(" << blockCounter << ")    " << buffer << endl;
    if (blockCounter == 19)
      firstModule = buffer;
    if (buffer == (firstModule + ":")){
      //cout << buffer << " <===========================================" << endl;
      read = true;
    }
    if (read) {
      startCounter++;
      if (startCounter == 4+1)   // position of module position in x
	mpX = buffer.Atof();
      if (startCounter == 5+1)   // position of module position in y
	mpY = buffer.Atof();
      if (startCounter == 6+1)   // position of module position in z
	mpZ = buffer.Atof();
      if (startCounter == 7+1)   // position of module size in x
	msX = buffer.Atof();
      if (startCounter == 8+1)   // position of module size in y
	msY = buffer.Atof();
      if (startCounter == 17+1)   // position of pad size in x
	psX = buffer.Atof();
      if (startCounter == 18+1)   // position of pad size in y
	psY = buffer.Atof();
      //printf("module position: (%.1f, %.1f, %.1f) module size: (%.1f, %.1f) pad size: (%.2f, %.2f) pad area: %.2f\n",mpX,mpY,mpZ,2*msX,2*msY,psX,psY,psX*psY);
      if (startCounter == 22+1) {
	startCounter = 0; // reset
	it = layerView.find(mpZ);
	if (it == layerView.end()){	
	  title.Form("pad_size_layer_at_z_%.2fm",mpZ);  
	  layerView[mpZ] = new TCanvas(title,title,1200,1000);
	  fLayerDummy->DrawCopy("");
	  // now print cm2 in the center
	layerView[mpZ]->cd();
	title.Form("cm^{2}");  // print cm2
	TPaveText *text = new TPaveText(0 - 28.5,
					0 - 28.5,
					0 + 28.5,
					0 + 28.5
					);
	text->SetFillStyle(1001);
	text->SetLineColor(1);
        text->SetFillColor(kWhite);
	text->AddText(title);
	text->Draw("same");
	}
	layerView[mpZ]->cd();
	//	title.Form("%2.0fcm^{2}",psX*psY);  // print pad size
	title.Form("%.0f",psX*psY);  // print pad size
	TPaveText *text = new TPaveText(mpX - msX,
					mpY - msY,
					mpX + msX,
					mpY + msY
					/*
					  fModuleMap[fModuleOccupancyMemoryMapIt->first]->ModulePositionX - fModuleMap[fModuleOccupancyMemoryMapIt->first]->ModuleSizeX,
					  fModuleMap[fModuleOccupancyMemoryMapIt->first]->ModulePositionY - fModuleMap[fModuleOccupancyMemoryMapIt->first]->ModuleSizeY,
					  fModuleMap[fModuleOccupancyMemoryMapIt->first]->ModulePositionX + fModuleMap[fModuleOccupancyMemoryMapIt->first]->ModuleSizeX,
					  fModuleMap[fModuleOccupancyMemoryMapIt->first]->ModulePositionY + fModuleMap[fModuleOccupancyMemoryMapIt->first]->ModuleSizeY
					*/
					);
	text->SetFillStyle(1001);
	text->SetLineColor(1);
	//        text->SetFillColor(kViolet);

	// vary background color
//        if ((int)(psX*psY+.5) == 2)
//        {
//          text->SetFillColor(kOrange + 9);
//        } 
//        else 
        if (psX*psY <= 5)
        {
          text->SetFillColor(kOrange + 10 - ((int)(psX*psY+.5)-1) * 4);
//          printf("%2.1f: %d\n", psX*psY, 10 - ((int)(psX*psY+.5)-1) * 3);
        } 
        else if (psX*psY <= 10)
	{
          text->SetFillColor(kSpring + 10 - ((int)(psX*psY+.5)-1-5) * 2);
//          printf("%2.1f: %d\n", psX*psY, 10 - ((int)(psX*psY+.5)-1-5) * 2);
        } 
        else if (psX*psY > 10)
	{
          text->SetFillColor(kGreen);
//          printf("%2.1f: %s\n", psX*psY, "green");
        } 

	text->AddText(title);
	text->Draw("same");
	//layerView[mpZ]->Update();
      }
    }  
    blockCounter++;
  }
  digipar.close();
  for (it = layerView.begin(); it != layerView.end(); it++) 
    it->second->SaveAs(TString(it->second->GetTitle())+".png");
}
