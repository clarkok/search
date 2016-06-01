include_directories(${gtest_SOURCE_DIR}/include)
include_directories(${gtest_SOURCE_DIR})

add_executable(test_all
    ../third-party/googletest/src/gtest-all.cc
    word-list-test.cpp
    index-test.cpp
    document-test.cpp
    utils-test.cpp
    search-driver-test.cpp
)
target_link_libraries(test_all
    gtest_main
    libsearch
    pthread
    ${CMAKE_SOURCE_DIR}/third-party/leveldb/out-static/libleveldb.a
    ${CMAKE_SOURCE_DIR}/third-party/leveldb/out-static/libmemenv.a
)
