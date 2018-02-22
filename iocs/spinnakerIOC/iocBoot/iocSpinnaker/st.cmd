< envPaths
errlogInit(20000)

dbLoadDatabase("$(TOP)/dbd/spinnakerApp.dbd")
spinnakerApp_registerRecordDeviceDriver(pdbbase) 

# Prefix for all records
epicsEnvSet("PREFIX", "13SP1:")
# Use this line for the first Point Grey camera in the system
epicsEnvSet("CAMERA_ID", "0")
# Use this line for a specific camera by serial number, in this case a Grasshopper3 USB
#epicsEnvSet("CAMERA_ID", "17476170")
# Use this line for a specific camera by serial number, in this case a BlackFlyS GigE
#epicsEnvSet("CAMERA_ID", "17165235")

# The port name for the detector
epicsEnvSet("PORT",   "SP1")
# Really large queue so we can stream to disk at full camera speed
epicsEnvSet("QSIZE",  "2000")   
# The maximim image width; used for row profiles in the NDPluginStats plugin
epicsEnvSet("XSIZE",  "2048")
# The maximim image height; used for column profiles in the NDPluginStats plugin
epicsEnvSet("YSIZE",  "2048")
# The maximum number of time series points in the NDPluginStats plugin
epicsEnvSet("NCHANS", "2048")
# The maximum number of frames buffered in the NDPluginCircularBuff plugin
epicsEnvSet("CBUFFS", "500")
# The search path for database files
epicsEnvSet("EPICS_DB_INCLUDE_PATH", "$(ADCORE)/db")
# Define NELEMENTS to be enough for a 2048x2048x3 (color) image
epicsEnvSet("NELEMENTS", "12592912")

# The following environment variables define the property numbers.  These must be consistent with the enum values in ADSpinnaker.cpp
epicsEnvSet(SPVideoMode,             0)
epicsEnvSet(SPFrameRate,             1)
epicsEnvSet(SPFrameRateAuto,         2)
epicsEnvSet(SPFrameRateEnable,       3)
epicsEnvSet(SPExposure,              4)
epicsEnvSet(SPExposureAuto,          5)
epicsEnvSet(SPGain,                  6)
epicsEnvSet(SPGainAuto,              7)
epicsEnvSet(SPBlackLevel,            8)
epicsEnvSet(SPBlackLevelAuto,        9)
epicsEnvSet(SPBlackLevelBalanceAuto, 10)
epicsEnvSet(SPSaturation,            11)
epicsEnvSet(SPSaturationEnable,      12)
epicsEnvSet(SPGamma,                 13)
epicsEnvSet(SPGammaEnable,           14)
epicsEnvSet(SPSharpening,            15)
epicsEnvSet(SPSharpeningAuto,        16)
epicsEnvSet(SPSharpeningEnable,      17)
epicsEnvSet(SPPixelFormat,           18)
epicsEnvSet(SPConvertPixelFormat,    19)
epicsEnvSet(SPTriggerSource,         20)
epicsEnvSet(SPTriggerActivation,     21)
epicsEnvSet(SPTriggerDelay,          22)
epicsEnvSet(SPSoftwareTrigger,       23)
epicsEnvSet(SPBalanceRatio,          24)
epicsEnvSet(SPBalanceRatioSelector,  25)
epicsEnvSet(SPBalanceWhiteAuto,      26)

# ADSpinnakerConfig(const char *portName, const char *cameraId, int traceMask, int memoryChannel,
#                 int maxBuffers, size_t maxMemory, int priority, int stackSize)
ADSpinnakerConfig("$(PORT)", $(CAMERA_ID), 0xff, 0)
asynSetTraceIOMask($(PORT), 0, 2)
# Set ASYN_TRACE_WARNING and ASYN_TRACE_ERROR
asynSetTraceMask($(PORT), 0, 0x21)
#asynSetTraceFile($(PORT), 0, "asynTrace.out")
#asynSetTraceInfoMask($(PORT), 0, 0xf)

# Main database.  This just loads and modifies ADBase.template
dbLoadRecords("$(ADSPINNAKER)/db/spinnaker.template", "P=$(PREFIX),R=cam1:,PORT=$(PORT)")

# Video mode
dbLoadRecords("$(ADSPINNAKER)/db/spinnakerMenuProp.template",  "P=$(PREFIX),R=cam1:,PORT=$(PORT),PROP=VideoMode,PN=$(SPVideoMode)")

# Frame rate
dbLoadRecords("$(ADSPINNAKER)/db/spinnakerFloatProp.template",   "P=$(PREFIX),R=cam1:,PORT=$(PORT),PROP=FrameRate,PN=$(SPFrameRate),VAL=10.,READBACK=1")
dbLoadRecords("$(ADSPINNAKER)/db/spinnakerPropAuto.template",    "P=$(PREFIX),R=cam1:,PORT=$(PORT),PROP=FrameRate,PN=$(SPFrameRateAuto)")
dbLoadRecords("$(ADSPINNAKER)/db/spinnakerPropEnable.template",  "P=$(PREFIX),R=cam1:,PORT=$(PORT),PROP=FrameRate,PN=$(SPFrameRateEnable)")

# Exposure
dbLoadRecords("$(ADSPINNAKER)/db/spinnakerFloatProp.template", "P=$(PREFIX),R=cam1:,PORT=$(PORT),PROP=Exposure,PN=$(SPExposure),PINI=NO")
dbLoadRecords("$(ADSPINNAKER)/db/spinnakerPropAuto.template",  "P=$(PREFIX),R=cam1:,PORT=$(PORT),PROP=Exposure,PN=$(SPExposureAuto)")

# Gain
dbLoadRecords("$(ADSPINNAKER)/db/spinnakerFloatProp.template", "P=$(PREFIX),R=cam1:,PORT=$(PORT),PROP=SPGain,PN=$(SPGain),PINI=NO")
dbLoadRecords("$(ADSPINNAKER)/db/spinnakerPropAuto.template",  "P=$(PREFIX),R=cam1:,PORT=$(PORT),PROP=SPGain,PN=$(SPGainAuto)")

# Black level
dbLoadRecords("$(ADSPINNAKER)/db/spinnakerFloatProp.template", "P=$(PREFIX),R=cam1:,PORT=$(PORT),PROP=BlackLevel,PN=$(SPBlackLevel),VAL=0.0")
dbLoadRecords("$(ADSPINNAKER)/db/spinnakerPropAuto.template",  "P=$(PREFIX),R=cam1:,PORT=$(PORT),PROP=BlackLevel,PN=$(SPBlackLevelAuto)")

# Black level balance.  No value, only auto
dbLoadRecords("$(ADSPINNAKER)/db/spinnakerPropAuto.template",  "P=$(PREFIX),R=cam1:,PORT=$(PORT),PROP=BlackLevelBalance,PN=$(SPBlackLevelBalanceAuto)")

# White balance controls
dbLoadRecords("$(ADSPINNAKER)/db/spinnakerFloatProp.template", "P=$(PREFIX),R=cam1:,PORT=$(PORT),PROP=WhiteBalanceRatio,    PN=$(SPBalanceRatio),VAL=0.0")
dbLoadRecords("$(ADSPINNAKER)/db/spinnakerMenuProp.template",  "P=$(PREFIX),R=cam1:,PORT=$(PORT),PROP=WhiteBalanceSelector, PN=$(SPBalanceRatioSelector)")
dbLoadRecords("$(ADSPINNAKER)/db/spinnakerPropAuto.template",  "P=$(PREFIX),R=cam1:,PORT=$(PORT),PROP=WhiteBalance,         PN=$(SPBalanceWhiteAuto)")

# Saturation
dbLoadRecords("$(ADSPINNAKER)/db/spinnakerFloatProp.template",  "P=$(PREFIX),R=cam1:,PORT=$(PORT),PROP=Saturation,PN=$(SPSaturation),VAL=0.1")
dbLoadRecords("$(ADSPINNAKER)/db/spinnakerPropEnable.template", "P=$(PREFIX),R=cam1:,PORT=$(PORT),PROP=Saturation,PN=$(SPSaturationEnable)")

# Gamma
dbLoadRecords("$(ADSPINNAKER)/db/spinnakerFloatProp.template",  "P=$(PREFIX),R=cam1:,PORT=$(PORT),PROP=Gamma,PN=$(SPGamma),VAL=1.0")
dbLoadRecords("$(ADSPINNAKER)/db/spinnakerPropEnable.template", "P=$(PREFIX),R=cam1:,PORT=$(PORT),PROP=Gamma,PN=$(SPGammaEnable)")

# Sharpening
dbLoadRecords("$(ADSPINNAKER)/db/spinnakerFloatProp.template",   "P=$(PREFIX),R=cam1:,PORT=$(PORT),PROP=Sharpening,PN=$(SPSharpening)")
dbLoadRecords("$(ADSPINNAKER)/db/spinnakerPropAuto.template",    "P=$(PREFIX),R=cam1:,PORT=$(PORT),PROP=Sharpening,PN=$(SPSharpeningAuto)")
dbLoadRecords("$(ADSPINNAKER)/db/spinnakerPropEnable.template",  "P=$(PREFIX),R=cam1:,PORT=$(PORT),PROP=Sharpening,PN=$(SPSharpeningEnable)")

# Pixel format
dbLoadRecords("$(ADSPINNAKER)/db/spinnakerMenuProp.template",  "P=$(PREFIX),R=cam1:,PORT=$(PORT),PROP=PixelFormat,PN=$(SPPixelFormat)")

# Convert pixel format.  This has a non-generic template file because we constrain the menu choices.
dbLoadRecords("$(ADSPINNAKER)/db/spinnakerConvertPixelFormat.template", "P=$(PREFIX),R=cam1:,PORT=$(PORT),PROP=ConvertPixelFormat,PN=$(SPConvertPixelFormat)")

# Trigger source
dbLoadRecords("$(ADSPINNAKER)/db/spinnakerMenuProp.template",  "P=$(PREFIX),R=cam1:,PORT=$(PORT),PROP=TriggerSource,PN=$(SPTriggerSource)")

# Trigger activation
dbLoadRecords("$(ADSPINNAKER)/db/spinnakerMenuProp.template",  "P=$(PREFIX),R=cam1:,PORT=$(PORT),PROP=TriggerActivation,PN=$(SPTriggerActivation)")

# Trigger delay
dbLoadRecords("$(ADSPINNAKER)/db/spinnakerFloatProp.template", "P=$(PREFIX),R=cam1:,PORT=$(PORT),PROP=TriggerDelay,PN=$(SPTriggerDelay),VAL=0.01")

# Software trigger
dbLoadRecords("$(ADSPINNAKER)/db/spinnakerCmdProp.template", "P=$(PREFIX),R=cam1:,PORT=$(PORT),PROP=SoftwareTrigger,PN=$(SPSoftwareTrigger)")

# Create a standard arrays plugin
NDStdArraysConfigure("Image1", 5, 0, "$(PORT)", 0, 0)
# Use this line for 8-bit data only
#dbLoadRecords("$(ADCORE)/db/NDStdArrays.template", "P=$(PREFIX),R=image1:,PORT=Image1,ADDR=0,TIMEOUT=1,NDARRAY_PORT=$(PORT),TYPE=Int8,FTVL=CHAR,NELEMENTS=$(NELEMENTS)")
# Use this line for 8-bit or 16-bit data
dbLoadRecords("$(ADCORE)/db/NDStdArrays.template", "P=$(PREFIX),R=image1:,PORT=Image1,ADDR=0,TIMEOUT=1,NDARRAY_PORT=$(PORT),TYPE=Int16,FTVL=SHORT,NELEMENTS=$(NELEMENTS)")

# Load all other plugins using commonPlugins.cmd
< $(ADCORE)/iocBoot/commonPlugins.cmd
set_requestfile_path("$(ADSPINNAKER)/spinnakerApp/Db")

iocInit()

# save things every thirty seconds
create_monitor_set("auto_settings.req", 30,"P=$(PREFIX)")

# Wait for enum callbacks to complete
epicsThreadSleep(1.0)

# Wait for callbacks on the property limits (DRVL, DRVH) to complete
epicsThreadSleep(1.0)


