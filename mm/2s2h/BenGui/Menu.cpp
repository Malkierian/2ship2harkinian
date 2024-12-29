#include "Menu.h"
#include "UIWidgets.hpp"
#include "BenPort.h"
#include "BenInputEditorWindow.h"
#include "window/gui/GuiMenuBar.h"
#include "window/gui/GuiElement.h"
#include "DeveloperTools/SaveEditor.h"
#include "DeveloperTools/ActorViewer.h"
#include "DeveloperTools/CollisionViewer.h"
#include "DeveloperTools/EventLog.h"
#include "2s2h/Enhancements/GfxPatcher/AuthenticGfxPatches.h"
#include "HudEditor.h"
#include "Notification.h"
#include <variant>
#include <fmt/format.h>
#include "variables.h"
#include <variant>
#include <tuple>

extern "C" {
#include "z64.h"
#include "functions.h"
extern PlayState* gPlayState;
}
std::vector<ImVec2> windowTypeSizes = { {} };

extern std::unordered_map<s16, const char*> warpPointSceneList;
extern void Warp();

namespace BenGui {
    extern std::shared_ptr<std::vector<Ship::WindowBackend>> availableWindowBackends;
    extern std::unordered_map<Ship::WindowBackend, const char*> availableWindowBackendsMap;
    extern Ship::WindowBackend configWindowBackend;
}

namespace Ship {
SidebarEntry searchSidebarEntry = {
    "Search",
    1,
    { { { "Menu Theme", "", "Searches all menus for the given text, including tooltips.", WIDGET_SEARCH } } }
};
uint8_t searchSidebarIndex = 0;
ColorOption menuThemeIndex = COLOR_INDIGO;

std::string disabledTempTooltip;
const char* disabledTooltip;
bool disabledValue = false;
std::vector<MainMenuEntry> menuEntries;
ImGuiTextFilter menuSearch;
std::vector<SidebarEntry> settingsSidebar;
std::vector<SidebarEntry> enhancementsSidebar;
std::vector<SidebarEntry> devToolsSidebar;

bool operator==(Color_RGB8 const& l, Color_RGB8 const& r) noexcept {
    return l.r == r.r && l.g == r.g && l.b == r.b;
}

bool operator==(Color_RGBA8 const& l, Color_RGBA8 const& r) noexcept {
    return l.r == r.r && l.g == r.g && l.b == r.b && l.a == r.a;
}

bool operator<(Color_RGB8 const& l, Color_RGB8 const& r) noexcept {
    return (l.r < r.r && l.g <= r.g && l.b <= r.b) || (l.r <= r.r && l.g < r.g && l.b <= r.b) ||
        (l.r <= r.r && l.g <= r.g && l.b < r.b);
}

bool operator<(Color_RGBA8 const& l, Color_RGBA8 const& r) noexcept {
    return (l.r < r.r && l.g <= r.g && l.b <= r.b && l.a <= r.a) ||
        (l.r <= r.r && l.g < r.g && l.b <= r.b && l.a <= r.a) ||
        (l.r <= r.r && l.g <= r.g && l.b < r.b && l.a <= r.a) ||
        (l.r <= r.r && l.g <= r.g && l.b <= r.b && l.a < r.a);
}

bool operator>(Color_RGB8 const& l, Color_RGB8 const& r) noexcept {
    return (l.r > r.r && l.g >= r.g && l.b >= r.b) || (l.r >= r.r && l.g > r.g && l.b >= r.b) ||
        (l.r >= r.r && l.g >= r.g && l.b > r.b);
}

bool operator>(Color_RGBA8 const& l, Color_RGBA8 const& r) noexcept {
    return (l.r > r.r && l.g >= r.g && l.b >= r.b && l.a >= r.a) ||
        (l.r >= r.r && l.g > r.g && l.b >= r.b && l.a >= r.a) ||
        (l.r >= r.r && l.g >= r.g && l.b > r.b && l.a >= r.a) ||
        (l.r >= r.r && l.g >= r.g && l.b >= r.b && l.a > r.a);
}

std::unordered_map<ColorOption, ImVec4> menuTheme = { 
    { COLOR_WHITE, UIWidgets::Colors::White },
    { COLOR_GRAY, UIWidgets::Colors::Gray },
    { COLOR_DARK_GRAY, UIWidgets::Colors::DarkGray },
    { COLOR_INDIGO, UIWidgets::Colors::Indigo },
    { COLOR_RED, UIWidgets::Colors::Red },
    { COLOR_DARK_RED, UIWidgets::Colors::DarkRed },
    { COLOR_LIGHT_GREEN, UIWidgets::Colors::LightGreen },
    { COLOR_GREEN, UIWidgets::Colors::Green },
    { COLOR_DARK_GREEN, UIWidgets::Colors::DarkGreen },
    { COLOR_YELLOW, UIWidgets::Colors::Yellow }
};

std::map<DisableOption, disabledInfo> disabledMap = {
    { DISABLE_FOR_CAMERAS_OFF,
    { [](disabledInfo& info) -> bool {
        return !CVarGetInteger("gEnhancements.Camera.DebugCam.Enable", 0) &&
            !CVarGetInteger("gEnhancements.Camera.FreeLook.Enable", 0);
    },
    "Both Debug Camera and Free Look are Disabled" } },
    { DISABLE_FOR_DEBUG_CAM_ON,
    { [](disabledInfo& info) -> bool { return CVarGetInteger("gEnhancements.Camera.DebugCam.Enable", 0); },
    "Debug Camera is Enabled" } },
    { DISABLE_FOR_DEBUG_CAM_OFF,
    { [](disabledInfo& info) -> bool { return !CVarGetInteger("gEnhancements.Camera.DebugCam.Enable", 0); },
    "Debug Camera is Disabled" } },
    { DISABLE_FOR_FREE_LOOK_ON,
    { [](disabledInfo& info) -> bool { return CVarGetInteger("gEnhancements.Camera.FreeLook.Enable", 0); },
    "Free Look is Enabled" } },
    { DISABLE_FOR_FREE_LOOK_OFF,
    { [](disabledInfo& info) -> bool { return !CVarGetInteger("gEnhancements.Camera.FreeLook.Enable", 0); },
    "Free Look is Disabled" } },
    { DISABLE_FOR_GYRO_OFF,
    { [](disabledInfo& info) -> bool { return !CVarGetInteger("gEnhancements.Camera.FirstPerson.GyroEnabled", 0); },
    "Gyro Aiming is Disabled" } },
    { DISABLE_FOR_GYRO_ON,
    { [](disabledInfo& info) -> bool { return CVarGetInteger("gEnhancements.Camera.FirstPerson.GyroEnabled", 0); },
    "Gyro Aiming is Enabled" } },
    { DISABLE_FOR_RIGHT_STICK_OFF,
    { [](disabledInfo& info) -> bool {
        return !CVarGetInteger("gEnhancements.Camera.FirstPerson.RightStickEnabled", 0);
    },
    "Right Stick Aiming is Disabled" } },
    { DISABLE_FOR_AUTO_SAVE_OFF,
    { [](disabledInfo& info) -> bool { return !CVarGetInteger("gEnhancements.Saving.Autosave", 0); },
    "AutoSave is Disabled" } },
    { DISABLE_FOR_NULL_PLAY_STATE,
    { [](disabledInfo& info) -> bool { return gPlayState == NULL; }, "Save Not Loaded" } },
    { DISABLE_FOR_DEBUG_MODE_OFF,
    { [](disabledInfo& info) -> bool { return !CVarGetInteger("gDeveloperTools.DebugEnabled", 0); },
    "Debug Mode is Disabled" } },
    { DISABLE_FOR_NO_VSYNC,
    { [](disabledInfo& info) -> bool { return !Ship::Context::GetInstance()->GetWindow()->CanDisableVerticalSync(); },
    "Disabling VSync not supported" } },
    { DISABLE_FOR_NO_WINDOWED_FULLSCREEN,
    { [](disabledInfo& info) -> bool {
        return !Ship::Context::GetInstance()->GetWindow()->SupportsWindowedFullscreen();
    },
    "Windowed Fullscreen not supported" } },
    { DISABLE_FOR_NO_MULTI_VIEWPORT,
    { [](disabledInfo& info) -> bool {
        return !Ship::Context::GetInstance()->GetWindow()->GetGui()->SupportsViewports();
    },
    "Multi-viewports not supported" } },
    { DISABLE_FOR_NOT_DIRECTX,
    { [](disabledInfo& info) -> bool {
        return Ship::Context::GetInstance()->GetWindow()->GetWindowBackend() !=
            Ship::WindowBackend::FAST3D_DXGI_DX11;
    },
    "Available Only on DirectX" } },
    { DISABLE_FOR_DIRECTX,
    { [](disabledInfo& info) -> bool {
        return Ship::Context::GetInstance()->GetWindow()->GetWindowBackend() ==
            Ship::WindowBackend::FAST3D_DXGI_DX11;
    },
    "Not Available on DirectX" } },
    { DISABLE_FOR_MATCH_REFRESH_RATE_ON,
    { [](disabledInfo& info) -> bool { return CVarGetInteger("gMatchRefreshRate", 0); },
    "Match Refresh Rate is Enabled" } },
    { DISABLE_FOR_MOTION_BLUR_MODE,
    { [](disabledInfo& info) -> bool {
        info.value = CVarGetInteger("gEnhancements.Graphics.MotionBlur.Mode", 0);
        return !info.value;
    },
    "Motion Blur Mode mismatch" } },
    { DISABLE_FOR_MOTION_BLUR_OFF,
    { [](disabledInfo& info) -> bool { return !R_MOTION_BLUR_ENABLED; }, "Motion Blur is Disabled" } },
    { DISABLE_FOR_FRAME_ADVANCE_OFF,
    { [](disabledInfo& info) -> bool { return !(gPlayState != nullptr && gPlayState->frameAdvCtx.enabled); },
    "Frame Advance is Disabled" } },
    { DISABLE_FOR_WARP_POINT_NOT_SET,
    { [](disabledInfo& info) -> bool { return !CVarGetInteger(WARP_POINT_CVAR "Saved", 0); },
    "Warp Point Not Saved" } },
    { DISABLE_FOR_INTRO_SKIP_OFF,
    { [](disabledInfo& info) -> bool { return !CVarGetInteger("gEnhancements.Cutscenes.SkipIntroSequence", 0); },
    "Intro Skip Not Selected" } }
};

void UpdateWindowBackendObjects() {
    Ship::WindowBackend runningWindowBackend = Ship::Context::GetInstance()->GetWindow()->GetWindowBackend();
    int32_t configWindowBackendId = Ship::Context::GetInstance()->GetConfig()->GetInt("Window.Backend.Id", -1);
    if (Ship::Context::GetInstance()->GetWindow()->IsAvailableWindowBackend(configWindowBackendId)) {
        BenGui::configWindowBackend = static_cast<Ship::WindowBackend>(configWindowBackendId);
    } else {
        BenGui::configWindowBackend = runningWindowBackend;
    }

    BenGui::availableWindowBackends = Ship::Context::GetInstance()->GetWindow()->GetAvailableWindowBackends();
    for (auto& backend : *BenGui::availableWindowBackends) {
        BenGui::availableWindowBackendsMap[backend] = windowBackendsMap[backend];
    }
}

void FreeLookPitchMinMax() {
    f32 maxY = CVarGetFloat("gEnhancements.Camera.FreeLook.MaxPitch", 72.0f);
    f32 minY = CVarGetFloat("gEnhancements.Camera.FreeLook.MinPitch", -49.0f);
    CVarSetFloat("gEnhancements.Camera.FreeLook.MaxPitch", std::max(maxY, minY));
    CVarSetFloat("gEnhancements.Camera.FreeLook.MinPitch", std::min(maxY, minY));
}

void InsertSidebarSearch() {
    menuEntries[0].sidebarEntries.insert(menuEntries[0].sidebarEntries.begin() + searchSidebarIndex,
        searchSidebarEntry);
    CVarSetInteger(menuEntries[0].sidebarCvar, CVarGetInteger(menuEntries[0].sidebarCvar, 0) + 1);
}

void AddSettings() {
    // General Settings
    settingsSidebar.push_back(
        { "General",
        3,
        { {
            { "Menu Theme",
            "gSettings.MenuTheme",
            "Changes the Theme of the Menu Widgets.",
            WIDGET_CVAR_COMBOBOX,
        { .defaultVariant = COLOR_INDIGO, .comboBoxOptions = menuThemeOptions } },
#if not defined(__SWITCH__) and not defined(__WIIU__)
        { "Menu Controller Navigation", CVAR_IMGUI_CONTROLLER_NAV,
        "Allows controller navigation of the 2Ship menu (Settings, Enhancements,...)\nCAUTION: "
        "This will disable game inputs while the menu is visible.\n\nD-pad to move between "
        "items, A to select, B to move up in scope.",
        WIDGET_CVAR_CHECKBOX },
        { "Cursor Always Visible",
        "gSettings.CursorVisibility",
        "Makes the cursor always visible, even in full screen.",
        WIDGET_CVAR_CHECKBOX,
        {},
        [](widgetInfo& info) {
            Ship::Context::GetInstance()->GetWindow()->SetForceCursorVisibility(
                CVarGetInteger("gSettings.CursorVisibility", 0));
        } },
#endif
        { "Search In Sidebar",
        "gSettings.SidebarSearch",
        "Displays the Search menu as a sidebar entry in Settings instead of in the header.",
        WIDGET_CVAR_CHECKBOX,
        {},
        [](widgetInfo& info) {
            if (CVarGetInteger("gSettings.SidebarSearch", 0)) {
                InsertSidebarSearch();
            } else {
                std::erase_if(menuEntries[0].sidebarEntries, [](SidebarEntry entry) {
                    return strncmp(entry.label.c_str(), searchSidebarEntry.label.c_str(), 5) == 0;
                    });
                int32_t sidebarVal = CVarGetInteger(menuEntries[0].sidebarCvar, 0);
                if (sidebarVal < menuEntries.size()) {
                    sidebarVal -= 1;
                } else {
                    sidebarVal = menuEntries.size() - 1;
                }
                CVarSetInteger(menuEntries[0].sidebarCvar, sidebarVal);
            }
        } },
        { "Search Input Autofocus", "gSettings.SearchAutofocus",
        "Search input box gets autofocus when visible. Does not affect using other widgets.",
        WIDGET_CVAR_CHECKBOX },
        { .widgetName = "Alt Assets Tab hotkey",
        .widgetCVar = "gEnhancements.Mods.AlternateAssetsHotkey",
        .widgetTooltip = "Allows pressing the Tab key to toggle alternate assets",
        .widgetType = WIDGET_CVAR_CHECKBOX,
        .widgetOptions = { .defaultVariant = true } },
        { "Open App Files Folder",
        "",
        "Opens the folder that contains the save and mods folders, etc.",
        WIDGET_BUTTON,
        {},
        [](widgetInfo& info) {
            std::string filesPath = Ship::Context::GetInstance()->GetAppDirectoryPath();
            SDL_OpenURL(std::string("file:///" + std::filesystem::absolute(filesPath).string()).c_str());
        } },
            } } });
    // Audio Settings
    settingsSidebar.push_back(
        { "Audio",
        3,
        { { { "Master Volume: %.0f%%",
        "gSettings.Audio.MasterVolume",
        "Adjust overall sound volume.",
        WIDGET_CVAR_SLIDER_FLOAT,
        { .min = 0.0f,
        .max = 1.0f,
        .defaultVariant = 1.0f,
        .showButtons = false,
        .format = "",
        .isPercentage = true } },
        { "Main Music Volume: %.0f%%",
        "gSettings.Audio.MainMusicVolume",
        "Adjust the Background Music volume.",
        WIDGET_CVAR_SLIDER_FLOAT,
        { .min = 0.0f,
        .max = 1.0f,
        .defaultVariant = 1.0f,
        .showButtons = false,
        .format = "",
        .isPercentage = true },
        [](widgetInfo& info) {
            AudioSeq_SetPortVolumeScale(SEQ_PLAYER_BGM_MAIN,
                CVarGetFloat("gSettings.Audio.MainMusicVolume", 1.0f));
        } },
        { "Sub Music Volume: %.0f%%",
        "gSettings.Audio.SubMusicVolume",
        "Adjust the Sub Music volume.",
        WIDGET_CVAR_SLIDER_FLOAT,
        { .min = 0.0f,
        .max = 1.0f,
        .defaultVariant = 1.0f,
        .showButtons = false,
        .format = "",
        .isPercentage = true },
        [](widgetInfo& info) {
            AudioSeq_SetPortVolumeScale(SEQ_PLAYER_BGM_SUB,
                CVarGetFloat("gSettings.Audio.SubMusicVolume", 1.0f));
        } },
        { "Sound Effects Volume: %.0f%%",
        "gSettings.Audio.SoundEffectsVolume",
        "Adjust the Sound Effects volume.",
        WIDGET_CVAR_SLIDER_FLOAT,
        { .min = 0.0f,
        .max = 1.0f,
        .defaultVariant = 1.0f,
        .showButtons = false,
        .format = "",
        .isPercentage = true },
        [](widgetInfo& info) {
            AudioSeq_SetPortVolumeScale(SEQ_PLAYER_SFX,
                CVarGetFloat("gSettings.Audio.SoundEffectsVolume", 1.0f));
        } },
        { "Fanfare Volume: %.0f%%",
        "gSettings.Audio.FanfareVolume",
        "Adjust the Fanfare volume.",
        WIDGET_CVAR_SLIDER_FLOAT,
        { .min = 0.0f,
        .max = 1.0f,
        .defaultVariant = 1.0f,
        .showButtons = false,
        .format = "",
        .isPercentage = true },
        [](widgetInfo& info) {
            AudioSeq_SetPortVolumeScale(SEQ_PLAYER_FANFARE,
                CVarGetFloat("gSettings.Audio.FanfareVolume", 1.0f));
        } },
        { "Ambience Volume: %.0f%%",
        "gSettings.Audio.AmbienceVolume",
        "Adjust the Ambient Sound volume.",
        WIDGET_CVAR_SLIDER_FLOAT,
        { .min = 0.0f,
        .max = 1.0f,
        .defaultVariant = 1.0f,
        .showButtons = false,
        .format = "",
        .isPercentage = true },
        [](widgetInfo& info) {
            AudioSeq_SetPortVolumeScale(SEQ_PLAYER_AMBIENCE,
                CVarGetFloat("gSettings.Audio.AmbienceVolume", 1.0f));
        } },
        { "Audio API", NULL, "Sets the audio API used by the game. Requires a relaunch to take effect.",
        WIDGET_AUDIO_BACKEND } } } });
    // Graphics Settings
    static int32_t maxFps;
    const char* tooltip = "";
    if (Ship::Context::GetInstance()->GetWindow()->GetWindowBackend() == Ship::WindowBackend::FAST3D_DXGI_DX11) {
        maxFps = 360;
        tooltip = "Uses Matrix Interpolation to create extra frames, resulting in smoother graphics. This is "
            "purely visual and does not impact game logic, execution of glitches etc.\n\nA higher target "
            "FPS than your monitor's refresh rate will waste resources, and might give a worse result.";
    } else {
        maxFps = Ship::Context::GetInstance()->GetWindow()->GetCurrentRefreshRate();
        tooltip = "Uses Matrix Interpolation to create extra frames, resulting in smoother graphics. This is "
            "purely visual and does not impact game logic, execution of glitches etc.";
    }
    settingsSidebar.push_back(
        { "Graphics",
        3,
        { { { "Toggle Fullscreen",
        "gSettings.Fullscreen",
        "Toggles Fullscreen On/Off.",
        WIDGET_CVAR_CHECKBOX,
        {},
        [](widgetInfo& info) { Ship::Context::GetInstance()->GetWindow()->ToggleFullscreen(); } },
#ifndef __APPLE__
        { "Internal Resolution: %.0f%%",
        CVAR_INTERNAL_RESOLUTION,
        "Multiplies your output resolution by the value inputted, as a more intensive but effective "
        "form of anti-aliasing.",
        WIDGET_CVAR_SLIDER_FLOAT,
        { .min = 0.5f,
        .max = 2.0f,
        .defaultVariant = 1.0f,
        .showButtons = false,
        .format = "",
        .isPercentage = true },
        [](widgetInfo& info) {
            Ship::Context::GetInstance()->GetWindow()->SetResolutionMultiplier(
                CVarGetFloat(CVAR_INTERNAL_RESOLUTION, 1));
        } },
#endif
#ifndef __WIIU__
        { "Anti-aliasing (MSAA): %d",
        CVAR_MSAA_VALUE,
        "Activates MSAA (multi-sample anti-aliasing) from 2x up to 8x, to smooth the edges of rendered "
        "geometry.\n"
        "Higher sample count will result in smoother edges on models, but may reduce performance.",
        WIDGET_CVAR_SLIDER_INT,
        { 1, 8, 1 },
        [](widgetInfo& info) {
            Ship::Context::GetInstance()->GetWindow()->SetMsaaLevel(CVarGetInteger(CVAR_MSAA_VALUE, 1));
        } },
#endif

        { "Current FPS: %d",
        "gInterpolationFPS",
        tooltip,
        WIDGET_CVAR_SLIDER_INT,
        { 20, maxFps, 20 },
        [](widgetInfo& info) {
            int32_t defaultVariant = std::get<int32_t>(info.widgetOptions.defaultVariant);
            if (CVarGetInteger(info.widgetCVar, defaultVariant) == defaultVariant) {
                info.widgetName = "Current FPS: Original (%d)";
            } else {
                info.widgetName = "Current FPS: %d";
            }
        },
        [](widgetInfo& info) {
            if (disabledMap.at(DISABLE_FOR_MATCH_REFRESH_RATE_ON).active)
                info.activeDisables.push_back(DISABLE_FOR_MATCH_REFRESH_RATE_ON);
        } },
        { "Match Refresh Rate",
        "",
        "Matches interpolation value to the current game's window refresh rate.",
        WIDGET_BUTTON,
        {},
        [](widgetInfo& info) {
            int hz = Ship::Context::GetInstance()->GetWindow()->GetCurrentRefreshRate();
            if (hz >= 20 && hz <= 360) {
                CVarSetInteger("gInterpolationFPS", hz);
                Ship::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesOnNextTick();
            }
        },
        [](widgetInfo& info) { info.isHidden = disabledMap.at(DISABLE_FOR_NOT_DIRECTX).active; } },
        { "Match Refresh Rate",
        "gMatchRefreshRate",
        "Matches interpolation value to the current game's window refresh rate.",
        WIDGET_CVAR_CHECKBOX,
        {},
        nullptr,
        [](widgetInfo& info) { info.isHidden = disabledMap.at(DISABLE_FOR_DIRECTX).active; } },
        { "Jitter fix : >= % d FPS",
        "gExtraLatencyThreshold",
        "When Interpolation FPS setting is at least this threshold, add one frame of input "
        "lag (e.g. 16.6 ms for 60 FPS) in order to avoid jitter. This setting allows the "
        "CPU to work on one frame while GPU works on the previous frame.\nThis setting "
        "should be used when your computer is too slow to do CPU + GPU work in time.",
        WIDGET_CVAR_SLIDER_INT,
        { 0, 360, 80 },
        nullptr,
        [](widgetInfo& info) { info.isHidden = disabledMap.at(DISABLE_FOR_NOT_DIRECTX).active; } },
        { "Renderer API (Needs reload)", NULL, "Sets the renderer API used by the game.", WIDGET_VIDEO_BACKEND },
        { "Enable Vsync",
        CVAR_VSYNC_ENABLED,
        "Enables Vsync.",
        WIDGET_CVAR_CHECKBOX,
        {},
        nullptr,
        [](widgetInfo& info) { info.isHidden = disabledMap.at(DISABLE_FOR_NO_VSYNC).active; } },
        { "Windowed Fullscreen",
        CVAR_SDL_WINDOWED_FULLSCREEN,
        "Enables Windowed Fullscreen Mode.",
        WIDGET_CVAR_CHECKBOX,
        {},
        nullptr,
        [](widgetInfo& info) { info.isHidden = disabledMap.at(DISABLE_FOR_NO_WINDOWED_FULLSCREEN).active; } },
        { "Allow multi-windows",
        CVAR_ENABLE_MULTI_VIEWPORTS,
        "Allows multiple windows to be opened at once. Requires a reload to take effect.",
        WIDGET_CVAR_CHECKBOX,
        {},
        nullptr,
        [](widgetInfo& info) { info.isHidden = disabledMap.at(DISABLE_FOR_NO_MULTI_VIEWPORT).active; } },
        { "Texture Filter (Needs reload)",
        CVAR_TEXTURE_FILTER,
        "Sets the applied Texture Filtering.",
        WIDGET_CVAR_COMBOBOX,
        { .comboBoxOptions = textureFilteringMap } } } } });
    // Input Editor
    settingsSidebar.push_back({ "Input Editor",
        1,
        { { { "Popout Input Editor",
        "gWindows.BenInputEditor",
        "Enables the separate Input Editor window.",
        WIDGET_WINDOW_BUTTON,
        { .size = UIWidgets::Sizes::Inline, .windowName = "2S2H Input Editor" } } } } });

    settingsSidebar.push_back(
        { "Notifications",
        1,
        { {
            { "Position",
            "gNotifications.Position",
            "Which corner of the screen notifications appear in.",
            WIDGET_CVAR_COMBOBOX,
        { .defaultVariant = 3, .comboBoxOptions = notificationPosition } },
        { "Duration: %.0f seconds",
        "gNotifications.Duration",
        "How long notifications are displayed for.",
        WIDGET_CVAR_SLIDER_FLOAT,
        { .min = 3.0f, .max = 30.0f, .defaultVariant = 10.0f, .format = "%.1f", .step = 0.1f } },
        { "Background Opacity: %.0f%%",
        "gNotifications.BgOpacity",
        "How opaque the background of notifications is.",
        WIDGET_CVAR_SLIDER_FLOAT,
        { .min = 0.0f, .max = 1.0f, .defaultVariant = 0.5f, .format = "%.0f%%", .isPercentage = true } },
        { "Size %.1f",
        "gNotifications.Size",
        "How large notifications are.",
        WIDGET_CVAR_SLIDER_FLOAT,
        { .min = 1.0f, .max = 5.0f, .defaultVariant = 1.8f, .format = "%.1f", .step = 0.1f } },
        { "Test Notification",
        "",
        "Displays a test notification.",
        WIDGET_BUTTON,
        {},
        [](widgetInfo& info) {
            Notification::Emit({
                .itemIcon = "__OTR__icon_item_24_static_yar/gQuestIconGoldSkulltulaTex",
                .prefix = "This",
                .message = "is a",
                .suffix = "test.",
                });
        } },
            } } });

    if (CVarGetInteger("gSettings.SidebarSearch", 0)) {
        settingsSidebar.insert(settingsSidebar.begin() + searchSidebarIndex, searchSidebarEntry);
    }
}
int32_t motionBlurStrength;

void AddEnhancements() {
    // Camera Snap Fix
    enhancementsSidebar.push_back(
        { "Camera",
        3,
        { {
            { .widgetName = "Fixes", .widgetType = WIDGET_SEPARATOR_TEXT },
        { "Fix Targeting Camera Snap",
        "gEnhancements.Camera.FixTargettingCameraSnap",
        "Fixes the camera snap that occurs when you are moving and press the targeting button.",
        WIDGET_CVAR_CHECKBOX,
        {} },
        { .widgetName = "First Person", .widgetType = WIDGET_SEPARATOR_TEXT },
        { "Disable Auto-Centering",
        "gEnhancements.Camera.FirstPerson.DisableFirstPersonAutoCenterView",
        "Disables the auto-centering of the camera in first person mode.",
        WIDGET_CVAR_CHECKBOX,
        {},
        nullptr,
        [](widgetInfo& info) {
            if (disabledMap.at(DISABLE_FOR_GYRO_ON).active)
                info.activeDisables.push_back(DISABLE_FOR_GYRO_ON);
        } },
        { "Invert X Axis", "gEnhancements.Camera.FirstPerson.InvertX",
        "Inverts the X Axis of the Camera in First Person Mode.", WIDGET_CVAR_CHECKBOX },
        { "Invert Y Axis",
        "gEnhancements.Camera.FirstPerson.InvertY",
        "Inverts the Y Axis of the Camera in First Person Mode.",
        WIDGET_CVAR_CHECKBOX,
        { .defaultVariant = true } },
        { "X Axis Sensitivity: %.0f%%",
        "gEnhancements.Camera.FirstPerson.SensitivityX",
        "Adjusts the Sensitivity of the X Axis in First Person Mode.",
        WIDGET_CVAR_SLIDER_FLOAT,
        { .min = 0.01f, .max = 2.0f, .defaultVariant = 1.0f, .format = "%.0f%%", .isPercentage = true },
        nullptr },
        { "Y Axis Sensitivity: %.0f%%",
        "gEnhancements.Camera.FirstPerson.SensitivityY",
        "Adjusts the Sensitivity of the Y Axis in First Person Mode.",
        WIDGET_CVAR_SLIDER_FLOAT,
        { .min = 0.01f, .max = 2.0f, .defaultVariant = 1.0f, .format = "%.0f%%", .isPercentage = true },
        nullptr },
        { "Gyro Aiming",
        "gEnhancements.Camera.FirstPerson.GyroEnabled",
        "Enables Gyro Aiming in First Person Mode.",
        WIDGET_CVAR_CHECKBOX,
        {},
        nullptr },
        { "Invert Gyro X Axis",
        "gEnhancements.Camera.FirstPerson.GyroInvertX",
        "Inverts the X Axis of the Gyro in First Person Mode.",
        WIDGET_CVAR_CHECKBOX,
        {},
        nullptr,
        [](widgetInfo& info) { info.isHidden = disabledMap.at(DISABLE_FOR_GYRO_OFF).active; } },
        { "Invert Gyro Y Axis",
        "gEnhancements.Camera.FirstPerson.GyroInvertY",
        "Inverts the Y Axis of the Gyro in First Person Mode.",
        WIDGET_CVAR_CHECKBOX,
        {},
        nullptr,
        [](widgetInfo& info) { info.isHidden = disabledMap.at(DISABLE_FOR_GYRO_OFF).active; } },
        { "Gyro X Axis Sensitivity: %.0f%%",
        "gEnhancements.Camera.FirstPerson.GyroSensitivityX",
        "Adjusts the Sensitivity of the X Axis of the Gyro in First Person Mode.",
        WIDGET_CVAR_SLIDER_FLOAT,
        { .min = 0.01f, .max = 2.0f, .defaultVariant = 1.0f, .format = "%.0f%%", .isPercentage = true },
        nullptr,
        [](widgetInfo& info) { info.isHidden = disabledMap.at(DISABLE_FOR_GYRO_OFF).active; } },
        { "Gyro Y Axis Sensitivity: %.0f%%",
        "gEnhancements.Camera.FirstPerson.GyroSensitivityY",
        "Adjusts the Sensitivity of the Y Axis of the Gyro in First Person Mode.",
        WIDGET_CVAR_SLIDER_FLOAT,
        { .min = 0.01f, .max = 2.0f, .defaultVariant = 1.0f, .format = "%.0f%%", .isPercentage = true },
        nullptr,
        [](widgetInfo& info) { info.isHidden = disabledMap.at(DISABLE_FOR_GYRO_OFF).active; } },
        { "Right Stick Aiming", "gEnhancements.Camera.FirstPerson.RightStickEnabled",
        "Enables Right Stick Aiming in First Person Mode.", WIDGET_CVAR_CHECKBOX },
        { "Move while aiming",
        "gEnhancements.Camera.FirstPerson.MoveInFirstPerson",
        "Allows movement with the left stick while in first person mode.",
        WIDGET_CVAR_CHECKBOX,
        {},
        nullptr,
        [](widgetInfo& info) {
            if (disabledMap.at(DISABLE_FOR_RIGHT_STICK_OFF).active)
                info.activeDisables.push_back(DISABLE_FOR_RIGHT_STICK_OFF);
        } },
        { "Invert Right Stick X Axis",
        "gEnhancements.Camera.FirstPerson.RightStickInvertX",
        "Inverts the X Axis of the Right Stick in First Person Mode.",
        WIDGET_CVAR_CHECKBOX,
        {},
        nullptr,
        [](widgetInfo& info) { info.isHidden = disabledMap.at(DISABLE_FOR_RIGHT_STICK_OFF).active; } },
        { "Invert Right Stick Y Axis",
        "gEnhancements.Camera.FirstPerson.RightStickInvertY",
        "Inverts the Y Axis of the Right Stick in First Person Mode.",
        WIDGET_CVAR_CHECKBOX,
        { .defaultVariant = true },
        nullptr,
        [](widgetInfo& info) { info.isHidden = disabledMap.at(DISABLE_FOR_RIGHT_STICK_OFF).active; } },
        { "Right Stick X Axis Sensitivity: %.0f%%",
        "gEnhancements.Camera.FirstPerson.RightStickSensitivityX",
        "Adjusts the Sensitivity of the X Axis of the Right Stick in First Person Mode.",
        WIDGET_CVAR_SLIDER_FLOAT,
        { .min = 0.01f, .max = 2.0f, .defaultVariant = 1.0f, .format = "%.0f%%", .isPercentage = true },
        nullptr,
        [](widgetInfo& info) { info.isHidden = disabledMap.at(DISABLE_FOR_RIGHT_STICK_OFF).active; } },
        { "Right Stick Y Axis Sensitivity: %.0f%%",
        "gEnhancements.Camera.FirstPerson.RightStickSensitivityY",
        "Adjusts the Sensitivity of the Y Axis of the Right Stick in First Person Mode.",
        WIDGET_CVAR_SLIDER_FLOAT,
        { .min = 0.01f, .max = 2.0f, .defaultVariant = 1.0f, .format = "%.0f%%", .isPercentage = true },
        nullptr,
        [](widgetInfo& info) { info.isHidden = disabledMap.at(DISABLE_FOR_RIGHT_STICK_OFF).active; } },
            },
        // Camera Enhancements
        { { .widgetName = "Cameras", .widgetType = WIDGET_SEPARATOR_TEXT },
        { "Free Look",
        "gEnhancements.Camera.FreeLook.Enable",
        "Enables free look camera control\nNote: You must remap C buttons off of the right "
        "stick in the controller config menu, and map the camera stick to the right stick.",
        WIDGET_CVAR_CHECKBOX,
        {},
        nullptr,
        [](widgetInfo& info) {
            if (disabledMap.at(DISABLE_FOR_DEBUG_CAM_ON).active)
                info.activeDisables.push_back(DISABLE_FOR_DEBUG_CAM_ON);
        } },
        { "Camera Distance: %d",
        "gEnhancements.Camera.FreeLook.MaxCameraDistance",
        "Maximum Camera Distance for Free Look.",
        WIDGET_CVAR_SLIDER_INT,
        { 100, 900, 185 },
        nullptr,
        [](widgetInfo& info) { info.isHidden = disabledMap.at(DISABLE_FOR_FREE_LOOK_OFF).active; } },
        { "Camera Transition Speed: %d",
        "gEnhancements.Camera.FreeLook.TransitionSpeed",
        "Can someone help me?",
        WIDGET_CVAR_SLIDER_INT,
        { 1, 900, 25 },
        nullptr,
        [](widgetInfo& info) { info.isHidden = disabledMap.at(DISABLE_FOR_FREE_LOOK_OFF).active; } },
        { "Max Camera Height Angle: %.0f\xC2\xB0",
        "gEnhancements.Camera.FreeLook.MaxPitch",
        "Maximum Height of the Camera.",
        WIDGET_CVAR_SLIDER_FLOAT,
        { .min = -89.0f, .max = 89.0f, .defaultVariant = 72.0f, .format = "%.0f\xC2\xB0" },
        [](widgetInfo& info) { FreeLookPitchMinMax(); },
        [](widgetInfo& info) { info.isHidden = disabledMap.at(DISABLE_FOR_FREE_LOOK_OFF).active; } },
        { "Min Camera Height Angle: %.0f\xC2\xB0",
        "gEnhancements.Camera.FreeLook.MinPitch",
        "Minimum Height of the Camera.",
        WIDGET_CVAR_SLIDER_FLOAT,
        { .min = -89.0f, .max = 89.0f, .defaultVariant = -49.0f, .format = "%.0f\xC2\xB0" },
        [](widgetInfo& info) { FreeLookPitchMinMax(); },
        [](widgetInfo& info) { info.isHidden = disabledMap.at(DISABLE_FOR_FREE_LOOK_OFF).active; } },
        { "Debug Camera",
        "gEnhancements.Camera.DebugCam.Enable",
        "Enables free camera control.",
        WIDGET_CVAR_CHECKBOX,
        {},
        nullptr,
        [](widgetInfo& info) {
            if (disabledMap.at(DISABLE_FOR_FREE_LOOK_ON).active) {
                info.activeDisables.push_back(DISABLE_FOR_FREE_LOOK_ON);
            }
        } },
        { "Invert Camera X Axis",
        "gEnhancements.Camera.RightStick.InvertXAxis",
        "Inverts the Camera X Axis",
        WIDGET_CVAR_CHECKBOX,
        {},
        nullptr,
        [](widgetInfo& info) {
            if (disabledMap.at(DISABLE_FOR_CAMERAS_OFF).active) {
                info.activeDisables.push_back(DISABLE_FOR_CAMERAS_OFF);
            }
        } },
        { "Invert Camera Y Axis",
        "gEnhancements.Camera.RightStick.InvertYAxis",
        "Inverts the Camera Y Axis",
        WIDGET_CVAR_CHECKBOX,
        {},
        nullptr,
        [](widgetInfo& info) {
            if (disabledMap.at(DISABLE_FOR_CAMERAS_OFF).active) {
                info.activeDisables.push_back(DISABLE_FOR_CAMERAS_OFF);
            }
        } },
        { "Third-Person Horizontal Sensitivity: %.0f",
        "gEnhancements.Camera.RightStick.CameraSensitivity.X",
        "Adjust the Sensitivity of the x axis when in Third Person.",
        WIDGET_CVAR_SLIDER_FLOAT,
        { .min = 0.01f, .max = 5.0f, .defaultVariant = 1.0f, .format = "%.0f%%", .isPercentage = true },
        nullptr,
        [](widgetInfo& info) {
            if (disabledMap.at(DISABLE_FOR_CAMERAS_OFF).active) {
                info.activeDisables.push_back(DISABLE_FOR_CAMERAS_OFF);
            }
        } },
        { "Third-Person Vertical Sensitivity: %.0f",
        "gEnhancements.Camera.RightStick.CameraSensitivity.Y",
        "Adjust the Sensitivity of the x axis when in Third Person.",
        WIDGET_CVAR_SLIDER_FLOAT,
        { .min = 0.01f, .max = 5.0f, .defaultVariant = 1.0f, .format = "%.0f%%", .isPercentage = true },
        nullptr,
        [](widgetInfo& info) {
            if (disabledMap.at(DISABLE_FOR_CAMERAS_OFF).active) {
                info.activeDisables.push_back(DISABLE_FOR_CAMERAS_OFF);
            }
        } },
        { "Enable Roll (6\xC2\xB0 of Freedom)",
        "gEnhancements.Camera.DebugCam.6DOF",
        "This allows for all six degrees of movement with the camera, NOTE: Yaw will work "
        "differently in this system, instead rotating around the focal point"
        ", rather than a polar axis.",
        WIDGET_CVAR_CHECKBOX,
        {},
        nullptr,
        [](widgetInfo& info) { info.isHidden = disabledMap.at(DISABLE_FOR_DEBUG_CAM_OFF).active; } },
        { "Camera Speed: %.0f",
        "gEnhancements.Camera.DebugCam.CameraSpeed",
        "Adjusts the speed of the Camera.",
        WIDGET_CVAR_SLIDER_FLOAT,
        { .min = 0.1f, .max = 3.0f, .defaultVariant = 0.5f, .format = "%.0f%%", .isPercentage = true },
        nullptr,
        [](widgetInfo& info) { info.isHidden = disabledMap.at(DISABLE_FOR_DEBUG_CAM_OFF).active; } } } } });
    // Cheats
    enhancementsSidebar.push_back(
        { "Cheats",
        3,
        { { { "Infinite Health", "gCheats.InfiniteHealth", "Always have full Hearts.", WIDGET_CVAR_CHECKBOX, {} },
        { "Infinite Magic", "gCheats.InfiniteMagic", "Always have full Magic.", WIDGET_CVAR_CHECKBOX, {} },
        { "Infinite Rupees", "gCheats.InfiniteRupees", "Always have a full Wallet.", WIDGET_CVAR_CHECKBOX, {} },
        { "Infinite Consumables", "gCheats.InfiniteConsumables",
        "Always have max Consumables, you must have collected the consumables first.", WIDGET_CVAR_CHECKBOX },
        { "Longer Deku Flower Glide", "gCheats.LongerFlowerGlide",
        "Allows Deku Link to glide longer, no longer dropping after a certain distance.",
        WIDGET_CVAR_CHECKBOX },
        { "No Clip", "gCheats.NoClip", "Allows Link to phase through collision.", WIDGET_CVAR_CHECKBOX },
        { "Unbreakable Razor Sword", "gCheats.UnbreakableRazorSword",
        "Allows to Razor Sword to be used indefinitely without dulling its blade.", WIDGET_CVAR_CHECKBOX },
        { "Unrestricted Items", "gCheats.UnrestrictedItems", "Allows all Forms to use all Items.",
        WIDGET_CVAR_CHECKBOX },
        { "Hookshot Anywhere", "gCheats.HookshotAnywhere", "Allows most surfaces to be hookshot-able",
        WIDGET_CVAR_CHECKBOX },
        { "Moon Jump on L", "gCheats.MoonJumpOnL", "Holding L makes you float into the air.",
        WIDGET_CVAR_CHECKBOX },
        { "Elegy of Emptiness Anywhere", "gCheats.ElegyAnywhere", "Allows Elegy of Emptiness outside of Ikana",
        WIDGET_CVAR_CHECKBOX },
        { "Stop Time in Dungeons",
        "gCheats.TempleTimeStop",
        "Stops time from advancing in selected areas. Requires a room change to update.\n\n"
        "- Off: Vanilla behaviour.\n"
        "- Temples: Stops time in Woodfall, Snowhead, Great Bay, and Stone Tower Temples.\n"
        "- Temples + Mini Dungeons: In addition to the above temples, stops time in both Spider "
        "Houses, Pirate's Fortress, Beneath the Well, Ancient Castle of Ikana, and Secret Shrine.",
        WIDGET_CVAR_COMBOBOX,
        { .comboBoxOptions = timeStopOptions } } } } });
    // Gameplay Enhancements
    enhancementsSidebar.push_back(
        { "Gameplay",
        3,
        { { { .widgetName = "Player", .widgetType = WIDGET_SEPARATOR_TEXT },
        { "Fast Deku Flower Launch", "gEnhancements.Player.FastFlowerLaunch",
        "Speeds up the time it takes to be able to get maximum height from launching out of a deku flower",
        WIDGET_CVAR_CHECKBOX },
        { "Instant Putaway", "gEnhancements.Player.InstantPutaway",
        "Allows Link to instantly puts away held item without waiting.", WIDGET_CVAR_CHECKBOX },
        { "Fierce Deity Putaway", "gEnhancements.Player.FierceDeityPutaway",
        "Allows Fierce Deity Link to put away his sword.", WIDGET_CVAR_CHECKBOX },
        { "Climb speed",
        "gEnhancements.Player.ClimbSpeed",
        "Increases the speed at which Link climbs vines and ladders.",
        WIDGET_CVAR_SLIDER_INT,
        { 1, 5, 1 } },
        { "Dpad Equips", "gEnhancements.Dpad.DpadEquips", "Allows you to equip items to your d-pad",
        WIDGET_CVAR_CHECKBOX },
        { "Always Win Doggy Race",
        "gEnhancements.Minigames.AlwaysWinDoggyRace",
        "Makes the Doggy Race easier to win.",
        WIDGET_CVAR_COMBOBOX,
        { .comboBoxOptions = alwaysWinDoggyraceOptions } },
        { "Milk Run Reward Options",
        "gEnhancements.Minigames.CremiaHugs",
        "Choose what reward you get for winning the Milk Run minigame after the first time. \n"
        "-Vanilla: Reward is Random\n"
        "-Hug: Get the hugging cutscene\n"
        "-Rupee: Get the rupee reward",
        WIDGET_CVAR_COMBOBOX,
        { .comboBoxOptions = cremiaRewardOptions } },
        { "Cucco Shack Cucco Count",
        "gEnhancements.Minigames.CuccoShackCuccoCount",
        "Choose how many cuccos you need to raise to make Grog happy.",
        WIDGET_CVAR_SLIDER_INT,
        { 1, 10, 10 } },
        { "Swordsman School Winning Score",
        "gEnhancements.Minigames.SwordsmanSchoolScore",
        "Sets the score required to win the Swordsman School.",
        WIDGET_CVAR_SLIDER_INT,
        { 1, 30, 30 } },
        { "Fast Magic Arrow Equip Animation", "gEnhancements.Equipment.MagicArrowEquipSpeed",
        "Removes the animation for equipping Magic Arrows.", WIDGET_CVAR_CHECKBOX },
        { "Instant Fin Boomerangs Recall", "gEnhancements.PlayerActions.InstantRecall",
        "Pressing B will instantly recall the fin boomerang back to Zora Link after they are thrown.",
        WIDGET_CVAR_CHECKBOX },
        { "Two-Handed Sword Spin Attack", "gEnhancements.Equipment.TwoHandedSwordSpinAttack",
        "Enables magic spin attacks for the Fierce Deity Sword and Great Fairy's Sword.",
        WIDGET_CVAR_CHECKBOX } },
        { { .widgetName = "Modes", .widgetType = WIDGET_SEPARATOR_TEXT },
        { "Play as Kafei", "gModes.PlayAsKafei", "Requires scene reload to take effect.", WIDGET_CVAR_CHECKBOX },
        { "Hyrule Warriors Styled Link", "gModes.HyruleWarriorsStyledLink",
        "When acquired, places the Keaton and Fierce Deity masks on Link similarly to how he wears them in "
        "Hyrule Warriors",
        WIDGET_CVAR_CHECKBOX },
        { "Time Moves when you Move", "gModes.TimeMovesWhenYouMove",
        "Time only moves when Link is not standing still.", WIDGET_CVAR_CHECKBOX },
        { "Mirrored World",
        "gModes.MirroredWorld.Mode",
        "Mirrors the world horizontally.",
        WIDGET_CVAR_CHECKBOX,
        {},
        ([](widgetInfo& info) {
            if (CVarGetInteger("gModes.MirroredWorld.Mode", 0)) {
                CVarSetInteger("gModes.MirroredWorld.State", 1);
            } else {
                CVarClear("gModes.MirroredWorld.State");
            }
            }) } },
        { { .widgetName = "Saving", .widgetType = WIDGET_SEPARATOR_TEXT },
        { "Persistent Owl Saves", "gEnhancements.Saving.PersistentOwlSaves",
        "Continuing a save will not remove the owl save. Playing Song of "
        "Time, allowing the moon to crash or finishing the "
        "game will remove the owl save and become the new last save.",
        WIDGET_CVAR_CHECKBOX },
        { "Pause Menu Save", "gEnhancements.Saving.PauseSave",
        "Re-introduce the pause menu save system. Pressing B in the pause menu will give you the "
        "option to create a persistent Owl Save from your current location.\n\nWhen loading back "
        "into the game, you will be placed either at the entrance of the dungeon you saved in, or "
        "in South Clock Town.",
        WIDGET_CVAR_CHECKBOX },
        { "Autosave",
        "gEnhancements.Saving.Autosave",
        "Automatically create a persistent Owl Save on the chosen interval.\n\nWhen loading "
        "back into the game, you will be placed either at the entrance of the dungeon you "
        "saved in, or in South Clock Town.",
        WIDGET_CVAR_CHECKBOX,
        {},
        ([](widgetInfo& info) { RegisterAutosave(); }) },
        { "Autosave Interval: %d minutes",
        "gEnhancements.Saving.AutosaveInterval",
        "Sets the interval between Autosaves.",
        WIDGET_CVAR_SLIDER_INT,
        { 1, 60, 5 },
        nullptr,
        [](widgetInfo& info) {
            if (disabledMap.at(DISABLE_FOR_AUTO_SAVE_OFF).active) {
                info.activeDisables.push_back(DISABLE_FOR_AUTO_SAVE_OFF);
            }
        } },
        { .widgetName = "Time Cycle", .widgetType = WIDGET_SEPARATOR_TEXT },
        { "Do not reset Bottle content", "gEnhancements.Cycle.DoNotResetBottleContent",
        "Playing the Song Of Time will not reset the bottles' content.", WIDGET_CVAR_CHECKBOX },
        { "Do not reset Consumables", "gEnhancements.Cycle.DoNotResetConsumables",
        "Playing the Song Of Time will not reset the consumables.", WIDGET_CVAR_CHECKBOX },
        { "Do not reset Razor Sword", "gEnhancements.Cycle.DoNotResetRazorSword",
        "Playing the Song Of Time will not reset the Sword back to Kokiri Sword.", WIDGET_CVAR_CHECKBOX },
        { "Do not reset Rupees", "gEnhancements.Cycle.DoNotResetRupees",
        "Playing the Song Of Time will not reset the your rupees.", WIDGET_CVAR_CHECKBOX },
        { "Do not reset Time Speed", "gEnhancements.Cycle.DoNotResetTimeSpeed",
        "Playing the Song Of Time will not reset the current time speed set by Inverted Song of Time.",
        WIDGET_CVAR_CHECKBOX },
              {
                  .widgetName = "Keep Express Mail",
                  .widgetCVar = "gEnhancements.Cycle.KeepExpressMail",
                  .widgetTooltip = "Allows the player to keep the Express Mail in their inventory after delivering it "
                  "the first time, so that both deliveries can be done within one cycle",
        .widgetType = WIDGET_CVAR_CHECKBOX,
        .widgetCallback = [](widgetInfo& info) { RegisterKeepExpressMail(); },
              },
        { .widgetName = "Unstable",
        .widgetType = WIDGET_SEPARATOR_TEXT,
        .widgetOptions = { .color = UIWidgets::Colors::Yellow } },
        { "Disable Save Delay", "gEnhancements.Saving.DisableSaveDelay",
        "Removes the arbitrary 2 second timer for saving from the original game. This is known to "
        "cause issues when attempting the 0th Day Glitch",
        WIDGET_CVAR_CHECKBOX } } } });
    // Graphics Enhancements
    enhancementsSidebar.push_back(
        { "Graphics",
        3,
        { {
            { .widgetName = "Clock", .widgetType = WIDGET_SEPARATOR_TEXT },
        { "Clock Type",
        "gEnhancements.Graphics.ClockType",
        "Swaps between Graphical and Text only Clock types.",
        WIDGET_CVAR_COMBOBOX,
        { .comboBoxOptions = clockTypeOptions } },
        { "24 Hours Clock", "gEnhancements.Graphics.24HoursClock", "Changes from a 12 Hour to a 24 Hour Clock",
        WIDGET_CVAR_CHECKBOX },
        { .widgetName = "Mods", .widgetType = WIDGET_SEPARATOR_TEXT },
        { .widgetName = "Use Alternate Assets",
        .widgetCVar = "gEnhancements.Mods.AlternateAssets",
        .widgetTooltip = "Toggle between standard assets and alternate assets. Usually mods will indicate if "
        "this setting has to be used or not.",
        .widgetType = WIDGET_CVAR_CHECKBOX },
        { .widgetName = "Motion Blur", .widgetType = WIDGET_SEPARATOR_TEXT },
        { "Motion Blur Mode",
        "gEnhancements.Graphics.MotionBlur.Mode",
        "Selects the Mode for Motion Blur.",
        WIDGET_CVAR_COMBOBOX,
        { .comboBoxOptions = motionBlurOptions, .labelPosition = UIWidgets::LabelPosition::None } },
        { "Interpolate",
        "gEnhancements.Graphics.MotionBlur.Interpolate",
        "Change motion blur capture to also happen on interpolated frames instead of only on game frames.\n"
        "This notably reduces the overall motion blur strength but smooths out the trails.",
        WIDGET_CVAR_CHECKBOX,
        {},
        nullptr,
        [](widgetInfo& info) { info.isHidden = disabledMap.at(DISABLE_FOR_MOTION_BLUR_MODE).value == 1; } },
        { "On/Off",
        "",
        "Enables Motion Blur.",
        WIDGET_CHECKBOX,
        { .valuePointer = (bool*)&R_MOTION_BLUR_ENABLED },
        nullptr,
        [](widgetInfo& info) {
            info.widgetOptions.valuePointer = (bool*)&R_MOTION_BLUR_ENABLED;
            info.isHidden = disabledMap.at(DISABLE_FOR_MOTION_BLUR_MODE).value != 0;
        } },
        { "Strength",
        "gEnhancements.Graphics.MotionBlur.Strength",
        "Motion Blur strength.",
        WIDGET_CVAR_SLIDER_INT,
        { 0, 255, 180 },
        nullptr,
        [](widgetInfo& info) { info.isHidden = disabledMap.at(DISABLE_FOR_MOTION_BLUR_MODE).value != 2; } },
        { "Strength",
        "",
        "Motion Blur strength.",
        WIDGET_SLIDER_INT,
        { 0, 255, 180, {}, &motionBlurStrength },
        [](widgetInfo& info) { R_MOTION_BLUR_ALPHA = motionBlurStrength; },
        [](widgetInfo& info) {
            motionBlurStrength = R_MOTION_BLUR_ALPHA;
            info.isHidden = disabledMap.at(DISABLE_FOR_MOTION_BLUR_MODE).value != 0 ||
                disabledMap.at(DISABLE_FOR_MOTION_BLUR_OFF).active;
        } },
        { .widgetName = "Other", .widgetType = WIDGET_SEPARATOR_TEXT },
        { "3D Item Drops", "gEnhancements.Graphics.3DItemDrops", "Makes item drops 3D", WIDGET_CVAR_CHECKBOX },
        { "Authentic Logo", "gEnhancements.Graphics.AuthenticLogo",
        "Hide the game version and build details and display the authentic "
        "model and texture on the boot logo start screen",
        WIDGET_CVAR_CHECKBOX },
        { "Bow Reticle", "gEnhancements.Graphics.BowReticle", "Gives the bow a reticle when you draw an arrow.",
        WIDGET_CVAR_CHECKBOX },
        { "Disable Black Bar Letterboxes", "gEnhancements.Graphics.DisableBlackBars",
        "Disables Black Bar Letterboxes during cutscenes and Z-targeting\nNote: there may be "
        "minor visual glitches that were covered up by the black bars\nPlease disable this "
        "setting before reporting a bug.",
        WIDGET_CVAR_CHECKBOX },
        { .widgetName = "Unstable",
        .widgetType = WIDGET_SEPARATOR_TEXT,
        .widgetOptions = { .color = UIWidgets::Colors::Yellow } },
        { "Disable Scene Geometry Distance Check",
        "gEnhancements.Graphics.DisableSceneGeometryDistanceCheck",
        "Disables the distance check for scene geometry, allowing it to be drawn no matter how far "
        "away it is from the player. This may have unintended side effects.",
        WIDGET_CVAR_CHECKBOX,
        {},
        [](widgetInfo& info) { GfxPatcher_ApplyGeometryIssuePatches(); } },
        { "Widescreen Actor Culling", "gEnhancements.Graphics.ActorCullingAccountsForWidescreen",
        "Adjusts the culling planes to account for widescreen resolutions. "
        "This may have unintended side effects.",
        WIDGET_CVAR_CHECKBOX },
              {
                  "Increase Actor Draw Distance: %dx",
                  "gEnhancements.Graphics.IncreaseActorDrawDistance",
                  "Increase the range in which Actors are drawn. This may have unintended side effects.",
                  WIDGET_CVAR_SLIDER_INT,
        { 1, 5, 1 },
              },
            } } });
    enhancementsSidebar.push_back(
        { "Items/Songs",
        3,
        { // Mask Enhancements
            { { .widgetName = "Masks", .widgetType = WIDGET_SEPARATOR_TEXT },
        { "Blast Mask has Powder Keg Force", "gEnhancements.Masks.BlastMaskKeg",
        "Blast Mask can also destroy objects only the Powder Keg can.", WIDGET_CVAR_CHECKBOX },
        { "Fast Transformation", "gEnhancements.Masks.FastTransformation",
        "Removes the delay when using transformation masks.", WIDGET_CVAR_CHECKBOX },
        { "Fierce Deity's Mask Anywhere", "gEnhancements.Masks.FierceDeitysAnywhere",
        "Allow using Fierce Deity's mask outside of boss rooms.", WIDGET_CVAR_CHECKBOX },
        { "Persistent Bunny Hood", "gEnhancements.Masks.PersistentBunnyHood.Enabled",
        "Permanently toggle a speed boost from the bunny hood by pressing "
        "'A' on it in the mask menu.",
        WIDGET_CVAR_CHECKBOX },
        { "No Blast Mask Cooldown", "gEnhancements.Masks.NoBlastMaskCooldown",
        "Eliminates the Cooldown between Blast Mask usage.", WIDGET_CVAR_CHECKBOX } },
        // Song Enhancements
        { { .widgetName = "Ocarina", .widgetType = WIDGET_SEPARATOR_TEXT },
        { "Better Song of Double Time", "gEnhancements.Songs.BetterSongOfDoubleTime",
        "When playing the Song of Double Time, you can now choose the exact time you want to go to, similar to "
        "the 3DS version.",
        WIDGET_CVAR_CHECKBOX },
        { "Enable Sun's Song", "gEnhancements.Songs.EnableSunsSong",
        "Enables the partially implemented Sun's Song. RIGHT-DOWN-UP-RIGHT-DOWN-UP to play it. "
        "This song will make time move very fast until either Link moves to a different scene, "
        "or when the time switches to a new time period.",
        WIDGET_CVAR_CHECKBOX },
        { "Dpad Ocarina", "gEnhancements.Playback.DpadOcarina", "Enables using the Dpad for Ocarina playback.",
        WIDGET_CVAR_CHECKBOX },
        { "Pause Owl Warp", "gEnhancements.Songs.PauseOwlWarp",
        "Allows warping to registered Owl Statues from the pause menu map screen. "
        "Requires that you can play Song of Soaring normally.\n\n"
        "Accounts for Index-Warp being active, by presenting all valid warps for the registered map points. "
        "Great Bay Coast warp is always given for index 0 warp as a convenience.",
        WIDGET_CVAR_CHECKBOX },
        { "Zora Eggs For Bossa Nova",
        "gEnhancements.Songs.ZoraEggCount",
        "The number of eggs required to unlock new wave bossa nova.",
        WIDGET_CVAR_SLIDER_INT,
        { 1, 7, 7 } },
        { "Prevent Dropped Ocarina Inputs", "gEnhancements.Playback.NoDropOcarinaInput",
        "Prevent dropping inputs when playing the ocarina quickly.", WIDGET_CVAR_CHECKBOX },
        { "Skip Scarecrow Song", "gEnhancements.Playback.SkipScarecrowSong",
        "Pierre appears when the Ocarina is pulled out.", WIDGET_CVAR_CHECKBOX },
        { "Faster Song Playback", "gEnhancements.Songs.FasterSongPlayback", "Speeds up the playback of songs.",
        WIDGET_CVAR_CHECKBOX } } } });
    enhancementsSidebar.push_back(
        { "Time Savers",
        3,
        { // Cutscene Skips
            { { .widgetName = "Cutscenes", .widgetType = WIDGET_SEPARATOR_TEXT },
        { "Hide Title Cards", "gEnhancements.Cutscenes.HideTitleCards", "Hides Title Cards when entering areas.",
        WIDGET_CVAR_CHECKBOX },
        { "Skip Entrance Cutscenes", "gEnhancements.Cutscenes.SkipEntranceCutscenes",
        "Skip cutscenes that occur when first entering a new area.", WIDGET_CVAR_CHECKBOX },
        { "Skip to File Select", "gEnhancements.Cutscenes.SkipToFileSelect",
        "Skip the opening title sequence and go straight to the file select menu after boot.",
        WIDGET_CVAR_CHECKBOX },
        { "Skip Intro Sequence", "gEnhancements.Cutscenes.SkipIntroSequence",
        "When starting a game you will be taken straight to South Clock Town as Deku Link.",
        WIDGET_CVAR_CHECKBOX },
        { "Skip First Cycle",
        "gEnhancements.Cutscenes.SkipFirstCycle",
        "When starting a game you will be taken straight to South Clock Town as Human Link "
        "with Deku Mask, Ocarina, Song of Time, and Song of Healing.",
        WIDGET_CVAR_CHECKBOX,
        {},
        nullptr,
        [](widgetInfo& info) { info.isHidden = disabledMap.at(DISABLE_FOR_INTRO_SKIP_OFF).active; } },
        { "Skip Story Cutscenes", "gEnhancements.Cutscenes.SkipStoryCutscenes",
        "Disclaimer: This doesn't do much yet, we will be progressively adding more skips over time.",
        WIDGET_CVAR_CHECKBOX },
        { "Skip Misc Interactions", "gEnhancements.Cutscenes.SkipMiscInteractions",
        "Disclaimer: This doesn't do much yet, we will be progressively adding more skips over time.",
        WIDGET_CVAR_CHECKBOX } },
        // Dialogue Enhancements
        { { .widgetName = "Dialogue", .widgetType = WIDGET_SEPARATOR_TEXT },
        { "Fast Bank Selection", "gEnhancements.Dialogue.FastBankSelection",
        "Pressing the Z or R buttons while the Deposit/Withdrawal Rupees dialogue is open will set "
        "the Rupees to Links current Rupees or 0 respectively.",
        WIDGET_CVAR_CHECKBOX },
        { "Fast Text", "gEnhancements.Dialogue.FastText",
        "Speeds up text rendering, and enables holding of B progress to next message.",
        WIDGET_CVAR_CHECKBOX } } } });
    enhancementsSidebar.push_back(
        { "Fixes",
        3,
        { // Fixes
            { { .widgetName = "Fixes", .widgetType = WIDGET_SEPARATOR_TEXT },
        { "Fix Ammo Count Color", "gFixes.FixAmmoCountEnvColor",
        "Fixes a missing gDPSetEnvColor, which causes the ammo count to be "
        "the wrong color prior to obtaining magic or other conditions.",
        WIDGET_CVAR_CHECKBOX },
        { "Fix Fierce Deity Z-Target movement", "gEnhancements.Fixes.FierceDeityZTargetMovement",
        "Fixes Fierce Deity movement being choppy when Z-targeting", WIDGET_CVAR_CHECKBOX },
        { "Fix Hess and Weirdshot Crash", "gEnhancements.Fixes.HessCrash",
        "Fixes a crash that can occur when performing a HESS or Weirdshot.", WIDGET_CVAR_CHECKBOX },
        { "Fix Text Control Characters", "gEnhancements.Fixes.ControlCharacters",
        "Fixes certain control characters not functioning properly "
        "depending on their position within the text.",
        WIDGET_CVAR_CHECKBOX },
        { "Fix Ikana Great Fairy Fountain Color", "gFixes.FixIkanaGreatFairyFountainColor",
        "Fixes a bug that results in the Ikana Great Fairy fountain looking green instead of yellow, this was "
        "fixed in the EU version",
        WIDGET_CVAR_CHECKBOX },
        { .widgetName = "Fix Texture overflow OOB",
        .widgetCVar = "gEnhancements.Fixes.FixTexturesOOB",
        .widgetTooltip = "Fixes textures that normally overflow to be patched with the correct size or format",
        .widgetType = WIDGET_CVAR_CHECKBOX,
        .widgetOptions = { .defaultVariant = true },
        .widgetCallback = [](widgetInfo& info) { GfxPatcher_ApplyOverflowTexturePatches(); } },
        { "Fix Completed Heart Container Audio", "gEnhancements.Fixes.CompletedHeartContainerAudio",
        "Fixes a bug that results in the wrong audio playing upon receiving a 4th piece of heart to "
        "fill a new heart container.",
        WIDGET_CVAR_CHECKBOX } } } });
    enhancementsSidebar.push_back(
        { "Restorations",
        3,
        { // Restorations
            { { .widgetName = "Restorations", .widgetType = WIDGET_SEPARATOR_TEXT },
        { "Constant Distance Backflips and Sidehops", "gEnhancements.Restorations.ConstantFlipsHops",
        "Backflips and Sidehops travel a constant distance as they did in OoT.", WIDGET_CVAR_CHECKBOX },
        { "Power Crouch Stab", "gEnhancements.Restorations.PowerCrouchStab",
        "Crouch stabs will use the power of Link's previous melee attack, as is in MM JP 1.0 and OoT.",
        WIDGET_CVAR_CHECKBOX },
        { "Side Rolls", "gEnhancements.Restorations.SideRoll", "Restores side rolling from OoT.",
        WIDGET_CVAR_CHECKBOX },
        { "Tatl ISG", "gEnhancements.Restorations.TatlISG", "Restores Navi ISG from OoT, but now with Tatl.",
        WIDGET_CVAR_CHECKBOX },
        { "Woodfall Mountain Appearance", "gEnhancements.Restorations.WoodfallMountainAppearance",
        "Restores the appearance of Woodfall mountain to not look poisoned "
        "when viewed from Termina Field after clearing Woodfall Temple\n\n"
        "Requires a scene reload to take effect",
        WIDGET_CVAR_CHECKBOX } } } });

    enhancementsSidebar.push_back(
        { "Difficulty Options",
        3,
        { { { "Disable Takkuri Steal", "gEnhancements.Cheats.DisableTakkuriSteal",
        "Prevents the Takkuri from stealing key items like bottles and swords. It may still steal other items.",
        WIDGET_CVAR_CHECKBOX },
        { "Deku Guard Search Balls",
        "gEnhancements.Cheats.DekuGuardSearchBalls",
        "Choose when to show the Deku Palace Guards' search balls\n"
        "- Never: Never show the search balls. This matches Majora's Mask 3D behaviour\n"
        "- Night Only: Only show the search balls at night. This matches original N64 behaviour.\n"
        "- Always: Always show the search balls.",
        WIDGET_CVAR_COMBOBOX,
        { .defaultVariant = DekuGuardSearchBallsOptions::DEKU_GUARD_SEARCH_BALLS_NIGHT_ONLY,
        .comboBoxOptions = dekuGuardSearchBallsOptions } } } } });
    enhancementsSidebar.push_back({ "HUD Editor",
        1,
        { // HUD Editor
            { { "Popout HUD Editor",
            "gWindows.HudEditor",
            "Enables the HUD Editor window, allowing you to modify your HUD",
            WIDGET_WINDOW_BUTTON,
        { .size = UIWidgets::Sizes::Inline, .windowName = "HUD Editor" } } } } });
    enhancementsSidebar.push_back(
        { "Item Tracker",
          1,
        { // Item Tracker Settings
            { { "Popout Item Tracker Settings",
            "gWindows.ItemTrackerSettings",
            "",
            WIDGET_WINDOW_BUTTON,
        { .size = UIWidgets::Sizes::Inline, .windowName = "Item Tracker Settings" } } } } });
}

void AddDevTools() {
    devToolsSidebar.push_back(
        { "General",
        3,
        { { { "Popout Menu", "gSettings.Menu.Popout", "Changes the menu display from overlay to windowed.",
        WIDGET_CVAR_CHECKBOX },
        { "Set Warp Point",
        "",
        "Creates warp point that you can teleport to later",
        WIDGET_BUTTON,
        {},
        [](widgetInfo& info) {
            Player* player = GET_PLAYER(gPlayState);

            CVarSetInteger(WARP_POINT_CVAR "Entrance", gSaveContext.save.entrance);
            CVarSetInteger(WARP_POINT_CVAR "Room", gPlayState->roomCtx.curRoom.num);
            CVarSetFloat(WARP_POINT_CVAR "X", player->actor.world.pos.x);
            CVarSetFloat(WARP_POINT_CVAR "Y", player->actor.world.pos.y);
            CVarSetFloat(WARP_POINT_CVAR "Z", player->actor.world.pos.z);
            CVarSetFloat(WARP_POINT_CVAR "Rotation", player->actor.shape.rot.y);
            CVarSetInteger(WARP_POINT_CVAR "Saved", 1);
            Ship::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesOnNextTick();
        },
        [](widgetInfo& info) { info.isHidden = disabledMap.at(DISABLE_FOR_NULL_PLAY_STATE).active; } },
        { "Scene Room ID",
        "",
        "",
        WIDGET_TEXT,
        {},
        nullptr,
        [](widgetInfo& info) {
            u32 sceneId = Entrance_GetSceneIdAbsolute(
                CVarGetInteger(WARP_POINT_CVAR "Entrance", ENTRANCE(SOUTH_CLOCK_TOWN, 0)));
            info.widgetName = fmt::format("{} Room {}", warpPointSceneList[sceneId],
                CVarGetInteger(WARP_POINT_CVAR "Room", 0));
            info.isHidden = disabledMap.at(DISABLE_FOR_NULL_PLAY_STATE).active ||
                disabledMap.at(DISABLE_FOR_WARP_POINT_NOT_SET).active;
        } },
        { ICON_FA_TIMES,
        "",
        "Clear warp point",
        WIDGET_BUTTON,
        { .size = UIWidgets::Sizes::Inline, .sameLine = true },
        [](widgetInfo& info) {
            CVarClear(WARP_POINT_CVAR "Entrance");
            CVarClear(WARP_POINT_CVAR "Room");
            CVarClear(WARP_POINT_CVAR "X");
            CVarClear(WARP_POINT_CVAR "Y");
            CVarClear(WARP_POINT_CVAR "Z");
            CVarClear(WARP_POINT_CVAR "Rotation");
            CVarClear(WARP_POINT_CVAR "Saved");
            Ship::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesOnNextTick();
        },
        [](widgetInfo& info) {
            info.isHidden = disabledMap.at(DISABLE_FOR_NULL_PLAY_STATE).active ||
                disabledMap.at(DISABLE_FOR_WARP_POINT_NOT_SET).active;
        } },
        { "Warp",
        "",
        "Teleport to the set warp point",
        WIDGET_BUTTON,
        { .size = UIWidgets::Sizes::Inline, .sameLine = true },
        [](widgetInfo& info) { Warp(); },
        [](widgetInfo& info) {
            info.isHidden = disabledMap.at(DISABLE_FOR_NULL_PLAY_STATE).active ||
                disabledMap.at(DISABLE_FOR_WARP_POINT_NOT_SET).active;
        } } },
        { { "Debug Mode",
        "gDeveloperTools.DebugEnabled",
        "Enables Debug Mode, allowing you to select maps with L + R + Z.",
        WIDGET_CVAR_CHECKBOX,
        {},
        [](widgetInfo& info) {
            if (!CVarGetInteger("gDeveloperTools.DebugEnabled", 0)) {
                CVarClear("gDeveloperTools.DebugSaveFileMode");
                CVarClear("gDeveloperTools.PreventActorUpdate");
                CVarClear("gDeveloperTools.PreventActorDraw");
                CVarClear("gDeveloperTools.PreventActorInit");
                CVarClear("gDeveloperTools.DisableObjectDependency");
                if (gPlayState != NULL) {
                    gPlayState->frameAdvCtx.enabled = false;
                }
                RegisterDebugSaveCreate();
                RegisterPreventActorUpdateHooks();
                RegisterPreventActorDrawHooks();
                RegisterPreventActorInitHooks();
            }
        } },
        { "Better Map Select",
        "gDeveloperTools.BetterMapSelect.Enabled",
        "Overrides the original map select with a translated, more user-friendly version.",
        WIDGET_CVAR_CHECKBOX,
        {},
        nullptr,
        [](widgetInfo& info) { info.isHidden = disabledMap.at(DISABLE_FOR_DEBUG_MODE_OFF).active; } },
        { "Debug Save File Mode",
        "gDeveloperTools.DebugSaveFileMode",
        "Change the behavior of creating saves while debug mode is enabled:\n\n"
        "- Empty Save: The default 3 heart save file in first cycle\n"
        "- Vanilla Debug Save: Uses the title screen save info (8 hearts, all items and masks)\n"
        "- 100\% Save: All items, equipment, mask, quest status and bombers notebook complete",
        WIDGET_CVAR_COMBOBOX,
        { 0, 0, 0, debugSaveOptions },
        [](widgetInfo& info) { RegisterDebugSaveCreate(); },
        [](widgetInfo& info) { info.isHidden = disabledMap.at(DISABLE_FOR_DEBUG_MODE_OFF).active; } },
        { "Prevent Actor Update",
        "gDeveloperTools.PreventActorUpdate",
        "Prevents Actors from updating.",
        WIDGET_CVAR_CHECKBOX,
        {},
        [](widgetInfo& info) { RegisterPreventActorUpdateHooks(); },
        [](widgetInfo& info) { info.isHidden = disabledMap.at(DISABLE_FOR_DEBUG_MODE_OFF).active; } },
        { "Prevent Actor Draw",
        "gDeveloperTools.PreventActorDraw",
        "Prevents Actors from drawing.",
        WIDGET_CVAR_CHECKBOX,
        {},
        [](widgetInfo& info) { RegisterPreventActorDrawHooks(); },
        [](widgetInfo& info) { info.isHidden = disabledMap.at(DISABLE_FOR_DEBUG_MODE_OFF).active; } },
        { "Prevent Actor Init",
        "gDeveloperTools.PreventActorInit",
        "Prevents Actors from initializing.",
        WIDGET_CVAR_CHECKBOX,
        {},
        [](widgetInfo& info) { RegisterPreventActorInitHooks(); },
        [](widgetInfo& info) { info.isHidden = disabledMap.at(DISABLE_FOR_DEBUG_MODE_OFF).active; } },
        { "Disable Object Dependency",
        "gDeveloperTools.DisableObjectDependency",
        "Disables dependencies when loading objects.",
        WIDGET_CVAR_CHECKBOX,
        {},
        nullptr,
        [](widgetInfo& info) { info.isHidden = disabledMap.at(DISABLE_FOR_DEBUG_MODE_OFF).active; } },
        { "Log Level",
        "gDeveloperTools.LogLevel",
        "The log level determines which messages are printed to the "
        "console. This does not affect the log file output",
        WIDGET_CVAR_COMBOBOX,
        { 0, 0, 0, logLevels },
        [](widgetInfo& info) {
            Ship::Context::GetInstance()->GetLogger()->set_level(
                (spdlog::level::level_enum)CVarGetInteger("gDeveloperTools.LogLevel", 1));
        },
        [](widgetInfo& info) { info.isHidden = disabledMap.at(DISABLE_FOR_DEBUG_MODE_OFF).active; } },
        { "Frame Advance",
        "",
        "This allows you to advance through the game one frame at a time on command. "
        "To advance a frame, hold Z and tap R on the second controller. Holding Z "
        "and R will advance a frame every half second. You can also use the buttons below.",
        WIDGET_CHECKBOX,
        {},
        nullptr,
        [](widgetInfo& info) {
            info.isHidden = disabledMap.at(DISABLE_FOR_NULL_PLAY_STATE).active ||
                disabledMap.at(DISABLE_FOR_DEBUG_MODE_OFF).active;
            if (gPlayState != nullptr) {
                info.widgetOptions.valuePointer = (bool*)&gPlayState->frameAdvCtx.enabled;
            } else {
                info.widgetOptions.valuePointer = (bool*)nullptr;
            }
        } },
        { "Advance 1",
        "",
        "Advance 1 frame.",
        WIDGET_BUTTON,
        { .size = UIWidgets::Sizes::Inline },
        [](widgetInfo& info) { CVarSetInteger("gDeveloperTools.FrameAdvanceTick", 1); },
        [](widgetInfo& info) {
            info.isHidden = disabledMap.at(DISABLE_FOR_FRAME_ADVANCE_OFF).active ||
                disabledMap.at(DISABLE_FOR_DEBUG_MODE_OFF).active;
        } },
        { "Advance (Hold)",
        "",
        "Advance frames while the button is held.",
        WIDGET_BUTTON,
        { .size = UIWidgets::Sizes::Inline, .sameLine = true },
        nullptr,
        [](widgetInfo& info) {
            info.isHidden = disabledMap.at(DISABLE_FOR_FRAME_ADVANCE_OFF).active ||
                disabledMap.at(DISABLE_FOR_DEBUG_MODE_OFF).active;
        },
        [](widgetInfo& info) {
            if (ImGui::IsItemActive()) {
                CVarSetInteger("gDeveloperTools.FrameAdvanceTick", 1);
            }
        } } } } });
    // dev tools windows
    devToolsSidebar.push_back({ "Collision Viewer",
        1,
        { { { "Popout Collision Viewer",
        "gWindows.CollisionViewer",
        "Makes collision visible on screen",
        WIDGET_WINDOW_BUTTON,
        { .size = UIWidgets::Sizes::Inline, .windowName = "Collision Viewer" } } } } });
    devToolsSidebar.push_back(
        { "Stats",
        1,
        { { { "Popout Stats",
        "gOpenWindows.Stats",
        "Shows the stats window, with your FPS and frametimes, and the OS you're playing on",
        WIDGET_WINDOW_BUTTON,
        { .size = UIWidgets::Sizes::Inline, .windowName = "Stats" } } } } });
    devToolsSidebar.push_back(
        { "Console",
        1,
        { { { "Popout Console",
        "gOpenWindows.Console",
        "Enables the console window, allowing you to input commands. Type help for some examples",
        WIDGET_WINDOW_BUTTON,
        { .size = UIWidgets::Sizes::Inline, .windowName = "Console" } } } } });
    devToolsSidebar.push_back(
        { "Gfx Debugger",
        1,
        { { { "Popout Gfx Debugger",
        "gOpenWindows.GfxDebugger",
        "Enables the Gfx Debugger window, allowing you to input commands, type help for some examples",
        WIDGET_WINDOW_BUTTON,
        { .size = UIWidgets::Sizes::Inline, .windowName = "GfxDebuggerWindow" } } } } });
    devToolsSidebar.push_back({ "Save Editor",
        1,
        { { { "Popout Save Editor",
        "gWindows.SaveEditor",
        "Enables the Save Editor window, allowing you to edit your save file",
        WIDGET_WINDOW_BUTTON,
        { .size = UIWidgets::Sizes::Inline, .windowName = "Save Editor" } } } } });
    devToolsSidebar.push_back({ "Actor Viewer",
        1,
        { { { "Popout Actor Viewer",
        "gWindows.ActorViewer",
        "Enables the Actor Viewer window, allowing you to view actors in the world.",
        WIDGET_WINDOW_BUTTON,
        { .size = UIWidgets::Sizes::Inline, .windowName = "Actor Viewer" } } } } });
    devToolsSidebar.push_back({ "Event Log",
        1,
        { { { "Popout Event Log",
        "gWindows.EventLog",
        "Enables the event log window",
        WIDGET_WINDOW_BUTTON,
        { .size = UIWidgets::Sizes::Inline, .windowName = "Event Log" } } } } });
}

Menu::Menu(const std::string& consoleVariable, const std::string& name) : GuiWindow(consoleVariable, name) {
}

void Menu::InitElement() {
    popped = CVarGetInteger("gSettings.Menu.Popout", 0);
    poppedSize.x = CVarGetInteger("gSettings.Menu.PoppedWidth", 1280);
    poppedSize.y = CVarGetInteger("gSettings.Menu.PoppedHeight", 800);
    poppedPos.x = CVarGetInteger("gSettings.Menu.PoppedPos.x", 0);
    poppedPos.y = CVarGetInteger("gSettings.Menu.PoppedPos.y", 0);
    AddSettings();
    AddEnhancements();
    AddDevTools();

    menuEntries = { { "Settings", settingsSidebar, "gSettings.Menu.SettingsSidebarIndex" },
                    { "Enhancements", enhancementsSidebar, "gSettings.Menu.EnhancementsSidebarIndex" },
                    { "Developer Tools", devToolsSidebar, "gSettings.Menu.DevToolsSidebarIndex" } };

    UpdateWindowBackendObjects();
}

void Menu::UpdateElement() {
}

bool ModernMenuSidebarEntry(std::string label) {
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = g.CurrentWindow;
    ImGuiStyle& style = ImGui::GetStyle();
    ImVec2 pos = window->DC.CursorPos;
    const ImGuiID sidebarId = window->GetID(std::string(label + "##Sidebar").c_str());
    ImVec2 labelSize = ImGui::CalcTextSize(label.c_str(), ImGui::FindRenderedTextEnd(label.c_str()), true);
    pos.y += style.FramePadding.y;
    pos.x = window->WorkRect.GetCenter().x - labelSize.x / 2;
    ImRect bb = { pos - style.FramePadding, pos + labelSize + style.FramePadding };
    ImGui::ItemSize(bb, style.FramePadding.y);
    ImGui::ItemAdd(bb, sidebarId);
    bool hovered, held;
    bool pressed = ImGui::ButtonBehavior(bb, sidebarId, &hovered, &held);
    if (pressed) {
        ImGui::MarkItemEdited(sidebarId);
    }
    window->DrawList->AddRectFilled(pos - style.FramePadding, pos + labelSize + style.FramePadding,
                                    ImGui::GetColorU32((held && hovered) ? ImGuiCol_ButtonActive
                                                       : hovered         ? ImGuiCol_ButtonHovered
                                                                         : ImGuiCol_Button),
                                    3.0f);
    UIWidgets::RenderText(pos, label.c_str(), ImGui::FindRenderedTextEnd(label.c_str()), true);
    return pressed;
}

bool ModernMenuHeaderEntry(std::string label) {
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = g.CurrentWindow;
    ImGuiStyle& style = ImGui::GetStyle();
    ImVec2 pos = window->DC.CursorPos;
    const ImGuiID headerId = window->GetID(std::string(label + "##Header").c_str());
    ImVec2 labelSize = ImGui::CalcTextSize(label.c_str(), ImGui::FindRenderedTextEnd(label.c_str()), true);
    ImRect bb = { pos, pos + labelSize + style.FramePadding * 2 };
    ImGui::ItemSize(bb, style.FramePadding.y);
    ImGui::ItemAdd(bb, headerId);
    bool hovered, held;
    bool pressed = ImGui::ButtonBehavior(bb, headerId, &hovered, &held);
    window->DrawList->AddRectFilled(bb.Min, bb.Max,
                                    ImGui::GetColorU32((held && hovered) ? ImGuiCol_ButtonActive
                                                       : hovered         ? ImGuiCol_ButtonHovered
                                                                         : ImGuiCol_Button),
                                    3.0f);
    pos += style.FramePadding;
    UIWidgets::RenderText(pos, label.c_str(), ImGui::FindRenderedTextEnd(label.c_str()), true);
    return pressed;
}

void SearchMenuGetItem(widgetInfo& widget) {
    disabledTempTooltip = "This setting is disabled because: \n\n";
    disabledValue = false;
    disabledTooltip = " ";

    if (widget.preFunc != nullptr) {
        widget.activeDisables.clear();
        widget.isHidden = false;
        widget.preFunc(widget);
        if (widget.isHidden) {
            return;
        }
        if (!widget.activeDisables.empty()) {
            disabledValue = true;
            for (auto option : widget.activeDisables) {
                disabledTempTooltip += std::string("- ") + disabledMap.at(option).reason + std::string("\n");
            }
            disabledTooltip = disabledTempTooltip.c_str();
        }
    }

    if (widget.widgetOptions.sameLine) {
        ImGui::SameLine();
    }

    try {
        switch (widget.widgetType) {
        case WIDGET_CHECKBOX: {
            bool* pointer = std::get<bool*>(widget.widgetOptions.valuePointer);
            if (pointer == nullptr) {
                SPDLOG_ERROR("Checkbox Widget requires a value pointer, currently nullptr");
                assert(false);
                return;
            }
            if (UIWidgets::Checkbox(
                widget.widgetName.c_str(), pointer,
                { .color = menuTheme[menuThemeIndex],
                .tooltip = widget.widgetTooltip,
                .disabled = disabledValue,
                .disabledTooltip = disabledTooltip,
                .labelPosition = widget.widgetOptions.labelPosition == UIWidgets::LabelPosition::None
                ? UIWidgets::LabelPosition::Near
                : widget.widgetOptions.labelPosition })) {
                if (widget.widgetCallback != nullptr) {
                    widget.widgetCallback(widget);
                }
            }
        } break;
        case WIDGET_CVAR_CHECKBOX:
            if (UIWidgets::CVarCheckbox(
                widget.widgetName.c_str(), widget.widgetCVar,
                { .color = menuTheme[menuThemeIndex],
                .tooltip = widget.widgetTooltip,
                .disabled = disabledValue,
                .disabledTooltip = disabledTooltip,
                .defaultValue = static_cast<bool>(std::get<int32_t>(widget.widgetOptions.defaultVariant)),
                .labelPosition = widget.widgetOptions.labelPosition == UIWidgets::LabelPosition::None
                ? UIWidgets::LabelPosition::Near
                : widget.widgetOptions.labelPosition })) {
                if (widget.widgetCallback != nullptr) {
                    widget.widgetCallback(widget);
                }
            };
            break;
        case WIDGET_AUDIO_BACKEND: {
                auto currentAudioBackend = Ship::Context::GetInstance()->GetAudio()->GetAudioBackend();
                if (UIWidgets::Combobox(
                    "Audio API", &currentAudioBackend, audioBackendsMap,
                    { .color = menuTheme[menuThemeIndex],
                    .tooltip = widget.widgetTooltip,
                    .disabled =
                    Ship::Context::GetInstance()->GetAudio()->GetAvailableAudioBackends()->size() <= 1,
                    .disabledTooltip = "Only one audio API is available on this platform." })) {
                    Ship::Context::GetInstance()->GetAudio()->SetAudioBackend(currentAudioBackend);
                }
        } break;
        case WIDGET_VIDEO_BACKEND: {
            if (UIWidgets::Combobox(
                "Renderer API (Needs reload)", &BenGui::configWindowBackend, BenGui::availableWindowBackendsMap,
                { .color = menuTheme[menuThemeIndex],
                .tooltip = widget.widgetTooltip,
                .disabled = BenGui::availableWindowBackends->size() <= 1,
                .disabledTooltip = "Only one renderer API is available on this platform." })) {
                Ship::Context::GetInstance()->GetConfig()->SetInt("Window.Backend.Id",
                    static_cast<int32_t>(BenGui::configWindowBackend));
                Ship::Context::GetInstance()->GetConfig()->SetString("Window.Backend.Name",
                    windowBackendsMap.at(BenGui::configWindowBackend));
                Ship::Context::GetInstance()->GetConfig()->Save();
                UpdateWindowBackendObjects();
            }
        } break;
        case WIDGET_SEPARATOR:
            ImGui::Separator();
            break;
        case WIDGET_SEPARATOR_TEXT:
            if (widget.widgetOptions.color != COLOR_NONE) {
                ImGui::PushStyleColor(ImGuiCol_Text, widget.widgetOptions.color);
            }
            ImGui::SeparatorText(widget.widgetName.c_str());
            if (widget.widgetOptions.color != COLOR_NONE) {
                ImGui::PopStyleColor();
            }
            break;
        case WIDGET_TEXT:
            ImGui::AlignTextToFramePadding();
            ImGui::Text(widget.widgetName.c_str());
            break;
        case WIDGET_COMBOBOX: {
                int32_t* pointer = std::get<int32_t*>(widget.widgetOptions.valuePointer);
                if (pointer == nullptr) {
                    SPDLOG_ERROR("Combobox Widget requires a value pointer, currently nullptr");
                    assert(false);
                    return;
                }
                if (UIWidgets::Combobox(
                    widget.widgetName.c_str(), pointer, widget.widgetOptions.comboBoxOptions,
                    { .color = menuTheme[menuThemeIndex],
                    .tooltip = widget.widgetTooltip,
                    .disabled = disabledValue,
                    .disabledTooltip = disabledTooltip,
                    .labelPosition = widget.widgetOptions.labelPosition == UIWidgets::LabelPosition::None
                    ? UIWidgets::LabelPosition::Above
                    : widget.widgetOptions.labelPosition })) {
                    if (widget.widgetCallback != nullptr) {
                        widget.widgetCallback(widget);
                    }
                };
        } break;
        case WIDGET_CVAR_COMBOBOX:
            if (UIWidgets::CVarCombobox(
                widget.widgetName.c_str(), widget.widgetCVar, widget.widgetOptions.comboBoxOptions,
                { .color = menuTheme[menuThemeIndex],
                .tooltip = widget.widgetTooltip,
                .disabled = disabledValue,
                .disabledTooltip = disabledTooltip,
                .defaultIndex = static_cast<uint32_t>(std::get<int32_t>(widget.widgetOptions.defaultVariant)),
                .labelPosition = widget.widgetOptions.labelPosition == UIWidgets::LabelPosition::None
                ? UIWidgets::LabelPosition::Above
                : widget.widgetOptions.labelPosition })) {
                if (widget.widgetCallback != nullptr) {
                    widget.widgetCallback(widget);
                }
            }
            break;
        case WIDGET_SLIDER_INT: {
                int32_t* pointer = std::get<int32_t*>(widget.widgetOptions.valuePointer);
                if (pointer == nullptr) {
                    SPDLOG_ERROR("int32 Slider Widget requires a value pointer, currently nullptr");
                    assert(false);
                    return;
                }
                if (UIWidgets::SliderInt(widget.widgetName.c_str(), pointer,
                    std::get<int32_t>(widget.widgetOptions.min),
                    std::get<int32_t>(widget.widgetOptions.max),
                    {
                        .color = menuTheme[menuThemeIndex],
                        .tooltip = widget.widgetTooltip,
                        .disabled = disabledValue,
                        .disabledTooltip = disabledTooltip,
                    })) {
                    if (widget.widgetCallback != nullptr) {
                        widget.widgetCallback(widget);
                    }
                };
        } break;
        case WIDGET_SLIDER_FLOAT: {
            float* pointer = std::get<float*>(widget.widgetOptions.valuePointer);

            if (pointer == nullptr) {
                SPDLOG_ERROR("float Slider Widget requires a value pointer, currently nullptr");
                assert(false);
                return;
            }
            if (UIWidgets::SliderFloat(widget.widgetName.c_str(), pointer,
                std::get<float>(widget.widgetOptions.min),
                std::get<float>(widget.widgetOptions.max),
                { .color = menuTheme[menuThemeIndex],
                .tooltip = widget.widgetTooltip,
                .disabled = disabledValue,
                .disabledTooltip = disabledTooltip,
                .showButtons = widget.widgetOptions.showButtons,
                .format = widget.widgetOptions.format,
                .step = widget.widgetOptions.step,
                .isPercentage = widget.widgetOptions.isPercentage })) {
                if (widget.widgetCallback != nullptr) {
                    widget.widgetCallback(widget);
                }
            }
        } break;
        case WIDGET_CVAR_SLIDER_INT:
            if (UIWidgets::CVarSliderInt(widget.widgetName.c_str(), widget.widgetCVar,
                std::get<int32_t>(widget.widgetOptions.min),
                std::get<int32_t>(widget.widgetOptions.max),
                std::get<int32_t>(widget.widgetOptions.defaultVariant),
                {
                    .color = menuTheme[menuThemeIndex],
                    .tooltip = widget.widgetTooltip,
                    .disabled = disabledValue,
                    .disabledTooltip = disabledTooltip,
                })) {
                if (widget.widgetCallback != nullptr) {
                    widget.widgetCallback(widget);
                }
            };
            break;
        case WIDGET_CVAR_SLIDER_FLOAT: {
                if (UIWidgets::CVarSliderFloat(
                    widget.widgetName.c_str(), widget.widgetCVar, std::get<float>(widget.widgetOptions.min),
                    std::get<float>(widget.widgetOptions.max), std::get<float>(widget.widgetOptions.defaultVariant),
                    { .color = menuTheme[menuThemeIndex],
                    .tooltip = widget.widgetTooltip,
                    .disabled = disabledValue,
                    .disabledTooltip = disabledTooltip,
                    .showButtons = widget.widgetOptions.showButtons,
                    .format = widget.widgetOptions.format,
                    .step = widget.widgetOptions.step,
                    .isPercentage = widget.widgetOptions.isPercentage })) {
                    if (widget.widgetCallback != nullptr) {
                        widget.widgetCallback(widget);
                    }
                }
        } break;
        case WIDGET_BUTTON:
            if (UIWidgets::Button(widget.widgetName.c_str(),
                { menuTheme[menuThemeIndex], widget.widgetOptions.size, widget.widgetTooltip,
                disabledValue, disabledTooltip })) {
                if (widget.widgetCallback != nullptr) {
                    widget.widgetCallback(widget);
                }
            }
            break;
        case WIDGET_WINDOW_BUTTON: {
                if (widget.widgetOptions.windowName == nullptr || widget.widgetOptions.windowName[0] == '\0') {
                    std::string msg =
                        fmt::format("Error drawing window contents for {}: windowName not defined", widget.widgetName);
                    SPDLOG_ERROR(msg.c_str());
                    break;
                }
                auto window =
                    Ship::Context::GetInstance()->GetWindow()->GetGui()->GetGuiWindow(widget.widgetOptions.windowName);
                if (!window) {
                    std::string msg = fmt::format("Error drawing window contents: windowName {} does not exist",
                        widget.widgetOptions.windowName);
                    SPDLOG_ERROR(msg.c_str());
                    break;
                }
                UIWidgets::WindowButton(widget.widgetName.c_str(), widget.widgetCVar, window,
                    { .size = widget.widgetOptions.size, .tooltip = widget.widgetTooltip });
                if (!window->IsVisible()) {
                    window->DrawElement();
                }
        } break;
        case WIDGET_SEARCH: {
            if (ImGui::Button("Clear")) {
                menuSearch.Clear();
            }
            ImGui::SameLine();
            if (CVarGetInteger("gSettings.SearchAutofocus", 0) &&
                ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows) && !ImGui::IsAnyItemActive() &&
                !ImGui::IsMouseClicked(0)) {
                ImGui::SetKeyboardFocusHere(0);
            }
            menuSearch.Draw();
            std::string menuSearchText(menuSearch.InputBuf);

            if (menuSearchText == "") {
                ImGui::Text("Start typing to see results.");
                return;
            }
            ImGui::BeginChild("Search Results");
            for (auto& [menuLabel, menuSidebar, cvar] : menuEntries) {
                for (auto& sidebar : menuSidebar) {
                    for (auto& widgets : sidebar.columnWidgets) {
                        int column = 1;
                        for (auto& info : widgets) {
                            if (info.widgetType == WIDGET_SEARCH || info.widgetType == WIDGET_SEPARATOR ||
                                info.widgetType == WIDGET_SEPARATOR_TEXT || info.isHidden) {
                                continue;
                            }
                            std::string widgetStr =
                                std::string(info.widgetName) +
                                std::string(info.widgetTooltip != NULL ? info.widgetTooltip : "");
                            std::transform(menuSearchText.begin(), menuSearchText.end(), menuSearchText.begin(),
                                ::tolower);
                            menuSearchText.erase(std::remove(menuSearchText.begin(), menuSearchText.end(), ' '),
                                menuSearchText.end());
                            std::transform(widgetStr.begin(), widgetStr.end(), widgetStr.begin(), ::tolower);
                            widgetStr.erase(std::remove(widgetStr.begin(), widgetStr.end(), ' '), widgetStr.end());
                            if (widgetStr.find(menuSearchText) != std::string::npos) {
                                SearchMenuGetItem(info);
                                ImGui::PushStyleColor(ImGuiCol_Text, UIWidgets::Colors::Gray);
                                std::string origin =
                                    fmt::format("  ({} -> {}, Clmn {})", menuLabel, sidebar.label, column);
                                ImGui::Text("%s", origin.c_str());
                                ImGui::PopStyleColor();
                            }
                        }
                        column++;
                    }
                }
            }
            ImGui::EndChild();
        } break;
        default:
            break;
        }
        if (widget.postFunc != nullptr) {
            widget.postFunc(widget);
        }
    } catch (const std::bad_variant_access& e) {
        SPDLOG_ERROR("Failed to draw menu item \"{}\" due to: {}", widget.widgetName, e.what());
        assert(false);
    }
}

void Menu::Draw() {
    if (!IsVisible()) {
        return;
    }
    DrawElement();
    // Sync up the IsVisible flag if it was changed by ImGui
    SyncVisibilityConsoleVariable();
}

void Menu::DrawElement() {
    for (auto& [reason, info] : disabledMap) {
        info.active = info.evaluation(info);
    }
    menuThemeIndex = static_cast<ColorOption>(CVarGetInteger("gSettings.MenuTheme", 3));

    windowHeight = ImGui::GetMainViewport()->WorkSize.y;
    windowWidth = ImGui::GetMainViewport()->WorkSize.x;
    auto windowFlags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings;
    bool popout = CVarGetInteger("gSettings.Menu.Popout", 0) && allowPopout;
    if (popout) {
        windowFlags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoDocking;
    }
    if (popout != popped) {
        if (popout) {
            windowHeight = poppedSize.y;
            windowWidth = poppedSize.x;
            ImGui::SetNextWindowSize({ static_cast<float>(windowWidth), static_cast<float>(windowHeight) },
                                     ImGuiCond_Always);
            ImGui::SetNextWindowPos(poppedPos, ImGuiCond_Always);
        } else if (popped) {
            CVarSetFloat("gSettings.Menu.PoppedWidth", poppedSize.x);
            CVarSetFloat("gSettings.Menu.PoppedHeight", poppedSize.y);
            CVarSave();
        }
    }
    popped = popout;
    auto windowCond = ImGuiCond_Always;
    if (!popout) {
        ImGui::SetNextWindowSize({ static_cast<float>(windowWidth), static_cast<float>(windowHeight) }, windowCond);
        ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), windowCond, { 0.5f, 0.5f });
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    }
    if (!ImGui::Begin("Main Menu", NULL, windowFlags)) {
        if (!popout) {
            ImGui::PopStyleVar();
        }
        ImGui::End();
        return;
    }
    if (popped != popout) {
        if (!popout) {
            ImGui::PopStyleVar();
        }
        CVarSetInteger("gSettings.Menu.Popout", popped);
        CVarSetFloat("gSettings.Menu.PoppedWidth", poppedSize.x);
        CVarSetFloat("gSettings.Menu.PoppedHeight", poppedSize.y);
        CVarSetFloat("gSettings.Menu.PoppedPos.x", poppedSize.x);
        CVarSetFloat("gSettings.Menu.PoppedPos.y", poppedSize.y);
        CVarSave();
        ImGui::End();
        return;
    }
    ImGui::PushFont(OTRGlobals::Instance->fontStandardLargest);
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = g.CurrentWindow;
    ImGuiStyle& style = ImGui::GetStyle();
    windowHeight = window->WorkRect.GetHeight();
    windowWidth = window->WorkRect.GetWidth();

    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10.0f, 8.0f));
    auto sectionCount = menuEntries.size();
    const char* headerCvar = "gSettings.Menu.SelectedHeader";
    uint8_t headerIndex = CVarGetInteger(headerCvar, 0);
    ImVec2 pos = window->DC.CursorPos;
    float centerX = pos.x + windowWidth / 2 - (style.ItemSpacing.x * (sectionCount + 1));
    std::vector<ImVec2> headerSizes;
    float headerWidth = 200.0f + style.ItemSpacing.x;
    for (int i = 0; i < sectionCount; i++) {
        ImVec2 size = ImGui::CalcTextSize(menuEntries.at(i).label.c_str());
        headerSizes.push_back(size);
        headerWidth += size.x + style.FramePadding.x * 2;
        if (i + 1 < sectionCount) {
            headerWidth += style.ItemSpacing.x;
        }
    }
    ImVec2 menuSize = { std::fminf(1280, windowWidth), std::fminf(800, windowHeight) };
    pos += window->WorkRect.GetSize() / 2 - menuSize / 2;
    ImGui::SetNextWindowPos(pos);
    ImGui::BeginChild("Menu Block", menuSize,
                      ImGuiChildFlags_AutoResizeX | ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_AlwaysAutoResize,
                      ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar);

    std::vector<SidebarEntry> sidebar;
    float headerHeight = headerSizes.at(0).y + style.FramePadding.y * 2;
    ImVec2 buttonSize = ImGui::CalcTextSize(ICON_FA_TIMES_CIRCLE) + style.FramePadding * 2;
    bool scrollbar = false;
    if (headerWidth > menuSize.x - buttonSize.x * 3 - style.ItemSpacing.x * 3) {
        headerHeight += style.ScrollbarSize;
        scrollbar = true;
    }
    if (UIWidgets::Button(ICON_FA_TIMES_CIRCLE, { .size = UIWidgets::Sizes::Inline, .tooltip = "Close Menu (Esc)" })) {
        ToggleVisibility();

        // Update gamepad navigation after close based on if other menus are still visible
        auto mImGuiIo = &ImGui::GetIO();
        if (CVarGetInteger(CVAR_IMGUI_CONTROLLER_NAV, 0) &&
            Ship::Context::GetInstance()->GetWindow()->GetGui()->GetMenuOrMenubarVisible()) {
            mImGuiIo->ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
        } else {
            mImGuiIo->ConfigFlags &= ~ImGuiConfigFlags_NavEnableGamepad;
        }
    }
    ImGui::SameLine();
    ImGui::SetNextWindowSizeConstraints({ 0, headerHeight }, { headerWidth, headerHeight });
    ImVec2 headerSelSize = { menuSize.x - buttonSize.x * 3 - style.ItemSpacing.x * 3, headerHeight };
    if (scrollbar) {
        headerSelSize.y += style.ScrollbarSize;
    }
    bool autoFocus = CVarGetInteger("gSettings.SearchAutofocus", 0);
    bool headerSearch = !CVarGetInteger("gSettings.SidebarSearch", 0);
    ImGui::BeginChild("Header Selection", headerSelSize,
                      ImGuiChildFlags_AutoResizeX | ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_AlwaysAutoResize,
                      ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_HorizontalScrollbar);
    for (int i = 0; i < sectionCount; i++) {
        auto entry = menuEntries.at(i);
        uint8_t nextIndex = i;
        UIWidgets::PushStyleButton(menuTheme[menuThemeIndex]);
        if (headerIndex != i) {
            ImGui::PushStyleColor(ImGuiCol_Button, { 0, 0, 0, 0 });
        }
        if (ModernMenuHeaderEntry(entry.label)) {
            if (autoFocus) {
                menuSearch.Clear();
            }
            CVarSetInteger(headerCvar, i);
            CVarSave();
            nextIndex = i;
        }
        if (headerIndex != i) {
            ImGui::PopStyleColor();
        }
        UIWidgets::PopStyleButton();
        if (headerIndex == i) {
            sidebar = entry.sidebarEntries;
        }
        if (i + 1 < sectionCount) {
            ImGui::SameLine();
        }
        if (nextIndex != i) {
            headerIndex = nextIndex;
        }
    }
    std::string menuSearchText = "";
    if (headerSearch) {
        ImGui::SameLine();
        if (autoFocus && ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows) && !ImGui::IsAnyItemActive() &&
            !ImGui::IsMouseClicked(0)) {
            ImGui::SetKeyboardFocusHere(0);
        }
        ImGui::PushStyleColor(ImGuiCol_FrameBg, { 0, 0, 0, 0 });
        menuSearch.Draw("##search", 200.0f);
        menuSearchText = menuSearch.InputBuf;
        menuSearchText.erase(std::remove(menuSearchText.begin(), menuSearchText.end(), ' '), menuSearchText.end());
        if (menuSearchText.length() < 1) {
            ImGui::SameLine(headerWidth - 200.0f + style.ItemSpacing.x);
            ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.4f), "Search...");
        }
        ImGui::PopStyleColor();
    }
    ImGui::EndChild();
    ImGui::SameLine(menuSize.x - (buttonSize.x * 2) - style.ItemSpacing.x);
    if (UIWidgets::Button(ICON_FA_UNDO, { .color = UIWidgets::Colors::Red,
                                          .size = UIWidgets::Sizes::Inline,
                                          .tooltip = "Reset"
#ifdef __APPLE__
                                                     " (Command-R)"
#elif !defined(__SWITCH__) && !defined(__WIIU__)
                                                     " (Ctrl+R)"
#else
                                                     ""
#endif
                                        })) {
        std::reinterpret_pointer_cast<Ship::ConsoleWindow>(
            Ship::Context::GetInstance()->GetWindow()->GetGui()->GetGuiWindow("Console"))
            ->Dispatch("reset");
    }
    ImGui::SameLine();
    if (UIWidgets::Button(
            ICON_FA_POWER_OFF,
            { .color = UIWidgets::Colors::Red, .size = UIWidgets::Sizes::Inline, .tooltip = "Quit 2S2H" })) {
        if (!popped) {
            ToggleVisibility();
        }
        Ship::Context::GetInstance()->GetWindow()->Close();
    }
    ImGui::PopStyleVar();

    pos.y += headerHeight + style.ItemSpacing.y;
    pos.x = centerX - menuSize.x / 2 + (style.ItemSpacing.x * (sectionCount + 1));
    window->DrawList->AddRectFilled(pos, pos + ImVec2{ menuSize.x, 4 }, ImGui::GetColorU32({ 255, 255, 255, 255 }),
                                    true, style.WindowRounding);
    pos.y += style.ItemSpacing.y;
    float sectionHeight = menuSize.y - headerHeight - 4 - style.ItemSpacing.y * 2;
    float columnHeight = sectionHeight - style.ItemSpacing.y * 4;
    ImGui::SetNextWindowPos(pos + style.ItemSpacing * 2);
    float sidebarWidth = 200 - style.ItemSpacing.x;

    const char* sidebarCvar = menuEntries.at(headerIndex).sidebarCvar;

    uint8_t sectionIndex = CVarGetInteger(sidebarCvar, 0);
    if (sectionIndex > sidebar.size() - 1)
        sectionIndex = sidebar.size() - 1;
    if (sectionIndex < 0)
        sectionIndex = 0;
    float sectionCenterX = pos.x + (sidebarWidth / 2);
    float topY = pos.y;
    ImGui::SetNextWindowSizeConstraints({ sidebarWidth, 0 }, { sidebarWidth, columnHeight });
    ImGui::BeginChild((menuEntries.at(headerIndex).label + " Section").c_str(), { sidebarWidth, columnHeight * 3 },
                      ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_AlwaysAutoResize, ImGuiWindowFlags_NoTitleBar);
    for (size_t i = 0; i < sidebar.size(); i++) {
        auto sidebarEntry = sidebar.at(i);
        uint8_t nextIndex = i;
        UIWidgets::PushStyleButton(menuTheme[menuThemeIndex]);
        if (sectionIndex != i) {
            ImGui::PushStyleColor(ImGuiCol_Button, { 0, 0, 0, 0 });
        }
        if (ModernMenuSidebarEntry(sidebarEntry.label)) {
            if (autoFocus) {
                menuSearch.Clear();
            }
            CVarSetInteger(sidebarCvar, i);
            CVarSave();
            nextIndex = i;
        }
        if (sectionIndex != i) {
            ImGui::PopStyleColor();
        }
        UIWidgets::PopStyleButton();
        if (nextIndex != i) {
            sectionIndex = i;
        }
    }
    ImGui::EndChild();

    ImGui::PushFont(OTRGlobals::Instance->fontMonoLarger);
    pos = ImVec2{ sectionCenterX + (sidebarWidth / 2), topY } + style.ItemSpacing * 2;
    window->DrawList->AddRectFilled(pos, pos + ImVec2{ 4, sectionHeight - style.FramePadding.y * 2 },
                                    ImGui::GetColorU32({ 255, 255, 255, 255 }), true, style.WindowRounding);
    pos.x += 4 + style.ItemSpacing.x;
    ImGui::SetNextWindowPos(pos + style.ItemSpacing);
    float sectionWidth = menuSize.x - sidebarWidth - 4 - style.ItemSpacing.x * 4;
    std::string sectionMenuId = sidebar.at(sectionIndex).label + " Settings";
    int columns = sidebar.at(sectionIndex).columnCount;
    size_t columnFuncs = sidebar.at(sectionIndex).columnWidgets.size();
    if (windowWidth < 800) {
        columns = 1;
    }
    float columnWidth = (sectionWidth - style.ItemSpacing.x * columns) / columns;
    bool useColumns = columns > 1;
    if (!useColumns || (headerSearch && menuSearchText.length() > 0)) {
        ImGui::SameLine();
        ImGui::SetNextWindowSizeConstraints({ sectionWidth, 0 }, { sectionWidth, columnHeight });
        ImGui::BeginChild(sectionMenuId.c_str(), { sectionWidth, windowHeight * 4 }, ImGuiChildFlags_AutoResizeY,
                          ImGuiWindowFlags_NoTitleBar);
    }
    if (headerSearch && menuSearchText.length() > 0) {
        ImGui::BeginChild("Search Results");
        int searchCount = 0;
        for (auto& [menuLabel, menuSidebar, cvar] : menuEntries) {
            for (auto& sidebar : menuSidebar) {
                for (auto& widgets : sidebar.columnWidgets) {
                    int column = 1;
                    for (auto& info : widgets) {
                        if (info.widgetType == WIDGET_SEPARATOR || info.widgetType == WIDGET_SEPARATOR_TEXT ||
                            info.isHidden) {
                            continue;
                        }
                        std::string widgetStr = std::string(info.widgetName) +
                                                std::string(info.widgetTooltip != NULL ? info.widgetTooltip : "");
                        std::transform(menuSearchText.begin(), menuSearchText.end(), menuSearchText.begin(), ::tolower);
                        std::transform(widgetStr.begin(), widgetStr.end(), widgetStr.begin(), ::tolower);
                        widgetStr.erase(std::remove(widgetStr.begin(), widgetStr.end(), ' '), widgetStr.end());
                        if (widgetStr.find(menuSearchText) != std::string::npos) {
                            SearchMenuGetItem(info);
                            searchCount++;
                            ImGui::PushStyleColor(ImGuiCol_Text, UIWidgets::Colors::Gray);
                            std::string origin = fmt::format("  ({} -> {}, Clmn {})", menuLabel, sidebar.label, column);
                            ImGui::Text("%s", origin.c_str());
                            ImGui::PopStyleColor();
                        }
                    }
                    column++;
                }
            }
        }

        if (searchCount == 0) {
            ImGui::SetCursorPosX((ImGui::GetWindowWidth() - ImGui::CalcTextSize("No results found").x) / 2);
            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10.0f);
            ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.4f), "No results found");
            ImGui::SetCursorPosX((ImGui::GetWindowWidth() - ImGui::CalcTextSize("Clear Search").x) / 2 - 10.0f);
            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10.0f);
            if (UIWidgets::Button("Clear Search", { .size = UIWidgets::Sizes::Inline })) {
                menuSearch.Clear();
            }
        }

        ImGui::EndChild();
    } else {
        for (int i = 0; i < columnFuncs; i++) {
            std::string sectionId = fmt::format("{} Column {}", sectionMenuId, i);
            if (useColumns) {
                ImGui::SetNextWindowSizeConstraints({ columnWidth, 0 }, { columnWidth, columnHeight });
                ImGui::BeginChild(sectionId.c_str(), { columnWidth, windowHeight * 4 }, ImGuiChildFlags_AutoResizeY,
                                  ImGuiWindowFlags_NoTitleBar);
            }
            for (auto& entry : sidebar.at(sectionIndex).columnWidgets.at(i)) {
                SearchMenuGetItem(entry);
            }
            if (useColumns) {
                ImGui::EndChild();
            }
            if (i < columns - 1) {
                ImGui::SameLine();
            }
        }
    }
    if (!useColumns || menuSearchText.length() > 0) {
        ImGui::EndChild();
    }
    ImGui::PopFont();
    ImGui::PopFont();

    if (!popout) {
        ImGui::PopStyleVar();
    }
    ImGui::EndChild();
    if (popout) {
        poppedSize = ImGui::GetWindowSize();
        poppedPos = ImGui::GetWindowPos();
    }
    ImGui::End();
}
} // namespace BenGui
