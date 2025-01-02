#pragma once

#include <libultraship/libultraship.h>
#include "UIWidgets.hpp"
#include "Menu.h"
#include "2s2h/Enhancements/Enhancements.h"
#include "graphic/Fast3D/gfx_rendering_api.h"
#include "2s2h/DeveloperTools/DeveloperTools.h"

namespace BenGui {

static const std::unordered_map<int32_t, const char*> menuThemeOptions = {
    { UIWidgets::Color::Pink, "Pink"},
    { UIWidgets::Color::Red, "Red" },
    { UIWidgets::Color::DarkRed, "Dark Red" },
    { UIWidgets::Color::Orange, "Orange"},
    { UIWidgets::Color::LightGreen, "Light Green" },
    { UIWidgets::Color::Green, "Green" },
    { UIWidgets::Color::DarkGreen, "Dark Green" },
    { UIWidgets::Color::LightBlue, "Light Blue" },
    { UIWidgets::Color::Blue, "Blue" },
    { UIWidgets::Color::DarkBlue, "Dark Blue" },
    { UIWidgets::Color::Indigo, "Indigo" },
    { UIWidgets::Color::Violet, "Violet" },
    { UIWidgets::Color::Purple, "Purple" },
    { UIWidgets::Color::Brown, "Brown" },
    { UIWidgets::Color::DarkBrown, "Dark Brown" },
    { UIWidgets::Color::LightGray, "Light Gray" },
    { UIWidgets::Color::Gray, "Gray" },
    { UIWidgets::Color::DarkGray, "Dark Gray" },
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

static const std::unordered_map<int32_t, const char*> clockTypeOptions = {
    { CLOCK_TYPE_ORIGINAL, "Original" },
    { CLOCK_TYPE_3DS, "MM3D style" },
    { CLOCK_TYPE_TEXT_BASED, "Text only" },
};

static const std::unordered_map<int32_t, const char*> textureFilteringMap = {
    { FILTER_THREE_POINT, "Three-Point" },
    { FILTER_LINEAR, "Linear" },
    { FILTER_NONE, "None" },
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

class BenMenu : public Ship::Menu {
  public:
    BenMenu(const std::string& consoleVariable, const std::string& name);

    void InitElement() override;
    void DrawElement() override;
    void UpdateElement() override;
    void Draw() override;

    void AddSettings();
    void AddEnhancements();
    void AddDevTools();

  protected:
    std::vector<SidebarEntry> settingsSidebar;
    std::vector<SidebarEntry> enhancementsSidebar;
    std::vector<SidebarEntry> devToolsSidebar;
};
} // namespace BenGui