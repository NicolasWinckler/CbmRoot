#include "TStyle.h"

void much_histo_style()
 {
  gStyle->SetCanvasColor(10);
  gStyle->SetFrameFillColor(10);
  gStyle->SetHistLineWidth(4);
  gStyle->SetPadColor(10);
  gStyle->SetStatColor(10);
  gStyle->SetPalette(1);
  gStyle->SetTextSize(0.05);	
  gStyle->SetTitleSize(0.1);
  gStyle->SetLabelSize(0.05, "X");
  gStyle->SetLabelSize(0.05, "Y");	
  gStyle->SetTitleXSize(0.05);
  gStyle->SetTitleYSize(0.05);
  gStyle->SetTitleXOffset(0.8);	
  gStyle->SetTitleBorderSize(0);
  gStyle->SetTitleFillColor(10);	
  gStyle->SetPadBorderSize(0);
 }
 