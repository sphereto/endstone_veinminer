#include "vein_miner_plugin.h"
#include "version.h"

ENDSTONE_PLUGIN(/*name=*/"vein_miner", /*version=*/VEIN_MINER_VERSION, /*main_class=*/veinminer::VeinMinerPlugin)
{
    prefix = "VeinMiner";
    description = "Break one ore or log and the whole connected vein comes with it.";
    website = "https://github.com/sphereto/endstone_veinminer";
    authors = {"sphereto"};

    command("miner")
        .description("Manage VeinMiner settings")
        .usages("/miner", "/miner config", "/miner reload")
        .permissions("veinminer.use", "veinminer.config", "veinminer.reload");

    permission("veinminer.use")
        .description("Allows breaking connected veins of matching blocks.")
        .default_(endstone::PermissionDefault::True);

    permission("veinminer.config")
        .description("Allows editing the server-wide VeinMiner configuration.")
        .default_(endstone::PermissionDefault::Operator);

    permission("veinminer.reload")
        .description("Allows reloading the VeinMiner configuration from disk.")
        .default_(endstone::PermissionDefault::Operator);
}
