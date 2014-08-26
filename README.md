CbmRoot
========

Snapshot of CbmRoot (Revision: 25363) c.f. [CBM SVN repository](https://subversion.gsi.de/trac/fairroot/browser/cbmroot/trunk)


### Step by Step installation


1. Install [FairSoft](https://github.com/FairRootGroup/FairSoft/tree/dev)

    we use here "fair_install" as a directory name, you can use what you went! 
    ```bash
    mkdir ~/fair_install
    cd ~/fair_install
    #git clone https://github.com/FairRootGroup/FairSoft.git
    git clone -b dev https://github.com/FairRootGroup/FairSoft.git
    cd FairSoft
    ./configure.sh
    # 1) gcc (on Linux) 5) Clang (on OSX)
    # 1) No Debug Info
    # 2) Internet (install G4 files from internet)
    # path: ~/fair_install/FairSoftInst
    ```
    
2. Install [FairRoot](http://fairroot.gsi.de/?q=node/82)

    ```bash
    # Set the shell variable SIMPATH to the installation directory
    export SIMPATH=~/fair_install/FairSoftInst
    [setenv SIMPATH ~/fair_install/FairSoftInst]

    cd ~/fair_install
    git clone -b dev https://github.com/FairRootGroup/FairRoot.git
    cd FairRoot
    mkdir build
    cd build
    cmake -DCMAKE_CXX_FLAGS="c++11FLAG" -DCMAKE_INSTALL_PREFIX="~/fair_install/FairRootInst" ..
    # where c++11FLAG depends on the compiler e.g. 
    #-std=gnu++0x 
    # or -std=c++11 or -std=gnu++11 for GCC 4.7 and later.
    # for CLANG on mac OS X use the flag -std=c++11 -stdlib=libc++
    make
    make install
    ```

    To run the tests do:

    ```bash
    # To run test: make new shell, do not define SIMPATH
    cd ~/fair_install/FairRoot/build
    make test
    ```
  
3. Install CbmRoot
  
  Set the shell variable FAIRROOTPATH to the FairRoot installation directory. e.g. :
  ```bash
  export FAIRROOTPATH=~/fair_install/FairRootInst
  [setenv FAIRROOTPATH ~/fair_install/FairRootInst]
  ```
  
  choose the location of the installation e.g. :
  ```bash
  cd ~/fair_install
  mkdir exp
  git clone https://github.com/NicolasWinckler/CbmRoot.git
  cd CbmRoot
  mkdir build
  cd build
  cmake -DCMAKE_CXX_FLAGS="c++11FLAG" ..
  # where c++11FLAG depends on the compiler e.g. 
  #-std=gnu++0x 
  # or -std=c++11 or -std=gnu++11 for GCC 4.7 and later.
  # for CLANG on mac OS X use the flag -std=c++11 -stdlib=libc++
  make
  # or e.g. make -jn for n cores
  . config.sh    
  # or source config.csh
  ```
  