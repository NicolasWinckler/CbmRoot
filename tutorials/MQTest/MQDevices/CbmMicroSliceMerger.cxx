/* 
 * File:   CbmMicroSliceMerger.cxx
 * Author: winckler
 * 
 * Created on May 22, 2014, 5:06 PM
 */



#include <boost/thread.hpp>
#include <boost/bind.hpp>

#include "CbmMicroSliceMerger.h"
#include "FairMQLogger.h"


CbmMicroSliceMerger::CbmMicroSliceMerger() : fProcessorTask(new TProcessorTask())
{
}


CbmMicroSliceMerger::CbmMicroSliceMerger(unsigned int ComponentNumber) : fComponentNumber(ComponentNumber)
{
}


CbmMicroSliceMerger::~CbmMicroSliceMerger()
{
  delete fProcessorTask;
}

void CbmMicroSliceMerger::SetTask(TProcessorTask* task)
{
  fProcessorTask = task;
}

void CbmMicroSliceMerger::Init()
{
  FairMQDevice::Init();

  fProcessorTask->InitTask();
}



void CbmMicroSliceMerger::Run()
{
  LOG(INFO) << ">>>>>>> Run <<<<<<<";

  boost::thread rateLogger(boost::bind(&FairMQDevice::LogSocketRates, this));

  

  int receivedMsgs = 0;
  int sentMsgs = 0;

  bool received = false;
  
  while ( fState == RUNNING ) 
  {    
      
      FairMQMessage* msg = fTransportFactory->CreateMessage();
      
      received = fPayloadInputs->at(0)->Receive(msg);
      receivedMsgs++;
      
      if (received) 
      {
          
          fProcessorTask->Exec(msg, NULL);
          
          bool ReadyToSend=fProcessorTask->TSReady();
          
          if(ReadyToSend)
          {  
              fPayloadOutputs->at(0)->Send(msg);  
              sentMsgs++;
          }  
          received = false;
      }
      
      delete msg;
  }

  

  cout << "I've received " << receivedMsgs << " and sent " << sentMsgs << " messages!" << endl;

  boost::this_thread::sleep(boost::posix_time::milliseconds(5000));
  
  
  rateLogger.interrupt();
  rateLogger.join();
}