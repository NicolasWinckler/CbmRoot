# Dashboard is opened for submissions for a 24 hour period starting at
# the specified NIGHLY_START_TIME. Time is specified in 24 hour format.
set(CTEST_PROJECT_NAME "CbmRoot")
set(CTEST_NIGHTLY_START_TIME "00:00:00 CEST")

#SET (CTEST_DROP_METHOD "http")
#SET (CTEST_DROP_SITE "fairroot.gsi.de")
#SET (CTEST_DROP_LOCATION "/cgi-bin/HTTPUploadDartFile.cgi")
#SET (CTEST_TRIGGER_SITE "http://${CTEST_DROP_SITE}/cgi-bin/TriggerSiteDart2AndCDash.cgi")

set(CTEST_DROP_METHOD "http")
set(CTEST_DROP_SITE "fairroot.gsi.de")
set(CTEST_DROP_LOCATION "/CDash/submit.php?project=CbmRoot")
set(CTEST_DROP_SITE_CDASH TRUE)

set(CTEST_TESTING_TIMEOUT 60)
