/** CbmLitReconstructionQaLatex.h
 * @author Semen Lebedev <s.lebedev@gsi.de>
 * @since 2011
 * @version 1.0
 **
 **/

#ifndef CBMLITRECONSTRUCTIONQALATEX_H_
#define CBMLITRECONSTRUCTIONQALATEX_H_

#include "TObject.h"
#include <string>


#include <boost/property_tree/ptree.hpp>


class CbmLitReconstructionQaLatex
{
public:
   /* Constructor */
   CbmLitReconstructionQaLatex(){;}

   /* Destructor */
   virtual ~CbmLitReconstructionQaLatex(){;}

   /* This method should be called from a macro, it creates
    * a latex table.
    * @param files Input paths to json files*/
   static void PrintLatexTable(
         const std::vector<std::string>& files);

private:
   /* Prints vertically group name in the first column
    * and standard row in table.
    * @param groupName name of the group
    * @param nofRows number of rows in this group
    * @param rowName row name
    * @param name1 field name in the property tree
    * @param name2 field name in the property tree (if name2 == "" -> no print out)
    * @param name3 field name in the property tree (if name3 == "" -> no print out)
    * @param pts property tree*/
   static void PrintRowGroup(
         const std::string& groupName,
         Int_t nofRows,
         const std::string& rowName,
         const std::string& name1,
         const std::string& name2,
         const std::string& name3,
         const std::vector<boost::property_tree::ptree>& pts);

   /* Prints standard row in table.
    * @param rowName row name
    * @param name1 field name in the property tree
    * @param name2 field name in the property tree (if name2 == "" -> no print out)
    * @param name3 field name in the property tree (if name3 == "" -> no print out)
    * @param pts property tree*/
   static void PrintRow(
         const std::string& rowName,
         const std::string& name1,
         const std::string& name2,
         const std::string& name3,
         const std::vector<boost::property_tree::ptree>& pts);

   static std::string ToStringAutoPrecision(
         Double_t d);
};

#endif /* CBMLITRECONSTRUCTIONQALATEX_H_ */
