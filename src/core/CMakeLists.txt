add_library(NetScopeCore STATIC
   "logger/Logger.cpp"
   "timer/Timer.cpp"
   "threading/Thread.cpp"
   "event/EventBus.cpp"
   "event/EventQueue.h"
)

target_include_directories(NetScopeCore
    PUBLIC
        ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(NetScopeCore
    PUBLIC
        fmt::fmt
        spdlog::spdlog
)

target_compile_features(NetScopeCore PUBLIC cxx_std_17)