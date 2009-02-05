// -------------------------------------------------------------------------
// -----                   CbmMuchTrackFinder header file              -----
// -----                  Created 01/10/07  by A. Lebedev              -----
// -----                                                               -----
// -------------------------------------------------------------------------


/** CbmMuchTrackFinder
 *@author A.Lebedev <Andrey.Lebedev@gsi.de>
 **
 ** Abstract base class for concrete MUCH track finding algorithm.
 **/

#ifndef CBMMUCHTRACKFINDER_H
#define CBMMUCHTRACKFINDER_H 1

#include "TObject.h"

class TClonesArray;

class CbmMuchTrackFinder : public TObject
{

 public:

  /** Default constructor **/
  CbmMuchTrackFinder() { }; 


  /** Destructor **/
  virtual ~CbmMuchTrackFinder() { };


  /** Virtual method Init. If needed, to be implemented in the
   ** concrete class. Else no action.
   **/
  virtual void Init() { };


  /** Abstract method DoFind. To be implemented in the concrete class.
   ** Task: Read the hit array and fill the track array,
   ** pointers to which are given as arguments
   **/
  virtual Int_t DoFind(TClonesArray* hitArray,
                       TClonesArray* trackArray) = 0;


  ClassDef(CbmMuchTrackFinder,1);

};

#endif
