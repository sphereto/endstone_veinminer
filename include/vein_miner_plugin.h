#pragma once

#include <memory>
#include <utility>
#include <vector>

#include <endstone/endstone.hpp>

#include "vein_miner_config.h"
#include "vein_miner_form.h"
#include "vein_miner_listener.h"
#include "vein_miner_service.h"
#include "vein_miner_state.h"

namespace veinminer {

class VeinMinerPlugin : public endstone::Plugin {
public:
    void onEnable() override
    {
        config_store_ = std::make_unique<ConfigStore>(getDataFolder() / "config.toml");
        config_store_->load();

        state_store_ = std::make_unique<PlayerStateStore>();
        service_ = std::make_unique<VeinMinerService>();
        form_service_ = std::make_unique<VeinMinerFormService>();
        listener_ = std::make_unique<VeinMinerListener>(*config_store_, *state_store_, *service_);

        registerEvent(&VeinMinerListener::onBlockBreak, *listener_);

        use_permission_ = getServer().getPluginManager().getPermission("veinminer.use");
        ApplyUsePermissionDefault();

        getLogger().info("VeinMiner enabled");
    }

    void onDisable() override { getLogger().info("VeinMiner disabled"); }

    bool onCommand(endstone::CommandSender &sender, const endstone::Command &command,
                   const std::vector<std::string> &args) override
    {
        if (command.getName() != "miner") {
            return false;
        }

        if (args.empty()) {
            return HandleRoot(sender);
        }

        if (args[0] == "config") {
            return HandleConfig(sender);
        }

        if (args[0] == "reload") {
            return HandleReload(sender);
        }

        sender.sendErrorMessage("Usage: /miner [config|reload]");
        return true;
    }

private:
    bool HandleRoot(endstone::CommandSender &sender)
    {
        auto *player = sender.asPlayer();
        if (player == nullptr) {
            sender.sendMessage("VeinMiner. Use /miner config or /miner reload.");
            return true;
        }

        auto &state = state_store_->get(player->getUniqueId());
        const auto &config = config_store_->get();
        const ActivationMode mode = state.activation_mode_override.value_or(config.activation_mode);

        if (mode != ActivationMode::Toggle) {
            player->sendMessage("VeinMiner. Use /miner config to change your settings.");
            return true;
        }

        state.toggle_active = !state.toggle_active;
        player->sendMessage(state.toggle_active ? "VeinMiner: enabled" : "VeinMiner: disabled");
        return true;
    }

    bool HandleConfig(endstone::CommandSender &sender)
    {
        auto *player = sender.asPlayer();
        if (player == nullptr) {
            sender.sendErrorMessage("This command can only be used in-game.");
            return true;
        }

        const auto &config = config_store_->get();

        if (sender.hasPermission("veinminer.config")) {
            form_service_->SendGlobalForm(*player, config, [this](GlobalConfig updated) {
                config_store_->set(std::move(updated));
                config_store_->save();
                ApplyUsePermissionDefault();
            });
            return true;
        }

        if (!config.allow_personal_override) {
            sender.sendErrorMessage("Personal settings are disabled on this server.");
            return true;
        }

        const auto &state = state_store_->get(player->getUniqueId());
        const auto id = player->getUniqueId();
        form_service_->SendPersonalForm(*player, config, state, [this, id](PlayerState updated) {
            state_store_->set(id, std::move(updated));
        });
        return true;
    }

    bool HandleReload(endstone::CommandSender &sender)
    {
        if (!sender.hasPermission("veinminer.reload")) {
            sender.sendErrorMessage("You do not have permission to reload VeinMiner.");
            return true;
        }

        config_store_->load();
        ApplyUsePermissionDefault();
        sender.sendMessage("VeinMiner configuration reloaded.");
        return true;
    }

    void ApplyUsePermissionDefault()
    {
        if (use_permission_ == nullptr) {
            return;
        }
        const auto &config = config_store_->get();
        use_permission_->setDefault(config.use_permission_default == UsePermissionDefault::Everyone
                                         ? endstone::PermissionDefault::True
                                         : endstone::PermissionDefault::Operator);
    }

    std::unique_ptr<ConfigStore> config_store_;
    std::unique_ptr<PlayerStateStore> state_store_;
    std::unique_ptr<VeinMinerService> service_;
    std::unique_ptr<VeinMinerFormService> form_service_;
    std::unique_ptr<VeinMinerListener> listener_;
    endstone::Permission *use_permission_ = nullptr;
};

}  // namespace veinminer
