/** CbmRichFindRings.h
 * @author S. Das, contact Semen Lebedev <s.lebedev@gsi.de>
 * @since 2006
 * @version 1.0
 **/

#ifndef CBM_RICH_FIND_RINGS
#define CBM_RICH_FIND_RINGS 1

#include "FairTask.h"

class CbmRichRingFinder;
class TClonesArray;

class CbmRichFindRings : public FairTask
{
public:

  /* Default constructor */
  CbmRichFindRings();

  /* Standard constructor
   * @param finder Pointer to RICH ring finder concrete class
   * @param verbose Verbosity level */
  CbmRichFindRings(
        CbmRichRingFinder* finder,
        Int_t verbose = 1);

  /* Constructor with name and title
   * @param name Name of class
   * @param title Task title
   * @param finder Pointer to RICH ring finder concrete class
   * @param verbose Verbosity level */
  CbmRichFindRings(
        const char* name,
        const char* title = "FairTask",
        CbmRichRingFinder* finder = NULL,
        Int_t verbose = 1);

  /* Destructor */
  virtual ~CbmRichFindRings();

  /* Initialization at the beginning */
  virtual InitStatus Init();

  /* Task execution each event */
  virtual void Exec(
        Option_t* opt);

  /* Finish at the end */
  virtual void Finish();

  /* Accessors */
  CbmRichRingFinder* GetFinder() { return fFinder; };
  Int_t GetNofRings() { return fNofRings; };

  /* Set concrete track finder
   * @param finder Concrete ring finder*/
  void UseFinder(
        CbmRichRingFinder* finder) { fFinder = finder; };

private:
  CbmRichRingFinder* fFinder; // Pointer to RingFinder concrete class
  TClonesArray* fRichHitArray; // Input array of RICH hits
  TClonesArray* fProjArray; // Input array of projected tracks
  TClonesArray* fRingArray; // Output array of CbmRichRings
  Int_t fNofRings; // Number of rings created
  Int_t fVerbose; // Verbosity level

  ClassDef(CbmRichFindRings,1);

};

#endif
