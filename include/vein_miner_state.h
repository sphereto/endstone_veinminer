#pragma once

#include <optional>
#include <unordered_map>
#include <utility>

#include <endstone/util/uuid.h>

#include "vein_miner_config.h"

namespace veinminer {

struct PlayerState {
    std::optional<bool> enabled_override;
    std::optional<ActivationMode> activation_mode_override;
    bool toggle_active = false;
};

class PlayerStateStore {
public:
    PlayerState &get(const endstone::UUID &id) { return states_[id]; }

    void set(const endstone::UUID &id, PlayerState state) { states_[id] = std::move(state); }

private:
    std::unordered_map<endstone::UUID, PlayerState> states_;
};

}  // namespace veinminer
