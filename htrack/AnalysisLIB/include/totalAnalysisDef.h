//////////////////////////////////////////////////////////////////////
/// (C)opyright 2004
/// 
/// Institute of Computer Science V
/// Prof. M�ner
/// University of Mannheim, Germany
/// 
/// *******************************************************************
/// 
/// Designer(s):   Steinle / Gl�
/// 
/// *******************************************************************
/// 
/// Project:     Trackfinder for CBM-Project at GSI-Darmstadt, Germany
/// 
/// *******************************************************************
/// 
/// Description:
///
///   file:
///     - global definition of structs, defines and enums for project
///
/// *******************************************************************
///
/// $Author: csteinle $
/// $Date: 2007-06-06 14:18:41 $
/// $Revision: 1.4 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _TOTALANALYSISDEF_H
#define _TOTALANALYSISDEF_H


#define CORRECTIONCOUNTERNAME       "CCDistribution"												/**< Defines the name of the display */
#define CORRECTIONCOUNTERTITLE      "Correction Counter Distribution"								/**< Defines the title for the display */
#define CORRECTIONCOUNTERXAXITITLE  "Dim1/Dim2-ratio"												/**< Defines the text to display for the title of the x-axi */
#define CORRECTIONCOUNTERYAXITITLE  "Number of corrected transformations"							/**< Defines the text to display for the title of the y-axi */

#define numberOfCorrectionCounters 20
const double correctionCounterSteps[numberOfCorrectionCounters] = {0.25, 0.5, 0.75, 1.0, 1.25, 1.5, 1.75, 2.0, 2.25 , 2.5, 2.75, 3.0, 3.25, 3.5, 3.75, 4.0, 4.25, 4.5, 4.75, 5.0};
const double correctionCounterValues[numberOfCorrectionCounters] = {10, 20, 30, 40, 50, 60, 70, 80, 90 , 100, 110, 120, 130, 140, 150, 160, 170, 180, 190, 200};


#define NOTFINDABLETRACKSNAME          "NFTDistribution"											/**< Defines the name of the display */
#define NOTFINDABLETRACKSTITLE         "Not Findable Tracks Distribution"							/**< Defines the title for the display */
#define NOTFINDABLETRACKSXAXITITLE     "Quantization [dim2|dim3]"									/**< Defines the text to display for the title of the x-axi */
#define NOTFINDABLETRACKSYAXITITLE     "Number of not findable tracks"								/**< Defines the text to display for the title of the y-axi */

#define numberOfTracksWhichArenotFindable 20
const double tracksWhichAreNotFindableSteps[numberOfTracksWhichArenotFindable] = {10, 20, 30, 40, 50, 60, 70, 80, 90 , 100, 110, 120, 130, 140, 150, 160, 170, 180, 190, 200};
const double tracksWhichAreNotFindableValues[numberOfTracksWhichArenotFindable] = {100, 200, 300, 400, 500, 600, 700, 800, 900 , 1000, 1100, 1200, 1300, 1400, 1500, 1600, 1700, 1800, 1900, 2000};


#define TRACKSWITHGOODPRELUTSIGNATURENAME          "TWGPSDistribution"								/**< Defines the name of the display */
#define TRACKSWITHGOODPRELUTSIGNATURETITLE         "Tracks With Good Prelut Signature Distribution"	/**< Defines the title for the display */
#define TRACKSWITHGOODPRELUTSIGNATUREXAXITITLE     "Quantization [dim2|dim3]"						/**< Defines the text to display for the title of the x-axi */
#define TRACKSWITHGOODPRELUTSIGNATUREYAXITITLE     "Number of tracks"								/**< Defines the text to display for the title of the y-axi */

#define numberOfTracksWithAGoodPrelutSignature 20
const double tracksWithAGoodPrelutSignatureSteps[numberOfTracksWithAGoodPrelutSignature] = {10, 20, 30, 40, 50, 60, 70, 80, 90 , 100, 110, 120, 130, 140, 150, 160, 170, 180, 190, 200};
const double tracksWithAGoodPrelutSignatureValues[numberOfTracksWithAGoodPrelutSignature] = {100, 200, 300, 400, 500, 600, 700, 800, 900 , 1000, 1100, 1200, 1300, 1400, 1500, 1600, 1700, 1800, 1900, 2000};


#define TRACKSWITHGOODLUTSIGNATURENAME          "TWGLSDistribution"									/**< Defines the name of the display */
#define TRACKSWITHGOODLUTSIGNATURETITLE         "Tracks With Good Lut Signature Distribution"		/**< Defines the title for the display */
#define TRACKSWITHGOODLUTSIGNATUREXAXITITLE     "Quantization [dim2|dim3]"							/**< Defines the text to display for the title of the x-axi */
#define TRACKSWITHGOODLUTSIGNATUREYAXITITLE     "Number of tracks"									/**< Defines the text to display for the title of the y-axi */

#define numberOfTracksWithAGoodLutSignature 20
const double tracksWithAGoodLutSignatureSteps[numberOfTracksWithAGoodLutSignature] = {10, 20, 30, 40, 50, 60, 70, 80, 90 , 100, 110, 120, 130, 140, 150, 160, 170, 180, 190, 200};
const double tracksWithAGoodLutSignatureValues[numberOfTracksWithAGoodLutSignature] = {100, 200, 300, 400, 500, 600, 700, 800, 900 , 1000, 1100, 1200, 1300, 1400, 1500, 1600, 1700, 1800, 1900, 2000};


#define GOODPRELUTSIGNATURESNAME          "GPSDistribution"											/**< Defines the name of the display */
#define GOODPRELUTSIGNATURESTITLE         "Good Prelut Signature Distribution"						/**< Defines the title for the display */
#define GOODPRELUTSIGNATURESXAXITITLE     "Quantization [dim2|dim3]"								/**< Defines the text to display for the title of the x-axi */
#define GOODPRELUTSIGNATURESYAXITITLE     "Number of tracks"										/**< Defines the text to display for the title of the y-axi */

#define numberOfGoodPrelutSignatures 20
const double goodPrelutSignaturesSteps[numberOfGoodPrelutSignatures] = {10, 20, 30, 40, 50, 60, 70, 80, 90 , 100, 110, 120, 130, 140, 150, 160, 170, 180, 190, 200};
const double goodPrelutSignaturesValues[numberOfGoodPrelutSignatures] = {100, 200, 300, 400, 500, 600, 700, 800, 900 , 1000, 1100, 1200, 1300, 1400, 1500, 1600, 1700, 1800, 1900, 2000};


#define GOODLUTSIGNATURESNAME          "GLSDistribution"											/**< Defines the name of the display */
#define GOODLUTSIGNATURESTITLE         "Good Lut Signature Distribution"							/**< Defines the title for the display */
#define GOODLUTSIGNATURESXAXITITLE     "Quantization [dim2|dim3]"									/**< Defines the text to display for the title of the x-axi */
#define GOODLUTSIGNATURESYAXITITLE     "Number of tracks"											/**< Defines the text to display for the title of the y-axi */

#define numberOfGoodLutSignatures 20
const double goodLutSignaturesSteps[numberOfGoodLutSignatures] = {10, 20, 30, 40, 50, 60, 70, 80, 90 , 100, 110, 120, 130, 140, 150, 160, 170, 180, 190, 200};
const double goodLutSignaturesValues[numberOfGoodLutSignatures] = {100, 200, 300, 400, 500, 600, 700, 800, 900 , 1000, 1100, 1200, 1300, 1400, 1500, 1600, 1700, 1800, 1900, 2000};


#define GOODPRELUTSIGNATURETABLENAME          "GPSTDistribution"									/**< Defines the name of the display */
#define GOODPRELUTSIGNATURETABLETITLE         "Good Prelut Signature Table Distribution"			/**< Defines the title for the display */
#define GOODPRELUTSIGNATURETABLEXAXITITLE     "Signature"											/**< Defines the text to display for the title of the x-axi */
#define GOODPRELUTSIGNATURETABLEYAXITITLE     "Frequency of occurence"								/**< Defines the text to display for the title of the y-axi */


#define GOODLUTSIGNATURETABLENAME          "GLSTDistribution"										/**< Defines the name of the display */
#define GOODLUTSIGNATURETABLETITLE         "Good Lut Signature Table Distribution"					/**< Defines the title for the display */
#define GOODLUTSIGNATURETABLEXAXITITLE     "Signature"												/**< Defines the text to display for the title of the x-axi */
#define GOODLUTSIGNATURETABLEYAXITITLE     "Frequency of occurence"									/**< Defines the text to display for the title of the y-axi */


#define CODINGTABLENAME            "UCTDistribution"										/**< Defines the name of the display */
#define CODINGTABLETITLE           "Used CodingTable Distribution"					/**< Defines the title for the display */
#define CODINGTABLEXAXITITLE       "Signature"												/**< Defines the text to display for the title of the x-axi */
#define CODINGTABLEYAXITITLE       "Classification"									/**< Defines the text to display for the title of the y-axi */


#define GRADINGPTABLENAME          "UGPTDistribution"										/**< Defines the name of the display */
#define GRADINGPTABLETITLE         "Used GradingPTable Distribution"					/**< Defines the title for the display */
#define GRADINGPTABLEXAXITITLE     "Signature"												/**< Defines the text to display for the title of the x-axi */
#define GRADINGPTABLEYAXITITLE     "Classification"									/**< Defines the text to display for the title of the y-axi */


#define GRADINGRTABLENAME          "UGRTDistribution"										/**< Defines the name of the display */
#define GRADINGRTABLETITLE         "Used GradingRTable Distribution"					/**< Defines the title for the display */
#define GRADINGRTABLEXAXITITLE     "Signature"												/**< Defines the text to display for the title of the x-axi */
#define GRADINGRTABLEYAXITITLE     "Classification"									/**< Defines the text to display for the title of the y-axi */


#endif
