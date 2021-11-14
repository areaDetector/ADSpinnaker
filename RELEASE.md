ADSpinnaker Releases
==================

The latest untagged master branch can be obtained at
https://github.com/areaDetector/ADSpinnaker.

Tagged source code releases can be obtained at
https://github.com/areaDetector/ADSpinnaker/releases.

Tagged prebuilt binaries can be obtained at
https://cars.uchicago.edu/software/pub/ADSpinnaker.

The versions of EPICS base, asyn, and other synApps modules used for each release can be obtained from 
the EXAMPLE_RELEASE_PATHS.local, EXAMPLE_RELEASE_LIBS.local, and EXAMPLE_RELEASE_PRODS.local
files respectively, in the configure/ directory of the appropriate release of the 
[top-level areaDetector](https://github.com/areaDetector/areaDetector) repository.


Release Notes
=============
R3-3 (November 14, 2021)
-------------------
* Fixed a problem reading Integer GenICam feature values, and their min and max.
  The values were being cast from Int64 to Int32.  This was not correct.
  This was a problem, for example, with values whose maximum was 2^32-1, because the maximum was read as -1.
  This resulted in not being able to set the value, since the allowed maximum was less than the allowed minimum.

R3-2 (May 26, 2021)
-------------------
* Updated Spinnaker version from 2.0.0.147 to 2.4.0.147.
  Windows users will need to update their local install of Spinnaker to this version. 
  Linux users do not need to do anything, since all required files are included with ADSpinnaker.

R3-1 (October 2, 2020)
------------------------
* Changed the arguments passed to the constructor and to the ADSpinnakerConfig() command.
  The asynTrace argument was replaced with numSPBuffers.
  IMPORTANT NOTE: This change may require modification of startup scripts.
  The previous asynTrace argument was not used for any other driver, and was only used for debugging.
  The new numSPBuffers argument is the number of TransportLayer buffers to be allocated in Spinnaker.
  If this argument is 0 it defaults to 100, and it is set to a minimum value of 10.
  The Spinnaker default of 10 is not large enough to prevent dropped frames with some
  cameras when converting from Mono12Packed to Mono16 or from Bayer to RGB.
* Changed iocSpinnaker/st.cmd.base to remove the asynTrace argument to ADSpinnakerConfig().
  The numSPBuffers argument will default to 0, and thus get set to 100 by the driver.
* Added additional transport layer statistics records:
  - TotalPacketCount
  - ResendPacketCount
  - FailedPacketCount
  - InputBufferCount
  - OutputBufferCount
* Fixed a problem that was causing most of the statistics records not to update correctly.
* The transport layer statistics are now updated each time an image is received, independent
  of whether the ReadStatus record is processed. Changed the OPI display to make that clear.
* Moved the Spinnaker test programs from the src/ directory to exampleSrc/.

R3-0 (September 20, 2020)
------------------------
* Updated Spinnaker version from 1.2.0 to 2.0.0.147.
  Windows users will need to update their local install of Spinnaker to this version. 
  Linux users do not need to do anything, since all required files are included with ADSpinnaker.
* The BufferUnderrunCount record has changed to LostFrameCount, because this is what the feature
  is called in the new version of the SDK.

R2-2 (April 9, 2020)
------------------------
* Worked around a bug that is present in all versions of EPICS base.
  If a wait time is specified in the call to epicsMessageQueue::receive() it can fail to receive a message
  correctly if the wait time is almost exactly the same as the time between messages.
  This was causing ADSpinnaker to miss frames when the frame rate was close to 8.7 frames/s.
  Changed the logic to avoid specifying a timeout in epicsMessageQueue::receive(), sending a NULL message instead.
* Removed the memoryChannel argument to the constructor, it was not used.
* Change the order of calling Spinnaker's EndAcquisition() relative to stopping imageGrabTask.
  With the new logic described above EndAcquisition() was never returning when trying to stop acquisition at > 100 frames/s.
* Tested that ADSpinnaker runs on Centos 8. 
  It should also work on other Linux versions with gcc 7.5 and up, e.g. RHEL 8 and Debian 10.
* Added .bob files for Phoebus Display Manager
* Removed ADSpinnakerMore OPI files, these were not used.

R2-1 (January 5, 2020)
----------------------
* NOTE: ADSpinnaker is currently built with V1.20 of the Spinnaker SDK.  This is the version that must be
  installed on Windows machines for ADSpinnaker to work.  
  It will NOT work with later versions of the Spinnaker SDK.
* Changed SPFeature to use epicsInt64 rather than int for GenICam integer feature functions.
  This allows access to the full 64 bit range of feature values, while previously it was limited to 32 bits.
* Added vendor SFNC_GenTL*.xml files which are required to be present in the lib directory on Linux.

R2-0 (August 12, 2019)
----------------------
* This version inherits from ADGenICam.  It is considerably simpler than R1-0 because the ADGenICam base class
  handles many things.  It also supports all camera features, while R1-0 only supported a subset of the features.

R1-0 (May 29, 2019)
-------------------
* Initial release.  This version inherits directly from ADDriver, not from ADGenICam.

