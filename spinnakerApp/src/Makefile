TOP=../..
include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE

LIBRARY_IOC_WIN32 += ADSpinnaker
LIBRARY_IOC_Linux += ADSpinnaker

LIB_SRCS += spinnaker.cpp

LIB_LIBS_WIN32 += spinnaker
LIB_IBS_Linux  += spinnaker

DBD += spinnakerSupport.dbd

include $(ADCORE)/ADApp/commonLibraryMakefile

#=============================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

