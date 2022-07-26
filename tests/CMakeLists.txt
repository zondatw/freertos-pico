if (CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "GNU")
    set(CMAKE_THREAD_LIBS_INIT "-lpthread")
    set(CMAKE_HAVE_THREADS_LIBRARY 1)
    set(CMAKE_USE_WIN32_THREADS_INIT 0)
    set(CMAKE_USE_PTHREADS_INIT 1)
    set(THREADS_PREFER_PTHREAD_FLAG ON)

    include(FetchContent)
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG release-1.11.0
    )
    # For Windows: Prevent overriding the parent project's compiler/linker settings
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)

    # ctest
    enable_testing()

    add_executable(
        common_test
        hello_world.cc
    )
    target_link_libraries(
        common_test
        GTest::gtest_main
    )

    include(GoogleTest)
    gtest_discover_tests(common_test)
endif()