/**
 * \file CbmHistManager.cxx
 * \brief Histogram manager.
 * \author Semen Lebedev <s.lebedev@gsi.de>
 * \date 2011
 */

#include "CbmHistManager.h"
#include "TH1.h"
#include "TFile.h"
#include "TDirectory.h"
#include "TKey.h"
#include "TClass.h"
#include <boost/regex.hpp>
#include <vector>
#include <map>
#include <string>
#include <cassert>
#include <iostream>
#include <algorithm>
using std::string;
using std::map;
using std::cout;
using std::endl;
using std::vector;
using std::exception;
using std::sort;

class CompareH1NameMore:
   public std::binary_function<
   const TH1*,
   const TH1*,
   Bool_t>
{
public:
   Bool_t operator()(const TH1* hist1, const TH1* hist2) const {
      return string(hist1->GetName()) > string(hist2->GetName());
   }
};

CbmHistManager::CbmHistManager():
      fHistMap()
{

}

CbmHistManager::~CbmHistManager()
{

}

vector<TH1*> CbmHistManager::H1Vector(
      const string& pattern) const
{
   vector<TH1*> histos;
   map<string, TH1*>::const_iterator it;

   try {
		const boost::regex e(pattern);
		for (it = fHistMap.begin(); it != fHistMap.end(); it++) {
			if (boost::regex_match(it->first, e)) histos.push_back(it->second);
		}
   } catch (exception& ex) {
	   cout << "Exception in CbmHistManager::H1Vector: " << ex.what() << endl;
   }

   sort(histos.begin(), histos.end(), CompareH1NameMore());
   return histos;
}

void CbmHistManager::WriteToFile()
{
   map<string, TH1*>::iterator it;
   for (it = fHistMap.begin(); it != fHistMap.end(); it++){
      it->second->Write();
   }
}

void CbmHistManager::ReadFromFile(
      TFile* file)
{
   assert(file != NULL);
   cout << "-I- CbmHistManager::ReadFromFile" << endl;
   TDirectory* dir = gDirectory;
   TIter nextkey(dir->GetListOfKeys());
   TKey *key;
   Int_t c = 0;
   while (key = (TKey*) nextkey()) {
      TObject* obj = key->ReadObj();
      if (obj->IsA()->InheritsFrom (TH1::Class())) {
         TH1* h = (TH1*) obj;
         TH1* h1 = (TH1*)file->Get(h->GetName());
         Add(string(h->GetName()), h1);
         cout << c++ << " " << h->GetName()<< endl;
      }
   }
}

void CbmHistManager::Clear()
{
   map<string, TH1*>::iterator it;
   for (it = fHistMap.begin(); it != fHistMap.end(); it++) {
      delete (*it).second;
   }
   fHistMap.clear();
}

void CbmHistManager::ShrinkEmptyBins(
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

void CbmHistManager::ShrinkEmptyBinsByPattern(
      const string& pattern)
{
	vector<TH1*> effHistos = H1Vector(pattern);
	Int_t nofEffHistos = effHistos.size();
	for (Int_t iHist = 0; iHist < nofEffHistos; iHist++) {
		ShrinkEmptyBins(effHistos[iHist]->GetName());
	}
}

void CbmHistManager::Scale(
      const string& histName,
      Double_t scale)
{
	H1(histName)->Scale(scale);
}

void CbmHistManager::ScaleByPattern(
      const string& pattern,
      Double_t scale)
{
	vector<TH1*> effHistos = H1Vector(pattern);
	Int_t nofEffHistos = effHistos.size();
	for (Int_t iHist = 0; iHist < nofEffHistos; iHist++) {
		Scale(effHistos[iHist]->GetName(), scale);
	}
}

void CbmHistManager::Rebin(
      const string& histName,
      Int_t ngroup)
{
	TH1* hist = H1(histName);
	if (ngroup > 1) {
		hist->Rebin(ngroup);
		hist->Scale(1. / (Double_t)ngroup);
	}
}

void CbmHistManager::RebinByPattern(
      const string& pattern,
      Int_t ngroup)
{
	vector<TH1*> effHistos = H1Vector(pattern);
	Int_t nofEffHistos = effHistos.size();
	for (Int_t iHist = 0; iHist < nofEffHistos; iHist++) {
		Rebin(effHistos[iHist]->GetName(), ngroup);
	}
}

string CbmHistManager::ToString() const
{
	string str = "CbmHistManager list of histograms:\n";
	map<string, TH1*>::const_iterator it;
	for (it = fHistMap.begin(); it != fHistMap.end(); it++){
		str += it->first + "\n";
	}
	return str;
}
