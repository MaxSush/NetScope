#pragma once

#include "Config.h"

#include <optional>

namespace netscope
{
	class ConfigLoader
	{
	public:
		static Config Load(const std::optional<std::string>& overridePath = std::nullopt);

	private:
		static void ApplyFile(Config& config, const std::string& path);
	};
}