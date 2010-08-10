#include "roc/NxCalibrModule.h"

#include "dabc/Command.h"
#include "dabc/CommandsSet.h"

//#include "roc/Commands.h"
#include "roc/Message.h"
#include "roc/Board.h"

#include "roc/defines_roc.h"
#include "roc/defines_udp.h"

using namespace dabc;

roc::NxCalibrModule::NxCalibrModule(const char* name, dabc::Command* cmd, roc::Board* brd) :
   dabc::ModuleAsync(name, cmd),
   fBrd(brd),
   fWorkPeriod(100.),
   fCalibrPeriod(1.),
   fLoopCounts(0),
   fState(0)
{
   CreateTimer("CalibrTimer", -1., false);

   fWorkPeriod = GetCfgDouble("WorkPeriod", 100., cmd);
   fCalibrPeriod = GetCfgDouble("CalibrPeriod", 1., cmd);

   fLoopCounts = GetCfgInt("CalibrLoops", 0, cmd);
}

roc::NxCalibrModule::~NxCalibrModule()
{
}

int roc::NxCalibrModule::ExecuteCommand(dabc::Command* cmd)
{
   if (cmd->IsName("enableCalibration")) {
      fWorkPeriod = cmd->GetDouble("WorkPeriod", 100.);
      fCalibrPeriod = cmd->GetDouble("CalibrPeriod", 1.);
      fLoopCounts = cmd->GetInt("CalibrLoops", 0);

//      DOUT0(("Executing enableCalibration cnt:%d  state:%d", fLoopCounts, fState));

      if ((fLoopCounts!=0) && (fState==0)) {
         fState = 1;
         ShootTimer("CalibrTimer", fWorkPeriod);
         DOUT0(("Shoot timer %f", fWorkPeriod));

      } else
      if (fLoopCounts==0) {
        if (fState==2) {
           // when disabling, ensure that commands are executed
           switchCalibration(false, cmd);
           fState = 0;
           return cmd_postponed;
        } else
           fState = 0;
      }
      return cmd_true;
   }
   return dabc::ModuleAsync::ExecuteCommand(cmd);
}


void roc::NxCalibrModule::ProcessTimerEvent(dabc::Timer* timer)
{
//   DOUT0(("ProcessTimerEvent state:%d", fState));

   if (fState==0) return;

   if (fState==1) {
      if (fLoopCounts==0)
         fState = 0;
      else {
         switchCalibration(true);
         fState = 2;
         ShootTimer("CalibrTimer", fCalibrPeriod);
      }
   } else {
      switchCalibration(false);

      if (fLoopCounts>0) fLoopCounts--;
      if (fLoopCounts==0) {
         fState = 0;
      } else {
         ShootTimer("CalibrTimer", fWorkPeriod);
         fState = 1;
      }
   }
}


void roc::NxCalibrModule::switchCalibration(bool on, dabc::Command* master)
{
//   DOUT0(("switchCalibration %s", DBOOL(on)));

   if (fBrd==0) return;

   OperList lst;

   lst.addPut(ROC_ADDSYSMSG, roc::SYSMSG_USER_RECONFIGURE);
   lst.addPut(ROC_NX_FIFO_RESET, 1);
   lst.addPut(ROC_NX_FIFO_RESET, 0);
//   for (int n=0;n<1000;n++) 
      lst.addGet(ROC_HWV);
   
   lst.addPut(ROC_ADDSYSMSG, on ? roc::SYSMSG_USER_CALIBR_ON : roc::SYSMSG_USER_CALIBR_OFF);

   int res = fBrd->operGen(lst);

   if (master) dabc::Command::Reply(master, res==0);

}
