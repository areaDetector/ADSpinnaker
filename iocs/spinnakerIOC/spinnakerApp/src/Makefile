TOP=../..
include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE

PROD_NAME = spinnakerApp
PROD_IOC_WIN32 += $(PROD_NAME)
PROD_IOC_Linux += $(PROD_NAME)

# <name>.dbd will be created from <name>Include.dbd
DBD += $(PROD_NAME).dbd
$(PROD_NAME)_DBD += ADSpinnakerSupport.dbd

# <name>_registerRecordDeviceDriver.cpp will be created from <name>.dbd
$(PROD_NAME)_SRCS += $(PROD_NAME)_registerRecordDeviceDriver.cpp $(PROD_NAME)Main.cpp

# Add locally compiled object code
PROD_LIBS += ADSpinnaker
ifeq (debug, $(findstring debug, $(T_A)))
  PROD_LIBS_WIN32 += Spinnakerd_v140
else
  PROD_LIBS_WIN32 += Spinnaker_v140
endif

PROD_LIBS += ADGenICam

# We use PROD_SYS_LIBS even though libflycapture.so is in the ADSpinnaker tree 
# because we are doing static build but the file is a .so file, not a .a file.
PROD_SYS_LIBS_Linux += Spinnaker
PROD_SYS_LIBS_Linux += GCBase_gcc11_v3_0
PROD_SYS_LIBS_Linux += GenApi_gcc11_v3_0
PROD_SYS_LIBS_Linux += NodeMapData_gcc11_v3_0
PROD_SYS_LIBS_Linux += MathParser_gcc11_v3_0
PROD_SYS_LIBS_Linux += XmlParser_gcc11_v3_0
PROD_SYS_LIBS_Linux += Log_gcc11_v3_0

include $(ADCORE)/ADApp/commonDriverMakefile

#=============================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

