/**
 * \file CbmHistManager.h
 * \brief Histogram manager.
 * \author Semen Lebedev <s.lebedev@gsi.de>
 * \date 2011
 */

#ifndef CBMHISTMANAGER_H_
#define CBMHISTMANAGER_H_

#include "TObject.h"
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <cassert>

class TFile;
class TH1;
class TH1I;
class TH1F;
class TH2F;
class TH3F;

using std::map;
using std::make_pair;
using std::string;
using std::vector;

/**
 * \class CbmHistManager
 * \brief Histogram manager.
 * \author Semen Lebedev <s.lebedev@gsi.de>
 * \date 2011
 */
class CbmHistManager
{
public:

   /**
    * \brief Constructor.
    */
   CbmHistManager();

   /**
    * \brief Destructor.
    */
   virtual ~CbmHistManager();

   /**
    * \brief Add new histogram to manager.
    * \param[in] name Name of histogram.
    * \param[in] histo Pointer to histogram.
    */
   void Add(
         const string& name,
         TH1* histo)
   {
      fHistMap.insert(make_pair<string, TH1*>(name, histo));
   }

   /**
    * \brief Return pointer to TH1F histogram.
    * \param[in] name Name of histogram.
    * \return pointer to TH1F histogram.
    */
   TH1F* H1F(
         const string& name) const {
      if (fHistMap.count(name) == 0) { // Temporarily used for degugging
         std::cout << "Error: CbmHistManager::H1F(name): name=" << name << std::endl;
      }
      assert(fHistMap.count(name) != 0);
      return (TH1F*)fHistMap.find(name)->second;
   }

   /**
    * \brief Return pointer to TH2F histogram.
    * \param[in] name Name of histogram.
    * \return pointer to TH2F histogram.
    */
   TH2F* H2F(
         const string& name) const {
      assert(fHistMap.count(name) != 0);
      return (TH2F*)fHistMap.find(name)->second;
   }

   /**
    * \brief Return pointer to TH3F histogram.
    * \param[in] name Name of histogram.
    * \return pointer to TH3F histogram.
    */
   TH3F* H3F(
         const string& name) const {
      assert(fHistMap.count(name) != 0);
      return (TH3F*)fHistMap.find(name)->second;
   }

   /**
    * \brief Return pointer to TH1 histogram.
    * \param[in] name Name of histogram.
    * \return pointer to TH1 histogram.
    */
   TH1* H1(
         const string& name) const {
      if (fHistMap.count(name) == 0) { // Temporarily used for debugging
    	  std::cout << "Error: CbmHistManager::H1(name): name=" << name << std::endl;
      }
      assert(fHistMap.count(name) != 0);
      return fHistMap.find(name)->second;
   }

   /**
    * \brief Return vector of pointers to TH1 histogram.
    * \param[in] pattern Regex for histogram name.
    * \return Vector of pointers to TH1 histogram.
    */
   vector<TH1*> H1Vector(
         const string& pattern) const;

   /**
    * \brief Check existence of histogram in manager.
    * \param[in] name Name of histogram.
    * \return True if histogram exists in manager.
    */
   bool Exists(
         const string& name) const {
      return (fHistMap.count(name) == 0) ? false : true;
   }

   /**
    * \brief Write all histograms to current opened file.
    */
   void WriteToFile();

   /**
    * \brief Read histograms from file.
    * \param[in] file Pointer to file with histograms.
    */
   void ReadFromFile(
         TFile* file);

   /**
    * \brief Clear memory. Remove all histograms.
    */
   void Clear();

   /**
    * \brief Shrink empty bins in histogram for Xaxis from right side.
    * \param[in] histName Name of histogram.
    */
   void ShrinkEmptyBins(
         const string& histName);

   /**
    * \brief Shrink empty bins in histograms for Xaxis from right side.
    * \param[in] histPatternName Regular expression for histogram name.
    */
   void ShrinkEmptyBinsByPattern(
         const string& pattern);

   /**
    * \brief Scale histogram.
    * \param[in] histName Name of histogram.
    * \param[in] scale Scaling factor.
    */
   void Scale(
         const string& histName,
         Double_t scale);

   /**
    * \brief Scale histograms which name matches specified pattern.
    * \param[in] histPatternName Regular expression for histogram name.
    * \param[in] scale Scaling factor.
    */
   void ScaleByPattern(
         const string& pattern,
         Double_t scale);

   /**
    * \brief Rebin histogram.
    * \param[in] histName Name of histogram.
    * \param[in] ngroup Rebining factor.
    */
   void Rebin(
         const string& histName,
         Int_t ngroup);

   /**
    * \brief Rebin histograms which name matches specified pattern.
    * \param[in] histPatternName Regular expression for histogram name.
    * \param[in] ngroup Rebining factor.
    */
   void RebinByPattern(
         const string& pattern,
         Int_t ngroup);

   /**
    * \brief Return string representation of class.
    * \return string representation of class.
    */
   string ToString() const;

   /**
    * \brief Operator << for convenient output to std::ostream.
    * \return Insertion stream in order to be able to call a succession of insertion operations.
    */
   friend std::ostream& operator<<(std::ostream& strm, const CbmHistManager& histManager) {
      strm << histManager.ToString();
      return strm;
   }

private:
   // Map of histogram name to its pointer
   map<string, TH1*> fHistMap;
};

#endif /* CBMHISTMANAGER_H_ */
