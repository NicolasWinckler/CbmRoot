/* 
 * File:   CbmMicroSliceMerger.tpl
 * Author: winckler
 *
 * Created on July 15, 2014, 12:33 PM
 */



template <typename TPolicyTask>
CbmMicroSliceMerger<TPolicyTask>::CbmMicroSliceMerger() : fTSIndex(0), fProcessorTask(new TPolicyTask())
{
}

template <typename TPolicyTask>
CbmMicroSliceMerger<TPolicyTask>::~CbmMicroSliceMerger()
{
  delete fProcessorTask;
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
    MQLOG(INFO) << ">>>>>>> Run <<<<<<<";

    boost::thread rateLogger(boost::bind(&FairMQDevice::LogSocketRates, this));

    FairMQPoller* poller = fTransportFactory->CreatePoller(*fPayloadInputs);


    int receivedMsgs = 0;
    int sentMsgs = 0;
    bool ReadyToSend=false;

    size_t bytes_received = 0;

    while ( FairMQDevice::fState == FairMQDevice::RUNNING ) 
    {    

        FairMQMessage* msg = fTransportFactory->CreateMessage();
        poller->Poll(100);

          for (int i = 0; i < fNumInputs; i++)
          {
              if (poller->CheckInput(i))
                  bytes_received = fPayloadInputs->at(i)->Receive(msg);

              if (bytes_received)
              {
                  receivedMsgs++;
                  cout << "I've received " << receivedMsgs << " messages!" << endl;
                  fProcessorTask->SetPayload(msg);
                  fProcessorTask->Exec();

                  ReadyToSend=fProcessorTask->MsgReadyToSend();

                  if(ReadyToSend)
                  {  
                      fPayloadOutputs->at(0)->Send(msg);  
                      sentMsgs++;
                      fTSIndex++;
                  }
                  bytes_received = 0;
              }
          }
          delete msg; 
          // temporary
          if(ReadyToSend && sentMsgs==1) 
              break;
    }

    delete poller;

    cout << "I've received " << receivedMsgs << " and sent " << sentMsgs << " messages!" << endl;

    boost::this_thread::sleep(boost::posix_time::milliseconds(5000));

  
    try 
    {
        rateLogger.interrupt();
        rateLogger.join();
    } 
    catch(boost::thread_resource_error& e) 
    {
        MQLOG(ERROR) << e.what();
    }
    FairMQDevice::Shutdown();
}

