#include "2s2h/BenGui/UIWidgets.hpp"
#include "2s2h/BenGui/BenGui.hpp"
#include "CosmeticEditor.h"
#include "2s2h/ShipInit.hpp"

#include <algorithm>
#include <cctype>
#include <cstring>
#include <cstdlib>
#include <functional>
#include <math.h>
#include <string>
#include <unordered_map>
#include <vector>

#include <tinyxml2.h>
#include <fast/resource/type/DisplayList.h>
#include <ship/resource/archive/Archive.h>

extern "C" {
#include "macros.h"
}

static constexpr const char* CUSTOM_COSMETIC_GROUP = "Custom";
static constexpr const char* CUSTOM_CVAR_PREFIX = "gCosmetic.Custom.";
static constexpr const char* RAINBOW_SYNC_CVAR = "gCosmetics.RainbowSync";

struct CustomCosmeticBinding {
    std::string materialPath;
    size_t commandIndex = 0;
    bool isPrimColor = true;
    uint8_t defaultA = 255;
    uint8_t primM = 0;
    uint8_t primL = 0;
};

struct CustomCosmeticEntry {
    CosmeticOption option;
    std::string label;
    std::string baseCvar;
    std::string valuesCvar;
    std::string rainbowCvar;
    std::string lockedCvar;
    std::string changedCvar;
    std::string category;
    std::vector<CustomCosmeticBinding> bindings;
};

static std::vector<CustomCosmeticEntry> customCosmeticEntries;
static bool customHumanModelActive = false;
static bool customDekuModelActive = false;
static bool customGoronModelActive = false;
static bool customZoraModelActive = false;
static bool customFierceDeityModelActive = false;
static bool customKafeiModelActive = false;
static bool customHumanCosmeticsAvailable = false;
static bool customDekuCosmeticsAvailable = false;
static bool customGoronCosmeticsAvailable = false;
static bool customZoraCosmeticsAvailable = false;
static bool customFierceDeityCosmeticsAvailable = false;
static bool customKafeiCosmeticsAvailable = false;
static std::string sLastDynamicCosmeticsStateSignature;

enum class DynamicCosmeticForm {
    Human = 0,
    Deku,
    Goron,
    Zora,
    FierceDeity,
    Kafei,
    Other,
};

static bool IsCustomArchive(const std::shared_ptr<Ship::Archive>& archive) {
    if (archive == nullptr) {
        return false;
    }

    const auto& archivePath = archive->GetPath();
    return archivePath.find("\\mods\\") != std::string::npos || archivePath.find("/mods/") != std::string::npos;
}

static DynamicCosmeticForm GetDynamicMaterialForm(const std::string& materialPath) {
    if (materialPath.starts_with("objects/object_link_child/") ||
        materialPath.starts_with("__OTR__objects/object_link_child/")) {
        return DynamicCosmeticForm::Human;
    }
    if (materialPath.starts_with("objects/object_link_nuts/") ||
        materialPath.starts_with("__OTR__objects/object_link_nuts/")) {
        return DynamicCosmeticForm::Deku;
    }
    if (materialPath.starts_with("objects/object_link_goron/") ||
        materialPath.starts_with("__OTR__objects/object_link_goron/")) {
        return DynamicCosmeticForm::Goron;
    }
    if (materialPath.starts_with("objects/object_link_zora/") ||
        materialPath.starts_with("__OTR__objects/object_link_zora/")) {
        return DynamicCosmeticForm::Zora;
    }
    if (materialPath.starts_with("objects/object_link_boy/") ||
        materialPath.starts_with("__OTR__objects/object_link_boy/")) {
        return DynamicCosmeticForm::FierceDeity;
    }
    if (materialPath.starts_with("objects/object_test3/") || materialPath.starts_with("__OTR__objects/object_test3/")) {
        return DynamicCosmeticForm::Kafei;
    }

    return DynamicCosmeticForm::Other;
}

static int GetDynamicMaterialFormSortOrder(DynamicCosmeticForm form) {
    return static_cast<int>(form);
}

static bool IsSkeletonOverriddenByCustomArchive(Ship::ArchiveManager* archiveManager, const char* path) {
    if (archiveManager == nullptr) {
        return false;
    }

    return IsCustomArchive(archiveManager->GetArchiveFromFile(path));
}

static void RefreshCustomModelActiveFlags(Ship::ArchiveManager* archiveManager) {
    customHumanModelActive =
        IsSkeletonOverriddenByCustomArchive(archiveManager, "objects/object_link_child/gLinkHumanSkel");
    customDekuModelActive =
        IsSkeletonOverriddenByCustomArchive(archiveManager, "objects/object_link_nuts/gLinkDekuSkel");
    customGoronModelActive =
        IsSkeletonOverriddenByCustomArchive(archiveManager, "objects/object_link_goron/gLinkGoronSkel");
    customZoraModelActive =
        IsSkeletonOverriddenByCustomArchive(archiveManager, "objects/object_link_zora/gLinkZoraSkel");
    customFierceDeityModelActive =
        IsSkeletonOverriddenByCustomArchive(archiveManager, "objects/object_link_boy/gLinkFierceDeitySkel");
    customKafeiModelActive = IsSkeletonOverriddenByCustomArchive(archiveManager, "objects/object_test3/gKafeiSkel");
}

static void MarkCustomCosmeticsAvailable(const std::string& materialPath) {
    switch (GetDynamicMaterialForm(materialPath)) {
        case DynamicCosmeticForm::Human:
            customHumanCosmeticsAvailable = true;
            break;
        case DynamicCosmeticForm::Deku:
            customDekuCosmeticsAvailable = true;
            break;
        case DynamicCosmeticForm::Goron:
            customGoronCosmeticsAvailable = true;
            break;
        case DynamicCosmeticForm::Zora:
            customZoraCosmeticsAvailable = true;
            break;
        case DynamicCosmeticForm::FierceDeity:
            customFierceDeityCosmeticsAvailable = true;
            break;
        case DynamicCosmeticForm::Kafei:
            customKafeiCosmeticsAvailable = true;
            break;
        case DynamicCosmeticForm::Other:
            break;
    }
}

static void SanitizeCustomKey(std::string& value) {
    for (auto it = value.begin(); it != value.end();) {
        if (!std::isalnum(static_cast<unsigned char>(*it))) {
            it = value.erase(it);
        } else {
            ++it;
        }
    }
}

static bool TryLoadCustomDisplayListXml(Ship::ArchiveManager* archiveManager, Ship::ResourceManager* resourceManager,
                                        const std::string& materialPath, tinyxml2::XMLDocument& document,
                                        std::shared_ptr<Fast::DisplayList>& material, tinyxml2::XMLElement*& root) {
    auto file = archiveManager->LoadFile(materialPath);
    if (file == nullptr || !file->IsLoaded || file->Buffer == nullptr) {
        return false;
    }

    document.Parse(file->Buffer->data(), file->Buffer->size());
    if (document.Error()) {
        return false;
    }

    root = document.FirstChildElement();
    if (root == nullptr || std::string(root->Name()) != "DisplayList") {
        return false;
    }

    material = std::dynamic_pointer_cast<Fast::DisplayList>(resourceManager->LoadResource(materialPath));
    return material != nullptr;
}

static size_t FindDisplayListInstructionIndex(const Fast::DisplayList& displayList, const Gfx& expected,
                                              size_t searchStart) {
    for (size_t i = searchStart; i < displayList.Instructions.size(); i++) {
        const Gfx& current = displayList.Instructions[i];
        if (current.words.w0 == expected.words.w0 && current.words.w1 == expected.words.w1) {
            return i;
        }
    }

    return SIZE_MAX;
}

static void RefreshCustomCosmeticOption(CustomCosmeticEntry& entry) {
    entry.option = MakeCosmeticOption(entry.baseCvar.c_str(), entry.valuesCvar.c_str(), entry.rainbowCvar.c_str(),
                                      entry.lockedCvar.c_str(), entry.changedCvar.c_str(), entry.label.c_str(),
                                      COSMETICS_GROUP_MAX, entry.option.defaultColor, false, true, false);
}

static Color_RGBA8 GetCustomCosmeticColor(const CustomCosmeticEntry& entry) {
    if (CVarGetInteger(entry.option.changedCvar, 0)) {
        return CVarGetColor(entry.option.valuesCvar, entry.option.defaultColor);
    }

    return entry.option.defaultColor;
}

static void ClearCustomCosmeticValueCvars(const char* valuesCvar) {
    CVarClear(valuesCvar);
    CVarClear((std::string(valuesCvar) + ".R").c_str());
    CVarClear((std::string(valuesCvar) + ".G").c_str());
    CVarClear((std::string(valuesCvar) + ".B").c_str());
    CVarClear((std::string(valuesCvar) + ".A").c_str());
    CVarClear((std::string(valuesCvar) + ".Type").c_str());
}

static std::string BuildDynamicCosmeticsStateSignature() {
    auto resourceManager = Ship::Context::GetInstance()->GetResourceManager();
    auto archiveManager = resourceManager->GetArchiveManager();
    std::string signature = std::to_string(CVarGetInteger("gEnhancements.Mods.AlternateAssets", 0));

    auto archives = archiveManager->GetArchives();
    if (archives != nullptr) {
        for (const auto& archive : *archives) {
            signature += '|';
            signature += archive != nullptr ? archive->GetPath() : "";
        }
    }

    return signature;
}

static void RefreshBuiltInSuppressedCosmetics() {
    static constexpr const char* kPlayerColorCvars[] = {
        "gCosmetic.Player.HumanTunic.Color", "gCosmetic.Player.HumanHair.Color",  "gCosmetic.Player.DekuTunic.Color",
        "gCosmetic.Player.DekuHair.Color",   "gCosmetic.Player.GoronTunic.Color", "gCosmetic.Player.ZoraTunic.Color",
        "gCosmetic.Player.KafeiHair.Color",
    };
    static constexpr const char* kPlayerChangedCvars[] = {
        "gCosmetic.Player.HumanTunic.Changed", "gCosmetic.Player.HumanHair.Changed",
        "gCosmetic.Player.DekuTunic.Changed",  "gCosmetic.Player.DekuHair.Changed",
        "gCosmetic.Player.GoronTunic.Changed", "gCosmetic.Player.ZoraTunic.Changed",
        "gCosmetic.Player.KafeiHair.Changed",
    };
    static constexpr const char* kPlayerRainbowCvars[] = {
        "gCosmetic.Player.HumanTunic.Rainbow", "gCosmetic.Player.HumanHair.Rainbow",
        "gCosmetic.Player.DekuTunic.Rainbow",  "gCosmetic.Player.DekuHair.Rainbow",
        "gCosmetic.Player.GoronTunic.Rainbow", "gCosmetic.Player.ZoraTunic.Rainbow",
        "gCosmetic.Player.KafeiHair.Rainbow",
    };

    for (const char* cvar : kPlayerColorCvars) {
        ShipInit::Init(cvar);
    }
    for (const char* cvar : kPlayerChangedCvars) {
        ShipInit::Init(cvar);
    }
    for (const char* cvar : kPlayerRainbowCvars) {
        ShipInit::Init(cvar);
    }
}

bool IsCustomModelActiveForCosmeticId(const char* cosmeticId) {
    if (cosmeticId == nullptr) {
        return false;
    }

    if (strcmp(cosmeticId, "Player.HumanTunic") == 0 || strcmp(cosmeticId, "Player.HumanHair") == 0) {
        return customHumanModelActive && customHumanCosmeticsAvailable;
    }
    if (strcmp(cosmeticId, "Player.DekuTunic") == 0 || strcmp(cosmeticId, "Player.DekuHair") == 0) {
        return customDekuModelActive && customDekuCosmeticsAvailable;
    }
    if (strcmp(cosmeticId, "Player.GoronTunic") == 0) {
        return customGoronModelActive && customGoronCosmeticsAvailable;
    }
    if (strcmp(cosmeticId, "Player.ZoraTunic") == 0) {
        return customZoraModelActive && customZoraCosmeticsAvailable;
    }
    if (strcmp(cosmeticId, "Player.KafeiHair") == 0) {
        return customKafeiModelActive && customKafeiCosmeticsAvailable;
    }

    return false;
}

bool IsCustomHumanModelActive() {
    return customHumanModelActive;
}

bool IsCustomDekuModelActive() {
    return customDekuModelActive;
}

bool IsCustomGoronModelActive() {
    return customGoronModelActive;
}

bool IsCustomZoraModelActive() {
    return customZoraModelActive;
}

bool IsCustomKafeiModelActive() {
    return customKafeiModelActive;
}

void ApplyDynamicCosmetics() {
    auto resourceManager = Ship::Context::GetInstance()->GetResourceManager();
    auto archiveManager = resourceManager->GetArchiveManager();

    for (const auto& entry : customCosmeticEntries) {
        Color_RGBA8 color = GetCustomCosmeticColor(entry);

        for (const auto& binding : entry.bindings) {
            if (!IsCustomArchive(archiveManager->GetArchiveFromFile(binding.materialPath))) {
                continue;
            }

            auto material =
                std::dynamic_pointer_cast<Fast::DisplayList>(resourceManager->LoadResource(binding.materialPath));
            if (material == nullptr || binding.commandIndex >= material->Instructions.size()) {
                continue;
            }

            if (binding.isPrimColor) {
                material->Instructions[binding.commandIndex] =
                    gsDPSetPrimColor(binding.primM, binding.primL, color.r, color.g, color.b, binding.defaultA);
            } else {
                material->Instructions[binding.commandIndex] =
                    gsDPSetEnvColor(color.r, color.g, color.b, binding.defaultA);
            }
        }
    }
}

static void SetCustomCosmeticColor(const CustomCosmeticEntry& entry, Color_RGBA8 color) {
    CVarSetColor(entry.option.valuesCvar, color);
    CVarSetInteger(entry.option.rainbowCvar, 0);
    CVarSetInteger(entry.option.changedCvar, 1);
    ShipInit::Init(entry.option.valuesCvar);
    ShipInit::Init(entry.option.rainbowCvar);
    ShipInit::Init(entry.option.changedCvar);
    ApplyDynamicCosmetics();
    Ship::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
}

static void ResetCustomCosmeticColor(const CustomCosmeticEntry& entry) {
    CosmeticEditorResetElement(const_cast<CosmeticOption&>(entry.option), false);
    ApplyDynamicCosmetics();
    Ship::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
}

static void RandomizeCustomCosmeticColor(const CustomCosmeticEntry& entry) {
    Color_RGBA8 color = { static_cast<uint8_t>(rand() % 256), static_cast<uint8_t>(rand() % 256),
                          static_cast<uint8_t>(rand() % 256), 255 };
    SetCustomCosmeticColor(entry, color);
}

static void DrawCustomCosmeticColorRow(const CosmeticOption& cosmeticOption,
                                       const std::function<void()>& onColorChanged,
                                       const std::function<void()>& onRandomize,
                                       const std::function<void()>& onRainbowToggle,
                                       const std::function<void()>& onReset) {
    if (UIWidgets::CVarColorPicker(cosmeticOption.label, cosmeticOption.valuesCvar, cosmeticOption.defaultColor,
                                   cosmeticOption.supportsAlpha, cosmeticOption.lockedCvar, THEME_COLOR)) {
        onColorChanged();
    }

    ImGui::SameLine((ImGui::CalcTextSize("Message Light Blue (None No Shadow)").x * 1.0f) + 60.0f);
    if (UIWidgets::Button(
            ("Random##" + std::string(cosmeticOption.label)).c_str(),
            UIWidgets::ButtonOptions().Size(ImVec2(80, 31)).Padding(ImVec2(2.0f, 0.0f)).Color(THEME_COLOR))) {
        onRandomize();
    }

    ImGui::SameLine();
    if (UIWidgets::CVarCheckbox(("Rainbow##" + std::string(cosmeticOption.label)).c_str(), cosmeticOption.rainbowCvar,
                                UIWidgets::CheckboxOptions().Color(THEME_COLOR))) {
        onRainbowToggle();
    }

    ImGui::SameLine();
    UIWidgets::CVarCheckbox(("Locked##" + std::string(cosmeticOption.label)).c_str(), cosmeticOption.lockedCvar,
                            UIWidgets::CheckboxOptions().Color(THEME_COLOR));

    if (CVarGetInteger(cosmeticOption.changedCvar, 0)) {
        ImGui::SameLine();
        if (UIWidgets::Button(("Reset##" + std::string(cosmeticOption.label)).c_str(),
                              UIWidgets::ButtonOptions().Size(ImVec2(80, 31)).Padding(ImVec2(2.0f, 0.0f)))) {
            onReset();
        }
    }
}

void ScanDynamicCosmetics() {
    customCosmeticEntries.clear();
    customHumanCosmeticsAvailable = false;
    customDekuCosmeticsAvailable = false;
    customGoronCosmeticsAvailable = false;
    customZoraCosmeticsAvailable = false;
    customFierceDeityCosmeticsAvailable = false;
    customKafeiCosmeticsAvailable = false;

    auto resourceManager = Ship::Context::GetInstance()->GetResourceManager();
    auto archiveManager = resourceManager->GetArchiveManager();
    RefreshCustomModelActiveFlags(archiveManager.get());
    auto materialPaths = archiveManager->ListFiles("*");
    std::unordered_map<std::string, size_t> entryIndicesByKey;

    for (const auto& materialPath : *materialPaths) {
        if (!IsCustomArchive(archiveManager->GetArchiveFromFile(materialPath))) {
            continue;
        }

        tinyxml2::XMLDocument document;
        std::shared_ptr<Fast::DisplayList> material;
        tinyxml2::XMLElement* root = nullptr;
        if (!TryLoadCustomDisplayListXml(archiveManager.get(), resourceManager.get(), materialPath, document, material,
                                         root)) {
            continue;
        }

        size_t searchStart = 0;
        for (auto* child = root->FirstChildElement(); child != nullptr; child = child->NextSiblingElement()) {
            std::string childName = child->Name();
            bool isPrimColor = childName == "SetPrimColor";
            if (!isPrimColor && childName != "SetEnvColor") {
                continue;
            }

            const char* cosmeticEntry = child->Attribute("CosmeticEntry");
            const char* cosmeticCategory = child->Attribute("CosmeticCategory");
            if (cosmeticEntry == nullptr || cosmeticEntry[0] == '\0') {
                continue;
            }

            std::string key = cosmeticEntry;
            SanitizeCustomKey(key);
            if (key.empty()) {
                continue;
            }

            Gfx expectedInstruction;
            if (isPrimColor) {
                expectedInstruction =
                    gsDPSetPrimColor(child->IntAttribute("M"), child->IntAttribute("L"), child->IntAttribute("R"),
                                     child->IntAttribute("G"), child->IntAttribute("B"), child->IntAttribute("A"));
            } else {
                expectedInstruction = gsDPSetEnvColor(child->IntAttribute("R"), child->IntAttribute("G"),
                                                      child->IntAttribute("B"), child->IntAttribute("A"));
            }

            size_t commandIndex = FindDisplayListInstructionIndex(*material, expectedInstruction, searchStart);
            if (commandIndex == SIZE_MAX) {
                continue;
            }
            searchStart = commandIndex + 1;

            MarkCustomCosmeticsAvailable(materialPath);

            size_t entryIndex = 0;
            if (auto it = entryIndicesByKey.find(key); it != entryIndicesByKey.end()) {
                entryIndex = it->second;
            } else {
                entryIndex = customCosmeticEntries.size();
                entryIndicesByKey[key] = entryIndex;

                CustomCosmeticEntry entry;
                entry.label = cosmeticEntry;
                entry.category = (cosmeticCategory != nullptr) ? cosmeticCategory : "";
                entry.baseCvar = std::string(CUSTOM_CVAR_PREFIX) + key;
                entry.valuesCvar = entry.baseCvar + ".Value";
                entry.rainbowCvar = entry.baseCvar + ".Rainbow";
                entry.lockedCvar = entry.baseCvar + ".Locked";
                entry.changedCvar = entry.baseCvar + ".Changed";
                const Color_RGBA8 defaultColor = { static_cast<uint8_t>(child->IntAttribute("R")),
                                                   static_cast<uint8_t>(child->IntAttribute("G")),
                                                   static_cast<uint8_t>(child->IntAttribute("B")),
                                                   static_cast<uint8_t>(child->IntAttribute("A")) };
                entry.option =
                    MakeCosmeticOption(entry.baseCvar.c_str(), entry.valuesCvar.c_str(), entry.rainbowCvar.c_str(),
                                       entry.lockedCvar.c_str(), entry.changedCvar.c_str(), entry.label.c_str(),
                                       COSMETICS_GROUP_MAX, defaultColor, false, true, false);
                RefreshCustomCosmeticOption(entry);
                customCosmeticEntries.push_back(std::move(entry));
            }

            CustomCosmeticBinding binding;
            binding.materialPath = materialPath;
            binding.commandIndex = commandIndex;
            binding.isPrimColor = isPrimColor;
            binding.defaultA = static_cast<uint8_t>(child->IntAttribute("A"));
            binding.primM = static_cast<uint8_t>(child->IntAttribute("M"));
            binding.primL = static_cast<uint8_t>(child->IntAttribute("L"));
            customCosmeticEntries[entryIndex].bindings.push_back(std::move(binding));
        }
    }

    std::stable_sort(
        customCosmeticEntries.begin(), customCosmeticEntries.end(),
        [](const CustomCosmeticEntry& lhs, const CustomCosmeticEntry& rhs) {
            int lhsOrder = 2;
            int rhsOrder = 2;

            for (const auto& binding : lhs.bindings) {
                lhsOrder =
                    std::min(lhsOrder, GetDynamicMaterialFormSortOrder(GetDynamicMaterialForm(binding.materialPath)));
            }
            for (const auto& binding : rhs.bindings) {
                rhsOrder =
                    std::min(rhsOrder, GetDynamicMaterialFormSortOrder(GetDynamicMaterialForm(binding.materialPath)));
            }

            if (lhsOrder != rhsOrder) {
                return lhsOrder < rhsOrder;
            }

            if (lhs.category.empty() != rhs.category.empty()) {
                return !lhs.category.empty();
            }

            if (lhs.category != rhs.category) {
                return lhs.category < rhs.category;
            }

            return lhs.label < rhs.label;
        });

    for (auto& entry : customCosmeticEntries) {
        RefreshCustomCosmeticOption(entry);
    }

    ApplyDynamicCosmetics();
}

static void DrawCustomCosmeticRow(const CustomCosmeticEntry& entry) {
    DrawCustomCosmeticColorRow(
        entry.option,
        [&entry]() {
            CVarSetInteger(entry.option.rainbowCvar, 0);
            CVarSetInteger(entry.option.changedCvar, 1);
            ShipInit::Init(entry.option.rainbowCvar);
            ShipInit::Init(entry.option.changedCvar);
            ApplyDynamicCosmetics();
            Ship::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
        },
        [&entry]() { RandomizeCustomCosmeticColor(entry); },
        [&entry]() {
            CVarSetInteger(entry.option.changedCvar, 1);
            ShipInit::Init(entry.option.changedCvar);
            ApplyDynamicCosmetics();
            Ship::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
        },
        [&entry]() { ResetCustomCosmeticColor(entry); });
}

static void DrawCustomCosmeticCategory(const char* label, const std::vector<const CustomCosmeticEntry*>& entries) {
    ImGui::Text("%s", label);
    ImGui::SameLine((ImGui::CalcTextSize("Message Light Blue (None No Shadow)").x * 1.0f) + 60.0f);
    if (UIWidgets::Button(
            ("Random##" + std::string(label)).c_str(),
            UIWidgets::ButtonOptions().Size(ImVec2(80, 31)).Padding(ImVec2(2.0f, 0.0f)).Color(THEME_COLOR))) {
        for (const auto* entry : entries) {
            RandomizeCustomCosmeticColor(*entry);
        }
        Ship::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
        ApplyDynamicCosmetics();
    }
    ImGui::SameLine();
    if (UIWidgets::Button(("Reset##" + std::string(label)).c_str(),
                          UIWidgets::ButtonOptions().Size(ImVec2(80, 31)).Padding(ImVec2(2.0f, 0.0f)))) {
        for (const auto* entry : entries) {
            ResetCustomCosmeticColor(*entry);
        }
        ApplyDynamicCosmetics();
    }
    UIWidgets::Spacer();
    for (const auto* entry : entries) {
        DrawCustomCosmeticRow(*entry);
    }
    UIWidgets::Separator(true, true, 2.0f, 2.0f);
}

bool HasCustomCosmetics() {
    return !customCosmeticEntries.empty();
}

bool HasCustomCosmeticsRainbowEnabled() {
    for (const auto& entry : customCosmeticEntries) {
        if (CVarGetInteger(entry.option.rainbowCvar, 0)) {
            return true;
        }
    }

    return false;
}

void DrawDynamicCosmetics() {
    if (customCosmeticEntries.empty()) {
        return;
    }

    std::vector<const CustomCosmeticEntry*> currentEntries;
    std::string currentCategory;

    auto flushCategory = [&]() {
        if (currentEntries.empty()) {
            return;
        }

        const char* label = currentCategory.empty() ? CUSTOM_COSMETIC_GROUP : currentCategory.c_str();
        DrawCustomCosmeticCategory(label, currentEntries);
        currentEntries.clear();
    };

    for (const auto& entry : customCosmeticEntries) {
        if (entry.category != currentCategory) {
            flushCategory();
            currentCategory = entry.category;
        }
        currentEntries.push_back(&entry);
    }

    flushCategory();
}

void UpdateCustomCosmeticsRainbow(int hue, float rainbowSpeed, int& index) {
    for (const auto& entry : customCosmeticEntries) {
        if (CVarGetInteger(entry.option.rainbowCvar, 0)) {
            double frequency = 2 * M_PI / (360 * rainbowSpeed);
            Color_RGBA8 newColor;
            newColor.r = static_cast<uint8_t>(sin(frequency * (hue + index) + 0) * 127) + 128;
            newColor.g = static_cast<uint8_t>(sin(frequency * (hue + index) + (2 * M_PI / 3)) * 127) + 128;
            newColor.b = static_cast<uint8_t>(sin(frequency * (hue + index) + (4 * M_PI / 3)) * 127) + 128;
            newColor.a = 255;
            CVarSetColor(entry.option.valuesCvar, newColor);
        }
        if (!CVarGetInteger(RAINBOW_SYNC_CVAR, 0)) {
            index += static_cast<int>(60 * rainbowSpeed);
        }
    }
}

void RandomizeAllDynamicCosmetics() {
    for (const auto& entry : customCosmeticEntries) {
        if (CVarGetInteger(entry.option.lockedCvar, 0)) {
            continue;
        }

        Color_RGBA8 color = { static_cast<uint8_t>(rand() % 256), static_cast<uint8_t>(rand() % 256),
                              static_cast<uint8_t>(rand() % 256), 255 };
        CVarSetColor(entry.option.valuesCvar, color);
        CVarSetInteger(entry.option.rainbowCvar, 0);
        CVarSetInteger(entry.option.changedCvar, 1);
        ShipInit::Init(entry.option.valuesCvar);
        ShipInit::Init(entry.option.rainbowCvar);
        ShipInit::Init(entry.option.changedCvar);
    }

    ApplyDynamicCosmetics();
}

void ResetAllDynamicCosmetics() {
    for (const auto& entry : customCosmeticEntries) {
        if (CVarGetInteger(entry.option.lockedCvar, 0)) {
            continue;
        }

        CVarClear(entry.option.changedCvar);
        CVarClear(entry.option.rainbowCvar);
        CVarClear(entry.option.lockedCvar);
        ClearCustomCosmeticValueCvars(entry.option.valuesCvar);
        ShipInit::Init(entry.option.valuesCvar);
        ShipInit::Init(entry.option.rainbowCvar);
        ShipInit::Init(entry.option.lockedCvar);
        ShipInit::Init(entry.option.changedCvar);
    }

    ApplyDynamicCosmetics();
}

void SetAllDynamicCosmeticsRainbow(bool enabled) {
    for (const auto& entry : customCosmeticEntries) {
        if (CVarGetInteger(entry.option.lockedCvar, 0)) {
            continue;
        }

        CVarSetInteger(entry.option.rainbowCvar, enabled ? 1 : 0);
        if (enabled) {
            CVarSetInteger(entry.option.changedCvar, 1);
        }
        ShipInit::Init(entry.option.rainbowCvar);
        ShipInit::Init(entry.option.changedCvar);
    }
}

void RefreshDynamicCosmeticsStateIfNeeded() {
    const std::string signature = BuildDynamicCosmeticsStateSignature();
    if (signature == sLastDynamicCosmeticsStateSignature) {
        return;
    }

    sLastDynamicCosmeticsStateSignature = signature;
    ScanDynamicCosmetics();
    RefreshBuiltInSuppressedCosmetics();
}
