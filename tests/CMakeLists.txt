
add_executable(NetscopeTests
    "SQLiteDatabaseTest.cpp"
)

target_link_libraries(NetscopeTests 
    PRIVATE 
        NetScopeStorage
        NetScopeCore
        NetScopeProcess
)

add_test(NAME CoreLibraryTests COMMAND NetscopeTests)