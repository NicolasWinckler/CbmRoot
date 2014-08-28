MQTest
========
Example of a [FAIRMQ (ALFA)](https://github.com/FairRootGroup/FairRoot/tree/master/fairmq) application in CbmRoot.

### What the example does
In this example StsDigi and MuchDigi data are converted into [fles_ipc](https://github.com/cbm-fles/fles_ipc) format and transported to different nodes where they can be further processed (e.g. merged and stored).

In particular this example provides :
* 1 sampler that read [CbmRoot timeslices](https://github.com/NicolasWinckler/CbmRoot/blob/devmq/run/CbmTimeSlice.h) from root file, extract the stsDigi data, convert them into fles_ipc microslice format, and send them to a merger.
* 1 sampler that fill dummy MuchDigi in CbmRoot timeslices, convert them into fles_ipc microslice format and send them to a merger.
* 1 merger that receive and merges fles_ipc microslices into [fles storable timeslices](https://github.com/cbm-fles/fles_ipc/blob/master/StorableTimeslice.hpp) and send them to a file sink
* 1 File Sink that convert fles_ipc timeslices into CbmRoot timeslices and store them into a rootfile

![example of topology](Doc/CBM-MQTest-Topology01.jpg?raw=true "Simple topology example")

### Running the example

This example requires that [FairSoft](https://github.com/FairRootGroup/FairSoft/tree/dev), [FairRoot](https://github.com/FairRootGroup/FairRoot/tree/dev) and [fles_ipc](https://github.com/cbm-fles/fles_ipc) libraries are build with the c++11 flag.
For more details see the [step by step installation](https://github.com/NicolasWinckler/CbmRoot/tree/devmq).

Once the required installation done, simulated digi and CbmTimeSlices have to be produced via the root macro run_sim.C and run_digi.C found in the [CbmRoot/macro/run](https://github.com/NicolasWinckler/CbmRoot/tree/devmq/macro/run) directory, e.g.:
```bash
#in CbmRoot/macro/run
root -l
.x run_sim.C
.x run_digi.C
```
The produced test.raw.root and params.root files are input files of the samplers.
Provide the correct paths of these input files in the script found in CbmRoot/build/bin (or alternatively modify the root file names in the [CbmRoot/tutorials/MQTest/macro/startMultiSamplerPushPull_cbm.sh.in script](https://github.com/NicolasWinckler/CbmRoot/blob/devmq/tutorials/MQTest/macro/startMultiSamplerPushPull_cbm.sh.in) and recompile)

- - - -

### Details
#### Devices and tasks

This example uses three device classes, namely:
* 1 Sampler c.f. [FairMQSampler](https://github.com/FairRootGroup/FairRoot/blob/dev/base/MQ/FairMQSampler.h)
* 1 Merger c.f. [CbmMicroSliceMerger](https://github.com/NicolasWinckler/CbmRoot/blob/devmq/tutorials/MQTest/MQDevices/CbmMicroSliceMerger.h)
* 1 FileSink c.f. [CbmMQFileSink](https://github.com/NicolasWinckler/CbmRoot/blob/devmq/tutorials/MQTest/MQDevices/CbmMQFileSink.h)

The behavior of these classes depends on the task and/or subtask classes you use.

