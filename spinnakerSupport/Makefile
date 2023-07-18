TOP=../
include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE
#=============================


# Note, the following files were manually copied from the Spinnaker distribution lib directories on Windows
# and from /usr/lib on Linux.
ifeq (windows-x64, $(findstring windows-x64, $(T_A)))
LIB_INSTALLS    += ../os/windows-x64/Spinnaker_v140.lib
LIB_INSTALLS    += ../os/windows-x64/Spinnakerd_v140.lib

else ifeq (win32-x86, $(findstring win32-x86, $(T_A)))
LIB_INSTALLS    += ../os/win32-x86/Spinnaker_v140.lib
LIB_INSTALLS    += ../os/win32-x86/Spinnakerd_v140.lib

else ifeq (linux-x86_64, $(findstring linux-x86_64, $(T_A)))
LIB_INSTALLS    += ../os/linux-x86_64/libSpinnaker.so
LIB_INSTALLS    += ../os/linux-x86_64/libSpinnaker.so.3
LIB_INSTALLS    += ../os/linux-x86_64/libSpinnaker.so.3.1.0.79
LIB_INSTALLS    += ../os/linux-x86_64/libGenApi_gcc11_v3_0.so
LIB_INSTALLS    += ../os/linux-x86_64/libGCBase_gcc11_v3_0.so
LIB_INSTALLS    += ../os/linux-x86_64/libLog_gcc11_v3_0.so
LIB_INSTALLS    += ../os/linux-x86_64/libMathParser_gcc11_v3_0.so
LIB_INSTALLS    += ../os/linux-x86_64/libNodeMapData_gcc11_v3_0.so
LIB_INSTALLS    += ../os/linux-x86_64/libXmlParser_gcc11_v3_0.so

LIB_INSTALLS    +=  ../os/linux-x86_64/FLIR_GenTL.cti

else ifeq (linux-x86, $(findstring linux-x86, $(T_A)))

endif

#=============================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

