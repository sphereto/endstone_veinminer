#pragma once

#include <endstone/endstone.hpp>

#include "vein_miner_config.h"
#include "vein_miner_service.h"
#include "vein_miner_state.h"

namespace veinminer {

class VeinMinerListener {
public:
    VeinMinerListener(ConfigStore &config_store, PlayerStateStore &state_store, VeinMinerService &service)
        : config_store_(config_store), state_store_(state_store), service_(service)
    {
    }

    void onBlockBreak(endstone::BlockBreakEvent &event)
    {
        const auto &config = config_store_.get();
        if (!config.enabled) {
            return;
        }

        auto &player = event.getPlayer();
        if (!player.hasPermission("veinminer.use")) {
            return;
        }

        if (config.block_whitelist.find(event.getBlock().getType()) == config.block_whitelist.end()) {
            return;
        }

        const auto &state = state_store_.get(player.getUniqueId());
        if (!ShouldTrigger(player, config, state)) {
            return;
        }

        service_.BreakVein(player, event.getBlock(), config);
    }

private:
    bool ShouldTrigger(endstone::Player &player, const GlobalConfig &config, const PlayerState &state) const
    {
        if (state.enabled_override.has_value() && !state.enabled_override.value()) {
            return false;
        }

        const ActivationMode mode = state.activation_mode_override.value_or(config.activation_mode);
        switch (mode) {
        case ActivationMode::Always:
            return true;
        case ActivationMode::Toggle:
            return state.toggle_active;
        case ActivationMode::Sneak:
        default:
            return player.isSneaking();
        }
    }

    ConfigStore &config_store_;
    PlayerStateStore &state_store_;
    VeinMinerService &service_;
};

}  // namespace veinminer
