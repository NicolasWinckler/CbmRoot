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

#ifndef ROC_BOARDSVECTOR_H
#define ROC_BOARDSVECTOR_H

#include <vector>

#include "roc/Message.h"
#include "roc/Board.h"
#include "nxyter/FebBase.h"

namespace roc {

   typedef std::vector<roc::Message> MessagesVector;


   struct BoardRec {
      roc::Board*      brd;
      std::string      devname;
      nxyter::FebBase* feb0;
      nxyter::FebBase* feb1;
      uint32_t         gpio_mask_save;
      uint32_t         gpio_mask_on;
      uint32_t         gpio_mask_off;
      BoardRec(roc::Board* _brd) :
         brd(_brd), devname(), feb0(0), feb1(0),
         gpio_mask_save(0), gpio_mask_on(0), gpio_mask_off(0) {}
      BoardRec(const BoardRec& rec) :
         brd(rec.brd), devname(rec.devname), feb0(rec.feb0), feb1(rec.feb1),
         gpio_mask_save(rec.gpio_mask_save), gpio_mask_on(rec.gpio_mask_on), gpio_mask_off(rec.gpio_mask_off) {}
   };


   class BoardsVector : public std::vector<BoardRec> {
      public:
         BoardsVector() : std::vector<BoardRec>() {}
         virtual ~BoardsVector() {}

         bool addBoard(roc::Board* brd, std::string devname, std::string febkinds = "");
         void returnBoards();

         // returns number of adc channels on the feb
         int numAdc(unsigned n, unsigned nfeb);

         MessagesVector* readoutExtraMessages();

         void autoped_switch(bool on);

      protected:

         void autoped_issue_system_message(roc::Board* brd, uint32_t type);
         void autoped_setnxmode(nxyter::FebBase* feb, bool testtrig);

         void readFeb(uint32_t rocid, uint32_t febid, nxyter::FebBase* feb, MessagesVector* vect);

   };
}

#endif
