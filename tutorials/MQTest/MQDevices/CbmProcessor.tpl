/* 
 * File:   CbmProcessor.tpl
 * Author: winckler
 *
 * Created on July 16, 2014, 10:49 AM
 */




template <typename TPolicyTask>
CbmProcessor<TPolicyTask>::CbmProcessor() : fProcessorTask(new TPolicyTask())
{
}

template <typename TPolicyTask>
CbmProcessor<TPolicyTask>::~CbmProcessor()
{
  delete fProcessorTask;
}

template <typename TPolicyTask>
void CbmProcessor<TPolicyTask>::SetTask(TPolicyTask* task)
{
  fProcessorTask = task;
}


template <typename TPolicyTask>
void CbmProcessor<TPolicyTask>::Init()
{
  FairMQDevice::Init();

  fProcessorTask->InitTask();
}


template <typename TPolicyTask>
void CbmProcessor<TPolicyTask>::Run()
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