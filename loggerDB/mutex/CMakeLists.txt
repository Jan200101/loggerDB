
# freertos
include(CheckIncludeFile)
CHECK_INCLUDE_FILE("FreeRTOS.h" HAVE_FREERTOS_H)

set(HAVE_FREERTOS OFF)
if (HAVE_FREERTOS_H OR TARGET FreeRTOS-Kernel-Core)
    set(HAVE_FREERTOS ON)
endif()

# pthread
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads)

add_library(ldb_mutex INTERFACE)
target_sources(ldb_mutex INTERFACE mutex.c)

if (HAVE_FREERTOS)
    set(MUTEX_IMPLEMENTATION "FreeRTOS")
    target_sources(ldb_mutex INTERFACE null.c)
    target_link_libraries(ldb_mutex INTERFACE FreeRTOS-Kernel-Core)
elseif (CMAKE_USE_PTHREADS_INIT)
    set(MUTEX_IMPLEMENTATION "pthreads")
    target_sources(ldb_mutex INTERFACE pthread.c)
    target_link_libraries(ldb_mutex INTERFACE Threads::Threads)
else()
    set(MUTEX_IMPLEMENTATION "dummy")
    target_sources(ldb_mutex INTERFACE null.c)
endif()

message(STATUS "LDB Mutex Implementation: ${MUTEX_IMPLEMENTATION}")

