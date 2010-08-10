/********************************************************************
 * The Data Acquisition Backbone Core (DABC)
 ********************************************************************
 * Copyright (C) 2009-
 * GSI Helmholtzzentrum fuer Schwerionenforschung GmbH
 * Planckstr. 1
 * 64291 Darmstadt
 * Germany
 * Contact:  http://dabc.gsi.de
 ********************************************************************
 * This software can be used under the GPL license agreements as stated
 * in LICENSE.txt file which is part of the distribution.
 ********************************************************************/
#ifndef BNET_RocWorkerApplication
#define BNET_RocWorkerApplication

#include "bnet/MbsWorkerApplication.h"

#include "roc/BoardsVector.h"

namespace bnet {

   class RocWorkerApplication : public MbsWorkerApplication {

      public:
         RocWorkerApplication();

         virtual bool CreateReadout(const char* portname, int portnumber);

         virtual bool CreateCombiner(const char* name);

         virtual std::string GetCombinerOutputName(const char* name);

         virtual int ExecuteCommand(dabc::Command* cmd);

         virtual bool AfterAppModulesStopped();

         virtual bool BeforeAppModulesDestroyed();


//         virtual bool CreateBuilder(const char* name);

//         virtual bool CreateFilter(const char* name);
      protected:

         /** Due to optic number of ports can be less than number of ROCs specified in configuration
          * This function returns port number for specified ROC sequence number from configuration
          * For all optic connection return the only port number, which connected with abb device
          */
         int RecalculatePortNumber(int rocnumber);

         bool SwitchCalibrationMode(bool on);

         enum ECalibrationState { calNONE, calOFF, calON };


         int  fNumRealInputs; // real number of ports in combiner module
         int  fOpticPort; // port number where all optic data coming in

         /** This vector translate full roc number in actual Combiner port number (or in list of devices)
          * For second, third and so on optic connections value is equal to fOpticPort */
         std::vector<int> fPortMap;

         std::vector<std::string> fDevNamesDLM; /** List of created devices, only useful to send DLMs */
         roc::BoardsVector        fRocBrds; /** List of boards for put/get operations */
         std::vector<std::string> fDevNames; /** List of devices, 1-1 with number to rocs, can be duplication for optic */
         std::string              fCombinerName; /** name of combiner module */

         ECalibrationState fCalibrState;
   };
}

#endif
