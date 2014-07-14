/**
 * runTestDetectorProcessorBoost.cxx
 *
 *  Created on: Oct 26, 2012
 *      Author: dklein
 */

#include <iostream>
#include <csignal>

#include "FairMQLogger.h"
#include "CbmMicroSliceMerger.h"
#include "FairMQProcessor.h"

#ifdef NANOMSG
  #include "FairMQTransportFactoryNN.h"
#else
  #include "FairMQTransportFactoryZMQ.h"
#endif


using std::cout;
using std::cin;
using std::endl;
using std::stringstream;


CbmMicroSliceMerger processor;

static void s_signal_handler (int signal)
{
  cout << endl << "Caught signal " << signal << endl;

  processor.ChangeState(CbmMicroSliceMerger::STOP);
  processor.ChangeState(CbmMicroSliceMerger::END);

  cout << "Shutdown complete. Bye!" << endl;
  exit(1);
}

static void s_catch_signals (void)
{
  struct sigaction action;
  action.sa_handler = s_signal_handler;
  action.sa_flags = 0;
  sigemptyset(&action.sa_mask);
  sigaction(SIGINT, &action, NULL);
  sigaction(SIGTERM, &action, NULL);
}

int main(int argc, char** argv)
{
  if ( argc != 11 ) {
    cout << "Usage: testDetectorProcessor \tID numIoTreads\n"
              << "\t\tinputSocketType inputRcvBufSize inputMethod inputAddress\n"
              << "\t\toutputSocketType outputSndBufSize outputMethod outputAddress\n" << endl;
    return 1;
  }

  s_catch_signals();

  LOG(INFO) << "PID: " << getpid();

#ifdef NANOMSG
  FairMQTransportFactory* transportFactory = new FairMQTransportFactoryNN();
#else
  FairMQTransportFactory* transportFactory = new FairMQTransportFactoryZMQ();
#endif

  processor.SetTransport(transportFactory);

  int i = 1;

  processor.SetProperty(CbmMicroSliceMerger::Id, argv[i]);
  ++i;
  
  /*
  //if (strcmp(argv[i], "FairTestDetectorMQRecoTask") == 0) {
  if (strcmp(argv[i], "CbmMicroSliceMergerTask") == 0) {
    TProcessorTask* task = new TProcessorTask();
    processor.SetTask(task);
  } else {
    LOG(ERROR) << "task not supported.";
    exit(1);
  }
  ++i;
  */
  
  int numIoThreads;
  stringstream(argv[i]) >> numIoThreads;
  processor.SetProperty(CbmMicroSliceMerger::NumIoThreads, numIoThreads);
  ++i;

  processor.SetProperty(CbmMicroSliceMerger::NumInputs, 1);
  processor.SetProperty(CbmMicroSliceMerger::NumOutputs, 1);


  processor.ChangeState(CbmMicroSliceMerger::INIT);


  processor.SetProperty(CbmMicroSliceMerger::InputSocketType, argv[i], 0);
  ++i;
  int inputRcvBufSize;
  stringstream(argv[i]) >> inputRcvBufSize;
  processor.SetProperty(CbmMicroSliceMerger::InputRcvBufSize, inputRcvBufSize, 0);
  ++i;
  processor.SetProperty(CbmMicroSliceMerger::InputMethod, argv[i], 0);
  ++i;
  processor.SetProperty(CbmMicroSliceMerger::InputAddress, argv[i], 0);
  ++i;

  processor.SetProperty(CbmMicroSliceMerger::OutputSocketType, argv[i], 0);
  ++i;
  int outputSndBufSize;
  stringstream(argv[i]) >> outputSndBufSize;
  processor.SetProperty(CbmMicroSliceMerger::OutputSndBufSize, outputSndBufSize, 0);
  ++i;
  processor.SetProperty(CbmMicroSliceMerger::OutputMethod, argv[i], 0);
  ++i;
  processor.SetProperty(CbmMicroSliceMerger::OutputAddress, argv[i], 0);
  ++i;


  processor.ChangeState(CbmMicroSliceMerger::SETOUTPUT);
  processor.ChangeState(CbmMicroSliceMerger::SETINPUT);
  processor.ChangeState(CbmMicroSliceMerger::RUN);


  char ch;
  cin.get(ch);

  processor.ChangeState(CbmMicroSliceMerger::STOP);
  processor.ChangeState(CbmMicroSliceMerger::END);

  return 0;
}

