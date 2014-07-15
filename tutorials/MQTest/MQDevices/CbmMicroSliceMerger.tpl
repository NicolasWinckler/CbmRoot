/* 
 * File:   CbmMicroSliceMerger.tpl
 * Author: winckler
 *
 * Created on July 15, 2014, 12:33 PM
 */




template <typename TPolicyTask>
CbmMicroSliceMerger<TPolicyTask>::CbmMicroSliceMerger() : fProcessorTask(new TPolicyTask())
{
}

template <typename TPolicyTask>
CbmMicroSliceMerger<TPolicyTask>::CbmMicroSliceMerger(unsigned int ComponentNumber) : fComponentNumber(ComponentNumber)
{
}

template <typename TPolicyTask>
CbmMicroSliceMerger<TPolicyTask>::~CbmMicroSliceMerger()
{
  delete fProcessorTask;
}

template <typename TPolicyTask>
void CbmMicroSliceMerger<TPolicyTask>::SetTask(TPolicyTask* task)
{
  fProcessorTask = task;
}

template <typename TPolicyTask>
void CbmMicroSliceMerger<TPolicyTask>::SetMicroSliceNum(uint64_t MSNumber)
{
    fProcessorTask->SetMicroSliceNumber(MSNumber);
}

template <typename TPolicyTask>
void CbmMicroSliceMerger<TPolicyTask>::SetTimeSliceIdx(uint64_t TSIndex)
{
    fProcessorTask->SetTimeSliceIndex(TSIndex);
}


template <typename TPolicyTask>
void CbmMicroSliceMerger<TPolicyTask>::Init()
{
  FairMQDevice::Init();

  fProcessorTask->InitTask();
}


template <typename TPolicyTask>
void CbmMicroSliceMerger<TPolicyTask>::Run()
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