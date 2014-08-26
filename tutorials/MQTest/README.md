MQTest
========
Example of a [FAIRMQ (ALFA)](https://github.com/FairRootGroup/FairRoot/tree/master/fairmq) application in CbmRoot.

### Features
In this example StsDigi and MuchDigi data are converted into [fles_ipc](https://github.com/cbm-fles/fles_ipc) format and transported to different nodes where they can be further processed (e.g. merged and stored).

In particular this example provides :
* 1 sampler that read [CbmRoot timeslices](https://github.com/NicolasWinckler/CbmRoot/blob/devmq/run/CbmTimeSlice.h) from root file, extract the stsDigi data, convert them into fles_ipc microslice format, and send them to a merger.
* 1 sampler that fill dummy MuchDigi in CbmRoot timeslices, convert them into fles_ipc microslice format and send them to a merger.
* 1 merger that receive and merges fles_ipc microslices into [fles storable timeslices](https://github.com/cbm-fles/fles_ipc/blob/master/StorableTimeslice.hpp) and send them to a file sink
* 1 File Sink that convert fles_ipc timeslices into CbmRoot timeslices and store them into a rootfile

### Running the example

This example requires [FairSoft](https://github.com/FairRootGroup/FairSoft/tree/dev) and [FairRoot](https://github.com/FairRootGroup/FairRoot/tree/dev).
See for example the step by step installation
Once the required installation are done, simulated digi have to be produced via the root macro run_sim.C and run_digi.C found in the [CbmRoot/macro/run](https://github.com/NicolasWinckler/CbmRoot/tree/devmq/macro/run) directory:
```bash
#in CbmRoot/macro/run
root -l
.x run_sim.C
.x run_digi.C
```
The produced test.raw.root file is the input file of the samplers.

- - - -

### Details
#### Devices and tasks

This example uses three device classes, namely:
* 1 Sampler c.f. [FairMQSampler](https://github.com/FairRootGroup/FairRoot/blob/dev/base/MQ/FairMQSampler.h)
* 1 Merger c.f. [CbmMicroSliceMerger](https://github.com/NicolasWinckler/CbmRoot/blob/devmq/tutorials/MQTest/MQDevices/CbmMicroSliceMerger.h)
* 1 FileSink c.f. [CbmMQFileSink](https://github.com/NicolasWinckler/CbmRoot/blob/devmq/tutorials/MQTest/MQDevices/CbmMQFileSink.h)

The behavior of these classes depends on the task and/or subtask classes you use.

