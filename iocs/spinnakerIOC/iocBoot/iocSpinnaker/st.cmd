< envPaths
errlogInit(20000)

dbLoadDatabase("$(TOP)/dbd/spinnakerApp.dbd")
spinnakerApp_registerRecordDeviceDriver(pdbbase) 

# Prefix for all records
epicsEnvSet("PREFIX", "13SP1:")
# Use this line for the first Point Grey camera in the system
epicsEnvSet("CAMERA_ID", " ")
# Use this line for a specific camera by serial number, in this case a Flea2 Firewire camera
#epicsEnvSet("CAMERA_ID", "9211601")

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

# ADSpinnakerConfig(const char *portName, const char *cameraId, int traceMask, int memoryChannel,
#                 int maxBuffers, size_t maxMemory, int priority, int stackSize)
ADSpinnakerConfig("$(PORT)", "", 0x1, 0)
asynSetTraceIOMask($(PORT), 0, 2)
asynSetTraceMask($(PORT), 0, 0x1)
#asynSetTraceFile($(PORT), 0, "asynTrace.out")
#asynSetTraceInfoMask($(PORT), 0, 0xf)

dbLoadRecords("$(ADSPINNAKER)/db/spinnaker.template", "P=$(PREFIX),R=cam1:,PORT=$(PORT),ADDR=0,TIMEOUT=1")
#dbLoadTemplate("spinnaker.substitutions")

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


