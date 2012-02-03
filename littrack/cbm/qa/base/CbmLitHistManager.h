/**
 * \file CbmLitHistManager.h
 * \brief Histogram manager.
 * \author Semen Lebedev <s.lebedev@gsi.de>
 * \date 2011
 */

#ifndef CBMLITHISTMANAGER_H_
#define CBMLITHISTMANAGER_H_

#include <iostream>
#include <map>
#include <string>
#include <assert.h>

class TFile;
class TH1;
class TH1I;
class TH1F;
class TH2F;
class TH3F;

using std::map;
using std::pair;
using std::string;

/**
 * \class CbmLitHistManager
 * \brief Histogram manager.
 * \author Semen Lebedev <s.lebedev@gsi.de>
 * \date 2011
 */
class CbmLitHistManager
{
public:

   /**
    * \brief Constructor.
    */
   CbmLitHistManager();

   /**
    * \brief Destructor.
    */
   virtual ~CbmLitHistManager();

   /**
    * \brief Add new histogram to manager.
    * \param[in] name Name of histogram.
    * \param[in] histo Pointer to histogram.
    */
   void Add(
         const string& name,
         TH1* histo) {
      fHistMap.insert(pair<string, TH1*>(name, histo));
   }

   /**
    * \brief Return pointer to TH1F histogram.
    * \param[in] name Name of histogram.
    * \return pointer to TH1F histogram.
    */
   TH1F* H1F(
         const string& name) const {
      if (fHistMap.count(name) == 0) {
         std::cout << "fHistMap.count(name) == 0: name=" << name << std::endl;
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
    * \return pointe to TH1 histogram.
    */
   TH1* H1(
         const string& name) const {
      if (fHistMap.count(name) == 0) {
         std::cout << "H1: " <<  name << std::endl;
      }
      assert(fHistMap.count(name) != 0);
      return fHistMap.find(name)->second;
   }

   /**
    * \brief Check existence of histogram in manager.
    * \param[in] name Name of histogram.
    * \return True if histogram exists in manager.
    */
   bool Exists(
         const string& name) const {
      return fHistMap.count(name);
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
    * \brief Clears memory. Removes all histograms.
    */
   void Clear();

private:
   // Map of histogram name to its pointer
   map<string, TH1*> fHistMap;
};

#endif /* CBMLITHISTMANAGER_H_ */
