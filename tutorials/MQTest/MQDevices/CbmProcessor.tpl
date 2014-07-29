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

  size_t bytes_received = 0;
  
  while ( fState == RUNNING ) 
  {    
      
      FairMQMessage* msg = fTransportFactory->CreateMessage();
      
      bytes_received = fPayloadInputs->at(0)->Receive(msg);
      receivedMsgs++;
      
      if (bytes_received) 
      {
          cout << "I've received " << receivedMsgs << " messages!" << endl;
          fProcessorTask->Exec(msg, NULL);
          
          bool ReadyToSend=fProcessorTask->MsgReadyToSend();
          
          if(ReadyToSend)
          {  
              fPayloadOutputs->at(0)->Send(msg);  
              sentMsgs++;
          }  
          bytes_received = 0;
      }
      
      delete msg; 
      // temporary
      if(receivedMsgs==32 && sentMsgs==1) 
        break;
  }

  

  cout << "I've received " << receivedMsgs << " and sent " << sentMsgs << " messages!" << endl;

  boost::this_thread::sleep(boost::posix_time::milliseconds(5000));
  
  
  rateLogger.interrupt();
  rateLogger.join();
}