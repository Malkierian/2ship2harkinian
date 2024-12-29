#pragma once

#include <libultraship/libultraship.h>
#include "UIWidgets.hpp"
#include "MenuTypes.h"
#include "2s2h/Enhancements/Enhancements.h"
#include "graphic/Fast3D/gfx_rendering_api.h"
#include "2s2h/DeveloperTools/DeveloperTools.h"

namespace Ship {

static std::unordered_map<int32_t, const char*> menuThemeOptions = {
    { COLOR_WHITE, "White" },
    { COLOR_GRAY, "Gray" },
    { COLOR_DARK_GRAY, "Dark Gray" },
    { COLOR_INDIGO, "Indigo" },
    { COLOR_RED, "Red" },
    { COLOR_DARK_RED, "Dark Red" },
    { COLOR_LIGHT_GREEN, "Light Green" },
    { COLOR_GREEN, "Green" },
    { COLOR_DARK_GREEN, "Dark Green" },
    { COLOR_YELLOW, "Yellow" },
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

static const std::unordered_map<AudioBackend, const char*> audioBackendsMap = {
    { AudioBackend::WASAPI, "Windows Audio Session API" },
    { AudioBackend::SDL, "SDL" },
};

static std::unordered_map<WindowBackend, const char*> windowBackendsMap = {
    { WindowBackend::FAST3D_DXGI_DX11, "DirectX" },
    { WindowBackend::FAST3D_SDL_OPENGL, "OpenGL" },
    { WindowBackend::FAST3D_SDL_METAL, "Metal" },
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
class Menu : public GuiWindow {
public:
    using GuiWindow::GuiWindow;

    Menu(const std::string& consoleVariable, const std::string& name);

    void InitElement() override;
    void DrawElement() override;
    void UpdateElement() override;
    void Draw() override;

protected:
    ImVec2 mOriginalSize;
    std::string mName;
    uint32_t mWindowFlags;

private:
    bool allowPopout = true; // PortNote: should be set to false on small screen ports
    bool popped;
    ImVec2 poppedSize;
    ImVec2 poppedPos;
    float windowHeight;
    float windowWidth;
};
} // namespace Ship