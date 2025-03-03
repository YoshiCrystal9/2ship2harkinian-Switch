#include "BenGui.hpp"

#include <spdlog/spdlog.h>
#include <imgui.h>
#include <imgui_internal.h>
#include <libultraship/libultraship.h>
#include <Fast3D/gfx_pc.h>
#include "UIWidgets.hpp"
#include "HudEditor.h"
#include "2s2h/Enhancements/Audio/AudioEditor.h"
#include "CosmeticEditor.h"
#include "Notification.h"
#include "2s2h/Rando/CheckTracker/CheckTracker.h"

#ifdef __APPLE__
#include "graphic/Fast3D/gfx_metal.h"
#endif

#ifdef __SWITCH__
#include <port/switch/SwitchImpl.h>
#endif

#include "include/global.h"
#include "include/z64audio.h"

#include "Enhancements/Trackers/ItemTracker.h"
#include "Enhancements/Trackers/ItemTrackerSettings.h"
#include "Enhancements/Trackers/DisplayOverlay.h"
#include "BenMenu.h"

namespace BenGui {
// MARK: - Delegates

std::shared_ptr<BenMenuBar> mBenMenuBar;

std::shared_ptr<Ship::GuiWindow> mConsoleWindow;
std::shared_ptr<Ship::GuiWindow> mStatsWindow;
std::shared_ptr<Ship::GuiWindow> mGfxDebuggerWindow;
std::shared_ptr<Ship::GuiWindow> mInputEditorWindow;

std::shared_ptr<HookDebuggerWindow> mHookDebuggerWindow;
std::shared_ptr<SaveEditorWindow> mSaveEditorWindow;
std::shared_ptr<HudEditorWindow> mHudEditorWindow;
std::shared_ptr<CosmeticEditorWindow> mCosmeticEditorWindow;
std::shared_ptr<ActorViewerWindow> mActorViewerWindow;
std::shared_ptr<CollisionViewerWindow> mCollisionViewerWindow;
std::shared_ptr<EventLogWindow> mEventLogWindow;
std::shared_ptr<AudioEditor> mAudioEditorWindow;
std::shared_ptr<BenMenu> mBenMenu;
std::shared_ptr<Notification::Window> mNotificationWindow;
std::shared_ptr<Rando::CheckTracker::CheckTrackerWindow> mRandoCheckTrackerWindow;
std::shared_ptr<Rando::CheckTracker::SettingsWindow> mRandoCheckTrackerSettingsWindow;
std::shared_ptr<ItemTrackerWindow> mItemTrackerWindow;
std::shared_ptr<ItemTrackerSettingsWindow> mItemTrackerSettingsWindow;
std::shared_ptr<DisplayOverlayWindow> mDisplayOverlayWindow;

void SetupGuiElements() {
    auto gui = Ship::Context::GetInstance()->GetWindow()->GetGui();

    auto& style = ImGui::GetStyle();
    style.FramePadding = ImVec2(4.0f, 6.0f);
    style.ItemSpacing = ImVec2(8.0f, 6.0f);
    style.Colors[ImGuiCol_MenuBarBg] = UIWidgets::ColorValues.at(UIWidgets::Colors::DarkGray);

    mBenMenuBar = std::make_shared<BenMenuBar>(CVAR_MENU_BAR_OPEN, CVarGetInteger(CVAR_MENU_BAR_OPEN, 0));
    gui->SetMenuBar(std::reinterpret_pointer_cast<Ship::GuiMenuBar>(mBenMenuBar));

    if (!gui->GetMenuBar() && !CVarGetInteger("gSettings.DisableMenuShortcutNotify", 0)) {
#if defined(__SWITCH__) || defined(__WIIU__)
        gui->GetGameOverlay()->TextDrawNotification(30.0f, true, "Press - to access enhancements menu");
#else
        gui->GetGameOverlay()->TextDrawNotification(30.0f, true, "Press F1 to access enhancements menu");
#endif
    }

    mBenMenu = std::make_shared<BenMenu>("gWindows.Menu", "Settings Menu");
    gui->SetMenu(mBenMenu);

    mStatsWindow = gui->GetGuiWindow("Stats");
    if (mStatsWindow == nullptr) {
        SPDLOG_ERROR("Could not find stats window");
    }

    mConsoleWindow = gui->GetGuiWindow("Console");
    if (mConsoleWindow == nullptr) {
        SPDLOG_ERROR("Could not find console window");
    }

    mGfxDebuggerWindow = gui->GetGuiWindow("GfxDebuggerWindow");
    if (mGfxDebuggerWindow == nullptr) {
        SPDLOG_ERROR("Could not find input GfxDebuggerWindow");
    }

    mInputEditorWindow = gui->GetGuiWindow("2S2H Input Editor");
    if (mInputEditorWindow == nullptr) {
        SPDLOG_ERROR("Could not find input editor window");
    }

    mHookDebuggerWindow =
        std::make_shared<HookDebuggerWindow>("gWindows.HookDebugger", "Hook Debugger", ImVec2(480, 600));
    gui->AddGuiWindow(mHookDebuggerWindow);

    mSaveEditorWindow = std::make_shared<SaveEditorWindow>("gWindows.SaveEditor", "Save Editor", ImVec2(480, 600));
    gui->AddGuiWindow(mSaveEditorWindow);

    mHudEditorWindow = std::make_shared<HudEditorWindow>("gWindows.HudEditor", "HUD Editor", ImVec2(480, 600));
    gui->AddGuiWindow(mHudEditorWindow);

    mCosmeticEditorWindow =
        std::make_shared<CosmeticEditorWindow>("gWindows.CosmeticEditor", "Cosmetic Editor", ImVec2(480, 600));
    gui->AddGuiWindow(mCosmeticEditorWindow);

    mActorViewerWindow = std::make_shared<ActorViewerWindow>("gWindows.ActorViewer", "Actor Viewer", ImVec2(520, 600));
    gui->AddGuiWindow(mActorViewerWindow);

    mCollisionViewerWindow =
        std::make_shared<CollisionViewerWindow>("gWindows.CollisionViewer", "Collision Viewer", ImVec2(390, 475));
    gui->AddGuiWindow(mCollisionViewerWindow);

    mEventLogWindow = std::make_shared<EventLogWindow>("gWindows.EventLog", "Event Log", ImVec2(520, 600));
    gui->AddGuiWindow(mEventLogWindow);

    mAudioEditorWindow = std::make_shared<AudioEditor>("gWindows.AudioEditor", "Audio Editor", ImVec2(520, 600));
    gui->AddGuiWindow(mAudioEditorWindow);

    mItemTrackerWindow = std::make_shared<ItemTrackerWindow>("gWindows.ItemTracker", "Item Tracker");
    gui->AddGuiWindow(mItemTrackerWindow);

    mItemTrackerSettingsWindow = std::make_shared<ItemTrackerSettingsWindow>("gWindows.ItemTrackerSettings",
                                                                             "Item Tracker Settings", ImVec2(800, 400));
    gui->AddGuiWindow(mItemTrackerSettingsWindow);

    mDisplayOverlayWindow = std::make_shared<DisplayOverlayWindow>("gWindows.DisplayOverlay", "Display Overlay");
    gui->AddGuiWindow(mDisplayOverlayWindow);

    mNotificationWindow = std::make_shared<Notification::Window>("gWindows.Notifications", "Notifications Window");
    gui->AddGuiWindow(mNotificationWindow);
    mNotificationWindow->Show();

    mRandoCheckTrackerWindow = std::make_shared<Rando::CheckTracker::CheckTrackerWindow>(
        "gWindows.CheckTracker", "Check Tracker", ImVec2(375, 460));
    gui->AddGuiWindow(mRandoCheckTrackerWindow);

    mRandoCheckTrackerSettingsWindow = std::make_shared<Rando::CheckTracker::SettingsWindow>(
        "gWindows.CheckTrackerSettings", "Check Tracker Settings");
    gui->AddGuiWindow(mRandoCheckTrackerSettingsWindow);
}

void Destroy() {
    auto gui = Ship::Context::GetInstance()->GetWindow()->GetGui();

    gui->RemoveAllGuiWindows();
    mBenMenuBar = nullptr;
    mBenMenu = nullptr;
    mStatsWindow = nullptr;
    mConsoleWindow = nullptr;
    mGfxDebuggerWindow = nullptr;
    mInputEditorWindow = nullptr;
    mCollisionViewerWindow = nullptr;
    mEventLogWindow = nullptr;
    mNotificationWindow = nullptr;
    mRandoCheckTrackerWindow = nullptr;
    mRandoCheckTrackerSettingsWindow = nullptr;

    mHookDebuggerWindow = nullptr;
    mSaveEditorWindow = nullptr;
    mHudEditorWindow = nullptr;
    mCosmeticEditorWindow = nullptr;
    mActorViewerWindow = nullptr;
    mAudioEditorWindow = nullptr;
    mItemTrackerWindow = nullptr;
    mItemTrackerSettingsWindow = nullptr;
}
} // namespace BenGui
