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

class CompareH1NameMore:
   public std::binary_function<
   const TH1*,
   const TH1*,
   bool>
{
public:
   bool operator()(const TH1* hist1, const TH1* hist2) const {
      return string(hist1->GetName()) > string(hist2->GetName());
   }
};

CbmLitHistManager::CbmLitHistManager():
      fHistMap()
{

}

CbmLitHistManager::~CbmLitHistManager()
{

}

vector<TH1*> CbmLitHistManager::H1Vector(
      const string& pattern) const
{
   vector<TH1*> histos;
   map<string, TH1*>::const_iterator it;

   try {
		const boost::regex e(pattern);
		for (it = fHistMap.begin(); it != fHistMap.end(); it++) {
			if (boost::regex_match(it->first, e)) histos.push_back(it->second);
		}
   } catch (std::exception& ex) {
	   std::cout << "Exception in CbmLitHistManager::H1Vector: " << ex.what() << std::endl;
   }

   std::sort(histos.begin(), histos.end(), CompareH1NameMore());
   return histos;
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
   TIter nextkey(dir->GetListOfKeys());
   TKey *key;
   int c = 0;
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

void CbmLitHistManager::ShrinkEmptyBinsByPattern(
      const string& pattern)
{
	vector<TH1*> effHistos = H1Vector(pattern);
	Int_t nofEffHistos = effHistos.size();
	for (Int_t iHist = 0; iHist < nofEffHistos; iHist++) {
		ShrinkEmptyBins(effHistos[iHist]->GetName());
	}
}

void CbmLitHistManager::Scale(
      const string& histName,
      Double_t scale)
{
	H1(histName)->Scale(scale);
}

void CbmLitHistManager::ScaleByPattern(
      const string& pattern,
      Double_t scale)
{
	vector<TH1*> effHistos = H1Vector(pattern);
	Int_t nofEffHistos = effHistos.size();
	for (Int_t iHist = 0; iHist < nofEffHistos; iHist++) {
		Scale(effHistos[iHist]->GetName(), scale);
	}
}

void CbmLitHistManager::ScaleAll(
      Double_t scale)
{
   map<string, TH1*>::iterator it;
   Int_t c = 0;
   for (it = fHistMap.begin(); it != fHistMap.end(); it++) {
      (it->second)->Scale(1./(Double_t)scale);
   }
}

void CbmLitHistManager::Rebin(
      const string& histName,
      Int_t ngroup)
{
	TH1* hist = H1(histName);
	if (ngroup > 1) {
		hist->Rebin(ngroup);
		hist->Scale(1. / (double)ngroup);
	}
}

void CbmLitHistManager::RebinByPattern(
      const string& pattern,
      Int_t ngroup)
{
	vector<TH1*> effHistos = H1Vector(pattern);
	Int_t nofEffHistos = effHistos.size();
	for (Int_t iHist = 0; iHist < nofEffHistos; iHist++) {
		Rebin(effHistos[iHist]->GetName(), ngroup);
	}
}

string CbmLitHistManager::ToString() const
{
	string str = "CbmLitHistManager list of histograms:\n";
	map<string, TH1*>::const_iterator it;
	for (it = fHistMap.begin(); it != fHistMap.end(); it++){
		str += it->first + "\n";
	}
	return str;
}
