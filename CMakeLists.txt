cmake_minimum_required(VERSION 3.6.3)
project(ModeleXXL)


find_package(MPI REQUIRED)

include_directories(${MPI_INCLUDE_PATH})




if(MPI_COMPILE_FLAGS)
    set_target_properties(ModeleXXL PROPERTIES
            COMPILE_FLAGS "${MPI_COMPILE_FLAGS}")
endif()

if(MPI_LINK_FLAGS)
    set_target_properties(ModeleXXL PROPERTIES
            LINK_FLAGS "${MPI_LINK_FLAGS}")
endif()


set(SOURCE_FILES main.cpp Lamport.cpp Lamport.h Agent.cpp Agent.h)
add_executable(ModeleXXL ${SOURCE_FILES})

target_link_libraries(ModeleXXL ${MPI_LIBRARIES})