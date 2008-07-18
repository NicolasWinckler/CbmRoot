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
///     - class for holding the trackToPeak analysis information
///
/// *******************************************************************
///
/// $Author: csteinle $
/// $Date: 2006/07/17 11:27:22 $
/// $Revision: 1.1 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _TRACKTOPEAK_H
#define _TRACKTOPEAK_H


/* **************************************************************
 * CLASS trackToPeak							 				*
 * **************************************************************/

class trackToPeak {

public:

	unsigned int trackToNoPeak;					/**< Variable to store the number of tracks which correspond to no peak. This means not found. */
	unsigned int trackToOnePeak;				/**< Variable to store the number of tracks which correspond to one peak. This means found once. */
	unsigned int trackToMorePeaks;				/**< Variable to store the number of tracks which correspond to more peaks. This means found more than once. */
	unsigned int numberOfTracks;				/**< Variable to store the number of tracks which are found and. */
	unsigned int numberOfWellFoundTracks;		/**< Variable to store the number of tracks which are found with more than 70% of their hits. */
	unsigned int numberOfWrongFoundTracks;		/**< Variable to store the number of tracks which are found but should not be found. */
	unsigned int numberOfTracksWithP;			/**< Variable to store the number of tracks which should be found. */

/**
 * Default constructor
 */

	trackToPeak();

/**
 * Constructor
 */

	trackToPeak(const trackToPeak& value);

/**
 * Destructor
 */

	virtual ~trackToPeak();

/**
 * operator = ()
 */

	const trackToPeak& operator = (const trackToPeak& value);

};

#endif
