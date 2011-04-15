/* CbmLitTask.h
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2010
 * @version 1.0
 *
 * Base task class for Littrack tasks which
 * implements common functionality to Littrack.
 */

#ifndef CBMLITTASK_H_
#define CBMLITTASK_H_

#include "FairTask.h"

class CbmLitTask : public FairTask
{
public:
   /* Constructor */
   CbmLitTask();

   /* Destructor */
   virtual ~CbmLitTask();

   /* Sets the output directory for images.
     * @param dir Directory name.
     */
   void SetOutputDir(const std::string& dir) { fOutputDir = dir;}

protected:
   /* Determines current CBM setup. */
   void DetermineSetup();

   Bool_t fIsElectronSetup; // If "electron" setup detected than true
   Bool_t fIsSts; // If STS detected than true
   Bool_t fIsTrd; // If TRD detected than true
   Bool_t fIsMuch; // If MUCH detected than true
   Bool_t fIsTof; // If TOF detected than truer

   Int_t fEventNo; // Event counter
   Int_t fVerbose; // Verbose level

   std::string fOutputDir; // output directory for images

   ClassDef(CbmLitTask, 1);
};

#endif /* CBMLITTASK_H_ */
