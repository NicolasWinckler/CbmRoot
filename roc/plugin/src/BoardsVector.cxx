#include "roc/BoardsVector.h"

#include "dabc/Manager.h"
#include "dabc/Command.h"

#include "roc/Commands.h"
#include "roc/Gpio.h"
#include "nxyter/RocNx.h"

#include "roc/defines_roc.h"

bool roc::BoardsVector::addBoard(roc::Board* brd, std::string devname, std::string febkinds)
{
   if (brd==0) return false;

   BoardRec rec(brd);
   rec.devname = devname;

   if (febkinds.length()>0) {
      std::string feb0kind, feb1kind;

      unsigned pos = febkinds.find(",");
      if (pos == std::string::npos) {
         feb0kind = febkinds;
      } else {
         feb0kind = febkinds.substr(0, pos);
         feb1kind = febkinds.substr(pos+1, febkinds.length()-pos);
      }
      
//      DOUT0(("kind0 = %s kind1 = %s", feb0kind.c_str(), feb1kind.c_str()));

      if (feb0kind.length()>0) {
         int kind = nxyter::FebBase::stringToType(feb0kind.c_str());
         rec.feb0 = nxyter::FebBase::newFeb(brd, 0, kind);
         DOUT0(("kind0 = %d feb0 = %p", kind, rec.feb0));
      }

      if (feb1kind.length()>0) {
         int kind = nxyter::FebBase::stringToType(feb1kind.c_str());
         rec.feb1 = nxyter::FebBase::newFeb(brd, 1, kind);
         DOUT0(("kind1 = %d feb1 = %p", kind, rec.feb1));
      }
   }


   // setup AUX2

   if (rec.feb0 || rec.feb1) {

      roc::Gpio gpio(brd);

      gpio.getConfig(rec.gpio_mask_save);
      bool riseedge, falledge, throttled, extrafunc, altin;
      roc::Gpio::unpackConfig(rec.gpio_mask_save, 6,
            riseedge, falledge, throttled, extrafunc, altin);

      extrafunc = true;
      altin     = true;
      rec.gpio_mask_on = rec.gpio_mask_save;
      roc::Gpio::packConfig(rec.gpio_mask_on, 6,
            riseedge, falledge, throttled, extrafunc, altin);

      extrafunc = false;
      altin     = false;
      rec.gpio_mask_off = rec.gpio_mask_save;
      roc::Gpio::packConfig(rec.gpio_mask_off, 6,
            riseedge, falledge, throttled, extrafunc, altin);

      // setup test pulse generator
      nxyter::RocNx rocnx(brd);
      int32_t period=250000;              // 1 khz
      rocnx.fireTestPulse(period, 128, 0);
   }

   push_back(rec);

   return true;
};

void roc::BoardsVector::returnBoards()
{
   for (unsigned n=0;n<size();n++) {

      if (at(n).brd == 0) continue;
      
      if (at(n).feb0 || at(n).feb1) {
         roc::Gpio gpio(at(n).brd);
         gpio.setConfig(at(n).gpio_mask_save);

         nxyter::RocNx rocnx(at(n).brd);

         // stop test pulse generator
         rocnx.fireTestPulse(0, 0, 0);
      }

      delete at(n).feb0;

      delete at(n).feb1;

      dabc::Command* cmd = new roc::CmdReturnBoardPtr;
      cmd->SetPtr(roc::CmdReturnBoardPtr::Board(), at(n).brd);
      dabc::SetCmdReceiver(cmd, FORMAT(("%s/%s", dabc::Manager::DevicesFolderName(), at(n).devname.c_str())));
      dabc::mgr()->Execute(cmd);
   }
   clear();
}

int roc::BoardsVector::numAdc(unsigned n, unsigned nfeb)
{
   if (n>=size()) return 0;
   nxyter::FebBase* feb = 0;
   if (nfeb==0) feb = at(n).feb0; else
   if (nfeb==1) feb = at(n).feb1;
   return feb ? feb->getNumMonAdc() : 0;
}


void roc::BoardsVector::readFeb(uint32_t rocid, uint32_t febid, nxyter::FebBase* feb, MessagesVector* vect)
{
   if ((feb==0) || !feb->monAdcSupport()) return;

   roc::Message msg;

   static uint16_t adccnt = 0;

   for (int nch=0;nch<feb->getNumMonAdc(); nch++) {
      uint16_t val = 0;
      // if (feb->getMonAdc(nch, val)!=0) continue;

      // just to exclude real readings
      val = (adccnt++) & 0xfff;

//      DOUT0(("READ ADC roc %u feb %u adc %u value %u", rocid, febid, nch, val));

      msg.reset();
      msg.setMessageType(roc::MSG_SYS);
      msg.setRocNumber(rocid);
      msg.setSysMesType(roc::SYSMSG_ADC);
      msg.setSysMesData((febid << 31) | (nch << 24) | val);
      vect->push_back(msg);
   }
}

roc::MessagesVector* roc::BoardsVector::readoutExtraMessages()
{
   if (size()==0) return 0;

   roc::MessagesVector* vect = new roc::MessagesVector;

   DOUT5(("Readout extra messages"));

   roc::Message msg;

   for (unsigned n=0;n<size();n++) {

      if (at(n).brd == 0) continue;

      uint32_t rocid = at(n).brd->rocNumber();

      msg.reset();
      msg.setMessageType(roc::MSG_SYS);
      msg.setRocNumber(rocid);
      msg.setSysMesType(roc::SYSMSG_PCTIME);
      msg.setSysMesData((uint32_t) time(NULL));
      vect->push_back(msg);

      readFeb(rocid, 0, at(n).feb0, vect);

      readFeb(rocid, 1, at(n).feb1, vect);
   }

   return vect;
}

void roc::BoardsVector::autoped_issue_system_message(roc::Board* brd, uint32_t type)
{
   if (brd==0) return;
//   brd->operPPP(ROC_NX_FIFO_RESET, 1,   // and flush fifo
//                ROC_NX_FIFO_RESET, 0,
//                ROC_ADDSYSMSG, type ); // insert sysmsg
   brd->put(ROC_ADDSYSMSG, type);
}

void roc::BoardsVector::autoped_setnxmode(nxyter::FebBase* feb, bool testtrig)
{
   if (feb==0) return;

   for (int i=0; i<feb->numNx(); i++) {
      if (feb->getNxOffline(i)) continue; // skip of offline
      feb->nx(i).i2c().setTestModes(false, testtrig, 0);
   }
}

void roc::BoardsVector::autoped_switch(bool switch_on)
{
   for (unsigned nbrd=0; nbrd<size(); nbrd++) {
      if (at(nbrd).brd==0) continue;

      if ((at(nbrd).feb0==0) && (at(nbrd).feb1==0)) continue;

//      DOUT0(("Brd %p switch autoped %s", at(nbrd).brd, DBOOL(switch_on)));

      roc::Gpio gpio(at(nbrd).brd);

      if (switch_on) {
         autoped_issue_system_message(at(nbrd).brd, roc::SYSMSG_USER_RECONFIGURE);
         autoped_setnxmode(at(nbrd).feb0, true);
         autoped_setnxmode(at(nbrd).feb1, true);
         gpio.setConfig(at(nbrd).gpio_mask_on);
         autoped_issue_system_message(at(nbrd).brd, roc::SYSMSG_USER_CALIBR_ON);
      } else {
         autoped_issue_system_message(at(nbrd).brd, roc::SYSMSG_USER_RECONFIGURE);
         gpio.setConfig(at(nbrd).gpio_mask_off);
         autoped_setnxmode(at(nbrd).feb0, false);
         autoped_setnxmode(at(nbrd).feb1, false);
         autoped_issue_system_message(at(nbrd).brd, roc::SYSMSG_USER_CALIBR_OFF);
      }
   }
}



