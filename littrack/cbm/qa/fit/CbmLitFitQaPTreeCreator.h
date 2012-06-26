/**
 * \file CbmLitFitQaPTreeCreator.h
 * \brief Create property tree for fit QA.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2012
 */

#ifndef CBMLITFITQAPTREECREATOR_H_
#define CBMLITFITQAPTREECREATOR_H_

#include <string>
#include <boost/property_tree/ptree.hpp>
using boost::property_tree::ptree;
using std::string;
class CbmLitHistManager;

class CbmLitFitQaPTreeCreator
{
public:
   /**
    * \brief Constructor.
    */
   CbmLitFitQaPTreeCreator();

   /**
    * \brief Destructor.
    */
   virtual ~CbmLitFitQaPTreeCreator();

   /**
    * \brief Create property tree out of histogram manager and return it.
    * \param[in] histManager Histogram manager.
    * \return Created property tree.
    */
   ptree Create(
         CbmLitHistManager* histManager);

private:
   void ResidualAndPullToPtree(
         ptree& pt) const;

   void WrongCovToPtree(
         ptree& pt) const;

   CbmLitHistManager* fHM; // Histogram manager
};

#endif /* CBMLITFITQAPTREECREATOR_H_ */
