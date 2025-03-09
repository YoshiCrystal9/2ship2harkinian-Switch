#include "ItemTrackerSettings.h"
#include "ItemTracker.h"
#include "../../BenGui/UIWidgets.hpp"

namespace BenGui {
extern std::shared_ptr<ItemTrackerWindow> mItemTrackerWindow;
}

using namespace BenGui;

void ItemTrackerSettingsWindow::UpdateElement() {
}

void ItemTrackerSettingsWindow::InitElement() {
}

static const char* windowTypes[2] = { "Floating", "Window" };
static const char* displayTypes[3] = { "Hidden", "Main Window", "Separate" };
static const char* displayModes[2] = { "Always", "Combo Button Hold" };

void ItemTrackerSettingsWindow::DrawElement() {
    ImGui::SetNextWindowSize(ImVec2(733, 472), ImGuiCond_FirstUseEver);

    if (!ImGui::BeginChild("Item Tracker Settings")) {
        ImGui::EndChild();
        return;
    }

    if (CVarGetInteger("gWindows.ItemTracker", 0)) {
        UIWidgets::WindowButton("Hide Item Tracker", "gWindows.ItemTracker", mItemTrackerWindow,
                                { .size = UIWidgets::Sizes::Inline });
    } else {
        UIWidgets::WindowButton("Show Item Tracker", "gWindows.ItemTracker", mItemTrackerWindow,
                                { .size = UIWidgets::Sizes::Inline });
    }

    ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, { 8.0f, 8.0f });
    ImGui::BeginTable("itemTrackerSettingsTable", 2, ImGuiTableFlags_BordersH | ImGuiTableFlags_BordersV);
    ImGui::TableSetupColumn("General settings", ImGuiTableColumnFlags_WidthStretch, 200.0f);
    ImGui::TableSetupColumn("Section settings", ImGuiTableColumnFlags_WidthStretch, 200.0f);
    ImGui::TableHeadersRow();
    ImGui::TableNextRow();
    ImGui::TableNextColumn();
    ImGui::Text("BG Color");
    ImGui::SameLine();

    ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);

    ImGui::ColorEdit4("BG Color", (float*)mItemTrackerWindow->GetBgColorPtr(),
                      ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoLabel);

    UIWidgets::Combobox("Window Type", mItemTrackerWindow->GetWindowTypePtr(), windowTypes);
    UIWidgets::Checkbox("Enable Dragging", mItemTrackerWindow->GetIsDraggablePtr());
    UIWidgets::Checkbox("Only enable while paused", mItemTrackerWindow->GetOnlyShowPausedPtr());

    UIWidgets::SliderFloat("Icon size : %.0fpx", mItemTrackerWindow->GetIconSizePtr(),
                           { .format = "%.0f", .step = 1.0f, .min = 0.0f, .max = 128.0f });
    UIWidgets::SliderFloat("Icon margins : %.0fpx", mItemTrackerWindow->GetIconSpacingPtr(),
                           { .format = "%.0f", .step = 1.0f, .min = -5.0f, .max = 50.0f });
    UIWidgets::SliderFloat("Text size : %.0fpx", mItemTrackerWindow->GetTextSizePtr(),
                           { .format = "%.0f", .step = 1.0f, .min = 1.0f, .max = 30.0f });
    UIWidgets::SliderFloat("Text Offset : %0.fpx", mItemTrackerWindow->GetTextOffsetPtr(),
                           { .format = "%.0f", .step = 1.0f, .min = 0.0f, .max = 40.0f });

    ImGui::TableNextColumn();

    UIWidgets::Combobox("Inventory", mItemTrackerWindow->GetDrawModePtr(SECTION_INVENTORY), displayTypes);
    UIWidgets::Combobox("Masks", mItemTrackerWindow->GetDrawModePtr(SECTION_MASKS), displayTypes);
    UIWidgets::Combobox("Equipment", mItemTrackerWindow->GetDrawModePtr(SECTION_EQUIPMENT), displayTypes);
    UIWidgets::Combobox("Miscellaneous", mItemTrackerWindow->GetDrawModePtr(SECTION_MISC), displayTypes);
    UIWidgets::Combobox("Songs", mItemTrackerWindow->GetDrawModePtr(SECTION_SONGS), displayTypes);
    UIWidgets::Combobox("Stray Fairies", mItemTrackerWindow->GetDrawModePtr(SECTION_STRAY_FAIRIES), displayTypes);
    UIWidgets::Combobox("Gold Skulltulas", mItemTrackerWindow->GetDrawModePtr(SECTION_GOLD_SKULLTULAS), displayTypes);
    UIWidgets::Combobox("Dungeon Items", mItemTrackerWindow->GetDrawModePtr(SECTION_DUNGEON), displayTypes);

    UIWidgets::Checkbox(
        "Include Maps and Compasses", mItemTrackerWindow->GetIncludeMapsAndCompassesPtr(),
        UIWidgets::CheckboxOptions().Tooltip("Includes Maps and Compasses with the Dungeon Items section"));

    UIWidgets::Checkbox("Draw Current Ammo",
                        mItemTrackerWindow->GetCapacityModePtr(ItemTrackerCapacityMode::DrawCurrent));

    UIWidgets::Checkbox("Draw Current Capacity",
                        mItemTrackerWindow->GetCapacityModePtr(ItemTrackerCapacityMode::DrawCurCapacity));

    UIWidgets::Checkbox("Draw Max Capacity",
                        mItemTrackerWindow->GetCapacityModePtr(ItemTrackerCapacityMode::DrawMaxCapacity));

    ImGui::PopStyleVar(1);
    ImGui::EndTable();

    ImGui::EndChild();
}
