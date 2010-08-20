If(${CTEST_SCRIPT_ARG} MATCHES Nightly)
  add_subdirectory(qa)
  Message("Additional Nightly tests for TRD.")
EndIf(${CTEST_SCRIPT_ARG} MATCHES Nightly)
