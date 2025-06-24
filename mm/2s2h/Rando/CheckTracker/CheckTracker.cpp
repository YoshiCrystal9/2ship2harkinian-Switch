
#include "CheckTracker.h"
#include "2s2h/Rando/Logic/Logic.h"
#include "2s2h/ShipUtils.h"
#include "2s2h/BenGui/UIWidgets.hpp"
#include "2s2h/Rando/StaticData/StaticData.h"
#include <cstring>

// Image Icons
#include "assets/2s2h_assets.h"
#include "interface/parameter_static/parameter_static.h"
#include "assets/archives/icon_item_static/icon_item_static_yar.h"
#include "assets/interface/icon_item_dungeon_static/icon_item_dungeon_static.h"
#include "assets/archives/icon_item_24_static/icon_item_24_static_yar.h"
#include "assets/archives/schedule_dma_static/schedule_dma_static_yar.h"
#include "assets/interface/icon_item_field_static/icon_item_field_static.h"
#include "assets/objects/gameplay_keep/gameplay_keep.h"

extern "C" {
s16 Play_GetOriginalSceneId(s16 sceneId);
}

#define DEFINE_SCENE(_name, enumValue, _textId, _drawConfig, _restrictionFlags, _persistentCycleFlags, \
                     _entranceSceneId, betterMapSelectIndex, _humanName)                               \
    { enumValue, betterMapSelectIndex },
#define DEFINE_SCENE_UNSET(_enumValue)

static std::unordered_map<s32, s32> betterSceneIndex = {
#include "tables/scene_table.h"
};

#undef DEFINE_SCENE
#undef DEFINE_SCENE_UNSET

#define CVAR_NAME_SHOW_CHECK_TRACKER "gWindows.CheckTracker"
#define CVAR_NAME_SHOW_LOGIC "gRando.CheckTracker.OnlyShowChecksInLogic"
#define CVAR_NAME_HIDE_COLLECTED "gRando.CheckTracker.HideCollectedChecks"
#define CVAR_NAME_HIDE_SKIPPED "gRando.CheckTracker.HideSkippedChecks"
#define CVAR_NAME_SCROLL_TO_SCENE "gRando.CheckTracker.ScrollToCurrentScene"
#define CVAR_NAME_TRACKER_OPACITY "gRando.CheckTracker.Opacity"
#define CVAR_NAME_TRACKER_SCALE "gRando.CheckTracker.Scale"
#define CVAR_NAME_SHOW_CURRENT_SCENE "gRando.CheckTracker.ShowCurrentScene"
#define CVAR_SHOW_CHECK_TRACKER CVarGetInteger(CVAR_NAME_SHOW_CHECK_TRACKER, 0)
#define CVAR_SHOW_LOGIC CVarGetInteger(CVAR_NAME_SHOW_LOGIC, 0)
#define CVAR_HIDE_COLLECTED CVarGetInteger(CVAR_NAME_HIDE_COLLECTED, 0)
#define CVAR_HIDE_SKIPPED CVarGetInteger(CVAR_NAME_HIDE_SKIPPED, 0)
#define CVAR_SCROLL_TO_SCENE CVarGetInteger(CVAR_NAME_SCROLL_TO_SCENE, 0)
#define CVAR_TRACKER_OPACITY CVarGetFloat(CVAR_NAME_TRACKER_OPACITY, 0.5f)
#define CVAR_TRACKER_SCALE CVarGetFloat(CVAR_NAME_TRACKER_SCALE, 1.0f)
#define CVAR_SHOW_CURRENT_SCENE CVarGetInteger(CVAR_NAME_SHOW_CURRENT_SCENE, 0)

static bool sExpandedHeadersToggle = true;
static bool sExpandedHeadersState = true;
static s32 sScrollToTargetScene = -1;
static s32 sScrollToTargetEntrance = -1;
static ImGuiTextFilter sCheckTrackerFilter;
float trackerScale = 1.0f;
float searchBoxPadding = 15.0f;
ImVec4 trackerBG = ImVec4{ 0, 0, 0, 0.5f };

std::map<SceneId, std::vector<RandoCheckId>> sceneChecks;
std::vector<SceneId> sortedSceneIds;
std::unordered_map<RandoCheckId, std::string> readableCheckNames;
std::unordered_map<RandoCheckId, std::string> accessLogicFuncs;

std::vector<const char*> checkTypeIconList = {
    /*RCTYPE_UNKNOWN*/ gItemIconBombersNotebookTex,
    /*RCTYPE_BARREL*/ gBarrelTrackerIcon,
    /*RCTYPE_CHEST*/ gChestTrackerIcon,
    /*RCTYPE_COW*/ gItemIconRomaniMaskTex,
    /*RCTYPE_CRATE*/ gCrateTrackerIcon,
    /*RCTYPE_FREESTANDING*/ gRupeeCounterIconTex,
    /*RCTYPE_GRASS*/ gameplay_keep_Tex_053140,
    /*RCTYPE_HEART*/ gQuestIconPieceOfHeartTex,
    /*RCTYPE_MINIGAME*/ gArcheryScoreIconTex,
    /*RCTYPE_NPC*/ gItemIconBombersNotebookTex,
    /*RCTYPE_OWL*/ gWorldMapOwlFaceTex,
    /*RCTYPE_POT*/ gPotTrackerIcon,
    /*RCTYPE_REMAINS*/ gItemIconBombersNotebookTex,
    /*RCTYPE_SHOP*/ gItemIconAdultsWalletTex,
    /*RCTYPE_SKULL_TOKEN*/ gQuestIconGoldSkulltulaTex,
    /*RCTYPE_SNOWBALL*/ gMagicArrowEquipEffectTex,
    /*RCTYPE_SONG*/ gItemIconSongNoteTex,
    /*RCTYPE_STRAY_FAIRY*/ gStrayFairyGreatBayIconTex,
    /*RCTYPE_TINGLE_SHOP*/ gItemIconAdultsWalletTex,
};

std::string totalChecksFound() {
    std::string totalChecks;
    uint32_t collected = 0;
    uint32_t totalShuffled = 0;
    for (auto& [_, randoStaticCheck] : Rando::StaticData::Checks) {
        if (RANDO_SAVE_CHECKS[randoStaticCheck.randoCheckId].obtained ||
            RANDO_SAVE_CHECKS[randoStaticCheck.randoCheckId].skipped) {
            collected++;
        }
        if (RANDO_SAVE_CHECKS[randoStaticCheck.randoCheckId].shuffled) {
            totalShuffled++;
        }
    }
    totalChecks = std::to_string(collected);
    totalChecks += "/";
    totalChecks += std::to_string(totalShuffled);
    return totalChecks;
}

void DrawCheckTypeIcon(RandoCheckId randoCheckId) {
    RandoCheckType checkType = Rando::StaticData::Checks[randoCheckId].randoCheckType;
    ImGui::Image(Ship::Context::GetInstance()->GetWindow()->GetGui()->GetTextureByName(checkTypeIconList[checkType]),
                 checkType == RCTYPE_SONG  ? ImVec2(12.0f * trackerScale, 18.0f * trackerScale)
                 : checkType == RCTYPE_OWL ? ImVec2(18.0f * trackerScale, 9.0f * trackerScale)
                                           : ImVec2(18.0f * trackerScale, 18.0f * trackerScale),
                 ImVec2(0, 0), ImVec2(1, 1),
                 checkType == RCTYPE_FREESTANDING ? ImVec4(0.78f, 1, 0.39f, 1) : ImVec4(1, 1, 1, 1));
}

void initializeSceneChecks() {
    sceneChecks.clear();
    for (auto& [_, randoStaticCheck] : Rando::StaticData::Checks) {
        RandoSaveCheck& randoSaveCheck = RANDO_SAVE_CHECKS[randoStaticCheck.randoCheckId];
        if (!randoSaveCheck.shuffled) {
            continue;
        }

        SceneId sceneId = (SceneId)Play_GetOriginalSceneId(randoStaticCheck.sceneId);
        sceneChecks[sceneId].push_back(randoStaticCheck.randoCheckId);
    }

    sortedSceneIds.clear();
    for (auto& [sceneId, _] : sceneChecks) {
        sortedSceneIds.push_back(sceneId);
    }
    std::sort(sortedSceneIds.begin(), sortedSceneIds.end(),
              [](SceneId a, SceneId b) { return betterSceneIndex[a] < betterSceneIndex[b]; });
}

bool checkTrackerShouldShowRow(bool obtained, bool skipped) {
    bool showCheck = true;
    if ((CVAR_HIDE_COLLECTED && obtained) || (CVAR_HIDE_SKIPPED && skipped)) {
        showCheck = false;
    }
    return showCheck;
}

void CheckTrackerDrawLogicalList() {
    std::set<RandoRegionId> reachableRegions = {};
    // Get connected entrances from starting & warp points
    Rando::Logic::FindReachableRegions(RR_MAX, reachableRegions);
    // Get connected regions from current entrance (TODO: Make this optional)
    Rando::Logic::FindReachableRegions(Rando::Logic::GetRegionIdFromEntrance(gSaveContext.save.entrance),
                                       reachableRegions);

    std::vector<RandoRegionId> sortedRegionIds;
    for (auto& regionId : reachableRegions) {
        sortedRegionIds.push_back(regionId);
    }
    std::sort(sortedRegionIds.begin(), sortedRegionIds.end(), [](RandoRegionId a, RandoRegionId b) {
        return betterSceneIndex[Rando::Logic::Regions[a].sceneId] < betterSceneIndex[Rando::Logic::Regions[b].sceneId];
    });

    for (RandoRegionId regionId : sortedRegionIds) {
        if (CVAR_SCROLL_TO_SCENE && sScrollToTargetEntrance != -1 &&
            Rando::Logic::GetRegionIdFromEntrance(sScrollToTargetEntrance) == regionId) {
            ImGui::SetScrollHereY(0.0f);
            sScrollToTargetScene = -1;
            sScrollToTargetEntrance = -1;
        }
        auto& randoRegion = Rando::Logic::Regions[regionId];
        std::vector<std::pair<RandoCheckId, std::string>> availableChecks;
        std::vector<std::pair<std::string, std::string>> availableEvents;
        uint32_t obtainedCheckSum = 0;

        for (auto& [randoCheckId, accessLogicFunc] : randoRegion.checks) {
            auto& randoStaticCheck = Rando::StaticData::Checks[randoCheckId];
            auto& randoSaveCheck = RANDO_SAVE_CHECKS[randoCheckId];
            if (randoSaveCheck.shuffled && accessLogicFunc.first()) {
                if (randoSaveCheck.obtained) {
                    obtainedCheckSum++;
                    if (CVAR_HIDE_COLLECTED) {
                        continue;
                    }
                }

                if (randoSaveCheck.skipped && CVAR_HIDE_SKIPPED) {
                    continue;
                }

                if (!sCheckTrackerFilter.PassFilter(readableCheckNames[randoCheckId].c_str())) {
                    continue;
                }

                availableChecks.push_back({ randoCheckId, accessLogicFunc.second });
            }
        }

        for (auto& event : randoRegion.events) {
            if (!RANDO_EVENTS[event.first] && event.second()) {
                RANDO_EVENTS[event.first]++;
            }
        }

        if (availableChecks.size() > 0 || availableEvents.size() > 0) {
            std::string regionName = Ship_GetSceneName(randoRegion.sceneId);
            if (randoRegion.name != "") {
                regionName += " - ";
                regionName += randoRegion.name;
            }

            regionName += " (" + std::to_string(obtainedCheckSum) + "/" + std::to_string(availableChecks.size()) + ")";

            ImGui::PushID(regionId);
            ImGui::Separator();
            ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0, 0, 0, 0));
            if (sExpandedHeadersState != sExpandedHeadersToggle) {
                ImGui::SetNextItemOpen(sExpandedHeadersToggle);
            }
            if (ImGui::CollapsingHeader(regionName.c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {
                ImGui::Indent(20.0f);
                if (ImGui::BeginTable("Check Tracker", 2)) {
                    ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed, 16.0f);
                    ImGui::TableSetupColumn("Check");
                    ImGui::TableNextColumn();
                    for (auto& [name, accessLogicString] : availableEvents) {
                        ImGui::TableNextColumn();
                        ImGui::PushStyleColor(ImGuiCol_Text, UIWidgets::ColorValues.at(UIWidgets::Colors::White));
                        ImGui::Text("%s (Event)", name.c_str());
                        if (accessLogicString != "") {
                            UIWidgets::Tooltip(accessLogicString.c_str());
                        }
                        ImGui::PopStyleColor();
                        ImGui::TableNextColumn();
                    }
                    for (auto& [checkId, accessLogicString] : availableChecks) {
                        auto& randoStaticCheck = Rando::StaticData::Checks[checkId];
                        auto& randoSaveCheck = RANDO_SAVE_CHECKS[checkId];
                        ImGui::PushStyleColor(ImGuiCol_Text, randoSaveCheck.obtained
                                                                 ? UIWidgets::ColorValues.at(UIWidgets::Colors::Green)
                                                             : randoSaveCheck.skipped
                                                                 ? UIWidgets::ColorValues.at(UIWidgets::Colors::Indigo)
                                                                 : UIWidgets::ColorValues.at(UIWidgets::Colors::White));
                        if (checkTrackerShouldShowRow(randoSaveCheck.obtained, randoSaveCheck.skipped)) {
                            ImGui::BeginGroup();
                            float cursorPosY = ImGui::GetCursorPosY();
                            if (randoStaticCheck.randoCheckType == RCTYPE_OWL) {
                                ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 3.5f);
                            }
                            DrawCheckTypeIcon(checkId);
                            ImGui::TableNextColumn();
                            ImGui::SetCursorPosY(cursorPosY);
                            ImGui::Text("%s", readableCheckNames[checkId].c_str());
                            if (accessLogicString != "") {
                                UIWidgets::Tooltip(accessLogicString.c_str());
                            }
                            if (randoSaveCheck.obtained) {
                                ImGui::SameLine(0, 25.0f);
                                ImGui::Text("(%s)", Rando::StaticData::Items[randoSaveCheck.randoItemId].name);
                            } else if (randoSaveCheck.skipped) {
                                ImGui::SameLine(0, 25.0f);
                                ImGui::Text("(Skipped)");
                            }
                            ImGui::EndGroup();
                            ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0, ImGui::IsItemHovered()
                                                                                  ? IM_COL32(255, 255, 0, 128)
                                                                                  : IM_COL32(255, 255, 255, 0));
                            if (ImGui::IsItemClicked()) {
                                randoSaveCheck.skipped = !randoSaveCheck.skipped;
                            }
                            ImGui::TableNextColumn();
                        }
                        ImGui::PopStyleColor();
                    }
                    ImGui::EndTable();
                }
                ImGui::Unindent(20.0f);
            }
            ImGui::PopStyleColor();
            ImGui::PopID();
        }
    }
}

std::unordered_map<RandoCheckId, bool> checksInLogic;
static u32 lastFrame = 0;

void RefreshChecksInLogic() {
    if (gGameState == NULL || gGameState->frames - lastFrame < 20 || !CVAR_SHOW_LOGIC) {
        return;
    }

    lastFrame = gGameState->frames;
    checksInLogic.clear();

    std::set<RandoRegionId> reachableRegions = {
        RR_MAX,
        Rando::Logic::GetRegionIdFromEntrance(gSaveContext.save.entrance),
    };
    // Get connected entrances from starting & warp points
    Rando::Logic::FindReachableRegions(RR_MAX, reachableRegions);
    // Get connected regions from current entrance (TODO: Make this optional)
    Rando::Logic::FindReachableRegions(Rando::Logic::GetRegionIdFromEntrance(gSaveContext.save.entrance),
                                       reachableRegions);
    for (RandoRegionId regionId : reachableRegions) {
        auto& randoRegion = Rando::Logic::Regions[regionId];
        std::vector<std::pair<RandoCheckId, std::string>> availableChecks;

        for (auto& [randoCheckId, accessLogicFunc] : randoRegion.checks) {
            auto& randoStaticCheck = Rando::StaticData::Checks[randoCheckId];
            auto& randoSaveCheck = RANDO_SAVE_CHECKS[randoCheckId];
            if (randoSaveCheck.shuffled && !randoSaveCheck.obtained && accessLogicFunc.first()) {
                checksInLogic.insert({ randoCheckId, true });
            }
        }

        for (auto& event : randoRegion.events) {
            if (!RANDO_EVENTS[event.first] && event.second()) {
                RANDO_EVENTS[event.first]++;
            }
        }
    }
}

void CheckTrackerDrawNonLogicalList() {
    RefreshChecksInLogic();

    for (auto& sceneId : sortedSceneIds) {
        if (sceneId == SCENE_MAX) {
            continue;
        }

        auto& unfilteredChecks = sceneChecks[sceneId];
        std::vector<RandoCheckId> checks;
        uint32_t obtainedCheckSum = 0;

        for (auto& checkId : unfilteredChecks) {
            if (RANDO_SAVE_CHECKS[checkId].obtained) {
                obtainedCheckSum++;
                if (CVAR_HIDE_COLLECTED) {
                    continue;
                }
            }

            if (RANDO_SAVE_CHECKS[checkId].skipped) {
                obtainedCheckSum++;
                if (CVAR_HIDE_SKIPPED) {
                    continue;
                }
            }

            if (!sCheckTrackerFilter.PassFilter(readableCheckNames[checkId].c_str())) {
                continue;
            }

            if (CVAR_SHOW_CURRENT_SCENE && Play_GetOriginalSceneId(Rando::StaticData::Checks[checkId].sceneId) !=
                                               Play_GetOriginalSceneId(gPlayState->sceneId)) {
                continue;
            }

            checks.push_back(checkId);
        }

        if (checks.size() == 0) {
            continue;
        }

        if (!CVAR_SHOW_CURRENT_SCENE && CVAR_SCROLL_TO_SCENE && sScrollToTargetScene != -1 &&
            sScrollToTargetScene == sceneId) {
            ImGui::SetScrollHereY(0.0f);
            sScrollToTargetScene = -1;
            sScrollToTargetEntrance = -1;
        }

        ImGui::PushID(sceneId);
        ImGui::Separator();
        ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0, 0, 0, 0));
        std::string headerText = Ship_GetSceneName(sceneId);
        headerText += " (" + std::to_string(obtainedCheckSum) + "/" + std::to_string(unfilteredChecks.size()) + ")";

        ImGui::PushStyleColor(ImGuiCol_Text, obtainedCheckSum == unfilteredChecks.size()
                                                 ? UIWidgets::ColorValues.at(UIWidgets::Colors::Green)
                                                 : UIWidgets::ColorValues.at(UIWidgets::Colors::White));

        if (sExpandedHeadersState != sExpandedHeadersToggle) {
            ImGui::SetNextItemOpen(sExpandedHeadersToggle);
        }
        if (ImGui::CollapsingHeader(headerText.c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::Indent(20.0f);
            if (ImGui::BeginTable("Check Tracker", 2)) {
                ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed, (16.0f * trackerScale));
                ImGui::TableSetupColumn("Check");
                ImGui::TableNextColumn();
                for (auto& randoCheckId : checks) {
                    Rando::StaticData::RandoStaticCheck& randoStaticCheck = Rando::StaticData::Checks[randoCheckId];
                    RandoSaveCheck& randoSaveCheck = RANDO_SAVE_CHECKS[randoCheckId];
                    ImVec4 textColor = UIWidgets::ColorValues.at(UIWidgets::Colors::White);
                    if (randoSaveCheck.obtained) {
                        textColor = UIWidgets::ColorValues.at(UIWidgets::Colors::Green);
                    } else if (randoSaveCheck.skipped) {
                        textColor = UIWidgets::ColorValues.at(UIWidgets::Colors::Indigo);
                    } else if (CVAR_SHOW_LOGIC && !checksInLogic.contains(randoCheckId)) {
                        textColor = UIWidgets::ColorValues.at(UIWidgets::Colors::Gray);
                    }

                    if (checkTrackerShouldShowRow(randoSaveCheck.obtained, randoSaveCheck.skipped)) {
                        ImGui::PushStyleColor(ImGuiCol_Text, textColor);
                        ImGui::BeginGroup();
                        float cursorPosY = ImGui::GetCursorPosY();
                        if (Rando::StaticData::Checks[randoCheckId].randoCheckType == RCTYPE_OWL) {
                            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 3.5f);
                        }
                        DrawCheckTypeIcon(randoCheckId);
                        ImGui::TableNextColumn();

                        ImGui::SetCursorPosY(cursorPosY);
                        ImGui::Text("%s", readableCheckNames[randoCheckId].c_str());
                        if (randoSaveCheck.obtained) {
                            ImGui::SameLine(0, 25.0f);
                            ImGui::Text("(%s)", Rando::StaticData::Items[randoSaveCheck.randoItemId].name);
                        } else if (randoSaveCheck.skipped) {
                            ImGui::SameLine(0, 25.0f);
                            ImGui::Text("(Skipped)");
                        }
                        ImGui::SameLine();
                        ImGui::Dummy(ImVec2(ImGui::GetContentRegionAvail().x, 0));
                        ImGui::EndGroup();
                        ImGui::PopStyleColor();
                        std::string accessLogicString = accessLogicFuncs.find(randoCheckId) != accessLogicFuncs.end()
                                                            ? accessLogicFuncs[randoCheckId]
                                                            : "";
                        if (accessLogicString != "") {
                            UIWidgets::Tooltip(accessLogicString.c_str());
                        }
                        ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0, ImGui::IsItemHovered()
                                                                              ? IM_COL32(255, 255, 0, 128)
                                                                              : IM_COL32(255, 255, 255, 0));
                        if (ImGui::IsItemClicked()) {
                            randoSaveCheck.skipped = !randoSaveCheck.skipped;
                        }
                        ImGui::TableNextColumn();
                    }
                }
                ImGui::EndTable();
            }
            ImGui::Unindent(20.0f);
        }
        ImGui::PopStyleColor(2);
        ImGui::PopID();
    }
}

namespace Rando {

namespace CheckTracker {

void CheckTrackerWindow::Draw() {
    if (!CVAR_SHOW_CHECK_TRACKER) {
        return;
    }

    ImGui::PushStyleColor(ImGuiCol_TitleBgActive, trackerBG);
    ImGui::PushStyleColor(ImGuiCol_TitleBg, trackerBG);
    ImGui::PushStyleColor(ImGuiCol_WindowBg, trackerBG);
    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0, 0, 0, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 4.0f);

    ImGui::SetNextWindowSize(ImVec2(485.0f, 500.0f), ImGuiCond_FirstUseEver);

    ImGui::Begin("Check Tracker", nullptr, ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoFocusOnAppearing);

    trackerBG.w = ImGui::IsWindowDocked() ? 1.0f : CVAR_TRACKER_OPACITY;
    ImGui::SetWindowFontScale(trackerScale);

    if (!gPlayState || !IS_RANDO) {
        ImGui::SetCursorPosX((ImGui::GetWindowWidth() - ImGui::CalcTextSize("No Rando Save Loaded").x) / 2);
        ImGui::SetCursorPosY(ImGui::GetWindowHeight() / 2 - 10.0f);
        ImGui::TextColored(UIWidgets::ColorValues.at(UIWidgets::Colors::Gray), "No Rando Save Loaded");
        ImGui::End();
        ImGui::PopStyleColor(4);
        ImGui::PopStyleVar(1);
        return;
    }

    bool sameLine = !(ImGui::GetContentRegionAvail().x <= 300.0f * trackerScale);

    UIWidgets::PushStyleCombobox();
    sCheckTrackerFilter.Draw("##filter", (ImGui::GetContentRegionAvail().x -
                                          (sameLine ? (ImGui::CalcTextSize("Total: ").x +
                                                       ImGui::CalcTextSize(totalChecksFound().c_str()).x + 15.0f)
                                                    : 0)));
    UIWidgets::PopStyleCombobox();
    if (!sCheckTrackerFilter.IsActive()) {
        ImGui::SameLine(18.0f);
        ImGui::Text("Search");
    }

    if (sameLine) {
        ImGui::SameLine(ImGui::GetContentRegionAvail().x -
                        (ImGui::CalcTextSize("Total: ").x + ImGui::CalcTextSize(totalChecksFound().c_str()).x));
    }
    ImGui::Text("Total: %s", totalChecksFound().c_str());

    ImGui::BeginChild("Checks");
    // if (CVAR_SHOW_LOGIC) {
    //     CheckTrackerDrawLogicalList();
    // } else {
    CheckTrackerDrawNonLogicalList();
    // }
    sExpandedHeadersState = sExpandedHeadersToggle;
    ImGui::EndChild();

    ImGui::End();

    ImGui::PopStyleColor(4);
    ImGui::PopStyleVar(1);
}

void SettingsWindow::DrawElement() {
    ImGui::SeparatorText("Check Tracker Settings");
    if (ImGui::BeginTable("Settings Table", 2)) {
        ImGui::TableNextColumn();
        UIWidgets::CVarCheckbox("Dim Out of Logic Checks", CVAR_NAME_SHOW_LOGIC);
        UIWidgets::CVarCheckbox("Hide Collected Checks", CVAR_NAME_HIDE_COLLECTED);
        UIWidgets::CVarCheckbox("Hide Skipped Checks", CVAR_NAME_HIDE_SKIPPED);
        UIWidgets::CVarCheckbox("Auto Scroll To Current Scene", CVAR_NAME_SCROLL_TO_SCENE);
        UIWidgets::CVarCheckbox("Only Show Current Scene", CVAR_NAME_SHOW_CURRENT_SCENE);

        ImGui::TableNextColumn();

        if (UIWidgets::CVarSliderFloat("Opacity", CVAR_NAME_TRACKER_OPACITY,
                                       {
                                           .format = "%.1f",
                                           .step = 0.10f,
                                           .min = 0.0f,
                                           .max = 1.0f,
                                           .defaultValue = 0.5f,
                                       })) {
            trackerBG.w = CVAR_TRACKER_OPACITY;
        }
        if (UIWidgets::CVarSliderFloat("Scale", CVAR_NAME_TRACKER_SCALE,
                                       {
                                           .format = "%.1f",
                                           .step = 0.10f,
                                           .min = 0.7f,
                                           .max = 2.5f,
                                           .defaultValue = 1.0f,
                                       })) {
            trackerScale = CVAR_TRACKER_SCALE;
        }

        ImGui::EndTable();
    }
    if (UIWidgets::Button("Expand/Collapse All")) {
        sExpandedHeadersToggle = !sExpandedHeadersToggle;
    }
}

void Init() {
    for (auto& [randoCheckId, randoStaticCheck] : Rando::StaticData::Checks) {
        readableCheckNames[randoCheckId] = convertEnumToReadableName(randoStaticCheck.name);
    }
    for (auto& [randoRegionId, randoRegion] : Rando::Logic::Regions) {
        for (auto& [randoCheckId, accessLogicFunc] : randoRegion.checks) {
            accessLogicFuncs[randoCheckId] = accessLogicFunc.second;
        }
    }
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnSceneInit>([](s8 sceneId, s8 spawnNum) {
        if (CVAR_SCROLL_TO_SCENE) {
            sScrollToTargetScene = Play_GetOriginalSceneId(sceneId);
            sScrollToTargetEntrance = gSaveContext.save.entrance;
        }
    });

    trackerBG = { 0, 0, 0, CVAR_TRACKER_OPACITY };
    trackerScale = CVAR_TRACKER_SCALE;
}

void OnFileLoad() {
    if (!IS_RANDO) {
        return;
    }

    initializeSceneChecks();
}

} // namespace CheckTracker

} // namespace Rando
