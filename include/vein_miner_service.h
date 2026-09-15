#pragma once

#include <cstdlib>
#include <memory>
#include <queue>
#include <random>
#include <set>
#include <string>
#include <tuple>

#include <endstone/endstone.hpp>

#include "vein_miner_config.h"

namespace veinminer {

class VeinMinerService {
public:
    struct Result {
        int blocks_broken = 0;
        bool tool_broke = false;
    };

    Result BreakVein(endstone::Player &player, endstone::Block &origin, const GlobalConfig &config) const
    {
        Result result;
        const std::string target_type = origin.getType();
        const int origin_x = origin.getX();
        const int origin_y = origin.getY();
        const int origin_z = origin.getZ();

        std::set<std::tuple<int, int, int>> visited;
        std::queue<std::unique_ptr<endstone::Block>> pending;

        visited.emplace(origin_x, origin_y, origin_z);
        pending.push(origin.clone());

        bool tool_broke = false;

        while (!pending.empty() && result.blocks_broken < config.max_blocks_per_vein && !tool_broke) {
            auto current = std::move(pending.front());
            pending.pop();

            const bool is_origin =
                current->getX() == origin_x && current->getY() == origin_y && current->getZ() == origin_z;

            if (!is_origin) {
                if (!ConsumeDurability(player)) {
                    tool_broke = true;
                    break;
                }
                DropBlock(*current);
                current->setType("minecraft:air");
            }

            ++result.blocks_broken;

            for (const auto face : kFaces) {
                auto neighbor = current->getRelative(face);
                const int dx = neighbor->getX() - origin_x;
                const int dy = neighbor->getY() - origin_y;
                const int dz = neighbor->getZ() - origin_z;

                if (std::abs(dx) + std::abs(dy) + std::abs(dz) > config.max_search_radius) {
                    continue;
                }

                const auto key = std::make_tuple(neighbor->getX(), neighbor->getY(), neighbor->getZ());
                if (visited.contains(key)) {
                    continue;
                }
                if (neighbor->getType() != target_type) {
                    continue;
                }

                visited.insert(key);
                pending.push(std::move(neighbor));
            }
        }

        result.tool_broke = tool_broke;
        return result;
    }

private:
    static constexpr endstone::BlockFace kFaces[6] = {
        endstone::BlockFace::Down, endstone::BlockFace::Up,   endstone::BlockFace::North,
        endstone::BlockFace::South, endstone::BlockFace::West, endstone::BlockFace::East,
    };

    bool ConsumeDurability(endstone::Player &player) const
    {
        auto &inventory = player.getInventory();
        auto item = inventory.getItemInMainHand();
        if (!item.has_value()) {
            return true;
        }

        auto meta = item->getItemMeta();
        if (meta->isUnbreakable()) {
            return true;
        }

        const int unbreaking_level = meta->getEnchantLevel(endstone::Enchantment::Unbreaking);
        static thread_local std::mt19937 rng(std::random_device{}());
        std::uniform_real_distribution<double> distribution(0.0, 1.0);
        const double chance = 1.0 / static_cast<double>(unbreaking_level + 1);

        if (distribution(rng) > chance) {
            return true;
        }

        const int max_durability = item->getType().getMaxDurability();
        if (max_durability <= 0) {
            return true;
        }

        const int new_damage = meta->getDamage() + 1;
        if (new_damage >= max_durability) {
            inventory.setItemInMainHand(std::nullopt);
            return false;
        }

        meta->setDamage(new_damage);
        item->setItemMeta(meta.get());
        inventory.setItemInMainHand(*item);
        return true;
    }

    void DropBlock(endstone::Block &block) const
    {
        const auto *item_type = endstone::ItemType::get(block.getType());
        if (item_type == nullptr) {
            return;
        }
        block.getDimension().dropItem(block.getLocation(), item_type->createItemStack());
    }
};

}  // namespace veinminer
