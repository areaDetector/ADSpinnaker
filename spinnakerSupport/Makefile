TOP=../
include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE
#=============================


# Note, the following files were manually copied from the Spinnaker distribution lib directories on Windows
# and from /usr/lib on Linux.
ifeq ($(OS_CLASS), WIN32)
LIB_INSTALLS    += ../os/windows-x64/Spinnaker_v140.lib
LIB_INSTALLS    += ../os/windows-x64/Spinnakerd_v140.lib

else ifeq ($(OS_CLASS), Linux)
LIB_INSTALLS    += ../os/linux-x86_64/libSpinnaker.so
LIB_INSTALLS    += ../os/linux-x86_64/libSpinnaker.so.4
LIB_INSTALLS    += ../os/linux-x86_64/libSpinnaker.so.4.0.0.116
LIB_INSTALLS    += ../os/linux-x86_64/libGenApi_gcc11_v3_0.so
LIB_INSTALLS    += ../os/linux-x86_64/libGCBase_gcc11_v3_0.so
LIB_INSTALLS    += ../os/linux-x86_64/libLog_gcc11_v3_0.so
LIB_INSTALLS    += ../os/linux-x86_64/libMathParser_gcc11_v3_0.so
LIB_INSTALLS    += ../os/linux-x86_64/libNodeMapData_gcc11_v3_0.so
LIB_INSTALLS    += ../os/linux-x86_64/libXmlParser_gcc11_v3_0.so
LIB_INSTALLS    += ../os/linux-x86_64/Spinnaker_GenTL.cti
endif

#=============================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

