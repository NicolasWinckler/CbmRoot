/**
 * \file CbmLitHistManager.cxx
 * \brief Histogram manager.
 * \author Semen Lebedev <s.lebedev@gsi.de>
 * \date 2011
 */

#include "CbmLitHistManager.h"
#include "TH1.h"
#include "TFile.h"
#include "TDirectory.h"
#include "TKey.h"
#include "TClass.h"
#include <map>
#include <string>
#include <assert.h>
#include <iostream>
using std::string;
using std::map;
using std::cout;
using std::endl;

CbmLitHistManager::CbmLitHistManager():
      fHistMap()
{

}

CbmLitHistManager::~CbmLitHistManager()
{

}

void CbmLitHistManager::WriteToFile()
{
   map<string, TH1*>::iterator it;
   for (it = fHistMap.begin(); it != fHistMap.end(); it++){
      it->second->Write();
   }
}

void CbmLitHistManager::ReadFromFile(
      TFile* file)
{
   assert(file != NULL);
   cout << "-I- CbmLitHistManager::ReadFromFile" << endl;
   TDirectory* dir = gDirectory;
   TIter nextkey( dir->GetListOfKeys());
   TKey *key;
   int c = 0;
   while (key = (TKey*) nextkey()) {
      TObject* obj = key->ReadObj();
      if (obj->IsA()->InheritsFrom (TH1::Class())) {
         TH1* h = (TH1*) obj;
         TH1* h1 = (TH1*)file->Get(h->GetName());
         Add(h->GetName(), h1);
         cout << c++ << " " << h->GetName()<< endl;
      }
   }
}

void CbmLitHistManager::Clear()
{
   map<string, TH1*>::iterator it;
   for (it = fHistMap.begin(); it != fHistMap.end(); it++) {
      delete (*it).second;
   }
   fHistMap.clear();
}

void CbmLitHistManager::ShrinkEmptyBins(
      const string& histName)
{
   TH1* hist = H1(histName);
   Int_t nofBins = hist->GetNbinsX();
   Int_t shrinkBin = 0;
   for (Int_t iBin = nofBins; iBin > 0; iBin--) {
      Double_t content = hist->GetBinContent(iBin);
      if (content != 0.) {
         shrinkBin = iBin;
         break;
      }
   }
   hist->GetXaxis()->SetRange(1, shrinkBin + 1);
}
