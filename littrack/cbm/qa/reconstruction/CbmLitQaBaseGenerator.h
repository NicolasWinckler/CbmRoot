/**
 * \file CbmLitQaBaseGenerator.h
 *
 * \brief Base class for output generators.
 *
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2011
 */
#ifndef CBMLITQABASEGENERATOR_H_
#define CBMLITQABASEGENERATOR_H_

#include "CbmDetectorList.h"
#include <boost/property_tree/ptree.hpp>
#include <string>

/**
 * \class CbmLitQaBaseGenerator
 *
 * \brief Base class for output generators.
 *
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2011
 *
 */
class CbmLitQaBaseGenerator {
public:
   /**
    * \brief Constructor.
    */
   CbmLitQaBaseGenerator();

   /**
    * \brief Destructor.
    */
   virtual ~CbmLitQaBaseGenerator();

   /**
    * \brief Set detector presence to TRUE if QA results for this detector has to be displayed in output.
    *
    * \param[in] detId Id of the detector kMVD, kSTS...
    * \param[in] isDet True if detector is in the setup.
    */
   void SetDetectorPresence(
         DetectorId detId,
         bool isDet);

   /**
    * \brief Set explicitly electron setup of the detector.
    *
    * \param[in] isElectronSetup true if electron setup.
    */
   void SetIsElectronSetup(
         bool isElectronSetup) {
      fIsElectronSetup = isElectronSetup;
   }

protected:
   /**
    * \brief Return string with CSS style for the HTML page.
    * \return std::string with CSS style for the HTML page.
    */
   std::string HtmlHeadString();


   /**
    * \brief Print property value from property tree to std::string.
    *
    * Uniforms output style of properties. Colors background
    * in dependence on the correctness of the property.
    *
    * \param[in] valueTitle Property title which is printed before the property value.
    * \param[in] valueName Property name in the property tree.
    * \param[in] tag HTML tag which is used to print the property i.e. "p", "b", "td".
    * \return std::string with property in HTML format.
    */
   std::string HtmlPrintValue(
         const std::string& valueTitle,
         const std::string& valueName,
         const std::string& tag);

   bool fIsElectronSetup; // If "electron" setup detected than true
   bool fIsMvd; // If MVD detected than true
   bool fIsSts; // If STS detected than true
   bool fIsRich; // If RICH detected than true
   bool fIsTrd; // If TRD detected than true
   bool fIsMuch; // If MUCH detected than true
   bool fIsTof; // If TOF detected than true

   // Background colors for error highlighting
   std::string fErrorColor; // error
   std::string fWarningColor; // warning
   std::string fNormalColor; // normal

   // If TRUE than results are highlighted depending on the results of checking procedure
   bool fIsUseChecking;

   // Title of the page
   std::string fTitle;

   // Property tree of Qa results
   const boost::property_tree::ptree* fQa;
   // Property with ideal values
   const boost::property_tree::ptree* fIdeal;
   // Property tree with checked results
   const boost::property_tree::ptree* fCheck;
};

#endif /* CBMLITQABASEGENERATOR_H_ */
