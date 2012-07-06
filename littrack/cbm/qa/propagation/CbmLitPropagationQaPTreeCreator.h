/**
 * \file CbmLitPropagationQaPTreeCreator.h
 * \brief Create property tree for propagation QA.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2012
 */

#ifndef CBMLITPROPAGATIONQAPTREECREATOR_H_
#define CBMLITPROPAGATIONQAPTREECREATOR_H_

#include <string>
#include <boost/property_tree/ptree.hpp>
using boost::property_tree::ptree;
using std::string;
class CbmHistManager;

class CbmLitPropagationQaPTreeCreator
{
public:
   /**
    * \brief Constructor.
    */
   CbmLitPropagationQaPTreeCreator();

   /**
    * \brief Destructor.
    */
   virtual ~CbmLitPropagationQaPTreeCreator();

   /**
    * \brief Create property tree out of histogram manager and return it.
    * \param[in] histManager Histogram manager.
    * \return Created property tree.
    */
   ptree Create(
         CbmHistManager* histManager);

private:
   void ResAndPullToPtree(
         ptree& pt,
         const string& histName);

   CbmHistManager* fHM; // Histogram manager
};

#endif /* CBMLITPROPAGATIONQAPTREECREATOR_H_ */
