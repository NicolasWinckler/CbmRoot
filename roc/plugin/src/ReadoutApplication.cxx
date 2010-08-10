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
#include "roc/ReadoutApplication.h"

#include <time.h>


#include "dabc/Parameter.h"
#include "dabc/Command.h"
#include "dabc/timing.h"
#include "dabc/CommandsSet.h"
#include "dabc/Device.h"

#include "mbs/MbsTypeDefs.h"
#include "mbs/Factory.h"

#include "roc/defines_roc.h"
#include "roc/UdpBoard.h"
#include "roc/UdpDevice.h"
#include "roc/Commands.h"
#include "roc/OperList.h"
#include "roc/CombinerModule.h"


using namespace dabc;

roc::ReadoutApplication::ReadoutApplication() :
   dabc::Application(roc::xmlReadoutAppClass),
   fDevNamesDLM(),
   fRocBrds(),
   fDevNames()
{
   CreateParInt(roc::xmlDoCalibr, 1);

   CreateParInt(roc::xmlSyncScaleDown, -1);

   CreateParInt(roc::xmlNumRocs, 3);

   for (int nr=0; nr<NumRocs(); nr++) {
      CreateParStr(FORMAT(("%s%d", xmlRocIp, nr)));
      CreateParStr(FORMAT(("%s%d", xmlRocFebs, nr)));
   }
   CreateParInt(dabc::xmlBufferSize, 8192);
   CreateParInt(dabc::xmlNumBuffers, 100);

   CreateParStr(mbs::xmlServerKind, mbs::ServerKindToStr(mbs::StreamServer));

   CreateParStr(xmlRawFile, "");
   CreateParStr(xmlCalibrFile, "");
   CreateParInt(mbs::xmlSizeLimit, 0);

   CreateParInt(roc::xmlSpillRoc, -1);
   CreateParInt(roc::xmlSpillAux, -1);
   CreateParDouble(roc::xmlCalibrationPeriod, -1.);
   CreateParDouble(roc::xmlCalibrationLength, 0.5);
   
   DOUT3(("!!!! Readout application %s created !!!!", GetName()));
}

roc::ReadoutApplication::~ReadoutApplication()
{
   DOUT3(("!!!! Readout application %s  destructor !!!!", GetName()));
}

int roc::ReadoutApplication::DataServerKind() const
{
   return mbs::StrToServerKind(GetParStr(mbs::xmlServerKind).c_str());
}

std::string roc::ReadoutApplication::RocAddr(int nreadout) const
{
   if ((nreadout<0) || (nreadout>=NumRocs())) return "";
   return GetParStr(FORMAT(("%s%d", xmlRocIp, nreadout)));
}

bool roc::ReadoutApplication::CreateAppModules()
{
   fCalibrState = calNONE;

   DOUT2(("CreateAppModules starts..."));
   int res = false;
   dabc::Command* cmd;

   dabc::lgr()->SetLogLimit(10000000);

   dabc::mgr()->CreateMemoryPool(roc::xmlRocPool,
                                 GetParInt(dabc::xmlBufferSize, 8192),
                                 GetParInt(dabc::xmlNumBuffers, 100));

   fDevNamesDLM.clear();
   fRocBrds.clear();
   fDevNames.clear();

   int bnet_nodes = BNetCalibr();

   if (DoTaking()) {

      int numinputs(0), opticid(-1);

      for(int t=0; t<NumRocs(); t++) {
         if (roc::BoardConnector::CheckOptic(RocAddr(t).c_str()) != 0) {
            if (opticid<0) opticid = numinputs++; // only one input per all optics
         } else
            numinputs++;
      }

      cmd = new dabc::CmdCreateModule("roc::CombinerModule", "RocComb", "RocCombThrd");
      cmd->SetStr(dabc::xmlInputPoolName, roc::xmlRocPool);
      cmd->SetStr(dabc::xmlOutputPoolName, roc::xmlRocPool);
      cmd->SetInt(dabc::xmlNumInputs, numinputs);
      cmd->SetInt(dabc::xmlNumOutputs, 2);
      res = dabc::mgr()->Execute(cmd);
      DOUT3(("Create ROC combiner module = %s", DBOOL(res)));
      if(!res) return false;
      
      roc::CombinerModule* readout = dynamic_cast<roc::CombinerModule*> (dabc::mgr()->FindModule("RocComb"));
      if (readout==0) {
         EOUT(("Did not found readout module?"));
         return false;
      }

      std::string devname, devtype, thrdname, opticdevname;

      int intputid(0);

      dabc::Command* cmd = 0;

      for(int t=0; t<NumRocs(); t++) {

         std::string rocaddr = RocAddr(t);

         devname = dabc::format("Roc%ddev", t);

         bool isoptic = roc::BoardConnector::CheckOptic(rocaddr.c_str()) != 0;

         bool do_device_create = true;

         if (isoptic) {
            if (opticdevname.length()>0) {
               devname = opticdevname;

               cmd = new dabc::Command("AddExtraBoard");
               cmd->SetStr(roc::xmlBoardAddr, rocaddr);
               dabc::SetCmdReceiver(cmd, FORMAT(("%s/%s", dabc::Manager::DevicesFolderName(), opticdevname.c_str())));

               int rocid = dabc::mgr()->Execute(cmd);

               if (rocid<256) {
                  EOUT(("Cannot define ROCID of extra optic ROC"));
                  return false;
               }

               rocid-=256;
               DOUT0(("Extra ROC ID %d", rocid));
               do_device_create = false;

               cmd = new roc::CmdAddSlave(opticid, rocid);
               dabc::SetCmdReceiver(cmd, "RocComb");
               if (dabc::mgr()->Execute(cmd)!=cmd_true) {
                  EOUT(("Cannot add slave to RcoComb"));
                  return false;
               }

            } else

               opticdevname = devname;

            devtype = roc::typeAbbDevice;
            thrdname = dabc::format("RocAbbThrd%d", t);
         } else {
            devtype = roc::typeUdpDevice;
            thrdname = "RocReadoutThrd"; // dabc::format("RocDev%dThrd", t);
         }
         
         if (do_device_create) {
            cmd = new dabc::CmdCreateDevice(devtype.c_str(), devname.c_str(), thrdname.c_str());
            cmd->SetStr(roc::xmlBoardAddr, rocaddr);
            cmd->SetStr(roc::xmlRole, roc::roleToString(roleDAQ));

            if (!dabc::mgr()->Execute(cmd)) {
               EOUT(("Cannot create device %s for roc %s", devname.c_str(), rocaddr.c_str()));
               return false;
            }

            fDevNamesDLM.push_back(devname);

            cmd = new dabc::CmdCreateTransport(FORMAT(("RocComb/Input%d", intputid)), devname.c_str(), thrdname.c_str());
            cmd->SetStr(roc::xmlBoardAddr, rocaddr);

            if (!dabc::mgr()->Execute(cmd)) {
               EOUT(("Cannot connect readout module to device %s", devname.c_str()));
               return false;
            }

            intputid++;
         }

         cmd = new roc::CmdGetBoardPtr;
         cmd->SetKeepAlive();
         cmd->SetStr(roc::xmlBoardAddr, rocaddr);
         dabc::SetCmdReceiver(cmd, FORMAT(("%s/%s", dabc::Manager::DevicesFolderName(), devname.c_str())));
         roc::Board* brd = 0;
         if (dabc::mgr()->Execute(cmd))
            brd = (roc::Board*) cmd->GetPtr(roc::CmdGetBoardPtr::Board());
         dabc::Command::Finalise(cmd);

         if (brd==0) {
            EOUT(("Cannot get board pointer from device %s", devname.c_str()));
            return false;
         }

         fRocBrds.addBoard(brd, devname, GetParStr(FORMAT(("%s%d", xmlRocFebs, t))));

         for (int nfeb=0;nfeb<2;nfeb++)
            for (int nadc=0;nadc<fRocBrds.numAdc(t, nfeb); nadc++) {
               std::string parname = dabc::format("ROC%u_FEB%d_ADC%d", brd->rocNumber(), nfeb, nadc);

               if (!FindPar(parname.c_str())) {
                  dabc::RateParameter* par = new dabc::RateParameter(this, parname.c_str(), true, 5., "adc", 0, 0xfff, 4, 0);
                  par->SetDebugOutput(false);
               }
            }


         fDevNames.push_back(devname);
      }
      
      // SwitchCalibrationMode(false);
   }

   if (DoCalibr()) {

      cmd = new dabc::CmdCreateModule("roc::CalibrationModule", "RocCalibr", "RocCalibrThrd");
      cmd->SetInt(dabc::xmlNumInputs, bnet_nodes > 0 ? bnet_nodes : 1);
      cmd->SetInt(dabc::xmlNumOutputs, 2);
      res = dabc::mgr()->Execute(cmd);
      DOUT1(("Create ROC calibration module = %s", DBOOL(res)));
      if(!res) return false;

      if (DoTaking()) {
         res = dabc::mgr()->ConnectPorts("RocComb/Output0", "RocCalibr/Input0");
         DOUT1(("Connect readout and calibr modules = %s", DBOOL(res)));
         if(!res) return false;
      }
   }

   if (OutputFileName().length()>0) {
      if (bnet_nodes>0) {
         for (int port=0;port<bnet_nodes;port++) {
            cmd = new dabc::CmdCreateTransport(FORMAT(("RocCalibr/Input%d", port)), mbs::typeLmdInput);

            cmd->SetStr(mbs::xmlFileName, FORMAT(("%s_%d*.lmd", OutputFileName().c_str(), port+1)));

            res = dabc::mgr()->Execute(cmd);

            DOUT1(("Configure file %s for input %d in bnet calibration res = %s",
                  FORMAT(("%s_%d*.lmd", OutputFileName().c_str(), port+1)), port, DBOOL(res)));
            if(!res) return false;
         }

      } else {
         cmd = 0;
         if (DoTaking()) {
            cmd = new dabc::CmdCreateTransport("RocComb/Output1", mbs::typeLmdOutput);
            // no need to set extra size parameters - it will be taken from application
            // if (FileSizeLimit()>0) cmd->SetInt(mbs::xmlSizeLimit, FileSizeLimit());
         } else {
            cmd = new dabc::CmdCreateTransport("RocCalibr/Input0", mbs::typeLmdInput);
            // no need to extra set of buffer size - it will be taken from module itself
            //  cmd->SetInt(dabc::xmlBufferSize, GetParInt(dabc::xmlBufferSize, 8192));
         }

         cmd->SetStr(mbs::xmlFileName, OutputFileName().c_str());

         res = dabc::mgr()->Execute(cmd);

         DOUT1(("Create raw lmd %s file res = %s", (DoTaking() ? "output" : "input"), DBOOL(res)));
         if(!res) return false;
      }
   }

   if ((CalibrFileName().length()>0) && DoCalibr()) {

      const char* outtype = mbs::typeLmdOutput;

      if ((CalibrFileName().rfind(".root")!=std::string::npos) ||
          (CalibrFileName().rfind(".ROOT")!=std::string::npos))
              outtype = "roc::TreeOutput";

      cmd = new dabc::CmdCreateTransport("RocCalibr/Output1", outtype);

      cmd->SetStr(mbs::xmlFileName, CalibrFileName().c_str());
      // no need to set extra size parameters - it will be taken from application
      // if (FileSizeLimit()>0) cmd->SetInt(mbs::xmlSizeLimit, FileSizeLimit());

      res = dabc::mgr()->Execute(cmd);
      DOUT1(("Create calibr output file res = %s", DBOOL(res)));
      if(!res) return false;
   }


   if (DataServerKind() != mbs::NoServer) {

   ///// connect module to mbs server:
      const char* portname = DoCalibr() ? "RocCalibr/Output0" : "RocComb/Output0";
      cmd = new dabc::CmdCreateTransport(portname, mbs::typeServerTransport, "MbsServerThrd");

      // no need to set extra parameters - they will be taken from application !!!
//      cmd->SetStr(mbs::xmlServerKind, mbs::ServerKindToStr(DataServerKind())); //mbs::StreamServer ,mbs::TransportServer
//      cmd->SetInt(dabc::xmlBufferSize, GetParInt(dabc::xmlBufferSize, 8192));

      res = dabc::mgr()->Execute(cmd);
      DOUT2(("Connected %s output to Mbs server = %s", portname, DBOOL(res)));
      if(!res) return false;
   }

   return true;
}

bool roc::ReadoutApplication::AfterAppModulesStopped()
{
   if (fCalibrState == calON)
      SwitchCalibrationMode(false);
   return true;
}

bool roc::ReadoutApplication::BeforeAppModulesDestroyed()
{
   fRocBrds.returnBoards();
   fDevNames.clear();
   fDevNamesDLM.clear();

   return dabc::Application::BeforeAppModulesDestroyed();
}


bool roc::ReadoutApplication::IsModulesRunning()
{
   if (DoTaking())
      if (!dabc::mgr()->IsModuleRunning("RocComb")) return false;

   if (DoCalibr())
      if (!dabc::mgr()->IsModuleRunning("RocCalibr")) return false;

   return true;
}

int roc::ReadoutApplication::ExecuteCommand(dabc::Command* cmd)
{
   int res = cmd_false;

   if (cmd->IsName(roc::CmdCalibration::CmdName())) {
      bool flag = cmd->GetBool(roc::CmdCalibration::FlagName(), true);
      res = cmd_bool(SwitchCalibrationMode(flag));
   } else
      res = dabc::Application::ExecuteCommand(cmd);

   return res;
}

bool roc::ReadoutApplication::SwitchCalibrationMode(bool on)
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
         dabc::SetCmdReceiver(cmd, "RocComb");
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
