If($ENV{ctest_model} MATCHES Nightly)
  add_subdirectory(qa)
  Message("Additional Nightly tests for TRD.")
EndIf($ENV{ctest_model}  MATCHES Nightly)
