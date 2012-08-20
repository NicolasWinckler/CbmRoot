/**
 * \file CbmStsSimulationQaPTreeCreator.h
 * \brief Create property tree for STS simulation QA.
 * \author Volker Friese <v.friese@gsi.de>
 * \date 2012
 */

#ifndef CBMSTSSIMULATIONQAPTREECREATOR_H_
#define CBMSTSSIMULATIONQAPTREECREATOR_H_

#include <string>
#include <boost/property_tree/ptree.hpp>
using boost::property_tree::ptree;
using std::string;
class CbmHistManager;

/**
 * \class CbmStsSimulationQaPTreeCreator
 * \brief Create property tree for STS simulation QA.
 * \author Volker Friese <v.friese@gsi.de>
 * \date 2012
 */
class CbmStsSimulationQaPTreeCreator
{
public:
   /**
    * \brief Constructor.
    */
   CbmStsSimulationQaPTreeCreator();

   /**
    * \brief Destructor.
    */
   virtual ~CbmStsSimulationQaPTreeCreator();

   /**
    * \brief Create and return property for CbmLitHistManager.
    * \return property tree.
    */
   ptree Create(
         CbmHistManager* histManager);

private:

   void NofObjectsToPTree(
      ptree& pt);

   // Pointer to histogram manager
   CbmHistManager* fHM;
};

#endif /* CBMSTSSIMULATIONQAPTREECREATOR_H_ */
