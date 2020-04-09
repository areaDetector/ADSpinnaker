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

