
#include "DisplayOverlay.h"
#include <spdlog/fmt/fmt.h>
#include "public/bridge/consolevariablebridge.h"
#include "Context.h"
#include "Window.h"

extern "C" {
#include "variables.h"
#include "overlays/actors/ovl_Boss_07/z_boss_07.h"
uint64_t GetUnixTimestamp();
void Boss07_Wrath_DeathCutscene(Boss07*, PlayState*);
}

#include "ShipUtils.h"
#include "interface/parameter_static/parameter_static.h"
#include "GameInteractor/GameInteractor.h"

float windowScale = 1.0f;
ImVec4 windowBG = ImVec4(0, 0, 0, 0.5f);

std::string formatTimeDisplay(uint32_t value) {
    uint32_t sec = value / 10;
    uint32_t hh = sec / 3600;
    uint32_t mm = (sec - hh * 3600) / 60;
    uint32_t ss = sec - hh * 3600 - mm * 60;
    uint32_t ds = value % 10;
    return fmt::format("{}:{:0>2}:{:0>2}.{}", hh, mm, ss, ds);
}

static constexpr ImVec4 tintColor = {};

void DrawInGameTimer(uint32_t timer, ImVec4 color = ImVec4(1, 1, 1, 1)) {
    float windowScale = MAX(CVarGetFloat("gDisplayOverlay.Scale", 1.0f), 1.0f);

    std::string timerStr = formatTimeDisplay(timer);
    uint16_t textureIndex = 0;
    for (const auto c : timerStr) {
        if (c == ':' || c == '.') {
            textureIndex = 10;
        } else {
            textureIndex = c - '0';
        }
        if (c == '.') {
            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + (8.0f * windowScale));
            ImGui::Image(Ship::Context::GetInstance()->GetWindow()->GetGui()->GetTextureByName(digitList[textureIndex]),
                         ImVec2(8.0f * windowScale, 8.0f * windowScale), ImVec2(0, 0.5f), ImVec2(1, 1), color,
                         tintColor);
        } else {
            ImGui::Image(Ship::Context::GetInstance()->GetWindow()->GetGui()->GetTextureByName(digitList[textureIndex]),
                         ImVec2(8.0f * windowScale, 16.0f * windowScale), ImVec2(0, 0), ImVec2(1, 1), color, tintColor);
        }
        ImGui::SameLine(0, 0);
    }
}

void DisplayOverlayWindow::Draw() {
    if (!gPlayState) {
        return;
    }
    if (!CVarGetInteger("gWindows.DisplayOverlay", 0)) {
        return;
    }

    float windowScale = MAX(CVarGetFloat("gDisplayOverlay.Scale", 1.0f), 1.0f);
    ImVec4 windowBG = !CVarGetInteger("gDisplayOverlay.Background", 0) ? ImVec4(0, 0, 0, 0.5f) : ImVec4(0, 0, 0, 0);

    ImGui::PushStyleColor(ImGuiCol_WindowBg, windowBG);
    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0, 0, 0, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 4.0f);

    ImGui::Begin("Overlay", nullptr,
                 ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoFocusOnAppearing |
                     ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar |
                     ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoScrollbar);
    ImGui::SetWindowFontScale(windowScale);

    ImGui::Image(Ship::Context::GetInstance()->GetWindow()->GetGui()->GetTextureByName(gTimerClockIconTex),
                 ImVec2(16.0f * windowScale, 16.0f * windowScale));
    ImGui::SameLine(0, 10.0f);
    if (gSaveContext.save.shipSaveInfo.fileCompletedAt == 0) {
        DrawInGameTimer((GetUnixTimestamp() - gSaveContext.save.shipSaveInfo.fileCreatedAt) / 100);
    } else {
        DrawInGameTimer(
            (gSaveContext.save.shipSaveInfo.fileCompletedAt - gSaveContext.save.shipSaveInfo.fileCreatedAt) / 100,
            ImVec4(0, 1, 0, 1));
    }

    ImGui::End();

    ImGui::PopStyleVar(1);
    ImGui::PopStyleColor(2);
}

void DisplayOverlayWindow::InitElement() {
    COND_HOOK(OnSaveLoad, true, [](s16 fileNum) {
        if (gSaveContext.save.shipSaveInfo.fileCreatedAt == 0) {
            gSaveContext.save.shipSaveInfo.fileCreatedAt = GetUnixTimestamp();
        }
    });

    COND_ID_HOOK(OnActorUpdate, ACTOR_BOSS_07, true, [](Actor* actor) {
        Boss07* boss = (Boss07*)actor;
        if (boss->actionFunc == Boss07_Wrath_DeathCutscene && gSaveContext.save.shipSaveInfo.fileCompletedAt == 0) {
            gSaveContext.save.shipSaveInfo.fileCompletedAt = GetUnixTimestamp();
        }
    })
}
