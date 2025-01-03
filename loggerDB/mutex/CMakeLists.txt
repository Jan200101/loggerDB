
# freertos
include(CheckIncludeFile)
CHECK_INCLUDE_FILE("FreeRTOS.h" HAVE_FREERTOS_H)

set(HAVE_FREERTOS OFF)
if (HAVE_FREERTOS_H OR TARGET FreeRTOS-Kernel-Heap4)
    set(HAVE_FREERTOS ON)
endif()

# pthread
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads)

if (HAVE_FREERTOS)
    set(MUTEX_IMPLEMENTATION "freertos.c")
    set(MUTEX_LIBRARY)
elseif (CMAKE_USE_PTHREADS_INIT)
    set(MUTEX_IMPLEMENTATION "pthread.c")
    set(MUTEX_LIBRARY Threads::Threads)
else()
    set(MUTEX_IMPLEMENTATION "null.c")
    set(MUTEX_LIBRARY)
endif()

message(STATUS "LDB Mutex Implementation: ${MUTEX_IMPLEMENTATION}")

add_library(ldb_mutex INTERFACE)
target_sources(ldb_mutex INTERFACE
    ${MUTEX_IMPLEMENTATION}
    mutex.c
)
target_link_libraries(ldb_mutex INTERFACE ${MUTEX_LIBRARY})
