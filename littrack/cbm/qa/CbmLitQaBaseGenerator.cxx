/**
 * \file CbmLitQaBaseGenerator.cxx
 *
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2011
 */
#include "CbmLitQaBaseGenerator.h"
#include "../std/utils/CbmLitUtils.h"

CbmLitQaBaseGenerator::CbmLitQaBaseGenerator():
   fIsElectronSetup(true),
   fIsMvd(true),
   fIsSts(true),
   fIsRich(true),
   fIsTrd(true),
   fIsMuch(true),
   fIsTof(true),
   fErrorColor("#FF0000"),
   fWarningColor("#FFFF00"),
   fNormalColor("#00FF00"),
   fIsUseChecking(true),
   fTitle("")
{

}

CbmLitQaBaseGenerator::~CbmLitQaBaseGenerator()
{

}

void CbmLitQaBaseGenerator::SetDetectorPresence(
         DetectorId detId,
         bool isDet)
{
   switch(detId) {
      case kMVD: fIsMvd = isDet; break;
      case kSTS: fIsSts = isDet; break;
      case kRICH: fIsRich = isDet; break;
      case kTRD: fIsTrd = isDet; break;
      case kMUCH: fIsMuch = isDet; break;
      case kTOF: fIsTof = isDet; break;
      default: break;
   }
}

std::string CbmLitQaBaseGenerator::HtmlPrintValue(
      const std::string& valueTitle,
      const std::string& valueName,
      const std::string& tag)
{
   // Print the value with corresponding color
   float check = fCheck->get(valueName, -1.);
   std::string color = (check == -1.) ? fWarningColor :
      (check == 0.) ? fErrorColor : fNormalColor;

   // Hint text
   std::string hint = "";
   if (check == 0. || check == 1.) {
      float min = fIdeal->get(valueName + ".min", -1.);
      float max = fIdeal->get(valueName + ".max", -1.);
      hint = "Limits (" + lit::ToString<float>(min) + ", " + lit::ToString<float>(max) + ")";
   }

   std::stringstream ss;
   std::string value = lit::NumberToString(fQa->get(valueName, -1.), 1);
   ss << "<" << tag << " title=\"" << hint << "\" style=\"background-color:" << color
         << "\">" << valueTitle << value << "</" << tag << ">";
   return ss.str();
}

std::string CbmLitQaBaseGenerator::HtmlHeadString()
{
   std::string str = "<head><style type=\"text/css\">";
   str += "#efficiency";
   str += "{";
   str += "font-family:Verdana, Arial, Helvetica, sans-serif;";
//   str += "width:100%;";
   str += "border-collapse:collapse;";
   str += "}";
   str += "#efficiency td, #efficiency th";
   str += "{";
   str += "font-size:1em;";
   str += "border:1px solid #98bf21;";
   str += "padding:3px 7px 2px 7px;";
   str += "}";
   str += "#efficiency th";
   str += "{";
   str += "font-size:1.1em;";
   str += "text-align:left;";
   str += "padding-top:5px;";
   str += "padding-bottom:4px;";
   str += "background-color:#A7C942;";
   str += "color:#ffffff;";
   str += "}";
   str += "#efficiency tr.alt td";
   str += "{";
   str += "color:#000000;";
   str += "background-color:#EAF2D3;";
   str += "}";
   str += "</style></head>";
   return str;
}
