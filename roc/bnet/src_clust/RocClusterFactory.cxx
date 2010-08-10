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
#include "bnet/RocClusterFactory.h"

#include "bnet/RocClusterApplication.h"

bnet::RocClusterFactory bnet_roc_cluster_factory("bnet-roc-cluster");

const char* bnet::xmlRocClusterClass = "bnet::RocCluster";

dabc::Application* bnet::RocClusterFactory::CreateApplication(const char* classname, dabc::Command* cmd)
{
   if (strcmp(classname, xmlRocClusterClass)==0)
      return new bnet::RocClusterApplication();

   return dabc::Factory::CreateApplication(classname, cmd);
}

