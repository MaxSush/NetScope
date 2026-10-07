add_library(NetscopeHostname STATIC
	"cache/DnsCache.h" "cache/DnsCache.cpp"
	"parser/SniParser.h" "parser/SniParser.cpp"
	"parser/DnsResponseParser.h" "parser/DnsResponseParser.cpp"
	"resolver/RdnsResolver.h" "resolver/RdnsResolver.cpp"
)

target_include_directories(NetscopeHostname PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(NetscopeHostname PUBLIC
	NetScopeCore
	NetScopeFlow
)

target_compile_features(NetscopeHostname PUBLIC cxx_std_17)