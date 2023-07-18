TOP=../..
include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE

USR_INCLUDES += -I ../../../spinnakerSupport/include
USR_CXXFLAGS_Linux += -D LINUX
USR_CXXFLAGS_Linux += -std=c++11 -Wno-unknown-pragmas

#PROD_IOC_Linux += Acquisition
PROD_IOC_WIN32 += Acquisition
Acquisition_SRCS += Acquisition.cpp

PROD_IOC_Linux += BufferHandling
PROD_IOC_WIN32 += BufferHandling
BufferHandling_SRCS += BufferHandling.cpp

#PROD_IOC_Linux += ChunkData
PROD_IOC_WIN32 += ChunkData
ChunkData_SRCS += ChunkData.cpp

#PROD_IOC_Linux += Enumeration
PROD_IOC_WIN32 += Enumeration
Enumeration_SRCS += Enumeration.cpp

#PROD_IOC_Linux += GigEVisionPerformance
PROD_IOC_WIN32 += GigEVisionPerformance
GigEVisionPerformance_SRCS += GigEVisionPerformance.cpp
GigEVisionPerformance_SRCS += CpuUtil.cpp

#PROD_IOC_Linux += ImageFormatControl
PROD_IOC_WIN32 += ImageFormatControl
ImageFormatControl_SRCS += ImageFormatControl.cpp

PROD_IOC_WIN32 += ListFrameRates
ListFrameRate_SRCS += ListFrameRates.cpp

#PROD_IOC_Linux += MeasureTemperature
PROD_IOC_WIN32 += MeasureTemperature
MeasureTemperature_SRCS += MeasureTemperature.cpp

#PROD_IOC_Linux += NodeMapInfo
PROD_IOC_WIN32 += NodeMapInfo
NodeMapInfo_SRCS += NodeMapInfo.cpp

PROD_IOC_Linux += TestTLStats
PROD_IOC_WIN32 += TestTLStats
TestTLStats_SRCS += TestTLStats.cpp

PROD_IOC_WIN32 += TriggerDelayReadback
TriggerDelayReadback_SRCS += TriggerDelayReadback.cpp

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

#=============================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

