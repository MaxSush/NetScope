#include <gtest/gtest.h>
#include <config/ConfigLoader.h>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

namespace fs = std::filesystem;
using namespace netscope;

class ConfigLoaderTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        // Create a temporary directory for test config files
        m_tempDir = fs::temp_directory_path() / "netscope_test_configs";
        fs::create_directories(m_tempDir);
    }

    void TearDown() override
    {
        // Clean up temporary directory
        fs::remove_all(m_tempDir);
    }

    std::string CreateTempConfigFile(const std::string& filename, const std::string& content)
    {
        std::string path = (m_tempDir / filename).string();
        std::ofstream out(path);
        out << content;
        out.close();
        return path;
    }

    fs::path m_tempDir;
};

// 1. Test Defaults
TEST_F(ConfigLoaderTest, Load_Empty_ReturnsDefaults)
{
    // Pass a path that doesn't exist
    Config config = ConfigLoader::Load("non_existent_file.toml");
    Config defaults; // Default constructed for comparison

    EXPECT_EQ(config.databasePath, defaults.databasePath);
    EXPECT_EQ(config.flowIdleTimeout, defaults.flowIdleTimeout);
    EXPECT_TRUE(config.ignoredProcesses.empty());
}

// 2. Test Override File Applies Values
TEST_F(ConfigLoaderTest, Load_OverrideFile_AppliesValues)
{
    std::string tomlContent = R"(
[general]
database_path = "/custom/path/db.sqlite"
log_level = "debug"

[flow]
idle_timeout_secs = 10
tcp_idle_hard_timeout_secs = 300

[persistence]
batch_size = 500
retention_hours = 24
    )";

    std::string configPath = CreateTempConfigFile("override.toml", tomlContent);
    Config config = ConfigLoader::Load(configPath);

    EXPECT_EQ(config.databasePath, "/custom/path/db.sqlite");
    EXPECT_EQ(config.logLevel, "debug");
    EXPECT_EQ(config.flowIdleTimeout, Seconds(10));
    EXPECT_EQ(config.flowTcpIdleHardTimeout, Seconds(300));
    EXPECT_EQ(config.persistenceBatchSize, 500);
    EXPECT_EQ(config.retentionHours, 24);
}

// 3. Test Malformed File Graceful Fallback
TEST_F(ConfigLoaderTest, Load_MalformedFile_GracefulFallback)
{
    std::string tomlContent = R"(
[general]
database_path = "/custom/path/db.sqlite"
broken_key = [ [ [ not valid toml
    )";

    std::string configPath = CreateTempConfigFile("malformed.toml", tomlContent);
    
    // Should not throw, should just log error and return what it managed to parse (or defaults if it failed early)
    EXPECT_NO_THROW({
        Config config = ConfigLoader::Load(configPath);
        Config defaults;
        // Since it's malformed, it might fail to parse entirely and just return defaults.
        EXPECT_EQ(config.flowIdleTimeout, defaults.flowIdleTimeout);
    });
}

// 4. Test Partial File Applies Only Present Values
TEST_F(ConfigLoaderTest, Load_PartialFile_AppliesOnlyPresentValues)
{
    std::string tomlContent = R"(
[lifecycle]
interval_secs = 42
    )";

    std::string configPath = CreateTempConfigFile("partial.toml", tomlContent);
    Config config = ConfigLoader::Load(configPath);
    Config defaults;

    // The provided value should be updated
    EXPECT_EQ(config.lifecycleInterval, Seconds(42));
    
    // Other values should remain default
    EXPECT_EQ(config.databasePath, defaults.databasePath);
    EXPECT_EQ(config.flowIdleTimeout, defaults.flowIdleTimeout);
}

// 5. Test Ignored Processes Parsing
TEST_F(ConfigLoaderTest, Load_IgnoredProcesses_ParsedCorrectly)
{
    std::string tomlContent = R"(
[process]
ignored_processes = ["curl", "wget", "systemd-resolved"]
    )";

    std::string configPath = CreateTempConfigFile("ignored.toml", tomlContent);
    Config config = ConfigLoader::Load(configPath);

    EXPECT_EQ(config.ignoredProcesses.size(), 3);
    EXPECT_TRUE(config.ignoredProcesses.count("curl"));
    EXPECT_TRUE(config.ignoredProcesses.count("wget"));
    EXPECT_TRUE(config.ignoredProcesses.count("systemd-resolved"));
    EXPECT_FALSE(config.ignoredProcesses.count("chrome"));
}
