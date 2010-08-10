#include "TSystem.h"
//#include <stdint.h>

void testcalibr()
{
   roc::Board* brd = roc::Board::Connect("cbmtest01");
   
   if (brd==0) return;
   
   brd->setRocNumber(5);
   
   cout << "GetRocNumber = " << brd->getRocNumber() << endl;
   
   roc::Gpio gpio(brd);
   
   gpio.setSyncScale(1);
//   gpio.setConfig(2, false, false, false, false, false);
   gpio.setConfig(2, true, true, false, true, false);
   
   uint32_t value(0);
   gpio.getSyncScale(value);
   cout << "GPIO scaledown = " << value << endl;
      
   brd->startDaq();

   brd->enableCalibration(.3, .1);

   cout << "Start daq done" << endl;

//   gpio.setConfig(2, true, true, false, true, false);
//   gpio.setSyncScale(0);
//   cout << "Set gpio done" << endl;

   // start generator
   //brd->put(0x10040c, 0xffff);
   
//   void *buf = 0;
//   unsigned len = 0;
   
   long numdata(0), firstsys(0), epocherrs(0), numsys(0);
   
   uint32_t lastepoch(0), epoch;

   roc::Iterator iter(brd);
   
   while (iter.next(5)) {
      iter.msg().printData(7);
      numdata++;
      if (numdata>=1000000) break;
      
      if (iter.msg().isSysMsg() && (firstsys == 0)) firstsys = numdata;

      if (iter.msg().isSysMsg()) numsys++;
/*

      if (iter.msg().isEpochMsg() && (firstsys>0)) {
         epoch = iter.msg().getEpochNumber();
         if ((lastepoch>0) && (epoch-lastepoch!=2)) {
            cout << "!!!!!!!! EPOCH ERROR !!!!!!!!!!! " << endl;
            epocherrs++;
         }
         lastepoch = epoch;
      }
*/      
   }
   
//   gSystem->Sleep(3000);
   
   cout << "Num messages = " << numdata << endl;
   cout << "Num system messages = " << numsys << endl;
   cout << "First sysmessage = " << firstsys << endl;
   cout << "Epoch errors = " << epocherrs << endl;
   
   cout << "Sleeping finished" << endl;
   
   brd->stopDaq();

   brd->disableCalibration();

   cout << "Stopdaq done" << endl;

   // stop generator
   //brd->put(0x10040c, 0x0);

   // disable SYNC slave
   gpio.setConfig(2, false, false, false, false, false);

   cout << "disable SYNC done" << endl;

   roc::Board::Close(brd);

   cout << "close board done" << endl;

}
