# Dashboard is opened for submissions for a 24 hour period starting at
# the specified NIGHLY_START_TIME. Time is specified in 24 hour format.
set(PROJECT_NAME "CbmRoot")
set(NIGHTLY_START_TIME "00:00:00 CEST")

set(DROP_METHOD "http")
set(DROP_SITE "fairroot.gsi.de")
set(DROP_LOCATION "/CDash/submit.php?project=CbmRoot")
set(DROP_SITE_CDASH TRUE)

set(DART_TESTING_TIMEOUT 300)
