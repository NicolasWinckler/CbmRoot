# configuration path

ROCSYS := $(CURDIR)

CFGFILE = config/Makefile.arch

LOGINFILE = roclogin

ifdef dabcold
DABCTARBALL = $(wildcard dabc/dabcroc.*)
else
DABCTARBALL = $(wildcard dabc/dabcroc11.*)
endif 

ABBPACKTARBALL = $(wildcard abbpack/abbpack.*)

LOCALDABCSYS = $(CURDIR)/dabc

DUMMYFILE = lib/.dummy

FASTRULES = clean pack swpack

.PHONY:    all roclib dabccore dabcplugin rocdict rocgui simul \
           utils go4monitor bnetplugin abbpack abbplugin $(FASTRULES)

LOCALDABC=true
DABC_LIBDIR = $(ROCSYS)/lib

ifdef DABCSYS
ifneq ($(DABCSYS),$(LOCALDABCSYS))
LOCALDABC=false
DABC_LIBDIR = $(DABCSYS)/lib
endif
endif

ifndef DABCSYS
DABCSYS=$(LOCALDABCSYS)
endif

LIBTGTS = dabcplugin 

ifndef dabcold
LIBTGTS += bnetplugin
endif

ABBPLUGINDEP = roclib dabccore dabcplugin

ifdef withabb
LIBTGTS += abbpack abbplugin
ABBPLUGINDEP += abbpack
else
#if abbpack was already unpacked and compiled not try to recompile it
ifneq ($(wildcard abbpack/abbdaemon),)
LIBTGTS += abbplugin
endif
endif

all: roclib $(LIBTGTS) rocdict rocgui utils simul go4monitor

roclib:
	+cd roclib; $(MAKE) all ROCSYS=$(ROCSYS)

dabccore: 
ifeq ($(LOCALDABC),true)
ifeq ($(wildcard dabc/base),)
	cd dabc; tar xzf $(notdir $(DABCTARBALL))
endif
	cd dabc; $(MAKE) all
	cp -f dabc/bin/* bin
	cp -f dabc/lib/* lib
endif

abbpack:
ifeq ($(wildcard abbpack/Makefile),)
	cd abbpack; tar xzf $(notdir $(ABBPACKTARBALL)) 
endif	
	cd abbpack; $(MAKE) TGTBIN=$(ROCSYS)/bin TGTLIB=$(ROCSYS)/lib 

dabcplugin: roclib dabccore
	+cd plugin; $(MAKE) libs DABCSYS=$(DABCSYS) ROCSYS=$(ROCSYS) ROCTGTDIR=$(ROCSYS)
	@echo "DABC-ROC plugin done"

bnetplugin: dabcplugin 
	+cd bnet; $(MAKE) libs DABCSYS=$(DABCSYS) ROCSYS=$(ROCSYS) ROCTGTDIR=$(ROCSYS)
	@echo "DABC-ROC-BNET plugin done"

abbplugin: $(ABBPLUGINDEP)
	+cd abbplugin; $(MAKE) libs exes DABCSYS=$(DABCSYS) ROCSYS=$(ROCSYS) ROCTGTDIR=$(ROCSYS)
	@echo "DABC-ABB plugin done"

rocdict: $(LIBTGTS)
	+cd roclib; $(MAKE) dict ROCSYS=$(ROCSYS)

rocgui: rocdict
	+cd gui; $(MAKE) all ROCSYS=$(ROCSYS)

simul:
	+cd simulator; $(MAKE) all ROCSYS=$(ROCSYS)

utils: roclib $(LIBTGTS)
	+cd utilities; $(MAKE) all ROCSYS=$(ROCSYS)

go4monitor: roclib $(LIBTGTS)
ifdef GO4SYS
	+cd go4monitor; $(MAKE) all ROCSYS=$(ROCSYS)
else
	@echo "Cannot build go4monitor - GO4SYS not defined"
endif

clean:
ifdef GO4SYS
	cd go4monitor; $(MAKE) clean ROCSYS=$(ROCSYS)
endif
ifneq ($(wildcard $(CFGFILE)),)	
	cd utilities; $(MAKE) clean ROCSYS=$(ROCSYS)
	cd simulator; $(MAKE) clean ROCSYS=$(ROCSYS)
	cd gui; $(MAKE) clean ROCSYS=$(ROCSYS)
	cd roclib; $(MAKE) clean ROCSYS=$(ROCSYS)
endif
ifneq ($(wildcard $(DABCSYS)/config/*),)	
	cd bnet; $(MAKE) clean DABCSYS=$(DABCSYS) ROCSYS=$(ROCSYS)
	cd plugin; $(MAKE) clean DABCSYS=$(DABCSYS) ROCSYS=$(ROCSYS)
	cd abbplugin; $(MAKE) clean DABCSYS=$(DABCSYS) ROCSYS=$(ROCSYS)
endif
	@rm -rf $(filter-out $(ABBPACKTARBALL), $(wildcard abbpack/*))
	@rm -rf $(filter-out $(wildcard dabc/*.tar.gz), $(wildcard dabc/*))
	@rm -rf lib bin doxydoc $(CFGFILE) $(LOGINFILE)
	@echo "ROClib clean done"


pack: clean
	@echo "Producing roclib.tar.gz ..."
	tar chf roclib.tar . --exclude=.svn --exclude=roclib.tar
	gzip roclib.tar
	@echo "Tar ball roclib.tar.gz produced"

swpack: clean
	@echo "Producing roclib_sw.tar.gz ..."
	tar chf roclib_sw.tar abbpack abbplugin analysis bnet config dabc go4monitor gui plugin roclib simulator test_scripts utilities Makefile *.txt --exclude=.svn
	gzip roclib_sw.tar
	@echo "Tar ball roclib_sw.tar.gz produced"


## from here is configuration part, not required in other tools

ifdef debug
CXXOPT = -g -O2 -Wall -fPIC
else
CXXOPT = -O2 -fPIC
endif

ifeq ($(shell uname -m),x86_64)
CXXOPT    += -m64
endif


ifdef ROOTSYS
ISROOT = true
ROOT_BIN = $(ROOTSYS)/bin/
else
ifneq ($(shell which root-config 2>/dev/null),)
ISROOT = true
ROOT_BIN = $(shell root-config --exec-prefix)/bin/
endif
endif


$(CFGFILE):
	@echo "Generating $@"
	@rm -f $@
	@echo "#### Generated file, not change ###" >> $@
	@echo "CXXOPT = $(CXXOPT)" >> $@
	@echo "" >> $@
ifdef ISROOT
	@echo "ISROOT = true" >> $@
	@echo "ROOT_BIN = $(ROOT_BIN)" >> $@
	@echo "ROOT_LIBS = $(shell $(ROOT_BIN)root-config --libs)" >> $@
	@echo "ROOT_GLIBS = $(shell $(ROOT_BIN)root-config --glibs)" >> $@
	@echo "ROOT_INCLUDE = $(shell $(ROOT_BIN)root-config --incdir)" >> $@
	@echo "ROOT_CFLAGS = $(shell $(ROOT_BIN)root-config --cflags)" >> $@
	@echo "" >> $@
endif
	@echo "ROC_WITHDABC = true" >> $@
ifdef withabb
	@echo "ROCDICT_DEPLIBS += $(ROCSYS)/lib/libDabcAbb.so" >> $@
endif
	@echo "ROCDICT_DEPLIBS += $(ROCSYS)/lib/libDabcRoc.so $(DABC_LIBDIR)/libDabcMbs.so $(DABC_LIBDIR)/libDabcBase.so $(ROCSYS)/lib/libRoc.so" >> $@
	@echo "ROC_LIBSSET = -L$(DABC_LIBDIR) -lDabcBase -lDabcMbs -L$(ROCSYS)/lib -lRoc -lDabcRoc" >> $@
	@echo 'ROC_ROOTLIBSSET = $$(ROC_LIBSSET) -lRocDict' >> $@
ifdef withabb
ifdef owndaemon
	@echo "ROC_LIBSSET += -lpcidriver -lmprace -lDabcAbb" >> $@
else
	@echo "ROC_LIBSSET += -lpcidriver -lmprace -labbdaemon -lDabcAbb" >> $@
endif
endif

$(LOGINFILE):
	@rm -f $@
	@echo "# this is generated file, use it to configure enviroment for ROClib" >> $@
	@echo "# just type '. roclogin' in shell, not forget space in between" >> $@
	@echo "" >> $@
ifdef ROOTSYS
	@echo "# uncomment following lines to configure your ROOT installation" >> $@
	@echo '# export ROOTSYS=$(ROOTSYS)' >> $@
	@echo '# export LD_LIBRARY_PATH=.:$$ROOTSYS/lib:$$LD_LIBRARY_PATH' >> $@
	@echo '# export PATH=$$ROOTSYS/bin:$$PATH' >> $@
	@echo "" >> $@
endif
	@echo "# in GSI do . rootlogin 522-00 to use ROOT" >> $@
	@echo "" >> $@
	@echo 'if [ -z `which root-config 2>/dev/null` ]; then echo "ROOT is not configured!"; fi' >> $@
	@echo "" >> $@
	@echo 'export ROCSYS=$(ROCSYS)' >> $@
	@echo 'export PATH=$$ROCSYS/bin:$$PATH' >> $@
	@echo 'export LD_LIBRARY_PATH=.:$$ROCSYS/lib:$$LD_LIBRARY_PATH' >> $@
ifeq ($(LOCALDABC),true)
	@echo "" >> $@
	@echo 'export DABCSYS=$(LOCALDABCSYS)' >> $@
endif
ifdef GO4SYS
	@echo "" >> $@
	@echo "# this is required to run go4monitor analysis from any directory" >> $@
	@echo 'export LD_LIBRARY_PATH=$$LD_LIBRARY_PATH:$$ROCSYS/go4monitor' >> $@
endif
	@echo "" >> $@
	@echo 'echo "ROClib (DABC-based) is configured"' >> $@
	@echo 'echo "To run gui type:  rocGui roc_address"' >> $@
	@echo 'echo "To run rocutil:   rocutil b=roc_addess -"' >> $@
	@chmod +x $@
	@echo " $@ script created"

$(DUMMYFILE): Makefile $(CFGFILE) $(LOGINFILE)
	@(if [ ! -f lib ] ; then mkdir -p lib; fi)
	@(if [ ! -f bin ] ; then mkdir -p bin; fi)
	@(if [ ! -f $@ ] ; then touch $@; fi)

ifeq ($(findstring $(MAKECMDGOALS), $(FASTRULES)),)
-include $(DUMMYFILE)
endif

