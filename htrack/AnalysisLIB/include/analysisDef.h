//////////////////////////////////////////////////////////////////////
/// (C)opyright 2004
/// 
/// Institute of Computer Science V
/// Prof. M‰nner
/// University of Mannheim, Germany
/// 
/// *******************************************************************
/// 
/// Designer(s):   Steinle / Gl‰ﬂ
/// 
/// *******************************************************************
/// 
/// Project:     Trackfinder for CBM-Project at GSI-Darmstadt, Germany
/// 
/// *******************************************************************
/// 
/// Description:
///
///   class:
///     - class for defining the analysis definitions
///
/// *******************************************************************
///
/// $Author: csteinle $
/// $Date: 2007-06-15 13:55:23 $
/// $Revision: 1.4 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _ANALYSISDEF_H
#define _ANALYSISDEF_H


//#define PEAKCONTRIBUTIONCOMPTABILITYMODE		/**< This definition enables the comptability with an older version of this software. */
#undef PEAKCONTRIBUTIONCOMPTABILITYMODE			/**< This definition switches to a new version of this software development */

//#define FOUNDTRACKISREALTRACKCOMPTABILITYMODE	/**< This definition enables the comptability with an older version of this software. */
#undef FOUNDTRACKISREALTRACKCOMPTABILITYMODE	/**< This definition switches to a new version of this software development */


#define WELLFOUNDHITFACTOR 0.7					/**< This defines that a track is well found, if more than WELLFOUNDHITFACTOR of the original hits of this track are found. */


/**
 * If defined, the trackIds of the not found tracks would be
 * written to the standard output stream.
 */

//#define WHICHTRACKIDISNOTFOUND
#undef WHICHTRACKIDISNOTFOUND


#define TRACKWITHHITINCORRECTBORDERDISTRIBUTION  (1 <<  0)	/**< This definition sets the bit position for enabling the output for the prelut goodness results for the correct borders. */
#define TRACKWITHHITINBORDERDISTRIBUTION         (1 <<  1)	/**< This definition sets the bit position for enabling the output for the prelut goodness results for the borders. */
#define MIDDLEDISTANCEOFMAXIMUMANDCORRECTLAYER   (1 <<  2)	/**< This definition sets the bit position for enabling the output of the lut goodness results for the middle distance of the maximum and the correct cells. */
#define TRACKWITHHITWITHCORRECTSLOPEDISTRIBUTION (1 <<  3)	/**< This definition sets the bit position for enabling the output for the prelut goodness results for the slopes. */
#define TRACKWITHYZPROJECTION                    (1 <<  4)	/**< This definition sets the bit position for enabling the output for the yz-projection of one track. */

#define TRACKWITHHITINCORRECTCELLDISTRIBUTION    (1 <<  5)	/**< This definition sets the bit position for enabling the output for the lut goodness results for the correct cells. */
#define TRACKWITHHITINCELLDISTRIBUTION           (1 <<  6)	/**< This definition sets the bit position for enabling the output for the lut goodness results for the cells. */
#define MIDDLEDISTANCEOFMAXIMUMANDCORRECTCELL    (1 <<  7)	/**< This definition sets the bit position for enabling the output of the lut goodness results for the middle distance of the maximum and the correct cells. */
#define TRACKWITHSIGNATUREDISTRIBUTION           (1 <<  8)	/**< This definition sets the bit position for enabling the output for the lut goodness results for the tracks' hit signature. */

#define TRACKWITHMOMENTAERRORDISTRIBUTION        (1 <<  9)	/**< This definition sets the bit position for enabling the output for the Hough transform goodness results for the tracks. */

#define TRACKWITHMOMENTADISTRIBUTION             (1 << 10)	/**< This definition sets the bit position for enabling the output for the quantization goodness results separate for each dimension. */
#define COMPLETEMOMENTADISTRIBUTION              (1 << 11)	/**< This definition sets the bit position for enabling the output for the complete quantization goodness results for all quantized momentas. */
#define COMPLETECOORDINATEDISTRIBUTION           (1 << 12)	/**< This definition sets the bit position for enabling the output for the complete quantization goodness results for all Hough coordinates. */
#define TRACKWITHSAMECOORDINATEDISTRIBUTION      (1 << 13)	/**< This definition sets the bit position for enabling the output of the quantization goodness results for the coordinates with more than one track. */
#define NUMBEROFTRACKSWHICHCANNOTBEFOUND         (1 << 14)	/**< This definition sets the bit position for enabling the output of the number of tracks which cannot be found because of the quantization goodness. */

#define TRACKSPERCOLUMNDISTRIBUTION              (1 << 15)	/**< This definition sets the bit position for enabling the output about the distribution of the number of found tracks per column. */

#define TRACKSPERLAYERDISTRIBUTION               (1 << 16)	/**< This definition sets the bit position for enabling the output about the distribution of the number of found tracks per histogram layer. */

#define LUTCORRECTIONCOUNTER                     (1 << 17)	/**< This definition sets the bit position for enabling the output about the number of corrections in the lut object. */

#define GOODSIGNATURES                           (1 << 18)	/**< This definition sets the bit position for enabling the output about the good signatures in the prelut and lut. */

#define GOODSIGNATURETABLES                      (1 << 19)	/**< This definition sets the bit position for enabling the output about the good signature tables in the prelut and lut. */

#define USEDSIGNATURETABLES                      (1 << 20)	/**< This definition sets the bit position for enabling the output about the used signature tables. */

#define HOUGHPICTURES                            (1 << 21)	/**> This definition sets the bit position for enabling the displaying of some static Hough transform graphics for the thesis. */


#endif
