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
#ifndef ROC_READOUTAPPLICATION_H
#define ROC_READOUTAPPLICATION_H

#include "dabc/Application.h"
#include "dabc/Basic.h"
#include "dabc/threads.h"
#include "dabc/MemoryPool.h"
#include "dabc/Manager.h"
#include "dabc/Module.h"

#include "roc/Factory.h"
#include "roc/Board.h"
#include "roc/Commands.h"
#include "roc/BoardsVector.h"

namespace roc {

   class ReadoutApplication : public dabc::Application {
      public:
         ReadoutApplication();
         virtual ~ReadoutApplication();

         virtual bool IsModulesRunning();

         bool DoTaking() const { return GetParInt(xmlDoCalibr, 0) < 2; }
         bool DoCalibr() const { return GetParInt(xmlDoCalibr, 0) > 0; }
         int BNetCalibr() const { int v = GetParInt(xmlDoCalibr, 0); return v>100 ? v-100 : -1; }

         /** Number of ROCs connected to ReadoutModule*/
         int   NumRocs() const { return GetParInt(roc::xmlNumRocs, 1); }

         /** IP address for ROC of index*/
         std::string RocAddr(int index = 0) const;

         /** id number of Mbs server (stream, transport)*/
         int DataServerKind() const;

         std::string OutputFileName() const { return GetParStr(xmlRawFile, ""); }
         std::string CalibrFileName() const { return GetParStr(xmlCalibrFile, ""); }

         virtual bool CreateAppModules();
         virtual bool AfterAppModulesStopped();
         
         virtual bool BeforeAppModulesDestroyed();

         virtual int SMCommandTimeout() const { return 20; }

         virtual int ExecuteCommand(dabc::Command* cmd);

      protected:

         std::vector<std::string>  fDevNamesDLM; /** List of really created devices, can be used only for DLMS */
         roc::BoardsVector         fRocBrds; /** List of boards for put/get operations */
         std::vector<std::string>  fDevNames; /** List of devices, which correspond to full ROCs list */

         enum ECalibrationState { calNONE, calOFF, calON };

         ECalibrationState fCalibrState;

         bool SwitchCalibrationMode(bool on);
   };
}

#endif
