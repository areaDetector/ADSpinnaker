TOP=../..
include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE

LIBRARY_IOC_WIN32 += ADSpinnaker
LIBRARY_IOC_Linux += ADSpinnaker

LIB_SRCS += SPFeature.cpp
LIB_SRCS += ADSpinnaker.cpp

ifeq (debug, $(findstring debug, $(T_A)))
  LIB_LIBS_WIN32 += Spinnakerd_v140
else
  LIB_LIBS_WIN32 += Spinnaker_v140
endif

LIB_LIBS += ADGenICam

LIB_LIBS_Linux  += Spinnaker

DBD += ADSpinnakerSupport.dbd

USR_INCLUDES += -I ../../../spinnakerSupport/include
USR_CXXFLAGS_Linux += -D LINUX
USR_CXXFLAGS_Linux += -std=c++11 -Wno-unknown-pragmas

ifeq (debug, $(findstring debug, $(T_A)))
  PROD_LIBS_WIN32 += Spinnakerd_v140
else
  PROD_LIBS_WIN32 += Spinnaker_v140
endif

PROD_SYS_LIBS_Linux += Spinnaker
PROD_SYS_LIBS_Linux += GCBase_gcc11_v3_0
PROD_SYS_LIBS_Linux += GenApi_gcc11_v3_0
PROD_SYS_LIBS_Linux += Log_gcc11_v3_0
PROD_SYS_LIBS_Linux += MathParser_gcc11_v3_0
PROD_SYS_LIBS_Linux += NodeMapData_gcc11_v3_0
PROD_SYS_LIBS_Linux += XmlParser_gcc11_v3_0
#PROD_SYS_LIBS_Linux += ptgreyvideoencoder

include $(ADCORE)/ADApp/commonLibraryMakefile

#=============================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

