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
#include "bnet/RocFactory.h"

#include "bnet/RocWorkerApplication.h"

bnet::RocFactory bnet_roc_factory("bnet-roc");

const char* bnet::xmlRocWorkerClass = "bnet::RocWorker";

dabc::Application* bnet::RocFactory::CreateApplication(const char* classname, dabc::Command* cmd)
{
   if (strcmp(classname, xmlRocWorkerClass)==0)
      return new bnet::RocWorkerApplication();

   return dabc::Factory::CreateApplication(classname, cmd);
}

dabc::Module* bnet::RocFactory::CreateModule(const char* classname, const char* modulename, dabc::Command* cmd)
{
   return dabc::Factory::CreateModule(classname, modulename, cmd);
}
