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

R2-1 (December XXX, 2019)
* Changed SPFeature to use epicsInt64 rather than int for GenICam integer feature functions.
  This allows access to the full 64 bit range of feature values, while previously it was limited to 32 bits.

R2-0 (August 12, 2019)
* This version inherits from ADGenICam.  It is considerably simpler than R1-0 because the ADGenICam base class
  handles many things.  It also supports all camera features, while R1-0 only supported a subset of the features.

R1-0 (May 29, 2019)
----
* Initial release.  This version inherits directly from ADDriver, not from ADGenICam.

