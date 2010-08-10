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

#include "roc/Factory.h"

#include "roc/CombinerModule.h"
#include "roc/CalibrationModule.h"
#include "roc/ReadoutModule.h"
#include "roc/ReadoutApplication.h"
#include "roc/UdpDevice.h"

#include "dabc/Command.h"
#include "dabc/logging.h"

#include "mbs/LmdInput.h"
#include "roc/FileInput.h"

roc::Factory rocfactory("roc");

roc::Factory::Factory(const char* name) :
   dabc::Factory(name),
   roc::BoardConnector()
{
}

dabc::Application* roc::Factory::CreateApplication(const char* classname, dabc::Command* cmd)
{
   if (strcmp(classname, xmlReadoutAppClass)==0)
      return new roc::ReadoutApplication();

   return dabc::Factory::CreateApplication(classname, cmd);
}

dabc::Module* roc::Factory::CreateModule(const char* classname, const char* modulename, dabc::Command* cmd)
{
   DOUT2(("roc::Factory::CreateModule called for class:%s, module:%s", classname, modulename));

   if (strcmp(classname, "roc::CombinerModule")==0) {
      DOUT2(("roc::Factory::CreateModule - Created RocCombiner module %s ", modulename));
      return new roc::CombinerModule(modulename,cmd);
   } else
   if (strcmp(classname, "roc::CalibrationModule")==0) {
      DOUT2(("roc::Factory::CreateModule - Created roc::CalibrationModule module %s ", modulename));
      return new roc::CalibrationModule(modulename, cmd);
   } else
   if (strcmp(classname, "roc::ReadoutModule")==0) {
      DOUT2(("roc::Factory::CreateModule - Created roc::ReadoutModule module %s ", modulename));
      return new roc::ReadoutModule(modulename, cmd);
   }

   return 0;
}

dabc::Device* roc::Factory::CreateDevice(const char* classname, const char* devname, dabc::Command* cmd)
{
   const char* thrdname = cmd ? cmd->GetPar("Thread") : 0;

   if (strcmp(classname, roc::typeUdpDevice)==0) {
      DOUT2(("roc::Factory::CreateDevice - Creating ROC UdpDevice %s ...", devname));

      roc::UdpDevice* dev = new roc::UdpDevice(dabc::mgr()->GetDevicesFolder(true), devname, thrdname, cmd);

      if (!dev->IsConnected()) {
         dev->DestroyProcessor();
         return 0;
      }

      return dev;
   }

   return 0;
}

roc::Board* roc::Factory::DoConnect(const char* name, roc::ClientRole role)
{
   dabc::SetDebugLevel(0);

   if (dabc::mgr()==0)
      if (!dabc::Factory::CreateManager()) return 0;

   const char* subname = CheckFile(name);
   if (subname!=0) {

      char sbuf[1024];
      strcpy(sbuf, subname);

      int selectroc = -1;

      char* pos = strstr(sbuf,"&roc=");
      if (pos!=0) {
         selectroc = atoi(pos+5);
         *pos = 0;
      }

      DOUT0(("Creating LMD-file input %s selectroc = %d", sbuf, selectroc));

      mbs::LmdInput* inp = new mbs::LmdInput(sbuf, 0x100000);

      if (!inp->Init()) {
         EOUT(("Cannot create LMD input %s", sbuf));
         delete inp;
         return 0;
      }

      return new roc::FileInput(inp, selectroc);
   }

   subname = CheckUdp(name);
   if (subname==0) return 0; 

   //std::string devname = dabc::format("RocUdp%s", subname);
   std::string devname = "RocUdp";
   roc::UdpDevice* dev = dynamic_cast<roc::UdpDevice*> (dabc::mgr()->FindDevice(devname.c_str()));
   if (dev!=0) {
      EOUT(("UDP Device for board %s already exists!!!", subname));
      return 0;
   }

   DOUT3(("Create device"));

   dabc::Command* cmd = new dabc::CmdCreateDevice(typeUdpDevice, devname.c_str());
   cmd->SetStr(roc::xmlBoardAddr, subname);
   cmd->SetStr(roc::xmlRole, roleToString(role));
   if (!dabc::mgr()->Execute(cmd)) return 0;

   dev = dynamic_cast<roc::UdpDevice*> (dabc::mgr()->FindDevice(devname.c_str()));

   DOUT3(("Create device dev = %p", dev));

   if (dev==0) {
      EOUT(("UDP Device for board %s cannot be created!!!", subname));
      return 0;
   }

   if (!dev->InitAsBoard()) {
      dev->DestroyProcessor();
      return 0;
   }

   dabc::lgr()->SetLogLimit(1000000);

   return static_cast<roc::Board*> (dev);

}

bool roc::Factory::DoClose(roc::Board* brd)
{
   roc::FileInput* inp = dynamic_cast<roc::FileInput*> (brd);
   if (inp!=0) {
      delete inp;
      return true;
   }

   if (dabc::mgr()==0) return false;

   const char* devname = "RocUdp";

   roc::UdpDevice* dev = dynamic_cast<roc::UdpDevice*> (dabc::mgr()->FindDevice(devname));

   if ((dev==0) || ((roc::Board*) dev != brd)) {
      EOUT(("Board::Close FAIL"));
      return false;
   }

   dev->CloseAsBoard();

   if (!dev->Execute(roc::UdpDevice::CmdPutDisconnect, 4.)) return false;
   dev->DestroyProcessor();
   return true;
}
