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

#ifndef ROC_COMMANDS_H
#define ROC_COMMANDS_H


#ifndef DABC_Command
#include "dabc/Command.h"
#endif

#include <stdint.h>

namespace roc {

   class OperList;

   extern const char* AddrPar;
   extern const char* ValuePar;
   extern const char* ErrNoPar;
   extern const char* TmoutPar;
   extern const char* RawDataPar;
   extern const char* OperListPar;

   extern const char* xmlDoCalibr;
   extern const char* xmlRocIp;
   extern const char* xmlRocFebs;
   extern const char* xmlRawFile;
   extern const char* xmlCalibrFile;
   extern const char* xmlReadoutAppClass;

   class CmdPut : public dabc::Command {
     public:

        static const char* CmdName() { return "CmdPut"; }

        CmdPut(uint32_t address, uint32_t value, double tmout = 5.) :
            dabc::Command(CmdName())
        {
           SetUInt(AddrPar, address);
           SetUInt(ValuePar, value);
           SetDouble(TmoutPar, tmout);
        }
   };

  class CmdGet : public dabc::Command {
     public:

        static const char* CmdName() { return "CmdGet"; }

        CmdGet(uint32_t address, double tmout = 5.) :
            dabc::Command(CmdName())
        {
           SetUInt(AddrPar, address);
           SetDouble(TmoutPar, tmout);
        }
   };

  class CmdNOper : public dabc::Command {
     public:

        static const char* CmdName() { return "CmdNOper"; }

        CmdNOper(OperList* lst, double tmout = 5.) :
            dabc::Command(CmdName())
        {
           SetPtr(OperListPar, lst);
           SetDouble(TmoutPar, tmout);
        }
  };


  class CmdDLM : public dabc::Command {
     public:

        static const char* CmdName() { return "CmdDLM"; }

        CmdDLM(unsigned num, double tmout = 5.) :
            dabc::Command(CmdName())
        {
           SetUInt(AddrPar, num);
           SetDouble(TmoutPar, tmout);
        }
  };

  class CmdCalibration : public dabc::Command {
     public:

        static const char* CmdName() { return "RocCalibration"; }

        static const char* FlagName() { return "CalibrFlag"; }

        CmdCalibration(bool on) :
           dabc::Command(CmdName())
        {
           SetBool(FlagName(), on);
        }
  };

  class CmdGlobalCalibration : public dabc::Command {
     public:

        static const char* CmdName() { return "RocGlobalCalibration"; }

        static const char* FlagName() { return "CalibrFlag"; }

        CmdGlobalCalibration(bool on) :
           dabc::Command(CmdName())
        {
           SetBool(FlagName(), on);
        }
   };

  class CmdGetBoardPtr : public dabc::Command {
     public:

        static const char* CmdName() { return "GetBoardPtr"; }

        static const char* Board() { return "Board"; }

        CmdGetBoardPtr() :
           dabc::Command(CmdName())
        {
        }
   };

  class CmdReturnBoardPtr : public dabc::Command {
     public:

        static const char* CmdName() { return "ReturnBoardPtr"; }

        static const char* Board() { return "Board"; }

        CmdReturnBoardPtr() :
           dabc::Command(CmdName())
        {
        }
   };

  class CmdMessagesVector : public dabc::Command {
     public:

        static const char* CmdName() { return "MessagesVector"; }

        static const char* Vector() { return "Vecotr"; }

        CmdMessagesVector(void* vect) :
           dabc::Command(CmdName())
        {
           SetPtr(Vector(), vect);
        }
   };

  class CmdAddSlave : public dabc::Command {
     public:

        static const char* CmdName() { return "AddSlave"; }
        static const char* Master() { return "Master"; }
        static const char* RocId() { return "RocId"; }

        CmdAddSlave(unsigned master, unsigned rocid) :
           dabc::Command(CmdName())
        {
           SetUInt(Master(), master);
           SetUInt(RocId(), rocid);
        }
   };


}

#endif

