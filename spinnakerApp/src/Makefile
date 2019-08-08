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

#PROD_IOC_Linux += MeasureTemperature
PROD_IOC_WIN32 += MeasureTemperature
MeasureTemperature_SRCS += MeasureTemperature.cpp

PROD_IOC_WIN32 += TriggerDelayReadback
TriggerDelayReadback_SRCS += TriggerDelayReadback.cpp

PROD_IOC_WIN32 += ListFrameRates
ListFrameRate_SRCS += ListFrameRates.cpp


#PROD_IOC_Linux += Enumeration
PROD_IOC_WIN32 += Enumeration
Enumeration_SRCS += Enumeration.cpp

#PROD_IOC_Linux += Acquisition
PROD_IOC_WIN32 += Acquisition
Acquisition_SRCS += Acquisition.cpp

#PROD_IOC_Linux += NodeMapInfo
PROD_IOC_WIN32 += NodeMapInfo
NodeMapInfo_SRCS += NodeMapInfo.cpp

#PROD_IOC_Linux += ImageFormatControl
PROD_IOC_WIN32 += ImageFormatControl
ImageFormatControl_SRCS += ImageFormatControl.cpp

#PROD_IOC_Linux += GigEVisionPerformance
PROD_IOC_WIN32 += GigEVisionPerformance
GigEVisionPerformance_SRCS += GigEVisionPerformance.cpp
GigEVisionPerformance_SRCS += CpuUtil.cpp

#PROD_IOC_Linux += ChunkData
PROD_IOC_WIN32 += ChunkData
ChunkData_SRCS += ChunkData.cpp

ifeq (debug, $(findstring debug, $(T_A)))
  PROD_LIBS_WIN32 += Spinnakerd_v140
else
  PROD_LIBS_WIN32 += Spinnaker_v140
endif

PROD_SYS_LIBS_Linux += Spinnaker
PROD_SYS_LIBS_Linux += GCBase_gcc540_v3_0
PROD_SYS_LIBS_Linux += GenApi_gcc540_v3_0
PROD_SYS_LIBS_Linux += Log_gcc540_v3_0
PROD_SYS_LIBS_Linux += MathParser_gcc540_v3_0
PROD_SYS_LIBS_Linux += NodeMapData_gcc540_v3_0
PROD_SYS_LIBS_Linux += XmlParser_gcc540_v3_0
#PROD_SYS_LIBS_Linux += ptgreyvideoencoder

include $(ADCORE)/ADApp/commonLibraryMakefile

#=============================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

