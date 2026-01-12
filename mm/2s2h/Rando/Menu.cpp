#include "Rando/Rando.h"
#include "Rando/Spoiler/Spoiler.h"
#include "2s2h/BenGui/UIWidgets.hpp"
#include "Rando/CheckTracker/CheckTracker.h"
#include "Rando/MiscBehavior/ClockShuffle.h"
#include "build.h"
#include "2s2h/BenGui/BenMenu.h"
#include "2s2h/BenGui/BenGui.hpp"
#include "2s2h/Rando/Logic/Logic.h"
#include "2s2h/ShipInit.hpp"

extern "C" {
#include "overlays/actors/ovl_En_Sth/z_en_sth.h"
}

// TODO: This block should come from elsewhere, tied to data in Rando::StaticData::Options
std::unordered_map<int32_t, const char*> logicOptions = {
    { RO_LOGIC_GLITCHLESS, "Glitchless" },
    { RO_LOGIC_NO_LOGIC, "No Logic" },
    { RO_LOGIC_NEARLY_NO_LOGIC, "Nearly No Logic" },
    { RO_LOGIC_VANILLA, "Vanilla" },
};

std::unordered_map<int32_t, const char*> accessDungeonOptions = {
    { RO_ACCESS_DUNGEONS_FORM_AND_SONG, "Requires Transformation & Song" },
    { RO_ACCESS_DUNGEONS_FORM_OR_SONG, "Requires Transformation or Song" },
    { RO_ACCESS_DUNGEONS_FORM_ONLY, "Requires Only Transformation" },
    { RO_ACCESS_DUNGEONS_SONG_ONLY, "Requires Only Song" },
    { RO_ACCESS_DUNGEONS_OPEN, "Open" },
};

std::unordered_map<int32_t, const char*> accessTrialsOptions = {
    { RO_ACCESS_TRIALS_20_MASKS, "2-6-12-20 Masks" },
    { RO_ACCESS_TRIALS_REMAINS, "Requires Associated Remains" },
    { RO_ACCESS_TRIALS_FORMS, "Requires Associated Transformation" },
    { RO_ACCESS_TRIALS_OPEN, "Open" },
};

// clang-format off
std::vector<int32_t> incompatibleWithVanilla = {
    RO_SHUFFLE_BOSS_SOULS,
    RO_SHUFFLE_SWIM,
    RO_SHUFFLE_ENEMY_SOULS,
    RO_SHUFFLE_OCARINA_BUTTONS,
    RO_PLENTIFUL_ITEMS,
    RO_CLOCK_SHUFFLE,
};
// clang-format on

std::vector<RandoCheckId> checkExclusionList;
bool isExcludedInitialized = false;

namespace BenGui {
extern std::shared_ptr<Rando::CheckTracker::CheckTrackerWindow> mRandoCheckTrackerWindow;
extern std::shared_ptr<Rando::CheckTracker::SettingsWindow> mRandoCheckTrackerSettingsWindow;
extern std::shared_ptr<BenMenu> mBenMenu;
} // namespace BenGui

using namespace BenGui;
using namespace UIWidgets;

extern "C" {
#include "archives/icon_item_24_static/icon_item_24_static_yar.h"
}

// Clock UI rendering constants
static const ImVec4 CLOCK_DAY_TINT = ImVec4(1.0f, 0.85f, 0.3f, 1.0f);
static const ImVec4 CLOCK_NIGHT_TINT = ImVec4(0.3f, 0.5f, 1.0f, 1.0f);
static const float DISABLED_ITEM_ALPHA = 0.3f;
static const char* CLOCK_PROGRESSIVE_TOOLTIP =
    "\n\nTime items are not compatible with Progressive Time modes.\nSwitch to Random mode to use starting time.";

// Apply clock-specific rendering (tint colors and tooltips) based on progressive mode
static void ApplyClockItemRendering(RandoItemId item, ImVec4& tintColor, std::string& tooltipText,
                                    bool isProgressiveMode) {
    using namespace Rando::ClockItems;

    if (!IsClockItem(item)) {
        return; // Not a clock item, no special handling needed
    }

    // Apply day/night color tint
    if (IsDayClock(item)) {
        tintColor = CLOCK_DAY_TINT;
    } else {
        tintColor = CLOCK_NIGHT_TINT;
    }

    // Grey out and add tooltip if progressive mode is active
    if (item != RI_TIME_PROGRESSIVE && isProgressiveMode) {
        tintColor.w *= DISABLED_ITEM_ALPHA;
        tooltipText += CLOCK_PROGRESSIVE_TOOLTIP;
    }
}

void ClearIncompatibleSetting() {
    int32_t currentLogicSetting =
        CVarGetInteger(Rando::StaticData::Options[RO_LOGIC].cvar, Rando::StaticData::Options[RO_LOGIC].defaultValue);
    switch (currentLogicSetting) {
        // Vanilla can't add items without corresponding checks
        case RO_LOGIC_VANILLA:
            CVarClear(Rando::StaticData::Options[RO_PLENTIFUL_ITEMS].cvar);
            CVarClear(Rando::StaticData::Options[RO_SHUFFLE_BOSS_SOULS].cvar);
            CVarClear(Rando::StaticData::Options[RO_SHUFFLE_SWIM].cvar);
            CVarClear(Rando::StaticData::Options[RO_CLOCK_SHUFFLE].cvar);
            break;
        default:
            break;
    }
}

bool IncompatibleWithLogicSetting(int32_t option) {
    int32_t currentLogicSetting =
        CVarGetInteger(Rando::StaticData::Options[RO_LOGIC].cvar, Rando::StaticData::Options[RO_LOGIC].defaultValue);
    switch (currentLogicSetting) {
        case RO_LOGIC_VANILLA:
            if (std::find(incompatibleWithVanilla.begin(), incompatibleWithVanilla.end(), option) !=
                incompatibleWithVanilla.end()) {
                return true;
            }
            break;
        default:
            break;
    }
    return false;
}

void SortExcludedChecks() {
    std::sort(checkExclusionList.begin(), checkExclusionList.end());
}

void SaveExcludedChecks() {
    std::string excludedString = "";
    SortExcludedChecks();

    for (auto& data : checkExclusionList) {
        excludedString += std::to_string(data);
        excludedString += ",";
    }
    CVarSetString("gRando.ExcludedChecks", excludedString.c_str());
}

void LoadExcludedChecks() {
    std::string checksList = CVarGetString("gRando.ExcludedChecks", "");

    if (checksList != "") {
        std::string word;
        std::istringstream stream(checksList);
        while (std::getline(stream, word, ',')) {
            checkExclusionList.push_back((RandoCheckId)std::stoi(word));
        }
    }
    SortExcludedChecks();
}

static int checksInPool = 0;
static int itemsInPool = 0;
static int junkInPool = 0;
static bool ableToBalance = true;
static std::set<RandoItemId> setOfItemsInPool;
void RefreshMetrics() {
    setOfItemsInPool.clear();
    RandoSaveInfo randoSaveInfo;
    std::vector<RandoCheckId> checkPool;
    std::vector<RandoItemId> itemPool;

    // Load options from CVars
    for (auto& [randoOptionId, randoStaticOption] : Rando::StaticData::Options) {
        randoSaveInfo.randoSaveOptions[randoOptionId] =
            (uint32_t)CVarGetInteger(randoStaticOption.cvar, randoStaticOption.defaultValue);
    }
    auto startingItems = Rando::GetStartingItemsFromConfig();
    Rando::SetStartingItemsInSave(randoSaveInfo, startingItems);

    Rando::Logic::GeneratePools(randoSaveInfo, checkPool, itemPool);

    checksInPool = checkPool.size();
    itemsInPool = itemPool.size();
    junkInPool = 0;
    for (auto& item : itemPool) {
        setOfItemsInPool.insert(item);
        if (Rando::StaticData::Items[item].randoItemType == RITYPE_JUNK) {
            junkInPool++;
        }
    }
    for (auto& item : startingItems) {
        setOfItemsInPool.insert(item);
    }
    ableToBalance = checksInPool >= (itemsInPool - junkInPool);
}

static RegisterShipInitFunc refreshMetricsInit(RefreshMetrics, {
                                                                   // I Don't love this, but it works...
                                                                   "gRando.Options.RO_ACCESS_DUNGEONS",
                                                                   "gRando.Options.RO_ACCESS_MAJORA_MASKS_COUNT",
                                                                   "gRando.Options.RO_ACCESS_MAJORA_REMAINS_COUNT",
                                                                   "gRando.Options.RO_ACCESS_MOON_MASKS_COUNT",
                                                                   "gRando.Options.RO_ACCESS_MOON_REMAINS_COUNT",
                                                                   "gRando.Options.RO_ACCESS_TRIALS",
                                                                   "gRando.Options.RO_CLOCK_SHUFFLE",
                                                                   "gRando.Options.RO_CLOCK_SHUFFLE_PROGRESSIVE",
                                                                   "gRando.Options.RO_HINTS_BOSS_REMAINS",
                                                                   "gRando.Options.RO_HINTS_GOSSIP_STONES",
                                                                   "gRando.Options.RO_HINTS_HOOKSHOT",
                                                                   "gRando.Options.RO_HINTS_OATH_TO_ORDER",
                                                                   "gRando.Options.RO_HINTS_PURCHASEABLE",
                                                                   "gRando.Options.RO_HINTS_SPIDER_HOUSES",
                                                                   "gRando.Options.RO_TRAP_AMOUNT",
                                                                   "gRando.Options.RO_LOGIC",
                                                                   "gRando.Options.RO_MINIMUM_SKULLTULA_TOKENS",
                                                                   "gRando.Options.RO_MINIMUM_STRAY_FAIRIES",
                                                                   "gRando.Options.RO_PLENTIFUL_ITEMS",
                                                                   "gRando.Options.RO_SHUFFLE_BARREL_DROPS",
                                                                   "gRando.Options.RO_SHUFFLE_BOSS_REMAINS",
                                                                   "gRando.Options.RO_SHUFFLE_BOSS_SOULS",
                                                                   "gRando.Options.RO_SHUFFLE_COWS",
                                                                   "gRando.Options.RO_SHUFFLE_CRATE_DROPS",
                                                                   "gRando.Options.RO_SHUFFLE_ENEMY_DROPS",
                                                                   "gRando.Options.RO_SHUFFLE_ENEMY_SOULS",
                                                                   "gRando.Options.RO_SHUFFLE_FREESTANDING_ITEMS",
                                                                   "gRando.Options.RO_SHUFFLE_FROGS",
                                                                   "gRando.Options.RO_SHUFFLE_GOLD_SKULLTULAS",
                                                                   "gRando.Options.RO_SHUFFLE_GRASS_DROPS",
                                                                   "gRando.Options.RO_SHUFFLE_TRAPS",
                                                                   "gRando.Options.RO_SHUFFLE_OCARINA_BUTTONS",
                                                                   "gRando.Options.RO_SHUFFLE_OWL_STATUES",
                                                                   "gRando.Options.RO_SHUFFLE_POT_DROPS",
                                                                   "gRando.Options.RO_SHUFFLE_SHOPS",
                                                                   "gRando.Options.RO_SHUFFLE_SNOWBALL_DROPS",
                                                                   "gRando.Options.RO_SHUFFLE_SWIM",
                                                                   "gRando.Options.RO_SHUFFLE_TINGLE_SHOPS",
                                                                   "gRando.Options.RO_SHUFFLE_TRIFORCE_PIECES",
                                                                   "gRando.Options.RO_STARTING_CONSUMABLES",
                                                                   "gRando.Options.RO_STARTING_HEALTH",
                                                                   "gRando.Options.RO_STARTING_MAPS_AND_COMPASSES",
                                                                   "gRando.Options.RO_STARTING_RUPEES",
                                                                   "gRando.Options.RO_TRIFORCE_PIECES_MAX",
                                                                   "gRando.Options.RO_TRIFORCE_PIECES_REQUIRED",
                                                               });

static void DrawGeneralTab() {
    ImGui::BeginChild("randoSettings");
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 1, 1, 0.5f));
    ImGui::TextWrapped(
        "Explore the menus for various enhancements and time savers; most are not enabled by default in Rando.");
    ImGui::PopStyleColor();

    ImGui::SeparatorText("Seed Generation");
    UIWidgets::CVarCheckbox("Enable Rando (Randomizes new files upon creation)", "gRando.Enabled");

    if (UIWidgets::CVarCombobox("Seed", "gRando.SpoilerFileIndex", Rando::Spoiler::spoilerOptions)) {
        if (CVarGetInteger("gRando.SpoilerFileIndex", 0) == 0) {
            CVarSetString("gRando.SpoilerFile", "");
        } else {
            CVarSetString("gRando.SpoilerFile",
                          Rando::Spoiler::spoilerOptions[CVarGetInteger("gRando.SpoilerFileIndex", 0)].c_str());
        }
    }

    if (CVarGetInteger("gRando.SpoilerFileIndex", 0) == 0) {
        UIWidgets::PushStyleSlider();
        static char seed[256];
        std::string stringSeed = CVarGetString("gRando.InputSeed", "");
        strcpy(seed, stringSeed.c_str());
        ImGui::InputText("##Seed", seed, sizeof(seed), ImGuiInputTextFlags_CallbackAlways,
                         [](ImGuiInputTextCallbackData* data) {
                             CVarSetString("gRando.InputSeed", data->Buf);
                             return 0;
                         });
        if (stringSeed.length() < 1) {
            ImGui::SameLine(17.0f);
            ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.4f), "Leave blank for random seed");
        }
        UIWidgets::PopStyleSlider();

        UIWidgets::CVarCheckbox("Generate Spoiler File", "gRando.GenerateSpoiler",
                                CheckboxOptions().DefaultValue(true));
    }

    float mainWidth = 300.0f; // Arbitrary width for progress bars
    float itemProgress = mainWidth * (static_cast<float>(itemsInPool) / static_cast<float>(checksInPool));
    float junkProgress = static_cast<float>(junkInPool) / static_cast<float>(itemsInPool);

    ImGui::SeparatorText("Current Settings Metrics");
    ImGui::Text("Checks in pool: %d", checksInPool);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 4.0f);
    ImGui::PushStyleColor(ImGuiCol_PlotHistogram, UIWidgets::ColorValues.at(THEME_COLOR));
    ImGui::PushStyleColor(ImGuiCol_FrameBg, UIWidgets::ColorValues.at(UIWidgets::Colors::DarkGray));
    ImGui::ProgressBar(1.0f, ImVec2(mainWidth, 0.0f), "");
    ImGui::Text("Items in Pool: %d", itemsInPool);
    ImGui::SameLine();
    ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), "(%d Junk Items)", junkInPool);

    ImGui::ProgressBar(1.0f - junkProgress, ImVec2(itemProgress, 0.0f), "");
    ImGui::PopStyleColor(2);
    ImGui::PopStyleVar();
    ImGui::Text("Able to Balance:");
    ImGui::SameLine();
    if (ableToBalance) {
        ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Yes");
    } else {
        ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "No");
    }

    ImGui::SeparatorText("Enhancements");
    UIWidgets::CVarCheckbox("Container Style Matches Contents", "gRando.CSMC");
    UIWidgets::Tooltip("This will make the contents of a container match the container itself. This currently only "
                       "applies to chests and pots.");
    UIWidgets::WindowButton("Check Tracker", "gWindows.CheckTracker", BenGui::mRandoCheckTrackerWindow,
                            { .size = ImVec2((ImGui::GetContentRegionAvail().x - 48.0f), 40.0f) });
    ImGui::SameLine();
    if (UIWidgets::Button(ICON_FA_COG, { .size = ImVec2(40.0f, 40.0f) })) {
        BenGui::mRandoCheckTrackerSettingsWindow->ToggleVisibility();
    }
    ImGui::EndChild();
    ImGui::SameLine();
}

static void DrawLogicConditionsTab() {
    f32 columnWidth = ImGui::GetContentRegionAvail().x / 2 - (ImGui::GetStyle().ItemSpacing.x * 2);
    ImGui::BeginChild("randoLogicColumn1", ImVec2(columnWidth, 0));
    if (UIWidgets::CVarCombobox("Logic", Rando::StaticData::Options[RO_LOGIC].cvar, &logicOptions)) {
        ClearIncompatibleSetting();
    }
    UIWidgets::Tooltip(
        "Glitchless - The items are shuffled in a way that guarantees the seed is beatable without "
        "glitches.\n\n"
        "No Logic - The items are shuffled completely randomly, this can result in unbeatable seeds, and "
        "will require heavy use of glitches.\n\n"
        "Nearly No Logic - The items are shuffled completely randomly, with the following exceptions:\n"
        "- Oath to Order and Remains cannot be placed on the Moon.\n"
        "- Deku Mask, Zora Mask, Sonata, and Bossa Nova cannot be placed in their respective Temples or on "
        "the Moon.\n\n"
        "Vanilla - The items are not shuffled.\n"
        "Not compatible with settings that add items to the pool, like Boss Souls or Plentiful Items.");
    ImGui::EndChild();
    ImGui::SameLine();
    ImGui::BeginChild("randoLogicColumn2", ImVec2(columnWidth, 0));

    UIWidgets::CVarCombobox("Dungeon Access", Rando::StaticData::Options[RO_ACCESS_DUNGEONS].cvar,
                            &accessDungeonOptions);
    UIWidgets::Tooltip("Dungeon access requirements:\n\n"
                       "Requires Transformation & Song - Requires both the correct form and the song (Vanilla).\n\n"
                       "Requires Transformation or Song - Requires either the correct form or the song.\n\n"
                       "Requires Only Transformation - Requires only the correct form.\n\n"
                       "Requires Only Song - Requires only the correct song.\n\n"
                       "Open - Dungeons will be open with no requirements.");
    UIWidgets::CVarSliderInt("Majora Access Remains Required",
                             Rando::StaticData::Options[RO_ACCESS_MAJORA_REMAINS_COUNT].cvar,
                             IntSliderOptions().Min(0).Max(4).DefaultValue(0));
    UIWidgets::CVarSliderInt("Majora Access Masks Required",
                             Rando::StaticData::Options[RO_ACCESS_MAJORA_MASKS_COUNT].cvar,
                             IntSliderOptions().Min(0).Max(20).DefaultValue(0));
    UIWidgets::CVarSliderInt("Moon Access Remains Required",
                             Rando::StaticData::Options[RO_ACCESS_MOON_REMAINS_COUNT].cvar,
                             IntSliderOptions().Min(0).Max(4).DefaultValue(4));
    UIWidgets::CVarSliderInt("Moon Access Masks Required", Rando::StaticData::Options[RO_ACCESS_MOON_MASKS_COUNT].cvar,
                             IntSliderOptions().Min(0).Max(20).DefaultValue(0));
    UIWidgets::CVarCombobox("Trials Access", Rando::StaticData::Options[RO_ACCESS_TRIALS].cvar, &accessTrialsOptions);
    ImGui::EndChild();
}

static void DrawShufflesTab() {
    f32 columnWidth = ImGui::GetContentRegionAvail().x / 2 - (ImGui::GetStyle().ItemSpacing.x * 2);
    f32 halfHeight = 0;
    ImGui::SeparatorText("Shuffle Options");
    ImGui::BeginChild("randoShufflesColumn1", ImVec2(columnWidth, halfHeight));
    CVarCheckbox("Shuffle Songs", "gPlaceholderBool",
                 CheckboxOptions({ { .disabled = true, .disabledTooltip = "Coming Soon" } }).DefaultValue(true));
    CVarCheckbox("Shuffle Owl Statues", Rando::StaticData::Options[RO_SHUFFLE_OWL_STATUES].cvar);
    CVarCheckbox("Shuffle Shops", Rando::StaticData::Options[RO_SHUFFLE_SHOPS].cvar);
    CVarCheckbox("Shuffle Tingle Maps", Rando::StaticData::Options[RO_SHUFFLE_TINGLE_SHOPS].cvar);
    CVarCheckbox("Shuffle Boss Remains", Rando::StaticData::Options[RO_SHUFFLE_BOSS_REMAINS].cvar);
    CVarCheckbox("Shuffle Cows", Rando::StaticData::Options[RO_SHUFFLE_COWS].cvar);
    CVarCheckbox("Shuffle Gold Skulltula Tokens", Rando::StaticData::Options[RO_SHUFFLE_GOLD_SKULLTULAS].cvar);
    CVarSliderInt("Minimum Required Gold Skulltula Tokens",
                  Rando::StaticData::Options[RO_MINIMUM_SKULLTULA_TOKENS].cvar,
                  IntSliderOptions(
                      { { .tooltip = "Minimum Gold Skulltula tokens needed to obtain the Spider House checks.",
                          .disabled = !CVarGetInteger(Rando::StaticData::Options[RO_SHUFFLE_GOLD_SKULLTULAS].cvar, 0),
                          .disabledTooltip = "Only takes effect if Gold Skulltula Tokens are shuffled." } })
                      .Min(1)
                      .Max(SPIDER_HOUSE_TOKENS_REQUIRED)
                      .DefaultValue(SPIDER_HOUSE_TOKENS_REQUIRED));
    CVarSliderInt(
        "Minimum Required Stray Fairies", Rando::StaticData::Options[RO_MINIMUM_STRAY_FAIRIES].cvar,
        IntSliderOptions({ { .tooltip = "Minimum Stray Fairies needed to obtain the corresponding Great Fairy check.\n"
                                        "Does not affect the Clock Town fairy." } })
            .Min(1)
            .Max(STRAY_FAIRY_SCATTERED_TOTAL)
            .DefaultValue(STRAY_FAIRY_SCATTERED_TOTAL));
    ImGui::EndChild();
    ImGui::SameLine();
    ImGui::BeginChild("randoShufflesColumn2", ImVec2(columnWidth, halfHeight));
    CVarCheckbox("Shuffle Pot Drops", Rando::StaticData::Options[RO_SHUFFLE_POT_DROPS].cvar);
    CVarCheckbox("Shuffle Crate Drops", Rando::StaticData::Options[RO_SHUFFLE_CRATE_DROPS].cvar);
    CVarCheckbox("Shuffle Barrel Drops", Rando::StaticData::Options[RO_SHUFFLE_BARREL_DROPS].cvar);
    CVarCheckbox("Shuffle Snowball Drops", Rando::StaticData::Options[RO_SHUFFLE_SNOWBALL_DROPS].cvar);
    CVarCheckbox("Shuffle Grass Drops", Rando::StaticData::Options[RO_SHUFFLE_GRASS_DROPS].cvar);
    CVarCheckbox("Shuffle Frogs", Rando::StaticData::Options[RO_SHUFFLE_FROGS].cvar);
    CVarCheckbox("Shuffle Hive Drops", "gPlaceholderBool",
                 CheckboxOptions({ { .disabled = true, .disabledTooltip = "Coming Soon" } }));
    CVarCheckbox("Shuffle Freestanding Items", Rando::StaticData::Options[RO_SHUFFLE_FREESTANDING_ITEMS].cvar);
    CVarCheckbox("Shuffle Wonder Items", "gPlaceholderBool",
                 CheckboxOptions({ { .disabled = true, .disabledTooltip = "Coming Soon" } }));
    ImGui::EndChild();
    ImGui::SameLine();
    ImGui::BeginChild("randoLocationsColumn3", ImVec2(columnWidth, halfHeight));
    ImGui::EndChild();
}

static void DrawItemsTab() {
    f32 columnWidth = ImGui::GetContentRegionAvail().x / 3 - (ImGui::GetStyle().ItemSpacing.x * 2);
    ImGui::BeginChild("randoItemsColumn1", ImVec2(columnWidth, ImGui::GetContentRegionAvail().y));
    CVarCheckbox("Shuffle Swim", Rando::StaticData::Options[RO_SHUFFLE_SWIM].cvar,
                 CheckboxOptions({ { .tooltip = "Shuffles the ability to Swim, entering the Swim state or submerging\n"
                                                "into deep water will respawn Link.",
                                     .disabled = IncompatibleWithLogicSetting(RO_SHUFFLE_SWIM),
                                     .disabledTooltip = "Incompatible with current Logic Setting" } }));
    CVarCheckbox("Shuffle Ocarina Buttons", Rando::StaticData::Options[RO_SHUFFLE_OCARINA_BUTTONS].cvar,
                 CheckboxOptions({ { .tooltip = "Shuffles the Buttons used to play Ocarina Notes.\n"
                                                "You will be unable to play a song until you find all\n"
                                                "notes for the given melody.",
                                     .disabled = IncompatibleWithLogicSetting(RO_SHUFFLE_OCARINA_BUTTONS),
                                     .disabledTooltip = "Incompatible with current Logic Setting" } }));
    CVarCheckbox("Deku Stick Bag", "gPlaceholderBool",
                 CheckboxOptions({ { .disabled = true, .disabledTooltip = "Coming Soon" } }));
    CVarCheckbox("Deku Nut Bag", "gPlaceholderBool",
                 CheckboxOptions({ { .disabled = true, .disabledTooltip = "Coming Soon" } }));
    CVarCheckbox("Skeleton Key", "gPlaceholderBool",
                 CheckboxOptions({ { .disabled = true, .disabledTooltip = "Coming Soon" } }));
    CVarCheckbox("Child Wallet", "gPlaceholderBool",
                 CheckboxOptions({ { .disabled = true, .disabledTooltip = "Coming Soon" } }));
    CVarCheckbox("Infinite Upgrades", "gPlaceholderBool",
                 CheckboxOptions({ { .disabled = true, .disabledTooltip = "Coming Soon" } }));
    CVarCheckbox("Song of Double Time", "gPlaceholderBool",
                 CheckboxOptions({ { .disabled = true, .disabledTooltip = "Coming Soon" } }));
    CVarCheckbox("Inverted Song of Time", "gPlaceholderBool",
                 CheckboxOptions({ { .disabled = true, .disabledTooltip = "Coming Soon" } }));
    CVarCheckbox("Saria's Song", "gPlaceholderBool",
                 CheckboxOptions({ { .disabled = true, .disabledTooltip = "Coming Soon" } }));
    CVarCheckbox("Sun's Song", "gPlaceholderBool",
                 CheckboxOptions({ { .disabled = true, .disabledTooltip = "Coming Soon" } }));
    ImGui::EndChild();
    ImGui::SameLine();
    ImGui::BeginChild("randoItemsColumn2", ImVec2(columnWidth, ImGui::GetContentRegionAvail().y));
    CVarCheckbox(
        "Plentiful Items", Rando::StaticData::Options[RO_PLENTIFUL_ITEMS].cvar,
        CheckboxOptions({ { .tooltip = "Major items, masks, and keys will have an extra copy added to the item pool. \n"
                                       "Lesser items, stray fairies, and skulltula tokens will have a chance for an "
                                       "extra copy to be added to the item pool.",
                            .disabled = IncompatibleWithLogicSetting(RO_PLENTIFUL_ITEMS),
                            .disabledTooltip = "Incompatible with current Logic Setting" } }));
    CVarCheckbox(
        "Boss Souls", Rando::StaticData::Options[RO_SHUFFLE_BOSS_SOULS].cvar,
        CheckboxOptions({ { .tooltip = "Adds the \"souls\" of the five bosses to the item pool. Boss Souls are items "
                                       "that must be found in order for their corresponding boss to spawn.",
                            .disabled = IncompatibleWithLogicSetting(RO_SHUFFLE_BOSS_SOULS),
                            .disabledTooltip = "Incompatible with current Logic Setting" } }));
    CVarCheckbox("Enemy Drops", Rando::StaticData::Options[RO_SHUFFLE_ENEMY_DROPS].cvar,
                 CheckboxOptions({ { .tooltip = "Shuffles the first drop from a non Boss Enemy." } }));
    CVarCheckbox(
        "Enemy Souls", Rando::StaticData::Options[RO_SHUFFLE_ENEMY_SOULS].cvar,
        CheckboxOptions({ { .tooltip = "Adds the \"souls\" of regular enemies to the item pool. Enemy Souls are items "
                                       "that must be found in order for their corresponding enemy to spawn.",
                            .disabled = IncompatibleWithLogicSetting(RO_SHUFFLE_ENEMY_SOULS),
                            .disabledTooltip = "Incompatible with current Logic Setting" } }));
    CVarCheckbox("Shuffle Time", Rando::StaticData::Options[RO_CLOCK_SHUFFLE].cvar,
                 CheckboxOptions({ { .tooltip = "Breaks the 3-day cycle into 6 separate half-days (Day 1 Day/Night, "
                                                "Day 2 Day/Night, Day 3 Day/Night) that must be unlocked as items. "
                                                "Players can only access time periods they've obtained. Attempting to "
                                                "access unowned time redirects to the next owned half-day.",
                                     .disabled = IncompatibleWithLogicSetting(RO_CLOCK_SHUFFLE),
                                     .disabledTooltip = "Incompatible with current Logic Setting" } }));
    // Only show time progression options when shuffle time is enabled
    if (CVarGetInteger(Rando::StaticData::Options[RO_CLOCK_SHUFFLE].cvar, 0)) {
        static std::unordered_map<int32_t, const char*> clockModeOptions = {
            { RO_CLOCK_SHUFFLE_RANDOM, "Random" },
            { RO_CLOCK_SHUFFLE_ASCENDING, "Progressive: Ascending" },
            { RO_CLOCK_SHUFFLE_DESCENDING, "Progressive: Descending" },
        };
        {
            UIWidgets::CVarCombobox(
                "Time Progression Mode", Rando::StaticData::Options[RO_CLOCK_SHUFFLE_PROGRESSIVE].cvar,
                &clockModeOptions,
                UIWidgets::ComboboxOptions().Tooltip(
                    "Random: All 6 half-days shuffled randomly. Player starts with one random half-day.\n\n"
                    "Progressive Ascending: Unlocks half-days in order (D1, N1, D2, N2, D3, N3).\n\n"
                    "Progressive Descending: Unlocks half-days in reverse order (N3, D3, N2, D2, N1, D1)."));
        }
        // Terminal time slider (Final Hours start time)
        {
            int32_t terminalMinutes = CVarGetInteger(Rando::StaticData::Options[RO_CLOCK_TERMINAL_TIME].cvar, 0);
            int hours = terminalMinutes / 60;
            int minutes = terminalMinutes % 60;

            ImGui::Spacing();
            ImGui::Text("Final Hours Start Time: %02d:%02d", hours, minutes);
            ImGui::Spacing();
            UIWidgets::CVarSliderInt(
                "Final Hours Start Time", Rando::StaticData::Options[RO_CLOCK_TERMINAL_TIME].cvar,
                UIWidgets::IntSliderOptions()
                    .Min(0)
                    .Max(359)
                    .DefaultValue(0)
                    .LabelPosition(UIWidgets::LabelPosition::None)
                    .Tooltip("Controls when the final hours countdown begins (00:00 to 05:59). "
                             "When you run out of owned half-days, this allows the player control over how much "
                             "time is left before the moon crash.\n\n"
                             "This setting is baked into the seed and cannot be changed after generation."));
        }
    }

    CVarCheckbox("Triforce Hunt", Rando::StaticData::Options[RO_SHUFFLE_TRIFORCE_PIECES].cvar);
    ImGui::BeginDisabled(!CVarGetInteger(Rando::StaticData::Options[RO_SHUFFLE_TRIFORCE_PIECES].cvar, RO_GENERIC_OFF));
    CVarSliderInt(
        "Required Triforce Pieces", Rando::StaticData::Options[RO_TRIFORCE_PIECES_REQUIRED].cvar,
        IntSliderOptions({})
            .Min(1)
            .Max(CVarGetInteger(Rando::StaticData::Options[RO_TRIFORCE_PIECES_MAX].cvar, DEFAULT_TRIFORCE_PIECES_MAX))
            .DefaultValue(DEFAULT_TRIFORCE_PIECES_MAX));
    if (CVarSliderInt(
            "Shuffled Triforce Pieces", Rando::StaticData::Options[RO_TRIFORCE_PIECES_MAX].cvar,
            IntSliderOptions({})
                .Min(1)
                .Max(1000)
                .DefaultValue(DEFAULT_TRIFORCE_PIECES_MAX)
                .Tooltip("If the maximum amount of placeable pieces exceeds what will allow the seed to generate, the "
                         "amount will be adjusted automatically."))) {
        if (CVarGetInteger(Rando::StaticData::Options[RO_TRIFORCE_PIECES_REQUIRED].cvar, DEFAULT_TRIFORCE_PIECES_MAX) >
            CVarGetInteger(Rando::StaticData::Options[RO_TRIFORCE_PIECES_MAX].cvar, DEFAULT_TRIFORCE_PIECES_MAX)) {
            CVarSetInteger(
                Rando::StaticData::Options[RO_TRIFORCE_PIECES_REQUIRED].cvar,
                CVarGetInteger(Rando::StaticData::Options[RO_TRIFORCE_PIECES_MAX].cvar, DEFAULT_TRIFORCE_PIECES_MAX));
        }
    }

    ImGui::EndDisabled();
    ImGui::EndChild();
    ImGui::SameLine();
    ImGui::BeginChild("randoItemsColumn3", ImVec2(columnWidth, ImGui::GetContentRegionAvail().y));
    CVarCheckbox("Shuffle Traps", Rando::StaticData::Options[RO_SHUFFLE_TRAPS].cvar,
                 CheckboxOptions({ { .tooltip = "Ice Trap time!" } }));
    CVarSliderInt(
        "##trapcount", Rando::StaticData::Options[RO_TRAP_AMOUNT].cvar,
        IntSliderOptions({ { .tooltip = "How many Traps are shuffled into the Item Pool.",
                             .disabled = (bool)!CVarGetInteger(Rando::StaticData::Options[RO_SHUFFLE_TRAPS].cvar, 0),
                             .disabledTooltip = "Shuffle Traps is disabled." } })
            .LabelPosition(LabelPosition::None)
            .Color(UIWidgets::Colors(CVarGetInteger("gSettings.Menu.Theme", 5)))
            .Format("Traps: %i")
            .Min(1)
            .Max(100)
            .DefaultValue(5));
    ImGui::SeparatorText("Toggle Trap Types");
    CVarCheckbox(
        "Freeze Traps", "gRando.Traps.Freeze",
        CheckboxOptions({ { .tooltip = "Freezes Link in place.",
                            .disabled = (bool)!CVarGetInteger(Rando::StaticData::Options[RO_SHUFFLE_TRAPS].cvar, 0),
                            .disabledTooltip = "Shuffle Traps is disabled." } }));
    CVarCheckbox(
        "Blast Traps", "gRando.Traps.Blast",
        CheckboxOptions({ { .tooltip = "Link explodes with Powder Keg force.",
                            .disabled = (bool)!CVarGetInteger(Rando::StaticData::Options[RO_SHUFFLE_TRAPS].cvar, 0),
                            .disabledTooltip = "Shuffle Traps is disabled." } }));
    CVarCheckbox(
        "Shock Traps", "gRando.Traps.Shock",
        CheckboxOptions({ { .tooltip = "Shocks Link for a few seconds.",
                            .disabled = (bool)!CVarGetInteger(Rando::StaticData::Options[RO_SHUFFLE_TRAPS].cvar, 0),
                            .disabledTooltip = "Shuffle Traps is disabled." } }));
    CVarCheckbox(
        "Jinx Traps", "gRando.Traps.Jinx",
        CheckboxOptions({ { .tooltip = "Afflicts Link with Jinx.",
                            .disabled = (bool)!CVarGetInteger(Rando::StaticData::Options[RO_SHUFFLE_TRAPS].cvar, 0),
                            .disabledTooltip = "Shuffle Traps is disabled." } }));
    CVarCheckbox(
        "Wallet Traps", "gRando.Traps.Wallet",
        CheckboxOptions({ { .tooltip = "Links rupees scatter around him.",
                            .disabled = (bool)!CVarGetInteger(Rando::StaticData::Options[RO_SHUFFLE_TRAPS].cvar, 0),
                            .disabledTooltip = "Shuffle Traps is disabled." } }));
    CVarCheckbox( // This only spawns a Like Like, more enemies may be added in the future but each would need fine
                  // tuning
        "Like Like Traps", "gRando.Traps.Enemy",
        CheckboxOptions({ { .tooltip = "Spawns a Like Like on top of Link.",
                            .disabled = (bool)!CVarGetInteger(Rando::StaticData::Options[RO_SHUFFLE_TRAPS].cvar, 0),
                            .disabledTooltip = "Shuffle Traps is disabled." } }));
    CVarCheckbox(
        "Time Traps", "gRando.Traps.Time",
        CheckboxOptions({ { .tooltip = "Advances Time 90 Minutes (Game Time).",
                            .disabled = (bool)!CVarGetInteger(Rando::StaticData::Options[RO_SHUFFLE_TRAPS].cvar, 0),
                            .disabledTooltip = "Shuffle Traps is disabled." } }));
    ImGui::EndChild();
}

static void DrawStartingItemsTab() {
    f32 columnWidth = ImGui::GetContentRegionAvail().x / 2 - (ImGui::GetStyle().ItemSpacing.x * 2);
    f32 quarterHeight = ImGui::GetContentRegionAvail().y / 4 - (ImGui::GetStyle().ItemSpacing.y * 4);
    int tableColumns = 0;
    ImGui::BeginChild("randoStartingOptions", ImVec2(0, 120.0f));
    ImGui::SeparatorText("Starting Options");
    if (ImGui::BeginTable("Starting Options", 3)) {
        ImGui::TableNextColumn();
        CVarCheckbox("Wallet Full", Rando::StaticData::Options[RO_STARTING_RUPEES].cvar,
                     CheckboxOptions({ {
                         .tooltip = "Start with a full wallet",
                     } }));

        ImGui::TableNextColumn();
        CVarCheckbox("Consumables Full", Rando::StaticData::Options[RO_STARTING_CONSUMABLES].cvar,
                     CheckboxOptions({ {
                         .tooltip = "Start with full Deku Sticks and Deku Nuts",
                     } }));

        ImGui::TableNextColumn();
        CVarCheckbox("Maps and Compasses", Rando::StaticData::Options[RO_STARTING_MAPS_AND_COMPASSES].cvar,
                     CheckboxOptions({ {
                         .tooltip = "Enables maps and compasses everywhere",
                     } }));

        ImGui::TableNextColumn();
        CVarSliderInt("Health", Rando::StaticData::Options[RO_STARTING_HEALTH].cvar,
                      IntSliderOptions()
                          .Min(1)
                          .Max(20)
                          .DefaultValue(3)
                          .LabelPosition(LabelPosition::None)
                          .Format("%d Hearts")
                          .Color(Colors::Red));

        ImGui::EndTable();
    }
    ImGui::EndChild();
    ImGui::BeginChild("randoStartingItems1", ImVec2(0, quarterHeight));
    ImGui::SeparatorText("Starting Items");
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(15, 15));
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 1.0f, 1.0f, 0.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 1.0f, 1.0f, 0.2f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.0f, 1.0f, 1.0f, 0.1f));

    auto setStartingItemsList = Rando::GetStartingItemsFromConfig();

    uint32_t listIndex = 0;
    for (auto& startingItem : setStartingItemsList) {
        ImGui::PushID(listIndex);
        ImVec2 imageSize = ImVec2(42.0f, 42.0f);
        if ((startingItem >= RI_SONG_ELEGY && startingItem <= RI_SONG_TIME) || startingItem == RI_PROGRESSIVE_LULLABY) {
            imageSize.x /= 1.5f;
        }

        Rando::StaticData::RandoStaticItem randoStaticItem = Rando::StaticData::Items[startingItem];
        const char* texturePath = Rando::StaticData::GetIconTexturePath(startingItem);
        ImTextureID textureId = Ship::Context::GetInstance()->GetWindow()->GetGui()->GetTextureByName(texturePath);

        ImVec4 tintColor =
            Ship_GetItemColorTint(startingItem == RI_PROGRESSIVE_LULLABY ? ITEM_SONG_LULLABY : randoStaticItem.itemId);
        std::string tooltipText = randoStaticItem.name;
        bool isProgressiveMode = CVarGetInteger(Rando::StaticData::Options[RO_CLOCK_SHUFFLE_PROGRESSIVE].cvar,
                                                RO_CLOCK_SHUFFLE_RANDOM) != RO_CLOCK_SHUFFLE_RANDOM;
        ApplyClockItemRendering(startingItem, tintColor, tooltipText, isProgressiveMode);

        if (ImGui::ImageButton(std::to_string(listIndex).c_str(), textureId, imageSize, ImVec2(0, 0), ImVec2(1, 1),
                               ImVec4(0, 0, 0, 0), tintColor)) {
            setStartingItemsList.erase(setStartingItemsList.begin() + listIndex);
            Rando::SetStartingItemsInConfig(setStartingItemsList);
            RefreshMetrics();
        }
        UIWidgets::Tooltip(tooltipText.c_str());
        listIndex++;

        if ((listIndex + 1) % 15 != 0) {
            ImGui::SameLine();
        }
        ImGui::PopID();
    }

    ImGui::PopStyleColor(3);
    ImGui::PopStyleVar(2);

    ImGui::EndChild();
    ImGui::BeginChild("randoStartingItems2", ImVec2(0, 0));
    ImGui::SeparatorText("Available Items");

    for (auto& category : Rando::StaticData::StartingItemsMap) {
        tableColumns = 5;
        if (category.first == STARTING_ITEMS_MASK) {
            tableColumns++;
        } else if (category.first == STARTING_ITEMS_MISC) {
            tableColumns = 6; // Need 6 columns for the 6 time items on their own row
        }
        ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0, 0, 0, 0));
        if (ImGui::BeginChild(std::to_string(category.first).c_str(), ImVec2(0, 0),
                              ImGuiChildFlags_AlwaysAutoResize | ImGuiChildFlags_AutoResizeX |
                                  ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_Borders)) {
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 1.0f, 1.0f, 0.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 1.0f, 1.0f, 0.2f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.0f, 1.0f, 1.0f, 0.1f));

            if (ImGui::BeginTable(std::to_string(category.first).c_str(), tableColumns)) {
                for (int i = 0; i < tableColumns; i++) {
                    ImGui::TableSetupColumn("item", ImGuiTableColumnFlags_WidthFixed, 50.0f);
                }
                for (auto& item : category.second) {
                    if (setOfItemsInPool.count(item) == 0) {
                        // Skip items that are not in the item pool
                        continue;
                    }

                    ImVec2 imageSize = ImVec2(42.0f, 42.0f);
                    if ((item >= RI_SONG_ELEGY && item <= RI_SONG_TIME) || item == RI_PROGRESSIVE_LULLABY) {
                        imageSize.x /= 1.5f;
                    }

                    Rando::StaticData::RandoStaticItem randoStaticItem = Rando::StaticData::Items[item];
                    const char* texturePath = Rando::StaticData::GetIconTexturePath(item);
                    ImTextureID textureId =
                        Ship::Context::GetInstance()->GetWindow()->GetGui()->GetTextureByName(texturePath);

                    // Force new row for Song of Time, first frog, and first time item
                    if (item == RI_SONG_TIME || item == RI_FROG_BLUE || item == RI_TIME_DAY_1) {
                        ImGui::TableNextRow();
                    }
                    ImGui::TableNextColumn();

                    ImVec4 tintColor = Ship_GetItemColorTint(item == RI_PROGRESSIVE_LULLABY ? ITEM_SONG_LULLABY
                                                                                            : randoStaticItem.itemId);
                    std::string tooltipText = randoStaticItem.name;
                    bool isProgressiveMode =
                        CVarGetInteger(Rando::StaticData::Options[RO_CLOCK_SHUFFLE_PROGRESSIVE].cvar,
                                       RO_CLOCK_SHUFFLE_RANDOM) != RO_CLOCK_SHUFFLE_RANDOM;
                    ApplyClockItemRendering(item, tintColor, tooltipText, isProgressiveMode);

                    if (ImGui::ImageButton(std::to_string(item).c_str(), textureId, imageSize, ImVec2(0, 0),
                                           ImVec2(1, 1), ImVec4(0, 0, 0, 0), tintColor)) {
                        if (std::count(setStartingItemsList.begin(), setStartingItemsList.end(), item) <
                            (Rando::StaticData::MaxStartingItemsMap.count(item)
                                 ? Rando::StaticData::MaxStartingItemsMap[item]
                                 : 1)) {

                            setStartingItemsList.push_back(item);
                            Rando::SetStartingItemsInConfig(setStartingItemsList);
                            RefreshMetrics();
                        }
                    }
                    UIWidgets::Tooltip(tooltipText.c_str());
                }
                ImGui::EndTable();
            }

            ImGui::PopStyleColor(3);
            ImGui::PopStyleVar(2);
            ImGui::EndChild();
        }
        ImGui::PopStyleColor(1);
        if (category.first != STARTING_ITEMS_QUEST) {
            ImGui::SameLine();
        }
    }
    ImGui::EndChild();
}

static f32 CalcButtonWidth(const char* label) {
    const auto& style = ImGui::GetStyle();
    f32 buttonPaddingX = style.FramePadding.x;
    f32 buttonBorderAndSpacing = style.FrameBorderSize + style.ItemSpacing.x * 0.5f;
    return ImGui::CalcTextSize(label).x + (buttonPaddingX * 2) + buttonBorderAndSpacing;
}

static void DrawFilterWithButton(ImGuiTextFilter& filter, const char* placeholderText, const char* buttonLabel,
                                 f32 availableWidth, UIWidgets::Colors menuThemeColor, bool& actionFlagWhenFiltered,
                                 bool& actionFlagWhenAll, f32 filterButtonSpacing, f32 filterButtonOffset,
                                 f32 filterButtonPaddingY, f32 filterSearchLabelOffset) {
    UIWidgets::PushStyleCombobox(menuThemeColor);

    f32 buttonWidth = CalcButtonWidth(buttonLabel);
    filter.Draw("##filter", availableWidth - buttonWidth - filterButtonSpacing);
    if (!filter.IsActive()) {
        ImGui::SameLine(filterSearchLabelOffset);
        ImGui::Text("%s", placeholderText);
    }
    UIWidgets::PopStyleCombobox();

    // SameLine() uses absolute positioning from line start
    ImGui::SameLine(availableWidth - buttonWidth - filterButtonOffset);
    if (UIWidgets::Button(buttonLabel, { .padding = ImVec2(ImGui::GetStyle().FramePadding.x, filterButtonPaddingY),
                                         .color = menuThemeColor })) {
        if (filter.IsActive()) {
            actionFlagWhenFiltered = true;
        } else {
            actionFlagWhenAll = true;
        }
    }
}

static void DrawCheckFilterTab() {
    if (CVarGetInteger(Rando::StaticData::Options[RO_LOGIC].cvar, RO_LOGIC_GLITCHLESS) >= RO_LOGIC_VANILLA) {
        ImGui::TextColored(UIWidgets::ColorValues.at(UIWidgets::Colors::Red),
                           "This setting is not compatible with Vanilla Logic.");
        return;
    }

    auto menuThemeColor = UIWidgets::Colors(CVarGetInteger("gSettings.Menu.Theme", LightBlue));
    bool excludeAllChecks = false;
    bool excludeFiltered = false;
    bool removeFiltered = false;
    bool removeAllChecks = false;
    if (!isExcludedInitialized) {
        LoadExcludedChecks();
        isExcludedInitialized = true;
    }

    f32 columnWidth = ImGui::GetContentRegionAvail().x / 2 - (ImGui::GetStyle().ItemSpacing.x * 2);
    ImGui::BeginChild("randoIncludedChecks", ImVec2(columnWidth, ImGui::GetContentRegionAvail().y));
    ImGui::SeparatorText("Normal Checks");

    static ImGuiTextFilter includedFilter;
    const char* leftButtonLabel = includedFilter.IsActive() ? "Junk Filtered" : "Junk All";
    f32 availableWidth = ImGui::GetContentRegionAvail().x;
    DrawFilterWithButton(includedFilter, "Normal Search", leftButtonLabel, availableWidth, menuThemeColor,
                         excludeFiltered, excludeAllChecks, 30.0f, 24.0f, 6.0f, 18.0f);

    if (ImGui::BeginTable("Normal Checks", 1)) {
        ImGui::TableNextColumn();

        for (auto& includedChecks : Rando::StaticData::Checks) {
            if (includedChecks.first == RC_UNKNOWN) {
                continue;
            }

            if (!includedFilter.PassFilter(Rando::StaticData::CheckNames[includedChecks.first].c_str())) {
                continue;
            }

            if (std::binary_search(checkExclusionList.begin(), checkExclusionList.end(), includedChecks.first)) {
                continue;
            }

            ImGui::BeginGroup();
            ImGui::Text("%s", Rando::StaticData::CheckNames[includedChecks.first].c_str());
            ImGui::SameLine();
            ImGui::Dummy(ImVec2(ImGui::GetContentRegionAvail().x, 0));
            ImGui::EndGroup();

            ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0,
                                   ImGui::IsItemHovered() ? IM_COL32(255, 255, 0, 128) : IM_COL32(255, 255, 255, 0));
            if (ImGui::IsItemClicked()) {
                auto it = std::lower_bound(checkExclusionList.begin(), checkExclusionList.end(), includedChecks.first);
                if (it == checkExclusionList.end() || *it != includedChecks.first) {
                    checkExclusionList.insert(it, includedChecks.first);
                    SaveExcludedChecks();
                }
            }
            ImGui::TableNextColumn();
        }
        ImGui::EndTable();
    }
    ImGui::EndChild();
    ImGui::SameLine();
    ImGui::BeginChild("randoExcludedChecks", ImVec2(columnWidth, ImGui::GetContentRegionAvail().y));
    ImGui::SeparatorText("Forced Junk Checks");

    static ImGuiTextFilter excludedFilter;
    const char* rightButtonLabel = excludedFilter.IsActive() ? "Remove Filtered" : "Remove All";
    f32 rightAvailableWidth = ImGui::GetContentRegionAvail().x;
    DrawFilterWithButton(excludedFilter, "Junk Search", rightButtonLabel, rightAvailableWidth, menuThemeColor,
                         removeFiltered, removeAllChecks, 30.0f, 24.0f, 6.0f, 18.0f);

    if (ImGui::BeginTable("Forced Junk Checks", 1)) {
        ImGui::TableNextColumn();
        for (auto it = checkExclusionList.begin(); it != checkExclusionList.end();) {
            if (!excludedFilter.PassFilter(Rando::StaticData::CheckNames[*it].c_str())) {
                ++it;
                continue;
            }

            ImGui::Text("%s", Rando::StaticData::CheckNames[*it].c_str());

            ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0,
                                   ImGui::IsItemHovered() ? IM_COL32(255, 255, 0, 128) : IM_COL32(255, 255, 255, 0));
            if (ImGui::IsItemClicked()) {
                it = checkExclusionList.erase(it);
                SaveExcludedChecks();
            } else {
                ++it;
            }
            ImGui::TableNextColumn();
        }
        ImGui::EndTable();
    }
    ImGui::EndChild();

    // Junk all checks
    if (excludeAllChecks) {
        checkExclusionList.clear();
        checkExclusionList.reserve(Rando::StaticData::Checks.size());
        for (auto& includedChecks : Rando::StaticData::Checks) {
            if (includedChecks.first != RC_UNKNOWN) {
                checkExclusionList.push_back(includedChecks.first);
            }
        }
        SortExcludedChecks();
        SaveExcludedChecks();
        includedFilter.Clear();
        excludeAllChecks = false;
    }

    // Junk filtered checks
    if (excludeFiltered) {
        for (auto& includedChecks : Rando::StaticData::Checks) {
            if (includedChecks.first == RC_UNKNOWN) {
                continue;
            }

            if (!includedFilter.PassFilter(Rando::StaticData::CheckNames[includedChecks.first].c_str())) {
                continue;
            }

            auto it = std::lower_bound(checkExclusionList.begin(), checkExclusionList.end(), includedChecks.first);
            if (it == checkExclusionList.end() || *it != includedChecks.first) {
                checkExclusionList.insert(it, includedChecks.first);
            }
        }
        SaveExcludedChecks();
        includedFilter.Clear();
        excludeFiltered = false;
    }

    // Remove filtered checks: erase_if removes items where PassFilter returns true
    if (removeFiltered) {
        std::erase_if(checkExclusionList, [&](const RandoCheckId& checkId) {
            return excludedFilter.PassFilter(Rando::StaticData::CheckNames[checkId].c_str());
        });
        SaveExcludedChecks();
        excludedFilter.Clear();
        removeFiltered = false;
    }

    // Remove all checks
    if (removeAllChecks) {
        checkExclusionList.clear();
        SaveExcludedChecks();
        excludedFilter.Clear();
        removeAllChecks = false;
    }
}

static void DrawHintsTab() {
    f32 columnWidth = ImGui::GetContentRegionAvail().x / 3 - (ImGui::GetStyle().ItemSpacing.x * 2);
    f32 halfHeight = ImGui::GetContentRegionAvail().y / 2 - (ImGui::GetStyle().ItemSpacing.y * 2);
    ImGui::BeginChild("randoHintsColumn1", ImVec2(columnWidth, 0));
    CVarCheckbox(
        "Spider House", Rando::StaticData::Options[RO_HINTS_SPIDER_HOUSES].cvar,
        CheckboxOptions(
            { { .tooltip =
                    "Swamp Spider House: Hinted at his normal location within the Swamp Spider House\n\nOcean Spider "
                    "House: Hinted in South Clock Town day 1, by the main standing on the scaffolding." } }));
    CVarCheckbox(
        "Gossip Stone Static Hint", Rando::StaticData::Options[RO_HINTS_GOSSIP_STONES].cvar,
        CheckboxOptions(
            { { .tooltip = "Each gossip stone will give a static hint about the contents of a random location." } }));
    CVarCheckbox(
        "Gossip Stone Purchaseable", Rando::StaticData::Options[RO_HINTS_PURCHASEABLE].cvar,
        CheckboxOptions({ { .tooltip = "Gossip stones will offer a hint for a scaling rupee cost. This cost ranges "
                                       "from 10-250 rupees depending on how many checks are remaining in your seed. "
                                       "The hint will guaranteed be a check you have not obtained yet." } }));
    CVarCheckbox(
        "Boss Remains", Rando::StaticData::Options[RO_HINTS_BOSS_REMAINS].cvar,
        CheckboxOptions(
            { { .tooltip =
                    "Lists the location of the Boss remains on the guard recruitment posters around Clock Town" } }));
    CVarCheckbox("Oath to Order", Rando::StaticData::Options[RO_HINTS_OATH_TO_ORDER].cvar,
                 CheckboxOptions({ { .tooltip = "Once you have the Moon Access Requirements, talking to Skull Kid on "
                                                "the Clock Tower Rooftop will hint the location of Oath to Order" } }));
    CVarCheckbox(
        "General Actor Hints", "gPlaceholderBool",
        CheckboxOptions({ { .disabled = true,
                            .disabledTooltip = "Soon you will be able to disable these. Currently hinted:\n- Bomb Shop "
                                               "4th Item\n- Lottery\n- Great Fairy Fountains\n- Mountain Smithy" } })
            .DefaultValue(true));
    CVarCheckbox("Saria's Song", "gPlaceholderBool",
                 CheckboxOptions({ { .disabled = true, .disabledTooltip = "Coming Soon" } }));
    CVarCheckbox("Song of Soaring", "gPlaceholderBool",
                 CheckboxOptions({ { .disabled = true, .disabledTooltip = "Coming Soon" } }));
    CVarCheckbox(
        "Hookshot Location", Rando::StaticData::Options[RO_HINTS_HOOKSHOT].cvar,
        CheckboxOptions(
            { { .tooltip =
                    "The Zora in Great Bay Coast, near Pirates Fortress, will hint the location of the Hookshot." } }));
    ImGui::EndChild();
}

void Rando::RegisterMenu() {
    mBenMenu->AddMenuEntry("Rando", "gSettings.Menu.RandoSidebarSection");
    mBenMenu->AddSidebarEntry("Rando", "General", 1);
    WidgetPath path = { "Rando", "General", SECTION_COLUMN_1 };
    mBenMenu->AddWidget(path, "General", WIDGET_CUSTOM).CustomFunction([](WidgetInfo& info) { DrawGeneralTab(); });
    mBenMenu->AddSidebarEntry("Rando", "Logic/Conditions", 1);
    path.sidebarName = "Logic/Conditions";
    mBenMenu->AddWidget(path, "Logic/Conditions", WIDGET_CUSTOM).CustomFunction([](WidgetInfo& info) {
        DrawLogicConditionsTab();
    });
    mBenMenu->AddSidebarEntry("Rando", "Shuffle Options", 1);
    path.sidebarName = "Shuffle Options";
    mBenMenu->AddWidget(path, "Shuffle Options", WIDGET_CUSTOM).CustomFunction([](WidgetInfo& info) {
        DrawShufflesTab();
    });
    mBenMenu->AddSidebarEntry("Rando", "Check Filter", 1);
    path.sidebarName = "Check Filter";
    mBenMenu->AddWidget(path, "Check Filter", WIDGET_CUSTOM).CustomFunction([](WidgetInfo& info) {
        DrawCheckFilterTab();
    });
    mBenMenu->AddSidebarEntry("Rando", "Items", 1);
    path.sidebarName = "Items";
    mBenMenu->AddWidget(path, "Items", WIDGET_CUSTOM).CustomFunction([](WidgetInfo& info) { DrawItemsTab(); });
    mBenMenu->AddSidebarEntry("Rando", "Starting Items", 1);
    path.sidebarName = "Starting Items";
    mBenMenu->AddWidget(path, "Starting Items", WIDGET_CUSTOM).CustomFunction([](WidgetInfo& info) {
        DrawStartingItemsTab();
    });
    mBenMenu->AddSidebarEntry("Rando", "Hints", 1);
    path.sidebarName = "Hints";
    mBenMenu->AddWidget(path, "Hints", WIDGET_CUSTOM).CustomFunction([](WidgetInfo& info) { DrawHintsTab(); });
}

static RegisterMenuInitFunc initFunc(Rando::RegisterMenu);
