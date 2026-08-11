add_executable(NetScopeTests
    "tests.cpp"
    "SQLiteDatabaseTest.cpp"
)

target_link_libraries(NetScopeTests
    PRIVATE
        GTest::gtest_main
        NetScopeFlow     
        NetScopeProcess    
        NetScopeCore       
)

include(GoogleTest)
gtest_discover_tests(NetScopeTests)