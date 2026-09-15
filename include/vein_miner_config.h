#pragma once

#include <filesystem>
#include <fstream>
#include <string>
#include <unordered_set>

namespace veinminer {

enum class ActivationMode { Sneak = 0, Always = 1, Toggle = 2 };
enum class UsePermissionDefault { Everyone = 0, Operators = 1 };

struct GlobalConfig {
    bool enabled = true;
    ActivationMode activation_mode = ActivationMode::Sneak;
    int max_blocks_per_vein = 64;
    int max_search_radius = 8;
    UsePermissionDefault use_permission_default = UsePermissionDefault::Everyone;
    bool allow_personal_override = true;
    std::unordered_set<std::string> block_whitelist = {
        "minecraft:coal_ore",
        "minecraft:deepslate_coal_ore",
        "minecraft:iron_ore",
        "minecraft:deepslate_iron_ore",
        "minecraft:copper_ore",
        "minecraft:deepslate_copper_ore",
        "minecraft:gold_ore",
        "minecraft:deepslate_gold_ore",
        "minecraft:redstone_ore",
        "minecraft:deepslate_redstone_ore",
        "minecraft:lapis_ore",
        "minecraft:deepslate_lapis_ore",
        "minecraft:diamond_ore",
        "minecraft:deepslate_diamond_ore",
        "minecraft:emerald_ore",
        "minecraft:deepslate_emerald_ore",
        "minecraft:nether_gold_ore",
        "minecraft:nether_quartz_ore",
        "minecraft:ancient_debris",
        "minecraft:oak_log",
        "minecraft:spruce_log",
        "minecraft:birch_log",
        "minecraft:jungle_log",
        "minecraft:acacia_log",
        "minecraft:dark_oak_log",
        "minecraft:mangrove_log",
        "minecraft:cherry_log",
        "minecraft:crimson_stem",
        "minecraft:warped_stem",
    };
};

inline std::string ToString(ActivationMode mode)
{
    switch (mode) {
    case ActivationMode::Always:
        return "always";
    case ActivationMode::Toggle:
        return "toggle";
    case ActivationMode::Sneak:
    default:
        return "sneak";
    }
}

inline std::string ToString(UsePermissionDefault value)
{
    return value == UsePermissionDefault::Operators ? "operators" : "everyone";
}

class ConfigStore {
public:
    explicit ConfigStore(std::filesystem::path file_path) : file_path_(std::move(file_path)) {}

    const GlobalConfig &get() const { return config_; }

    void set(GlobalConfig config) { config_ = std::move(config); }

    void load()
    {
        if (!std::filesystem::exists(file_path_)) {
            save();
            return;
        }

        std::ifstream file(file_path_);
        std::string line;
        GlobalConfig loaded;

        while (std::getline(file, line)) {
            const auto eq = line.find('=');
            if (eq == std::string::npos) {
                continue;
            }

            const std::string key = Trim(line.substr(0, eq));
            const std::string value = Trim(line.substr(eq + 1));

            if (key == "enabled") {
                loaded.enabled = (value == "true");
            }
            else if (key == "activation-mode") {
                loaded.activation_mode = ParseActivationMode(Unquote(value));
            }
            else if (key == "max-blocks-per-vein") {
                loaded.max_blocks_per_vein = std::stoi(value);
            }
            else if (key == "max-search-radius") {
                loaded.max_search_radius = std::stoi(value);
            }
            else if (key == "use-permission-default") {
                loaded.use_permission_default = ParseUsePermissionDefault(Unquote(value));
            }
            else if (key == "allow-personal-override") {
                loaded.allow_personal_override = (value == "true");
            }
            else if (key == "block-whitelist") {
                loaded.block_whitelist = ParseBlockArray(value);
            }
        }

        config_ = loaded;
    }

    void save() const
    {
        std::filesystem::create_directories(file_path_.parent_path());
        std::ofstream file(file_path_, std::ios::trunc);

        file << "enabled = " << (config_.enabled ? "true" : "false") << "\n";
        file << "activation-mode = \"" << ToString(config_.activation_mode) << "\"\n";
        file << "max-blocks-per-vein = " << config_.max_blocks_per_vein << "\n";
        file << "max-search-radius = " << config_.max_search_radius << "\n";
        file << "use-permission-default = \"" << ToString(config_.use_permission_default) << "\"\n";
        file << "allow-personal-override = " << (config_.allow_personal_override ? "true" : "false") << "\n";
        file << "block-whitelist = [";

        bool first = true;
        for (const auto &block : config_.block_whitelist) {
            if (!first) {
                file << ", ";
            }
            file << "\"" << block << "\"";
            first = false;
        }
        file << "]\n";
    }

private:
    static std::string Trim(std::string value)
    {
        const auto begin = value.find_first_not_of(" \t\r\n");
        if (begin == std::string::npos) {
            return "";
        }
        const auto end = value.find_last_not_of(" \t\r\n");
        return value.substr(begin, end - begin + 1);
    }

    static std::string Unquote(const std::string &value)
    {
        if (value.size() >= 2 && value.front() == '"' && value.back() == '"') {
            return value.substr(1, value.size() - 2);
        }
        return value;
    }

    static ActivationMode ParseActivationMode(const std::string &value)
    {
        if (value == "always") {
            return ActivationMode::Always;
        }
        if (value == "toggle") {
            return ActivationMode::Toggle;
        }
        return ActivationMode::Sneak;
    }

    static UsePermissionDefault ParseUsePermissionDefault(const std::string &value)
    {
        if (value == "operators") {
            return UsePermissionDefault::Operators;
        }
        return UsePermissionDefault::Everyone;
    }

    static std::unordered_set<std::string> ParseBlockArray(const std::string &value)
    {
        std::unordered_set<std::string> result;
        std::string current;
        bool in_quotes = false;

        for (const char c : value) {
            if (c == '"') {
                in_quotes = !in_quotes;
                continue;
            }
            if (c == ',' && !in_quotes) {
                if (!current.empty()) {
                    result.insert(current);
                }
                current.clear();
                continue;
            }
            if (c == '[' || c == ']') {
                continue;
            }
            if (in_quotes) {
                current.push_back(c);
            }
        }
        if (!current.empty()) {
            result.insert(current);
        }
        return result;
    }

    std::filesystem::path file_path_;
    GlobalConfig config_;
};

}  // namespace veinminer
