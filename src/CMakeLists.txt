add_library(
    libsearch
    search.cpp db-wrapper.cpp index.cpp document.cpp word-list.cpp utils.cpp search-driver.cpp output.cpp dict.cpp
)

add_executable(
    search
    main.cpp
)

target_link_libraries(search
    libsearch
    pthread
    ${CMAKE_SOURCE_DIR}/third-party/leveldb/out-static/libleveldb.a
    ${CMAKE_SOURCE_DIR}/third-party/leveldb/out-static/libmemenv.a
    )
