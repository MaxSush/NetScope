find_package(SQLite3 REQUIRED)

add_library(NetScopeStorage STATIC
    "database/IDatabase.h"
    "database/SQLiteDatabase.cpp"
  "database/SQLiteDatabase.h" "database/DatabaseResult.h" "schema/SchemaManager.h" "schema/SchemaManager.cpp"
  "database/Statement.h" "database/Statement.cpp"  
  "repository/ProcessRepository.h" "repository/IProcessRepository.h" "repository/ProcessRepository.cpp" 
  "repository/IFlowRepository.h" "repository/FlowRepository.cpp" "repository/FlowRepository.h" "persistence/PersistenceService.h" "persistence/PersistenceService.cpp")

target_include_directories(NetScopeStorage
    PUBLIC
       ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(NetScopeStorage
    PRIVATE
    SQLite::SQLite3

    NetScopeProcess
    NetScopeCore
    NetScopeFlow
)

target_compile_features(NetScopeStorage PUBLIC cxx_std_17)
