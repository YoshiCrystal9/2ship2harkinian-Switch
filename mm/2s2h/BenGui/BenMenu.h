#ifndef BENMENU_H
#define BENMENU_H

#include "UIWidgets.hpp"
#include "Menu.h"
#include "2s2h/Enhancements/Enhancements.h"
#include "2s2h/DeveloperTools/DeveloperTools.h"
#include "graphic/Fast3D/backends/gfx_rendering_api.h"

namespace BenGui {

static const std::unordered_map<int32_t, const char*> menuThemeOptions = {
    { UIWidgets::Colors::Red, "Red" },
    { UIWidgets::Colors::DarkRed, "Dark Red" },
    { UIWidgets::Colors::Orange, "Orange" },
    { UIWidgets::Colors::Green, "Green" },
    { UIWidgets::Colors::DarkGreen, "Dark Green" },
    { UIWidgets::Colors::LightBlue, "Light Blue" },
    { UIWidgets::Colors::Blue, "Blue" },
    { UIWidgets::Colors::DarkBlue, "Dark Blue" },
    { UIWidgets::Colors::Indigo, "Indigo" },
    { UIWidgets::Colors::Violet, "Violet" },
    { UIWidgets::Colors::Purple, "Purple" },
    { UIWidgets::Colors::Brown, "Brown" },
    { UIWidgets::Colors::Gray, "Gray" },
    { UIWidgets::Colors::DarkGray, "Dark Gray" },
};

static const std::unordered_map<int32_t, const char*> alwaysWinDoggyraceOptions = {
    { ALWAYS_WIN_DOGGY_RACE_OFF, "Off" },
    { ALWAYS_WIN_DOGGY_RACE_MASKOFTRUTH, "When owning Mask of Truth" },
    { ALWAYS_WIN_DOGGY_RACE_ALWAYS, "Always" },
};

static const std::unordered_map<int32_t, const char*> cremiaRewardOptions = {
    { CREMIA_REWARD_RANDOM, "Vanilla" },
    { CREMIA_REWARD_ALWAYS_HUG, "Hug" },
    { CREMIA_REWARD_ALWAYS_RUPEE, "Rupee" },
};

static const std::unordered_map<int32_t, const char*> gibdoTradeSequenceOptions = {
    { GIBDO_TRADE_SEQUENCE_VANILLA, "Vanilla" },
    { GIBDO_TRADE_SEQUENCE_MM3D, "MM3D" },
    { GIBDO_TRADE_SEQUENCE_NO_TRADE, "No trade" },
};

static const std::unordered_map<int32_t, const char*> clockTypeOptions = {
    { CLOCK_TYPE_ORIGINAL, "Original" },
    { CLOCK_TYPE_3DS, "MM3D style" },
    { CLOCK_TYPE_TEXT_BASED, "Text only" },
};

static const std::unordered_map<int32_t, const char*> textureFilteringMap = {
    { Fast::FILTER_THREE_POINT, "Three-Point" },
    { Fast::FILTER_LINEAR, "Linear" },
    { Fast::FILTER_NONE, "None" },
};

static const std::unordered_map<int32_t, const char*> motionBlurOptions = {
    { MOTION_BLUR_DYNAMIC, "Dynamic (default)" },
    { MOTION_BLUR_ALWAYS_OFF, "Always Off" },
    { MOTION_BLUR_ALWAYS_ON, "Always On" },
};

static const std::unordered_map<int32_t, const char*> debugSaveOptions = {
    { DEBUG_SAVE_INFO_COMPLETE, "100\% save" },
    { DEBUG_SAVE_INFO_VANILLA_DEBUG, "Vanilla debug save" },
    { DEBUG_SAVE_INFO_NONE, "Empty save" },
};

static const std::unordered_map<int32_t, const char*> logLevels = {
    { DEBUG_LOG_TRACE, "Trace" }, { DEBUG_LOG_DEBUG, "Debug" }, { DEBUG_LOG_INFO, "Info" },
    { DEBUG_LOG_WARN, "Warn" },   { DEBUG_LOG_ERROR, "Error" }, { DEBUG_LOG_CRITICAL, "Critical" },
    { DEBUG_LOG_OFF, "Off" },
};

static const std::unordered_map<int32_t, const char*> timeStopOptions = {
    { TIME_STOP_OFF, "Off" },
    { TIME_STOP_TEMPLES, "Temples" },
    { TIME_STOP_TEMPLES_DUNGEONS, "Temples + Mini Dungeons" },
};

static const std::unordered_map<int32_t, const char*> notificationPosition = {
    { 0, "Top Left" }, { 1, "Top Right" }, { 2, "Bottom Left" }, { 3, "Bottom Right" }, { 4, "Hidden" },
};

static const std::unordered_map<int32_t, const char*> dekuGuardSearchBallsOptions = {
    { DEKU_GUARD_SEARCH_BALLS_NEVER, "Never" },
    { DEKU_GUARD_SEARCH_BALLS_NIGHT_ONLY, "Night Only" },
    { DEKU_GUARD_SEARCH_BALLS_ALWAYS, "Always" },
};

static const std::unordered_map<int32_t, const char*> skipGetItemCutscenesOptions = {
    { 0, "Never" },
    { 1, "Junk Items Only" },
    { 2, "Everything But Major" },
    { 3, "Always" },
};

static const std::unordered_map<int32_t, const char*> powerCrouchStabOptions = {
    { 0, "Patched (US/EU)" },
    { 1, "Unpatched (JP)" },
    { 2, "Unpatched (OoT)" },
};

static const std::unordered_map<int32_t, const char*> maskOfTruthGrottoOptions = {
    { HIDDEN_GROTTOS_VISIBLITY_OFF, "Off" },
    { HIDDEN_GROTTOS_VISIBLITY_WEAR_MASK_OF_TRUTH, "Wear Mask of Truth" },
    { HIDDEN_GROTTOS_VISIBLITY_HAVE_MASK_OF_TRUTH, "Have Mask of Truth" },
    { HIDDEN_GROTTOS_VISIBLITY_ALWAYS, "Always" },
};

static const std::unordered_map<int32_t, const char*> damageMultiplierOptions = {
    { 0, "1x" }, { 1, "2x" }, { 2, "4x" }, { 3, "8x" }, { 4, "16x" }, { 10, "1 Hit KO" },
};

class BenMenu : public Ship::Menu {
  public:
    BenMenu(const std::string& consoleVariable, const std::string& name);

    void InitElement() override;
    void DrawElement() override;
    void UpdateElement() override;
    void Draw() override;

    void AddSidebarEntry(std::string sectionName, std::string sidbarName, uint32_t columnCount);
    WidgetInfo& AddWidget(WidgetPath& pathInfo, std::string widgetName, WidgetType widgetType);
    void AddSettings();
    void AddEnhancements();
    void AddDevTools();
};
} // namespace BenGui

#endif // BENMENU_H
