#include "../../littrack/utils/CbmLitDrawHist.cxx"
#include "../../littrack/utils/CbmLitUtils.cxx"

void draw(){
    TFile *file = new TFile("");
    gStyle->SetHistLineWidth(3);
	SetStyles();

    TCanvas *c1 = new TCanvas("c1","c1",400,600);

}


