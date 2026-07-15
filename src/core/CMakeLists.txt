add_library(NetScopeCore STATIC
   "logger/Logger.cpp"
   "time/Timer.cpp"
   "threading/Thread.cpp"
   "event/EventBus.cpp"
   "event/EventQueue.h"
 "hash/HashUtils.h" "time/TimeUtils.h" "Config.h" "format/ByteFormatter.cpp" "format/ByteFormatter.h" "utilities/Expected.h"
 )

target_include_directories(NetScopeCore
    PUBLIC
       ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(NetScopeCore
    PRIVATE
        fmt::fmt
        spdlog::spdlog
)

target_compile_features(NetScopeCore PUBLIC cxx_std_17)