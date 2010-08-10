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
#include "bnet/RocWorkerApplication.h"

#include "dabc/logging.h"
#include "dabc/Manager.h"
#include "dabc/Device.h"
#include "dabc/Configuration.h"
#include "dabc/Parameter.h"

#include "bnet/GeneratorModule.h"

#include "roc/Board.h"
#include "roc/UdpBoard.h"
#include "roc/Commands.h"
#include "roc/CombinerModule.h"

#include "bnet/RocFactory.h"

bnet::RocWorkerApplication::RocWorkerApplication() :
   bnet::MbsWorkerApplication(xmlRocWorkerClass),
   fNumRealInputs(0),
   fOpticPort(-1),
   fPortMap(),
   fDevNamesDLM(),
   fRocBrds(),
   fDevNames(),
   fCombinerName(),
   fCalibrState(calNONE)
{
   // register application specific parameters here:

   CreateParInt(roc::xmlSyncScaleDown, 2);

   CreateParInt(roc::xmlSpillRoc, -1);
   CreateParInt(roc::xmlSpillAux, -1);
   CreateParDouble(roc::xmlCalibrationPeriod, -1.);
   CreateParDouble(roc::xmlCalibrationLength, 0.5);

   for (int nr=0;nr<NumReadouts();nr++)
      CreateParStr(FORMAT(("%s%d", roc::xmlRocFebs, nr)));
}

int bnet::RocWorkerApplication::RecalculatePortNumber(int rocnumber)
{
   return (rocnumber<0) || ((unsigned)rocnumber>=fPortMap.size()) ? -1 : fPortMap[rocnumber];
}


bool bnet::RocWorkerApplication::CreateCombiner(const char* name)
{
   fCombinerName = name;

   fOpticPort = -1;
   fNumRealInputs = 0;

   fPortMap.clear();
   fDevNamesDLM.clear();
   fRocBrds.clear();
   fDevNames.clear();

   std::string devname;

   for(int t=0; t<NumReadouts(); t++) {
      if (roc::BoardConnector::CheckOptic(ReadoutPar(t).c_str()) != 0) {
         devname = "AbbDev";
         if (fOpticPort<0) {
            fOpticPort = fNumRealInputs++; // only one input per all optics
            fDevNamesDLM.push_back(devname);
         }
         fPortMap.push_back(fOpticPort);
      } else {
         devname = dabc::format("RocUdpDev%d", t);
         fPortMap.push_back(fNumRealInputs);
         fNumRealInputs++;
         fDevNamesDLM.push_back(devname);
      }
      fDevNames.push_back(devname);
   }

   dabc::Command* cmd = new dabc::CmdCreateModule("roc::CombinerModule", name, "RocCombThrd");
   cmd->SetBool(roc::xmlBnetMode, true);
   cmd->SetStr(dabc::xmlInputPoolName, GetParStr(bnet::CfgReadoutPool, bnet::ReadoutPoolName).c_str());
   cmd->SetStr(dabc::xmlOutputPoolName, bnet::TransportPoolName);
   cmd->SetInt(dabc::xmlBufferSize, GetParInt(bnet::xmlTransportBuffer, 16*1024));
   cmd->SetInt(dabc::xmlNumInputs, fNumRealInputs);
   cmd->SetInt(dabc::xmlNumOutputs, 1);
   DOUT0(("Start Create ROC combiner module"));
   bool res = dabc::mgr()->Execute(cmd);
   DOUT0(("Create ROC combiner module = %s", DBOOL(res)));
   return res;
}


bool bnet::RocWorkerApplication::CreateReadout(const char* portname, int portnumber)
{
   // this method is expects, when number of port is equal to number of ROCs
   // due to optic device, which combines data from all ROCs in one port,
   // total number of ports can be less
   // We keep portnames as is and just define that rocid should be configured for that port
   // At the same time, for second, third and so on optic connections we should do extra work
   // witch optic device

   if (IsGenerator()) return false;

   dabc::Command* cmd = 0;

   // create devices for actual ports
   if (portnumber<fNumRealInputs) {

      // check number in list, which correspond to portname/portnumber
      int rocnumber = 0;
      for (unsigned n=0;n<fPortMap.size();n++)
         if (fPortMap[n] == portnumber) { rocnumber = n; break; }

      const char* devtype = 0;
      const char* thrdname = 0;
      const char* devname = fDevNames[rocnumber].c_str();

      // this is optic device
      if (portnumber==fOpticPort) {
         devtype = roc::typeAbbDevice;
         thrdname = "RocReadoutAbbThrd";
      } else {
         devtype = roc::typeUdpDevice;
         thrdname = "RocReadoutUdpThrd";
      }

      cmd = new dabc::CmdCreateDevice(devtype, devname, thrdname);
      cmd->SetStr(roc::xmlBoardAddr, ReadoutPar(rocnumber));
      cmd->SetStr(roc::xmlRole, roc::roleToString(roc::roleDAQ));

      if (!dabc::mgr()->Execute(cmd)) {
         EOUT(("Cannot create device %s for rocnumber %d", devname, rocnumber));
         return false;
      }
      cmd = new dabc::CmdCreateTransport(portname, devname, thrdname);
      cmd->SetStr(roc::xmlBoardAddr, ReadoutPar(rocnumber));

      if (!dabc::mgr()->Execute(cmd)) {
         EOUT(("Cannot connect readout module to device %s", devname));
         return false;
      }
   }

   // this means, this is second, third optic board
   if ((fPortMap[portnumber]==fOpticPort) && (portnumber!=fOpticPort)) {
      cmd = new dabc::Command("AddExtraBoard");
      cmd->SetStr(roc::xmlBoardAddr, ReadoutPar(portnumber));
      dabc::SetCmdReceiver(cmd, FORMAT(("%s/%s", dabc::Manager::DevicesFolderName(), fDevNames[portnumber].c_str())));
      int rocid = dabc::mgr()->Execute(cmd);

      if (rocid<256) {
         EOUT(("Cannot define ROCID of extra optic ROC"));
         return false;
      }
      rocid-=256;

      DOUT0(("Extra ROC ID %d", rocid));

      cmd = new roc::CmdAddSlave(fOpticPort, rocid);
      dabc::SetCmdReceiver(cmd, fCombinerName.c_str());
      if (dabc::mgr()->Execute(cmd)!=dabc::cmd_true) {
         EOUT(("Cannot add slave to combiner module"));
         return false;
      }
   }

   cmd = new roc::CmdGetBoardPtr;
   cmd->SetKeepAlive();
   cmd->SetStr(roc::xmlBoardAddr, ReadoutPar(portnumber));
   dabc::SetCmdReceiver(cmd, FORMAT(("%s/%s", dabc::Manager::DevicesFolderName(), fDevNames[portnumber].c_str())));
   roc::Board* brd = 0;
   if (dabc::mgr()->Execute(cmd))
      brd = (roc::Board*) cmd->GetPtr(roc::CmdGetBoardPtr::Board());
   dabc::Command::Finalise(cmd);

   if (brd==0) {
      EOUT(("Cannot get board pointer from device %s", fDevNames[portnumber].c_str()));
      return false;
   }

   fRocBrds.addBoard(brd, fDevNames[portnumber], GetParStr(FORMAT(("%s%d", roc::xmlRocFebs, portnumber))));


   for (int nfeb=0;nfeb<2;nfeb++)
      for (int nadc=0;nadc<fRocBrds.numAdc(portnumber, nfeb);nadc++) {
         std::string parname = dabc::format("ROC%u_FEB%d_ADC%d", brd->rocNumber(), nfeb, nadc);

         if (!FindPar(parname.c_str())) {
            dabc::RateParameter* par = new dabc::RateParameter(this, parname.c_str(), true, 5., "adc", 0, 0xfff, 4, 0);
            par->SetDebugOutput(false);
         }
      }


   return true;
}

std::string bnet::RocWorkerApplication::GetCombinerOutputName(const char* name)
{
   return dabc::format("%s/Output0", name);
}

int bnet::RocWorkerApplication::ExecuteCommand(dabc::Command* cmd)
{
   if (cmd->IsName(roc::CmdCalibration::CmdName())) {
//      bool flag = cmd->GetBool(roc::CmdCalibration::FlagName(), true);
//      dabc::Command* gcmd = new roc::CmdGlobalCalibration(flag);
//      dabc::SetCmdReceiver(gcmd, GetParStr(CfgClusterMgr).c_str(), dabc::xmlAppDfltName);
//      dabc::mgr()->Submit(gcmd);
//      return dabc::cmd_true;
      dabc::SetCmdReceiver(cmd, GetParStr(CfgClusterMgr).c_str(), dabc::xmlAppDfltName);
      dabc::mgr()->Submit(cmd);
      return dabc::cmd_postponed;
   } else
   if (cmd->IsName(roc::CmdGlobalCalibration::CmdName())) {
      bool flag = cmd->GetBool(roc::CmdGlobalCalibration::FlagName(), true);
      SwitchCalibrationMode(flag);
      return dabc::cmd_true;
   }

   return bnet::MbsWorkerApplication::ExecuteCommand(cmd);
}

bool bnet::RocWorkerApplication::AfterAppModulesStopped()
{
   if (fCalibrState == calON)
      SwitchCalibrationMode(false);
   return bnet::MbsWorkerApplication::AfterAppModulesStopped();
}

bool bnet::RocWorkerApplication::SwitchCalibrationMode(bool on)
{
   if ((fCalibrState == calON) && on) return true;

   if ((fCalibrState == calOFF) && !on) return true;

   DOUT0(("SwitchCalibration %s %5.2f", DBOOL(on), TimeStamp()*1e-6));

   if (on) {
      roc::MessagesVector* vect = fRocBrds.readoutExtraMessages();

      if (vect!=0) {

         for (unsigned n=0; n<vect->size(); n++)
            if (vect->at(n).isSysMsg() && vect->at(n).getSysMesType()==roc::SYSMSG_ADC) {
               uint32_t val = vect->at(n).getSysMesData();

               std::string parname = dabc::format("ROC%u_FEB%d_ADC%d", vect->at(n).getRocNumber(), val >> 31, (val >> 24) & 0x7f);

               SetParStr(parname.c_str(), dabc::format("%u", val & 0xffffff));

//               dabc::RateParameter* rate = dynamic_cast<dabc::RateParameter*> (FindPar(parname.c_str()));
//               if (rate) rate->ChangeRate(val & 0xffffff);
            }


         dabc::Command* cmd = new roc::CmdMessagesVector(vect);
         dabc::SetCmdReceiver(cmd, fCombinerName.c_str());
         dabc::mgr()->Submit(cmd);
      }
   }

   fRocBrds.autoped_switch(on);

/*
   for (unsigned n=0;n<fDevNamesDLM.size();n++) {
      dabc::Command* cmd = new roc::CmdDLM(on ? 10 : 11);

      dabc::SetCmdReceiver(cmd, FORMAT(("%s/%s", dabc::Manager::DevicesFolderName(), fDevNamesDLM[n].c_str())));

      dabc::mgr()->Submit(cmd);
   }
*/

   fCalibrState = on ? calON : calOFF;

   return true;
}


bool bnet::RocWorkerApplication::BeforeAppModulesDestroyed()
{
   fRocBrds.returnBoards();

   fDevNames.clear();
   fDevNamesDLM.clear();

   fCombinerName.clear();

   return bnet::MbsWorkerApplication::BeforeAppModulesDestroyed();
}

