#pragma once

#include <cctype>
#include <functional>
#include <sstream>
#include <string>
#include <unordered_set>
#include <vector>

#include <endstone/endstone.hpp>

#include "vein_miner_config.h"
#include "vein_miner_form_response.h"
#include "vein_miner_state.h"

namespace veinminer {

class VeinMinerFormService {
public:
    using GlobalSubmitCallback = std::function<void(GlobalConfig)>;
    using PersonalSubmitCallback = std::function<void(PlayerState)>;

    void SendGlobalForm(endstone::Player &player, const GlobalConfig &config, GlobalSubmitCallback callback) const
    {
        endstone::ModalForm form;
        form.setTitle("VeinMiner Server Settings");
        form.addControl(endstone::Toggle("Enabled", config.enabled));
        form.addControl(endstone::Dropdown("Default Activation Mode", ActivationModeOptions(),
                                           static_cast<int>(config.activation_mode)));
        form.addControl(
            endstone::Slider("Max Blocks Per Vein", 1, 256, 1, static_cast<float>(config.max_blocks_per_vein)));
        form.addControl(
            endstone::Slider("Max Search Radius", 1, 32, 1, static_cast<float>(config.max_search_radius)));
        form.addControl(endstone::Dropdown("Base Use Permission", UsePermissionOptions(),
                                           static_cast<int>(config.use_permission_default)));
        form.addControl(endstone::TextInput("Block Whitelist", "comma separated block ids", JoinWhitelist(config)));
        form.addControl(endstone::Toggle("Allow Personal Overrides", config.allow_personal_override));

        auto captured_config = config;
        form.setOnSubmit([callback, captured_config](endstone::Player *, std::string response) {
            callback(ParseGlobalForm(captured_config, response));
        });

        player.sendForm(form);
    }

    void SendPersonalForm(endstone::Player &player, const GlobalConfig &config, const PlayerState &state,
                          PersonalSubmitCallback callback) const
    {
        endstone::ModalForm form;
        form.setTitle("VeinMiner Personal Settings");
        form.addControl(endstone::Toggle("Enabled For Me", state.enabled_override.value_or(true)));
        form.addControl(endstone::Dropdown("My Activation Mode", PersonalActivationOptions(),
                                           PersonalActivationIndex(state)));

        auto captured_state = state;
        form.setOnSubmit([callback, captured_state](endstone::Player *, std::string response) {
            callback(ParsePersonalForm(captured_state, response));
        });

        player.sendForm(form);
    }

private:
    static std::vector<std::string> ActivationModeOptions() { return {"Sneak", "Always Active", "Toggle Command"}; }

    static std::vector<std::string> UsePermissionOptions() { return {"Everyone", "Operators Only"}; }

    static std::vector<std::string> PersonalActivationOptions()
    {
        return {"Server Default", "Sneak", "Always Active", "Toggle Command"};
    }

    static int PersonalActivationIndex(const PlayerState &state)
    {
        if (!state.activation_mode_override.has_value()) {
            return 0;
        }
        return static_cast<int>(state.activation_mode_override.value()) + 1;
    }

    static std::string JoinWhitelist(const GlobalConfig &config)
    {
        std::ostringstream out;
        bool first = true;
        for (const auto &block : config.block_whitelist) {
            if (!first) {
                out << ",";
            }
            out << block;
            first = false;
        }
        return out.str();
    }

    static std::vector<std::string> SplitWhitelist(const std::string &raw)
    {
        std::vector<std::string> result;
        std::string current;
        for (const char c : raw) {
            if (c == ',') {
                if (!current.empty()) {
                    result.push_back(current);
                }
                current.clear();
            }
            else if (!std::isspace(static_cast<unsigned char>(c))) {
                current.push_back(c);
            }
        }
        if (!current.empty()) {
            result.push_back(current);
        }
        return result;
    }

    static GlobalConfig ParseGlobalForm(GlobalConfig config, const std::string &response)
    {
        const auto values = FormResponseParser::Parse(response);
        if (values.size() < 7) {
            return config;
        }

        config.enabled = std::get<bool>(values[0]);
        config.activation_mode = static_cast<ActivationMode>(static_cast<int>(std::get<double>(values[1])));
        config.max_blocks_per_vein = static_cast<int>(std::get<double>(values[2]));
        config.max_search_radius = static_cast<int>(std::get<double>(values[3]));
        config.use_permission_default =
            static_cast<UsePermissionDefault>(static_cast<int>(std::get<double>(values[4])));

        const auto blocks = SplitWhitelist(std::get<std::string>(values[5]));
        config.block_whitelist = std::unordered_set<std::string>(blocks.begin(), blocks.end());

        config.allow_personal_override = std::get<bool>(values[6]);

        return config;
    }

    static PlayerState ParsePersonalForm(PlayerState state, const std::string &response)
    {
        const auto values = FormResponseParser::Parse(response);
        if (values.size() < 2) {
            return state;
        }

        state.enabled_override = std::get<bool>(values[0]);

        const int mode_index = static_cast<int>(std::get<double>(values[1]));
        if (mode_index == 0) {
            state.activation_mode_override.reset();
        }
        else {
            state.activation_mode_override = static_cast<ActivationMode>(mode_index - 1);
        }

        return state;
    }
};

}  // namespace veinminer
