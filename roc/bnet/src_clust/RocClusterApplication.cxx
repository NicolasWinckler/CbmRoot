#include "bnet/RocClusterApplication.h"

#include "bnet/RocClusterFactory.h"
#include "roc/Commands.h"

#include "dabc/Manager.h"
#include "dabc/Configuration.h"

bnet::RocClusterApplication::RocClusterApplication() :
   bnet::ClusterApplication(xmlRocClusterClass)
{

}

int bnet::RocClusterApplication::ExecuteCommand(dabc::Command* cmd)
{

   if (cmd->IsName(roc::CmdCalibration::CmdName())) {

      bool flag = cmd->GetBool(roc::CmdCalibration::FlagName(), true);

      for (int n=0;n<NumWorkers();n++)
         if (IsWorkerActive(n)) {
            dabc::Command* gcmd = new roc::CmdGlobalCalibration(flag);
            dabc::SetCmdReceiver(gcmd, GetWorkerNodeName(n), dabc::xmlAppDfltName);
            dabc::mgr()->Submit(gcmd);
         }

      return dabc::cmd_true;
   }

   return bnet::ClusterApplication::ExecuteCommand(cmd);

}
