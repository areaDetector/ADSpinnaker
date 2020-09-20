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

