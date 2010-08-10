# this is generic makefile for DABC application

include $(DABCSYS)/config/Makefile.config

ifdef ROCTGTDIR
ROCBNET_TGTDIR     = $(ROCTGTDIR)/lib
else
ROCBNET_TGTDIR     = $(TGTDLLPATH)
endif

BNETROC_LIBNAME     = $(LIB_PREFIX)BnetRoc
BNETROC_CLUST_LIBNAME = $(LIB_PREFIX)BnetRocClust


BNETROC_LIB         = $(ROCBNET_TGTDIR)/$(BNETROC_LIBNAME).$(DllSuf)
BNETROC_CLUST_LIB   = $(ROCBNET_TGTDIR)/$(BNETROC_CLUST_LIBNAME).$(DllSuf)


BNETROC_S           = $(wildcard src/*.$(SrcSuf))
BNETROC_O           = $(patsubst %.$(SrcSuf), $(BLD_DIR)/%.$(ObjSuf), $(BNETROC_S))
BNETROC_D           = $(patsubst %.$(SrcSuf), $(BLD_DIR)/%.$(DepSuf), $(BNETROC_S))

BNETROC_CLUST_S     = $(wildcard src_clust/*.$(SrcSuf))
BNETROC_CLUST_O     = $(patsubst %.$(SrcSuf), $(BLD_DIR)/%.$(ObjSuf), $(BNETROC_CLUST_S))
BNETROC_CLUST_D     = $(patsubst %.$(SrcSuf), $(BLD_DIR)/%.$(DepSuf), $(BNETROC_CLUST_S))


ALLDEPENDENC        += $(BNETROC_D) $(BNETROC_CLUST_D)

libs::  $(BNETROC_LIB) $(BNETROC_CLUST_LIB)

clean::
	rm -f $(BNETROC_LIB) $(BNETROC_CLUST_LIB)
	echo Clean $(BNETROC_LIB)

$(BNETROC_O) $(BNETROC_D) : INCLUDES += $(ROCSYS)/roclib $(ROCSYS)/plugin .

$(BNETROC_CLUST_O) $(BNETROC_CLUST_D) : INCLUDES += $(ROCSYS)/roclib $(ROCSYS)/plugin .

$(BNETROC_LIB):  $(BNETROC_O)
	@$(MakeLib) $(BNETROC_LIBNAME) "$(BNETROC_O)" $(ROCBNET_TGTDIR)
	@echo "$@  done"

$(BNETROC_CLUST_LIB):  $(BNETROC_CLUST_O)
	@$(MakeLib) $(BNETROC_CLUST_LIBNAME) "$(BNETROC_CLUST_O)" $(ROCBNET_TGTDIR)
	@echo "$@  done"

include $(DABCSYS)/config/Makefile.rules
