/*
 * CbmLitQaHistManager.h
 *
 *  Created on: 17.10.2011
 *      Author: slebedev
 */

#ifndef CBMLITQAHISTMANAGER_H_
#define CBMLITQAHISTMANAGER_H_

#include <map>
#include <string>
#include <iostream>
using namespace std;

#include "TH1.h"

class TH1F;
class TH2F;
class TH3F;

class CbmLitQaHistManager
{
public:

   CbmLitQaHistManager() {
      fHistMap.clear();
   }

   virtual ~CbmLitQaHistManager() {
      ;
   }

   /**
    * Add new histogram to manager.
    *\param name Name of the histogram.
    *\param histo Pointer to the histogram.
    */
   void Add(
         const string& name,
         TH1* histo) {
      fHistMap.insert(pair<string, TH1*>(name, histo));
   }

   /**
    * Return pointer to TH1F histogram.
    *\param name Name of the histogram.
    */
   TH1F* H1(
         const string& name) const {
      return (TH1F*)fHistMap.find(name)->second;
   }

   /**
    * Return pointer to TH2F histogram.
    *\param name Name of the histogram.
    */
   TH2F* H2(
         const string& name) const {
      return (TH2F*)fHistMap.find(name)->second;
   }

   /**
    * Return pointer to TH3F histogram.
    *\param name Name of the histogram.
    */
   TH3F* H3(
         const string& name) const {
      return (TH3F*)fHistMap.find(name)->second;
   }

   /**
    * Return pointer to TH1 histogram.
    *\param name Name of the histogram.
    */
   TH1* H(
         const string& name) const {
      return (TH1*)fHistMap.find(name)->second;
   }

   void WriteToFile() {
      map<string, TH1*>::iterator it;
      for (it = fHistMap.begin(); it != fHistMap.end(); it++){
         it->second->Write();
      }
   }

private:
   map<string, TH1*> fHistMap;
};

#endif /* CBMLITQAHISTMANAGER_H_ */
