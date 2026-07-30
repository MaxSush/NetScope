add_executable(NetScopeTests
    flow/FlowTests.cpp
    flow/FlowTableTests.cpp
    coordinator/CoordinatorTests.cpp
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