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
#ifndef BNET_RocClusterApplication
#define BNET_RocClusterApplication

#include "bnet/ClusterApplication.h"

namespace bnet {

   class RocClusterApplication : public bnet::ClusterApplication {

      public:
         RocClusterApplication();

         virtual int ExecuteCommand(dabc::Command* cmd);

   };
}

#endif
