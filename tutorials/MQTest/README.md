MQTest
========
Example of a [FAIRMQ (ALFA)](https://github.com/FairRootGroup/FairRoot/tree/master/fairmq) application in CbmRoot.

### Introduction
In this tutorial, Message Queue (MQ) devices and tasks are provided to construct topologies to transport data in the CbmRoot framework.
For example, this tutorial shows how StsDigi and MuchDigi data are converted into [fles_ipc](https://github.com/cbm-fles/fles_ipc) format and transported to different nodes where they can be further processed (e.g. merged and stored).

One simple example of 4-nodes topology that the tutorial provides consists of :
* 1 sampler that read [CbmRoot timeslices](https://github.com/NicolasWinckler/CbmRoot/blob/devmq/run/CbmTimeSlice.h) from root file, extract the stsDigi data, convert them into fles_ipc microslice format, and send them to a merger.
* 1 sampler that fill dummy MuchDigi in CbmRoot timeslices, convert them into fles_ipc microslice format and send them to a merger.
* 1 merger that receive and merges fles_ipc microslices into [fles storable timeslices](https://github.com/cbm-fles/fles_ipc/blob/master/StorableTimeslice.hpp) and send them to a file sink
* 1 File Sink that convert fles_ipc timeslices into CbmRoot timeslices and store them into a rootfile

![example of topology](Doc/CBM-MQTest-Topology01.jpg?raw=true "Simple topology example")

For now, the setup of topologies is done via scripts, where the necessary parameters to configure the devices (I/O methods, I/O numbers, ip adress, etc...) are found.
A Dynamic Deployment System ([DDS](https://github.com/FairRootGroup/DDS)) which is under development is aimed to replace the scripts.
Below is an example of script (corresponding to the topology above) : 

```bash
#!/bin/bash


buffSize="1000" # zeromq high-water mark is in messages
dataFormat="Boost"

# STS SAMPLER

ID="101"
inputFile="@CBMROOT_SOURCE_DIR@/tutorials/MQTest/macro/data/test.rawsts.root"
parameterFile="@CBMROOT_SOURCE_DIR@/tutorials/MQTest/macro/data/params.root"
branch="TimeSlice."
eventRate="0"
numIoThreads="1"
outputSocketType="push"
outputBufSize=$buffSize
outputMethod="connect"
outputAddress="tcp://localhost:5566"
xterm -hold -e @CMAKE_BINARY_DIR@/bin/testCbmStsDetectorSampler_$dataFormat $ID $inputFile $parameterFile $branch $eventRate $numIoThreads $outputSocketType $outputBufSize $outputMethod $outputAddress &

# MUCH SAMPLER

ID="201"
inputFile="@CBMROOT_SOURCE_DIR@/tutorials/MQTest/macro/data/test.rawmuch.root"
parameterFile="@CBMROOT_SOURCE_DIR@/tutorials/MQTest/macro/data/params.root"
branch="TimeSlice."
eventRate="0"
numIoThreads="1"
outputSocketType="push"
outputBufSize=$buffSize
outputMethod="connect"
outputAddress="tcp://localhost:5567"
xterm -hold -e @CMAKE_BINARY_DIR@/bin/testCbmMuchDetectorSampler_$dataFormat $ID $inputFile $parameterFile $branch $eventRate $numIoThreads $outputSocketType $outputBufSize $outputMethod $outputAddress &


# MICROSLICE MERGER (processor)

MicroSliceNumberToMerge="8"
TimeSliceIndex="1"
ID="301"
numInputs="2"
numIoThreads="1"
inputSocketType1="pull"
inputRcvBufSize1=$buffSize
inputMethod1="bind"
inputAddress1="tcp://*:5566"
inputSocketType2="pull"
inputRcvBufSize2=$buffSize
inputMethod2="bind"
inputAddress2="tcp://*:5567"
outputSocketType="push"
outputSndBufSize=$buffSize
outputMethod="connect"
outputAddress="tcp://localhost:5568"
xterm -hold -e @CMAKE_BINARY_DIR@/bin/testCbmDetectorProcessor_$dataFormat $MicroSliceNumberToMerge $TimeSliceIndex $ID $processorTask $numIoThreads $numInputs $inputSocketType1 $inputRcvBufSize1 $inputMethod1 $inputAddress1 $inputSocketType2 $inputRcvBufSize2 $inputMethod2 $inputAddress2 $outputSocketType $outputSndBufSize $outputMethod $outputAddress &


# FILESINK

ID="401"
numIoThreads="1"
inputSocketType="pull"
inputRcvBufSize=$buffSize
inputMethod="bind"
inputAddress="tcp://*:5568"
xterm -hold -e @CMAKE_BINARY_DIR@/bin/testCbmfileSink_$dataFormat $ID $numIoThreads $inputSocketType $inputRcvBufSize $inputMethod $inputAddress &
#alternative for regular sink:
#xterm -e @CMAKE_BINARY_DIR@/bin/sink $ID $numIoThreads $inputSocketType $inputRcvBufSize $inputAddress &

```


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

```bash
#in CbmRoot/build/bin
. /startMultiSamplerPushPull_cbm.sh
```

Four windows - corresponding to the four devices - will pop up.


- - - -

### Details
#### Devices and tasks

This example uses three device classes, namely:
* 1 Sampler c.f. [FairMQSampler](https://github.com/FairRootGroup/FairRoot/blob/dev/base/MQ/FairMQSampler.h)
* 1 Merger c.f. [CbmMicroSliceMerger](https://github.com/NicolasWinckler/CbmRoot/blob/devmq/tutorials/MQTest/MQDevices/CbmMicroSliceMerger.h)
* 1 FileSink c.f. [CbmMQFileSink](https://github.com/NicolasWinckler/CbmRoot/blob/devmq/tutorials/MQTest/MQDevices/CbmMQFileSink.h)

The behavior of these classes depends on the task and/or subtask classes you use.

