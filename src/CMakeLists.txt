set(sources main.cpp)

add_executable(qm ${sources})
target_link_libraries(qm ${CMAKE_THREAD_LIBS_INIT})
