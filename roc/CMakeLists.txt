# copy files from source directory to build directory

  add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/roc3
                     COMMAND cmake -E copy_directory  ${CMAKE_CURRENT_SOURCE_DIR}                             
                             ${CMAKE_CURRENT_BINARY_DIR}/roc3
                   WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                   )

If(CMAKE_SYSTEM_NAME MATCHES Darwin)
  # Unpack the dabc tarball and patch the needed files
  Add_Custom_Command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/roc3/dabc/Makefile
                   COMMAND tar -xzf dabcroc19.tar.gz
                   WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/roc3/dabc
                   DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/roc3/
                   )
  Add_Custom_Command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/roc3/patch_okay
                   COMMAND patch -p0 < roclib_macos.patch
                   COMMAND touch patch_okay
                   WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/roc3
                   DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/roc3/dabc/Makefile
                   )
Else(CMAKE_SYSTEM_NAME MATCHES Darwin)
  Add_Custom_Command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/roc3/patch_okay
                   COMMAND patch -p0 < roclib_linux.patch
                   COMMAND touch patch_okay
                   WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/roc3
                   DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/roc3
                   )
EndIf(CMAKE_SYSTEM_NAME MATCHES Darwin)

add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/roc3/lib/libRoc.so
                   COMMAND LD_LIBRARY_PATH=${ROOT_LIBRARY_DIR}
                           ROOTSYS=${ROOTSYS} make roclib 
                   WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/roc3
                   DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/roc3/patch_okay
                  )

add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/roc3/lib/libDabcBase.so
                   COMMAND make dabccore 
                   WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/roc3
                   DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/roc3/lib/libRoc.so
                  )

add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/roc3/lib/libDabcRoc.so
                          ${CMAKE_CURRENT_BINARY_DIR}/roc3/lib/libDabcMbs.so
                          ${CMAKE_CURRENT_BINARY_DIR}/roc3/lib/libRocDict.rootmap
                          ${CMAKE_CURRENT_BINARY_DIR}/roc3/lib/libRocDict.so
                   COMMAND LD_LIBRARY_PATH=${ROOT_LIBRARY_DIR}
                           ROOTSYS=${ROOTSYS} make rocdict
                   WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/roc3
                   DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/roc3/lib/libDabcBase.so
                  )

add_custom_command(OUTPUT 
                          ${LIBRARY_OUTPUT_PATH}/libDabcRoc.so
                          ${LIBRARY_OUTPUT_PATH}/libDabcBase.so
                          ${LIBRARY_OUTPUT_PATH}/libRoc.so
                          ${LIBRARY_OUTPUT_PATH}/libDabcMbs.so
                          ${LIBRARY_OUTPUT_PATH}/libRocDict.rootmap
                   COMMAND cmake -E copy_if_different lib/libDabcRoc.so ${LIBRARY_OUTPUT_PATH}
                   COMMAND cmake -E copy_if_different lib/libRoc.so ${LIBRARY_OUTPUT_PATH}
                   COMMAND cmake -E copy_if_different lib/libDabcBase.so ${LIBRARY_OUTPUT_PATH}
                   COMMAND cmake -E copy_if_different lib/libDabcMbs.so ${LIBRARY_OUTPUT_PATH}
                   COMMAND cmake -E copy_if_different lib/libRocDict.rootmap ${LIBRARY_OUTPUT_PATH}
                   COMMAND cmake -E copy_if_different lib/libRocDict.so ${LIBRARY_OUTPUT_PATH}
                   WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/roc3
                   DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/roc3/lib/libRoc.so 
                           ${CMAKE_CURRENT_BINARY_DIR}/roc3/lib/libDabcBase.so 
                           ${CMAKE_CURRENT_BINARY_DIR}/roc3/lib/libDabcMbs.so 
                           ${CMAKE_CURRENT_BINARY_DIR}/roc3/lib/libDabcRoc.so 
                           ${CMAKE_CURRENT_BINARY_DIR}/roc3/lib/libRocDict.rootmap 
                  )

# create a target which will always be build and does actually nothing. The target is only
# needed that the dependencies are build, f they are not up to date. If everything is up to
# dte nothing is done. This target depends on the libAfterImage.a in the library directory of the
# build directory.

add_custom_target(RocLib ALL
                  DEPENDS ${LIBRARY_OUTPUT_PATH}/libDabcRoc.so
                  DEPENDS ${LIBRARY_OUTPUT_PATH}/libDabcBase.so
                  DEPENDS ${LIBRARY_OUTPUT_PATH}/libDabcMbs.so
                  DEPENDS ${LIBRARY_OUTPUT_PATH}/libRoc.so
                  DEPENDS ${LIBRARY_OUTPUT_PATH}/libRocDict.rootmap
                  DEPENDS ${LIBRARY_OUTPUT_PATH}/libRocDict.so
                 )

# import libDabcRoc.so which is not in the scope of CMake. This step makes it possible that
# make clean will remove the library

add_library(DabcRoc SHARED IMPORTED GLOBAL)
add_library(DabcBase SHARED IMPORTED GLOBAL)
add_library(DabcMbs SHARED IMPORTED GLOBAL)
add_library(Roc SHARED IMPORTED GLOBAL)
add_library(RocDict SHARED IMPORTED GLOBAL)

# CMake knows now where to find the target Roc
# We can use now the target Roc somewhere else in the project
# In our case this is needed to be able to link the CbmRocSource
Set_Property(TARGET Roc PROPERTY IMPORTED_LOCATION ${LIBRARY_OUTPUT_PATH}/libRoc.so) 
Set_Property(TARGET RocDict PROPERTY IMPORTED_LOCATION ${LIBRARY_OUTPUT_PATH}/libRocDict.so) 
Set_Property(TARGET DabcRoc PROPERTY IMPORTED_LOCATION ${LIBRARY_OUTPUT_PATH}/libDabcRoc.so) 
Set_Property(TARGET DabcMbs PROPERTY IMPORTED_LOCATION ${LIBRARY_OUTPUT_PATH}/libDabcMbs.so) 
Set_Property(TARGET DabcBase PROPERTY IMPORTED_LOCATION ${LIBRARY_OUTPUT_PATH}/libDabcBase.so) 
     
