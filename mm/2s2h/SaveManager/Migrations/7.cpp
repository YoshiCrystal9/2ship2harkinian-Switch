#include "2s2h/SaveManager/SaveManager.h"
#include "z64.h"

void SaveManager_Migration_7(nlohmann::json& j) {
    if (!j["save"]["shipSaveInfo"].contains("filePlaytime")) {
        j["save"]["shipSaveInfo"]["filePlaytime"] = 0;
    }
}
