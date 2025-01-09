#include "BenMenu.h"
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
#include "StringHelper.h"
#include <fmt/format.h>
#include "variables.h"
#include <variant>
#include <tuple>

extern "C" {
#include "z64.h"
#include "functions.h"
extern PlayState* gPlayState;
}
extern std::unordered_map<s16, const char*> warpPointSceneList;
extern void Warp();

namespace BenGui {
extern std::shared_ptr<BenMenu> mBenMenu;
void FreeLookPitchMinMax() {
    f32 maxY = CVarGetFloat("gEnhancements.Camera.FreeLook.MaxPitch", 72.0f);
    f32 minY = CVarGetFloat("gEnhancements.Camera.FreeLook.MinPitch", -49.0f);
    CVarSetFloat("gEnhancements.Camera.FreeLook.MaxPitch", std::max(maxY, minY));
    CVarSetFloat("gEnhancements.Camera.FreeLook.MinPitch", std::min(maxY, minY));
}

widgetInfo& BenMenu::AddWidget(WidgetPath& pathInfo, std::string widgetName, WidgetType widgetType) {
    std::unordered_map<std::string, SidebarEntry>& sidebar =
        (pathInfo.sectionName == "Settings"
             ? settingsSidebar
             : (pathInfo.sectionName == "Enhancements" ? enhancementsSidebar : devToolsSidebar));
    uint8_t column = pathInfo.column - 1;
    if (sidebar.contains(pathInfo.sidebarName)) {
        while (sidebar.at(pathInfo.sidebarName).columnWidgets.size() < column + 1) {
            sidebar.at(pathInfo.sidebarName).columnWidgets.push_back({});
        }
    }
    SidebarEntry& entry = sidebar.at(pathInfo.sidebarName);
    entry.columnWidgets.at(column).push_back({ .name = widgetName, .type = widgetType });
    widgetInfo& widget = entry.columnWidgets.at(column).back();
    switch (widgetType) {
        case WIDGET_CHECKBOX:
        case WIDGET_CVAR_CHECKBOX:
            widget.options = UIWidgets::CheckboxOptions{};
            break;
        case WIDGET_SLIDER_FLOAT:
        case WIDGET_CVAR_SLIDER_FLOAT:
            widget.options = UIWidgets::FloatSliderOptions{};
            break;
        case WIDGET_SLIDER_INT:
        case WIDGET_CVAR_SLIDER_INT:
            widget.options = UIWidgets::IntSliderOptions{};
            break;
        case WIDGET_COMBOBOX:
        case WIDGET_CVAR_COMBOBOX:
        case WIDGET_AUDIO_BACKEND:
        case WIDGET_VIDEO_BACKEND:
            widget.options = UIWidgets::ComboboxOptions{};
            break;
        case WIDGET_BUTTON:
            widget.options = UIWidgets::ButtonOptions{};
            break;
        case WIDGET_WINDOW_BUTTON:
            widget.options = UIWidgets::ButtonOptions{ .size = UIWidgets::Sizes::Inline };
            break;
        case WIDGET_COLOR_24:
        case WIDGET_COLOR_32:
            break;
        case WIDGET_SEARCH:
        case WIDGET_SEPARATOR:
        case WIDGET_SEPARATOR_TEXT:
        case WIDGET_TEXT:
        default:
            widget.options = UIWidgets::TextOptions{};
    }
    return widget;
}

void BenMenu::AddSettings() {

    // General Settings
    WidgetPath path = { "Settings", "General", 1 };
    settingsSidebar.emplace("General", SidebarEntry{ .columnCount = 3 });
    settingsOrder.push_back("General");
    AddWidget(path, "Menu Theme", WIDGET_CVAR_COMBOBOX)
        .CVar("gSettings.MenuTheme")
        .Tooltip("Changes the Theme of the Menu Widgets.")
        .Options(UIWidgets::ComboboxOptions{ .defaultIndex = UIWidgets::Color::DarkBlue })
        .ComboMap(menuThemeOptions);
#if not defined(__SWITCH__) and not defined(__WIIU__)
    AddWidget(path, "Menu Controller Navigation", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_IMGUI_CONTROLLER_NAV)
        .Tooltip("Allows controller navigation of the 2Ship menu (Settings, Enhancements,...)\nCAUTION: "
                 "This will disable game inputs while the menu is visible.\n\nD-pad to move between "
                 "items, A to select, B to move up in scope.");
    AddWidget(path, "Cursor Always Visible", WIDGET_CVAR_CHECKBOX)
        .CVar("gSettings.CursorVisibility")
        .Tooltip("Makes the cursor always visible, even in full screen.")
        .Callback([](widgetInfo& info) {
            Ship::Context::GetInstance()->GetWindow()->SetForceCursorVisibility(
                CVarGetInteger("gSettings.CursorVisibility", 0));
        });
#endif
    AddWidget(path, "Search In Sidebar", WIDGET_CVAR_CHECKBOX)
        .CVar("gSettings.SidebarSearch")
        .Tooltip("Displays the Search menu as a sidebar entry in Settings instead of in the header.")
        .Callback([](widgetInfo& info) {
            if (CVarGetInteger("gSettings.SidebarSearch", 0)) {
                mBenMenu->InsertSidebarSearch();
            } else {
                mBenMenu->RemoveSidebarSearch();
            }
        });
    AddWidget(path, "Search Input Autofocus", WIDGET_CVAR_CHECKBOX)
        .CVar("gSettings.SearchAutofocus")
        .Tooltip("Search input box gets autofocus when visible. Does not affect using other widgets.");
    AddWidget(path, "Alt Assets Tab hotkey", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Mods.AlternateAssetsHotkey")
        .Tooltip("Allows pressing the Tab key to toggle alternate assets")
        .Options(UIWidgets::CheckboxOptions{ .defaultValue = true });
    AddWidget(path, "Open App Files Folder", WIDGET_BUTTON)
        .Tooltip("Opens the folder that contains the save and mods folders, etc.")
        .Callback([](widgetInfo& info) {
            std::string filesPath = Ship::Context::GetInstance()->GetAppDirectoryPath();
            SDL_OpenURL(std::string("file:///" + std::filesystem::absolute(filesPath).string()).c_str());
        });
    // Audio Settings
    path.sidebarName = "Audio";
    settingsSidebar.emplace("Audio", SidebarEntry{ .columnCount = 3 });
    settingsOrder.push_back("Audio");
    AddWidget(path, "Master Volume: %.0f%%", WIDGET_CVAR_SLIDER_FLOAT)
        .CVar("gSettings.Audio.MasterVolume")
        .Tooltip("Adjust overall sound volume.")
        .Options(UIWidgets::FloatSliderOptions{
            .showButtons = false, .format = "", .min = 0.0f, .max = 1.0f, .defaultValue = 1.0f, .isPercentage = true });
    AddWidget(path, "Main Music Volume: %.0f%%", WIDGET_CVAR_SLIDER_FLOAT)
        .CVar("gSettings.Audio.MainMusicVolume")
        .Tooltip("Adjust the Background Music volume.")
        .Options(UIWidgets::FloatSliderOptions{
            .showButtons = false,
            .format = "",
            .min = 0.0f,
            .max = 1.0f,
            .defaultValue = 1.0f,
            .isPercentage = true,
        })
        .Callback([](widgetInfo& info) {
            AudioSeq_SetPortVolumeScale(SEQ_PLAYER_BGM_MAIN, CVarGetFloat("gSettings.Audio.MainMusicVolume", 1.0f));
        });
    AddWidget(path, "Sub Music Volume: %.0f%%", WIDGET_CVAR_SLIDER_FLOAT)
        .CVar("gSettings.Audio.SubMusicVolume")
        .Tooltip("Adjust the Sub Music volume.")
        .Options(UIWidgets::FloatSliderOptions{
            .showButtons = false, .format = "", .min = 0.0f, .max = 1.0f, .defaultValue = 1.0f, .isPercentage = true })
        .Callback([](widgetInfo& info) {
            AudioSeq_SetPortVolumeScale(SEQ_PLAYER_BGM_SUB, CVarGetFloat("gSettings.Audio.SubMusicVolume", 1.0f));
        });
    AddWidget(path, "Sound Effects Volume: %.0f%%", WIDGET_CVAR_SLIDER_FLOAT)
        .CVar("gSettings.Audio.SoundEffectsVolume")
        .Tooltip("Adjust the Sound Effects volume.")
        .Options(UIWidgets::FloatSliderOptions{
            .showButtons = false, .format = "", .min = 0.0f, .max = 1.0f, .defaultValue = 1.0f, .isPercentage = true })
        .Callback([](widgetInfo& info) {
            AudioSeq_SetPortVolumeScale(SEQ_PLAYER_SFX, CVarGetFloat("gSettings.Audio.SoundEffectsVolume", 1.0f));
        });
    AddWidget(path, "Fanfare Volume: %.0f%%", WIDGET_CVAR_SLIDER_FLOAT)
        .CVar("gSettings.Audio.FanfareVolume")
        .Tooltip("Adjust the Fanfare volume.")
        .Options(UIWidgets::FloatSliderOptions{
            .showButtons = false, .format = "", .min = 0.0f, .max = 1.0f, .defaultValue = 1.0f, .isPercentage = true })
        .Callback([](widgetInfo& info) {
            AudioSeq_SetPortVolumeScale(SEQ_PLAYER_FANFARE, CVarGetFloat("gSettings.Audio.FanfareVolume", 1.0f));
        });
    AddWidget(path, "Ambience Volume: %.0f%%", WIDGET_CVAR_SLIDER_FLOAT)
        .CVar("gSettings.Audio.AmbienceVolume")
        .Tooltip("Adjust the Ambient Sound volume.")
        .Options(UIWidgets::FloatSliderOptions{
            .showButtons = false, .format = "", .min = 0.0f, .max = 1.0f, .defaultValue = 1.0f, .isPercentage = true })
        .Callback([](widgetInfo& info) {
            AudioSeq_SetPortVolumeScale(SEQ_PLAYER_AMBIENCE, CVarGetFloat("gSettings.Audio.AmbienceVolume", 1.0f));
        });
    AddWidget(path, "Audio API", WIDGET_AUDIO_BACKEND)
        .Tooltip("Sets the audio API used by the game. Requires a relaunch to take effect.");
    //    // Graphics Settings
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
    path.sidebarName = "Graphics";
    settingsSidebar.emplace("Graphics", SidebarEntry{ .columnCount = 3 });
    settingsOrder.push_back("Graphics");
    AddWidget(path, "Toggle Fullscreen", WIDGET_CVAR_CHECKBOX)
        .CVar("gSettings.Fullscreen")
        .Tooltip("Toggles Fullscreen On/Off.")
        .Callback([](widgetInfo& info) { Ship::Context::GetInstance()->GetWindow()->ToggleFullscreen(); });
#ifndef __APPLE__
    AddWidget(path, "Internal Resolution: %.0f%%", WIDGET_CVAR_SLIDER_FLOAT)
        .CVar(CVAR_INTERNAL_RESOLUTION)
        .Tooltip("Multiplies your output resolution by the value inputted, as a more intensive but effective "
                 "form of anti-aliasing.")
        .Options(UIWidgets::FloatSliderOptions{
            .showButtons = false, .format = "", .min = 0.5f, .max = 2.0f, .defaultValue = 1.0f, .isPercentage = true })
        .Callback([](widgetInfo& info) {
            Ship::Context::GetInstance()->GetWindow()->SetResolutionMultiplier(
                CVarGetFloat(CVAR_INTERNAL_RESOLUTION, 1));
        });
#endif
#ifndef __WIIU__
    AddWidget(path, "Anti-aliasing (MSAA): %d", WIDGET_CVAR_SLIDER_INT)
        .CVar(CVAR_MSAA_VALUE)
        .Tooltip("Activates MSAA (multi-sample anti-aliasing) from 2x up to 8x, to smooth the edges of rendered "
                 "geometry.\n"
                 "Higher sample count will result in smoother edges on models, but may reduce performance.")
        .Options(UIWidgets::IntSliderOptions{ .min = 1, .max = 8, .defaultValue = 1 })
        .Callback([](widgetInfo& info) {
            Ship::Context::GetInstance()->GetWindow()->SetMsaaLevel(CVarGetInteger(CVAR_MSAA_VALUE, 1));
        });
#endif

    AddWidget(path, "Current FPS: %d", WIDGET_CVAR_SLIDER_INT)
        .CVar("gInterpolationFPS")
        .Tooltip(tooltip)
        .Options(UIWidgets::IntSliderOptions{ .min = 20, .max = maxFps, .defaultValue = 20 })
        .Callback([](widgetInfo& info) {
            int32_t defaultValue = std::get<UIWidgets::IntSliderOptions>(info.options).defaultValue;
            if (CVarGetInteger(info.cVar, defaultValue) == defaultValue) {
                info.name = "Current FPS: Original (%d)";
            } else {
                info.name = "Current FPS: %d";
            }
        })
        .PreFunc([](widgetInfo& info) {
            if (mBenMenu->disabledMap.at(DISABLE_FOR_MATCH_REFRESH_RATE_ON).active)
                info.activeDisables.push_back(DISABLE_FOR_MATCH_REFRESH_RATE_ON);
        });
    AddWidget(path, "Match Refresh Rate", WIDGET_BUTTON)
        .Tooltip("Matches interpolation value to the current game's window refresh rate.")
        .Callback([](widgetInfo& info) {
            int hz = Ship::Context::GetInstance()->GetWindow()->GetCurrentRefreshRate();
            if (hz >= 20 && hz <= 360) {
                CVarSetInteger("gInterpolationFPS", hz);
                Ship::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesOnNextTick();
            }
        })
        .PreFunc([](widgetInfo& info) { info.isHidden = mBenMenu->disabledMap.at(DISABLE_FOR_NOT_DIRECTX).active; });
    AddWidget(path, "Match Refresh Rate", WIDGET_CVAR_CHECKBOX)
        .CVar("gMatchRefreshRate")
        .Tooltip("Matches interpolation value to the current game's window refresh rate.")
        .PreFunc([](widgetInfo& info) { info.isHidden = mBenMenu->disabledMap.at(DISABLE_FOR_DIRECTX).active; });
    AddWidget(path, "Jitter fix : >= % d FPS", WIDGET_CVAR_SLIDER_INT)
        .CVar("gExtraLatencyThreshold")
        .Tooltip("When Interpolation FPS setting is at least this threshold, add one frame of input "
                 "lag (e.g. 16.6 ms for 60 FPS) in order to avoid jitter. This setting allows the "
                 "CPU to work on one frame while GPU works on the previous frame.\nThis setting "
                 "should be used when your computer is too slow to do CPU + GPU work in time.")
        .Options(UIWidgets::IntSliderOptions{ .min = 0, .max = 360, .defaultValue = 80 })
        .PreFunc([](widgetInfo& info) { info.isHidden = mBenMenu->disabledMap.at(DISABLE_FOR_NOT_DIRECTX).active; });
    AddWidget(path, "Renderer API (Needs reload)", WIDGET_VIDEO_BACKEND)
        .Tooltip("Sets the renderer API used by the game.");
    AddWidget(path, "Enable Vsync", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_VSYNC_ENABLED)
        .Tooltip("Enables Vsync.")
        .PreFunc([](widgetInfo& info) { info.isHidden = mBenMenu->disabledMap.at(DISABLE_FOR_NO_VSYNC).active; });
    AddWidget(path, "Windowed Fullscreen", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_SDL_WINDOWED_FULLSCREEN)
        .Tooltip("Enables Windowed Fullscreen Mode.")
        .PreFunc([](widgetInfo& info) {
            info.isHidden = mBenMenu->disabledMap.at(DISABLE_FOR_NO_WINDOWED_FULLSCREEN).active;
        });
    AddWidget(path, "Allow multi-windows", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENABLE_MULTI_VIEWPORTS)
        .Tooltip("Allows multiple windows to be opened at once. Requires a reload to take effect.")
        .PreFunc(
            [](widgetInfo& info) { info.isHidden = mBenMenu->disabledMap.at(DISABLE_FOR_NO_MULTI_VIEWPORT).active; });
    AddWidget(path, "Texture Filter (Needs reload)", WIDGET_CVAR_COMBOBOX)
        .CVar(CVAR_TEXTURE_FILTER)
        .Tooltip("Sets the applied Texture Filtering.")
        .ComboMap(textureFilteringMap);

    path.sidebarName = "Input Editor";
    settingsSidebar.emplace("Input Editor", SidebarEntry{ .columnCount = 1 });
    settingsOrder.push_back("Input Editor");
    AddWidget(path, "Popout Input Editor", WIDGET_WINDOW_BUTTON)
        .CVar("gWindows.BenInputEditor")
        .Tooltip("Enables the separate Input Editor window.")
        .Options(UIWidgets::ButtonOptions{ .size = UIWidgets::Sizes::Inline })
        .WindowName("2S2H Input Editor");

    path.sidebarName = "Notifications";
    settingsSidebar.emplace("Notifications", SidebarEntry{ .columnCount = 1 });
    settingsOrder.push_back("Notifications");
    AddWidget(path, "Position", WIDGET_CVAR_COMBOBOX)
        .CVar("gNotifications.Position")
        .Tooltip("Which corner of the screen notifications appear in.")
        .Options(UIWidgets::ComboboxOptions{ .defaultIndex = 3 })
        .ComboMap(notificationPosition);
    AddWidget(path, "Duration: %.0f seconds", WIDGET_CVAR_SLIDER_FLOAT)
        .CVar("gNotifications.Duration")
        .Tooltip("How long notifications are displayed for.")
        .Options(UIWidgets::FloatSliderOptions{
            .format = "%.1f", .step = 0.1f, .min = 3.0f, .max = 30.0f, .defaultValue = 10.0f });
    AddWidget(path, "Background Opacity: %.0f%%", WIDGET_CVAR_SLIDER_FLOAT)
        .CVar("gNotifications.BgOpacity")
        .Tooltip("How opaque the background of notifications is.")
        .Options(UIWidgets::FloatSliderOptions{
            .format = "%.0f%%", .min = 0.0f, .max = 1.0f, .defaultValue = 0.5f, .isPercentage = true });
    AddWidget(path, "Size %.1f", WIDGET_CVAR_SLIDER_FLOAT)
        .CVar("gNotifications.Size")
        .Tooltip("How large notifications are.")
        .Options(UIWidgets::FloatSliderOptions{
            .format = "%.1f", .step = 0.1f, .min = 1.0f, .max = 5.0f, .defaultValue = 1.8f });
    AddWidget(path, "Test Notification", WIDGET_BUTTON)
        .Tooltip("Displays a test notification.")
        .Callback([](widgetInfo& info) {
            Notification::Emit({
                .itemIcon = "__OTR__icon_item_24_static_yar/gQuestIconGoldSkulltulaTex",
                .prefix = "This",
                .message = "is a",
                .suffix = "test.",
            });
        });
}
int32_t motionBlurStrength;

void BenMenu::AddEnhancements() {
    WidgetPath path = { "Enhancements", "Camera", 1 };
    enhancementsSidebar.emplace("Camera", SidebarEntry{ .columnCount = 3 });
    enhancementsOrder.push_back("Camera");
    // Camera Snap Fix
    AddWidget(path, "Fixes", WIDGET_SEPARATOR_TEXT);
    AddWidget(path, "Fix Targeting Camera Snap", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Camera.FixTargettingCameraSnap")
        .Tooltip("Fixes the camera snap that occurs when you are moving and press the targeting button.");
    AddWidget(path, "First Person", WIDGET_SEPARATOR_TEXT);
    AddWidget(path, "Disable Auto-Centering", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Camera.FirstPerson.DisableFirstPersonAutoCenterView")
        .Tooltip("Disables the auto-centering of the camera in first person mode.")
        .PreFunc([](widgetInfo& info) {
            if (mBenMenu->disabledMap.at(DISABLE_FOR_GYRO_ON).active)
                info.activeDisables.push_back(DISABLE_FOR_GYRO_ON);
        });
    AddWidget(path, "Invert X Axis", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Camera.FirstPerson.InvertX")
        .Tooltip("Inverts the X Axis of the Camera in First Person Mode.");
    AddWidget(path, "Invert Y Axis", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Camera.FirstPerson.InvertY")
        .Tooltip("Inverts the Y Axis of the Camera in First Person Mode.")
        .Options(UIWidgets::CheckboxOptions{ .defaultValue = true });
    AddWidget(path, "X Axis Sensitivity: %.0f%%", WIDGET_CVAR_SLIDER_FLOAT)
        .CVar("gEnhancements.Camera.FirstPerson.SensitivityX")
        .Tooltip("Adjusts the Sensitivity of the X Axis in First Person Mode.")
        .Options(UIWidgets::FloatSliderOptions{
            .format = "%.0f%%", .min = 0.01f, .max = 2.0f, .defaultValue = 1.0f, .isPercentage = true });
    AddWidget(path, "Y Axis Sensitivity: %.0f%%", WIDGET_CVAR_SLIDER_FLOAT)
        .CVar("gEnhancements.Camera.FirstPerson.SensitivityY")
        .Tooltip("Adjusts the Sensitivity of the Y Axis in First Person Mode.")
        .Options(UIWidgets::FloatSliderOptions{
            .format = "%.0f%%", .min = 0.01f, .max = 2.0f, .defaultValue = 1.0f, .isPercentage = true });
    AddWidget(path, "Gyro Aiming", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Camera.FirstPerson.GyroEnabled")
        .Tooltip("Enables Gyro Aiming in First Person Mode.");
    AddWidget(path, "Invert Gyro X Axis", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Camera.FirstPerson.GyroInvertX")
        .Tooltip("Inverts the X Axis of the Gyro in First Person Mode.")
        .PreFunc([](widgetInfo& info) { info.isHidden = mBenMenu->disabledMap.at(DISABLE_FOR_GYRO_OFF).active; });
    AddWidget(path, "Invert Gyro Y Axis", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Camera.FirstPerson.GyroInvertY")
        .Tooltip("Inverts the Y Axis of the Gyro in First Person Mode.")
        .PreFunc([](widgetInfo& info) { info.isHidden = mBenMenu->disabledMap.at(DISABLE_FOR_GYRO_OFF).active; });
    AddWidget(path, "Gyro X Axis Sensitivity: %.0f%%", WIDGET_CVAR_SLIDER_FLOAT)
        .CVar("gEnhancements.Camera.FirstPerson.GyroSensitivityX")
        .Tooltip("Adjusts the Sensitivity of the X Axis of the Gyro in First Person Mode.")
        .Options(UIWidgets::FloatSliderOptions{
            .format = "%.0f%%", .min = 0.01f, .max = 2.0f, .defaultValue = 1.0f, .isPercentage = true })
        .PreFunc([](widgetInfo& info) { info.isHidden = mBenMenu->disabledMap.at(DISABLE_FOR_GYRO_OFF).active; });
    AddWidget(path, "Gyro Y Axis Sensitivity: %.0f%%", WIDGET_CVAR_SLIDER_FLOAT)
        .CVar("gEnhancements.Camera.FirstPerson.GyroSensitivityY")
        .Tooltip("Adjusts the Sensitivity of the Y Axis of the Gyro in First Person Mode.")
        .Options(UIWidgets::FloatSliderOptions{
            .format = "%.0f%%", .min = 0.01f, .max = 2.0f, .defaultValue = 1.0f, .isPercentage = true })
        .PreFunc([](widgetInfo& info) { info.isHidden = mBenMenu->disabledMap.at(DISABLE_FOR_GYRO_OFF).active; });
    AddWidget(path, "Right Stick Aiming", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Camera.FirstPerson.RightStickEnabled")
        .Tooltip("Enables Right Stick Aiming in First Person Mode.");
    AddWidget(path, "Move while aiming", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Camera.FirstPerson.MoveInFirstPerson")
        .Tooltip("Allows movement with the left stick while in first person mode.")
        .PreFunc([](widgetInfo& info) {
            if (mBenMenu->disabledMap.at(DISABLE_FOR_RIGHT_STICK_OFF).active)
                info.activeDisables.push_back(DISABLE_FOR_RIGHT_STICK_OFF);
        });
    AddWidget(path, "Invert Right Stick X Axis", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Camera.FirstPerson.RightStickInvertX")
        .Tooltip("Inverts the X Axis of the Right Stick in First Person Mode.")
        .PreFunc(
            [](widgetInfo& info) { info.isHidden = mBenMenu->disabledMap.at(DISABLE_FOR_RIGHT_STICK_OFF).active; });
    AddWidget(path, "Invert Right Stick Y Axis", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Camera.FirstPerson.RightStickInvertY")
        .Tooltip("Inverts the Y Axis of the Right Stick in First Person Mode.")
        .Options(UIWidgets::CheckboxOptions{ .defaultValue = true })
        .PreFunc(
            [](widgetInfo& info) { info.isHidden = mBenMenu->disabledMap.at(DISABLE_FOR_RIGHT_STICK_OFF).active; });
    AddWidget(path, "Right Stick X Axis Sensitivity: %.0f%%", WIDGET_CVAR_SLIDER_FLOAT)
        .CVar("gEnhancements.Camera.FirstPerson.RightStickSensitivityX")
        .Tooltip("Adjusts the Sensitivity of the X Axis of the Right Stick in First Person Mode.")
        .Options(UIWidgets::FloatSliderOptions{
            .format = "%.0f%%", .min = 0.01f, .max = 2.0f, .defaultValue = 1.0f, .isPercentage = true })
        .PreFunc(
            [](widgetInfo& info) { info.isHidden = mBenMenu->disabledMap.at(DISABLE_FOR_RIGHT_STICK_OFF).active; });
    AddWidget(path, "Right Stick Y Axis Sensitivity: %.0f%%", WIDGET_CVAR_SLIDER_FLOAT)
        .CVar("gEnhancements.Camera.FirstPerson.RightStickSensitivityY")
        .Tooltip("Adjusts the Sensitivity of the Y Axis of the Right Stick in First Person Mode.")
        .Options(UIWidgets::FloatSliderOptions{
            .format = "%.0f%%", .min = 0.01f, .max = 2.0f, .defaultValue = 1.0f, .isPercentage = true })
        .PreFunc(
            [](widgetInfo& info) { info.isHidden = mBenMenu->disabledMap.at(DISABLE_FOR_RIGHT_STICK_OFF).active; });

    path.column = 2;
    AddWidget(path, "Cameras", WIDGET_SEPARATOR_TEXT);
    AddWidget(path, "Free Look", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Camera.FreeLook.Enable")
        .Tooltip("Enables free look camera control\nNote: You must remap C buttons off of the right "
                 "stick in the controller config menu, and map the camera stick to the right stick.")
        .PreFunc([](widgetInfo& info) {
            if (mBenMenu->disabledMap.at(DISABLE_FOR_DEBUG_CAM_ON).active)
                info.activeDisables.push_back(DISABLE_FOR_DEBUG_CAM_ON);
        });
    AddWidget(path, "Camera Distance: %d", WIDGET_CVAR_SLIDER_INT)
        .CVar("gEnhancements.Camera.FreeLook.MaxCameraDistance")
        .Tooltip("Maximum Camera Distance for Free Look.")
        .Options(UIWidgets::IntSliderOptions{ .min = 100, .max = 900, .defaultValue = 185 })
        .PreFunc([](widgetInfo& info) { info.isHidden = mBenMenu->disabledMap.at(DISABLE_FOR_FREE_LOOK_OFF).active; });
    AddWidget(path, "Camera Transition Speed: %d", WIDGET_CVAR_SLIDER_INT)
        .CVar("gEnhancements.Camera.FreeLook.TransitionSpeed")
        .Tooltip("Can someone help me?")
        .Options(UIWidgets::IntSliderOptions{ .min = 1, .max = 900, .defaultValue = 25 })
        .PreFunc([](widgetInfo& info) { info.isHidden = mBenMenu->disabledMap.at(DISABLE_FOR_FREE_LOOK_OFF).active; });
    AddWidget(path, "Max Camera Height Angle: %.0f\xC2\xB0", WIDGET_CVAR_SLIDER_FLOAT)
        .CVar("gEnhancements.Camera.FreeLook.MaxPitch")
        .Tooltip("Maximum Height of the Camera.")
        .Options(UIWidgets::FloatSliderOptions{
            .format = "%.0f\xC2\xB0", .min = -89.0f, .max = 89.0f, .defaultValue = 72.0f })
        .Callback([](widgetInfo& info) { FreeLookPitchMinMax(); })
        .PreFunc([](widgetInfo& info) { info.isHidden = mBenMenu->disabledMap.at(DISABLE_FOR_FREE_LOOK_OFF).active; });
    AddWidget(path, "Min Camera Height Angle: %.0f\xC2\xB0", WIDGET_CVAR_SLIDER_FLOAT)
        .CVar("gEnhancements.Camera.FreeLook.MinPitch")
        .Tooltip("Minimum Height of the Camera.")
        .Options(UIWidgets::FloatSliderOptions{
            .format = "%.0f\xC2\xB0", .min = -89.0f, .max = 89.0f, .defaultValue = -49.0f })
        .Callback([](widgetInfo& info) { FreeLookPitchMinMax(); })
        .PreFunc([](widgetInfo& info) { info.isHidden = mBenMenu->disabledMap.at(DISABLE_FOR_FREE_LOOK_OFF).active; });
    AddWidget(path, "Debug Camera", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Camera.DebugCam.Enable")
        .Tooltip("Enables free camera control.")
        .PreFunc([](widgetInfo& info) {
            if (mBenMenu->disabledMap.at(DISABLE_FOR_FREE_LOOK_ON).active) {
                info.activeDisables.push_back(DISABLE_FOR_FREE_LOOK_ON);
            }
        });
    AddWidget(path, "Invert Camera X Axis", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Camera.RightStick.InvertXAxis")
        .Tooltip("Inverts the Camera X Axis")
        .PreFunc([](widgetInfo& info) {
            if (mBenMenu->disabledMap.at(DISABLE_FOR_CAMERAS_OFF).active) {
                info.activeDisables.push_back(DISABLE_FOR_CAMERAS_OFF);
            }
        });
    AddWidget(path, "Invert Camera Y Axis", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Camera.RightStick.InvertYAxis")
        .Tooltip("Inverts the Camera Y Axis")
        .PreFunc([](widgetInfo& info) {
            if (mBenMenu->disabledMap.at(DISABLE_FOR_CAMERAS_OFF).active) {
                info.activeDisables.push_back(DISABLE_FOR_CAMERAS_OFF);
            }
        });
    AddWidget(path, "Third-Person Horizontal Sensitivity: %.0f", WIDGET_CVAR_SLIDER_FLOAT)
        .CVar("gEnhancements.Camera.RightStick.CameraSensitivity.X")
        .Tooltip("Adjust the Sensitivity of the x axis when in Third Person.")
        .Options(UIWidgets::FloatSliderOptions{
            .format = "%.0f%%", .min = 0.01f, .max = 5.0f, .defaultValue = 1.0f, .isPercentage = true })
        .PreFunc([](widgetInfo& info) {
            if (mBenMenu->disabledMap.at(DISABLE_FOR_CAMERAS_OFF).active) {
                info.activeDisables.push_back(DISABLE_FOR_CAMERAS_OFF);
            }
        });
    AddWidget(path, "Third-Person Vertical Sensitivity: %.0f", WIDGET_CVAR_SLIDER_FLOAT)
        .CVar("gEnhancements.Camera.RightStick.CameraSensitivity.Y")
        .Tooltip("Adjust the Sensitivity of the x axis when in Third Person.")
        .Options(UIWidgets::FloatSliderOptions{
            .format = "%.0f%%", .min = 0.01f, .max = 5.0f, .defaultValue = 1.0f, .isPercentage = true })
        .PreFunc([](widgetInfo& info) {
            if (mBenMenu->disabledMap.at(DISABLE_FOR_CAMERAS_OFF).active) {
                info.activeDisables.push_back(DISABLE_FOR_CAMERAS_OFF);
            }
        });
    AddWidget(path, "Enable Roll (6\xC2\xB0 of Freedom)", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Camera.DebugCam.6DOF")
        .Tooltip("This allows for all six degrees of movement with the camera, NOTE: Yaw will work "
                 "differently in this system, instead rotating around the focal point"
                 ", rather than a polar axis.")
        .PreFunc([](widgetInfo& info) { info.isHidden = mBenMenu->disabledMap.at(DISABLE_FOR_DEBUG_CAM_OFF).active; });
    AddWidget(path, "Camera Speed: %.0f", WIDGET_CVAR_SLIDER_FLOAT)
        .CVar("gEnhancements.Camera.DebugCam.CameraSpeed")
        .Tooltip("Adjusts the speed of the Camera.")
        .Options(UIWidgets::FloatSliderOptions{
            .format = "%.0f%%", .min = 0.1f, .max = 3.0f, .defaultValue = 0.5f, .isPercentage = true })
        .PreFunc([](widgetInfo& info) { info.isHidden = mBenMenu->disabledMap.at(DISABLE_FOR_DEBUG_CAM_OFF).active; });

    enhancementsSidebar.emplace("Cheats", SidebarEntry{ .columnCount = 3 });
    enhancementsOrder.push_back("Cheats");
    path = { "Enhancements", "Cheats", 1 };
    AddWidget(path, "Infinite Health", WIDGET_CVAR_CHECKBOX)
        .CVar("gCheats.InfiniteHealth")
        .Tooltip("Always have full Hearts.");
    AddWidget(path, "Infinite Magic", WIDGET_CVAR_CHECKBOX)
        .CVar("gCheats.InfiniteMagic")
        .Tooltip("Always have full Magic.");
    AddWidget(path, "Infinite Rupees", WIDGET_CVAR_CHECKBOX)
        .CVar("gCheats.InfiniteRupees")
        .Tooltip("Always have a full Wallet.");
    AddWidget(path, "Infinite Consumables", WIDGET_CVAR_CHECKBOX)
        .CVar("gCheats.InfiniteConsumables")
        .Tooltip("Always have max Consumables, you must have collected the consumables first.");
    AddWidget(path, "Longer Deku Flower Glide", WIDGET_CVAR_CHECKBOX)
        .CVar("gCheats.LongerFlowerGlide")
        .Tooltip("Allows Deku Link to glide longer, no longer dropping after a certain distance.");
    AddWidget(path, "No Clip", WIDGET_CVAR_CHECKBOX)
        .CVar("gCheats.NoClip")
        .Tooltip("Allows Link to phase through collision.");
    AddWidget(path, "Unbreakable Razor Sword", WIDGET_CVAR_CHECKBOX)
        .CVar("gCheats.UnbreakableRazorSword")
        .Tooltip("Allows to Razor Sword to be used indefinitely without dulling its blade.");
    AddWidget(path, "Unrestricted Items", WIDGET_CVAR_CHECKBOX)
        .CVar("gCheats.UnrestrictedItems")
        .Tooltip("Allows all Forms to use all Items.");
    AddWidget(path, "Hookshot Anywhere", WIDGET_CVAR_CHECKBOX)
        .CVar("gCheats.HookshotAnywhere")
        .Tooltip("Allows most surfaces to be hookshot-able");
    AddWidget(path, "Moon Jump on L", WIDGET_CVAR_CHECKBOX)
        .CVar("gCheats.MoonJumpOnL")
        .Tooltip("Holding L makes you float into the air.");
    AddWidget(path, "Elegy of Emptiness Anywhere", WIDGET_CVAR_CHECKBOX)
        .CVar("gCheats.ElegyAnywhere")
        .Tooltip("Allows Elegy of Emptiness outside of Ikana");
    AddWidget(path, "Stop Time in Dungeons", WIDGET_CVAR_COMBOBOX)
        .CVar("gCheats.TempleTimeStop")
        .Tooltip("Stops time from advancing in selected areas. Requires a room change to update.\n\n"
                 "- Off: Vanilla behaviour.\n"
                 "- Temples: Stops time in Woodfall, Snowhead, Great Bay, and Stone Tower Temples.\n"
                 "- Temples + Mini Dungeons: In addition to the above temples, stops time in both Spider "
                 "Houses, Pirate's Fortress, Beneath the Well, Ancient Castle of Ikana, and Secret Shrine.")
        .ComboMap(timeStopOptions);
    //// Gameplay Enhancements
    path = { "Enhancements", "Gameplay", 1 };
    enhancementsSidebar.emplace("Gameplay", SidebarEntry{ .columnCount = 3 });
    enhancementsOrder.push_back("Gameplay");
    AddWidget(path, "Player", WIDGET_SEPARATOR_TEXT);
    AddWidget(path, "Fast Deku Flower Launch", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Player.FastFlowerLaunch")
        .Tooltip("Speeds up the time it takes to be able to get maximum height from launching out of a "
                 "deku flower");
    AddWidget(path, "Instant Putaway", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Player.InstantPutaway")
        .Tooltip("Allows Link to instantly puts away held item without waiting.");
    AddWidget(path, "Fierce Deity Putaway", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Player.FierceDeityPutaway")
        .Tooltip("Allows Fierce Deity Link to put away his sword.");
    AddWidget(path, "Climb speed", WIDGET_CVAR_SLIDER_INT)
        .CVar("gEnhancements.Player.ClimbSpeed")
        .Tooltip("Increases the speed at which Link climbs vines and ladders.")
        .Options(UIWidgets::IntSliderOptions{ .min = 1, .max = 5, .defaultValue = 1 });
    AddWidget(path, "Dpad Equips", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Dpad.DpadEquips")
        .Tooltip("Allows you to equip items to your d-pad");
    AddWidget(path, "Always Win Doggy Race", WIDGET_CVAR_COMBOBOX)
        .CVar("gEnhancements.Minigames.AlwaysWinDoggyRace")
        .Tooltip("Makes the Doggy Race easier to win.")
        .ComboMap(alwaysWinDoggyraceOptions);
    AddWidget(path, "Milk Run Reward Options", WIDGET_CVAR_COMBOBOX)
        .CVar("gEnhancements.Minigames.CremiaHugs")
        .Tooltip("Choose what reward you get for winning the Milk Run minigame after the first time. \n"
                 "-Vanilla: Reward is Random\n"
                 "-Hug: Get the hugging cutscene\n"
                 "-Rupee: Get the rupee reward")
        .ComboMap(cremiaRewardOptions);
    AddWidget(path, "Cucco Shack Cucco Count", WIDGET_CVAR_SLIDER_INT)
        .CVar("gEnhancements.Minigames.CuccoShackCuccoCount")
        .Tooltip("Choose how many cuccos you need to raise to make Grog happy.")
        .Options(UIWidgets::IntSliderOptions{ .min = 1, .max = 10, .defaultValue = 10 });
    AddWidget(path, "Swordsman School Winning Score", WIDGET_CVAR_SLIDER_INT)
        .CVar("gEnhancements.Minigames.SwordsmanSchoolScore")
        .Tooltip("Sets the score required to win the Swordsman School.")
        .Options(UIWidgets::IntSliderOptions{ .min = 1, .max = 30, .defaultValue = 30 });
    AddWidget(path, "Fast Magic Arrow Equip Animation", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Equipment.MagicArrowEquipSpeed")
        .Tooltip("Removes the animation for equipping Magic Arrows.");
    AddWidget(path, "Instant Fin Boomerangs Recall", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.PlayerActions.InstantRecall")
        .Tooltip("Pressing B will instantly recall the fin boomerang back to Zora Link after they are thrown.");
    AddWidget(path, "Two-Handed Sword Spin Attack", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Equipment.TwoHandedSwordSpinAttack")
        .Tooltip("Enables magic spin attacks for the Fierce Deity Sword and Great Fairy's Sword.");

    path.column = 2;
    AddWidget(path, "Modes", WIDGET_SEPARATOR_TEXT);
    AddWidget(path, "Play as Kafei", WIDGET_CVAR_CHECKBOX)
        .CVar("gModes.PlayAsKafei")
        .Tooltip("Requires scene reload to take effect.");
    AddWidget(path, "Hyrule Warriors Styled Link", WIDGET_CVAR_CHECKBOX)
        .CVar("gModes.HyruleWarriorsStyledLink")
        .Tooltip("When acquired, places the Keaton and Fierce Deity masks on Link similarly to how he "
                 "wears them in Hyrule Warriors");
    AddWidget(path, "Time Moves when you Move", WIDGET_CVAR_CHECKBOX)
        .CVar("gModes.TimeMovesWhenYouMove")
        .Tooltip("Time only moves when Link is not standing still.");
    AddWidget(path, "Mirrored World", WIDGET_CVAR_CHECKBOX)
        .CVar("gModes.MirroredWorld.Mode")
        .Tooltip("Mirrors the world horizontally.")
        .Callback([](widgetInfo& info) {
            if (CVarGetInteger("gModes.MirroredWorld.Mode", 0)) {
                CVarSetInteger("gModes.MirroredWorld.State", 1);
            } else {
                CVarClear("gModes.MirroredWorld.State");
            }
        });

    path.column = 3;
    AddWidget(path, "Saving", WIDGET_SEPARATOR_TEXT);
    AddWidget(path, "Persistent Owl Saves", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Saving.PersistentOwlSaves")
        .Tooltip("Continuing a save will not remove the owl save. Playing Song of "
                 "Time, allowing the moon to crash or finishing the "
                 "game will remove the owl save and become the new last save.");
    AddWidget(path, "Pause Menu Save", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Saving.PauseSave")
        .Tooltip("Re-introduce the pause menu save system. Pressing B in the pause menu will give you the "
                 "option to create a persistent Owl Save from your current location.\n\nWhen loading back "
                 "into the game, you will be placed either at the entrance of the dungeon you saved in, or "
                 "in South Clock Town.");
    AddWidget(path, "Autosave", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Saving.Autosave")
        .Tooltip("Automatically create a persistent Owl Save on the chosen interval.\n\nWhen loading "
                 "back into the game, you will be placed either at the entrance of the dungeon you "
                 "saved in, or in South Clock Town.")
        .Callback([](widgetInfo& info) { RegisterAutosave(); });
    AddWidget(path, "Autosave Interval: %d minutes", WIDGET_CVAR_SLIDER_INT)
        .CVar("gEnhancements.Saving.AutosaveInterval")
        .Tooltip("Sets the interval between Autosaves.")
        .Options(UIWidgets::IntSliderOptions{ .min = 1, .max = 60, .defaultValue = 5 })
        .PreFunc([](widgetInfo& info) {
            if (mBenMenu->disabledMap.at(DISABLE_FOR_AUTO_SAVE_OFF).active) {
                info.activeDisables.push_back(DISABLE_FOR_AUTO_SAVE_OFF);
            }
        });
    AddWidget(path, "Time Cycle", WIDGET_SEPARATOR_TEXT);
    AddWidget(path, "Do not reset Bottle content", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Cycle.DoNotResetBottleContent")
        .Tooltip("Playing the Song Of Time will not reset the bottles' content.");
    AddWidget(path, "Do not reset Consumables", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Cycle.DoNotResetConsumables")
        .Tooltip("Playing the Song Of Time will not reset the consumables.");
    AddWidget(path, "Do not reset Razor Sword", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Cycle.DoNotResetRazorSword")
        .Tooltip("Playing the Song Of Time will not reset the Sword back to Kokiri Sword.");
    AddWidget(path, "Do not reset Rupees", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Cycle.DoNotResetRupees")
        .Tooltip("Playing the Song Of Time will not reset the your rupees.");
    AddWidget(path, "Do not reset Time Speed", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Cycle.DoNotResetTimeSpeed")
        .Tooltip("Playing the Song Of Time will not reset the current time speed set by Inverted Song of Time.");
    AddWidget(path, "Keep Express Mail", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Cycle.KeepExpressMail")
        .Tooltip("Allows the player to keep the Express Mail in their inventory after delivering it "
                 "the first time, so that both deliveries can be done within one cycle")
        .Callback([](widgetInfo& info) { RegisterKeepExpressMail(); });
    AddWidget(path, "Unstable", WIDGET_SEPARATOR_TEXT)
        .Options(UIWidgets::TextOptions{ .color = UIWidgets::Color::Orange });
    AddWidget(path, "Disable Save Delay", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Saving.DisableSaveDelay")
        .Tooltip("Removes the arbitrary 2 second timer for saving from the original game. This is known to "
                 "cause issues when attempting the 0th Day Glitch");

    //// Graphics Enhancements
    enhancementsSidebar.emplace("Graphics", SidebarEntry{ .columnCount = 3 });
    enhancementsOrder.push_back("Graphics");
    path = { "Enhancements", "Graphics", 1 };
    AddWidget(path, "Clock", WIDGET_SEPARATOR_TEXT);
    AddWidget(path, "Clock Type", WIDGET_CVAR_COMBOBOX)
        .CVar("gEnhancements.Graphics.ClockType")
        .Tooltip("Swaps between Graphical and Text only Clock types.")
        .ComboMap(clockTypeOptions);
    AddWidget(path, "24 Hours Clock", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Graphics.24HoursClock")
        .Tooltip("Changes from a 12 Hour to a 24 Hour Clock");
    AddWidget(path, "Mods", WIDGET_SEPARATOR_TEXT);
    AddWidget(path, "Use Alternate Assets", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Mods.AlternateAssets")
        .Tooltip("Toggle between standard assets and alternate assets. Usually mods will indicate if "
                 "this setting has to be used or not.");
    AddWidget(path, "Motion Blur", WIDGET_SEPARATOR_TEXT);
    AddWidget(path, "Motion Blur Mode", WIDGET_CVAR_COMBOBOX)
        .CVar("gEnhancements.Graphics.MotionBlur.Mode")
        .Tooltip("Selects the Mode for Motion Blur.")
        .Options(UIWidgets::ComboboxOptions{ .labelPosition = UIWidgets::LabelPosition::None })
        .ComboMap(motionBlurOptions);
    AddWidget(path, "Interpolate", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Graphics.MotionBlur.Interpolate")
        .Tooltip("Change motion blur capture to also happen on interpolated frames instead of only on game frames.\n"
                 "This notably reduces the overall motion blur strength but smooths out the trails.")
        .PreFunc([](widgetInfo& info) {
            info.isHidden = mBenMenu->disabledMap.at(DISABLE_FOR_MOTION_BLUR_MODE).value == 1;
        });
    AddWidget(path, "On/Off", WIDGET_CHECKBOX)
        .Tooltip("Enables Motion Blur.")
        .ValuePointer((bool*)&R_MOTION_BLUR_ENABLED)
        .PreFunc([](widgetInfo& info) {
            info.valuePointer = (bool*)&R_MOTION_BLUR_ENABLED;
            info.isHidden = mBenMenu->disabledMap.at(DISABLE_FOR_MOTION_BLUR_MODE).value != 0;
        });
    AddWidget(path, "Strength", WIDGET_CVAR_SLIDER_INT)
        .CVar("gEnhancements.Graphics.MotionBlur.Strength")
        .Tooltip("Motion Blur strength.")
        .Options(UIWidgets::IntSliderOptions{ .min = 0, .max = 255, .defaultValue = 180 })
        .PreFunc([](widgetInfo& info) {
            info.isHidden = mBenMenu->disabledMap.at(DISABLE_FOR_MOTION_BLUR_MODE).value != 2;
        });
    AddWidget(path, "Strength", WIDGET_SLIDER_INT)
        .Tooltip("Motion Blur strength.")
        .Options(UIWidgets::IntSliderOptions{ .min = 0, .max = 255, .defaultValue = 180 })
        .ValuePointer(&motionBlurStrength)
        .Callback([](widgetInfo& info) { R_MOTION_BLUR_ALPHA = motionBlurStrength; })
        .PreFunc([](widgetInfo& info) {
            motionBlurStrength = R_MOTION_BLUR_ALPHA;
            info.isHidden = mBenMenu->disabledMap.at(DISABLE_FOR_MOTION_BLUR_MODE).value != 0 ||
                            mBenMenu->disabledMap.at(DISABLE_FOR_MOTION_BLUR_OFF).active;
        });

    path.column = 2;
    AddWidget(path, "Other", WIDGET_SEPARATOR_TEXT);
    AddWidget(path, "3D Item Drops", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Graphics.3DItemDrops")
        .Tooltip("Makes item drops 3D");
    AddWidget(path, "Authentic Logo", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Graphics.AuthenticLogo")
        .Tooltip("Hide the game version and build details and display the authentic "
                 "model and texture on the boot logo start screen");
    AddWidget(path, "Bow Reticle", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Graphics.BowReticle")
        .Tooltip("Gives the bow a reticle when you draw an arrow.");
    AddWidget(path, "Disable Black Bar Letterboxes", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Graphics.DisableBlackBars")
        .Tooltip("Disables Black Bar Letterboxes during cutscenes and Z-targeting\nNote: there may be "
                 "minor visual glitches that were covered up by the black bars\nPlease disable this "
                 "setting before reporting a bug.");
    AddWidget(path, "Unstable", WIDGET_SEPARATOR_TEXT)
        .Options(UIWidgets::TextOptions{ .color = UIWidgets::Color::Orange });
    AddWidget(path, "Disable Scene Geometry Distance Check", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Graphics.DisableSceneGeometryDistanceCheck")
        .Tooltip("Disables the distance check for scene geometry, allowing it to be drawn no matter how far "
                 "away it is from the player. This may have unintended side effects.")
        .Callback([](widgetInfo& info) { GfxPatcher_ApplyGeometryIssuePatches(); });
    AddWidget(path, "Widescreen Actor Culling", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Graphics.ActorCullingAccountsForWidescreen")
        .Tooltip("Adjusts the culling planes to account for widescreen resolutions. "
                 "This may have unintended side effects.");
    AddWidget(path, "Increase Actor Draw Distance: %dx", WIDGET_CVAR_SLIDER_INT)
        .CVar("gEnhancements.Graphics.IncreaseActorDrawDistance")
        .Tooltip("Increase the range in which Actors are drawn. This may have unintended side effects.")
        .Options(UIWidgets::IntSliderOptions{ .min = 1, .max = 5, .defaultValue = 1 });

    enhancementsSidebar.emplace("Items/Songs", SidebarEntry{ .columnCount = 3 });
    enhancementsOrder.push_back("Items/Songs");
    path = { "Enhancements", "Items/Songs", 1 };
    // Mask Enhancements
    AddWidget(path, "Masks", WIDGET_SEPARATOR_TEXT);
    AddWidget(path, "Blast Mask has Powder Keg Force", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Masks.BlastMaskKeg")
        .Tooltip("Blast Mask can also destroy objects only the Powder Keg can.");
    AddWidget(path, "Fast Transformation", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Masks.FastTransformation")
        .Tooltip("Removes the delay when using transformation masks.");
    AddWidget(path, "Fierce Deity's Mask Anywhere", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Masks.FierceDeitysAnywhere")
        .Tooltip("Allow using Fierce Deity's mask outside of boss rooms.");
    AddWidget(path, "Persistent Bunny Hood", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Masks.PersistentBunnyHood.Enabled")
        .Tooltip("Permanently toggle a speed boost from the bunny hood by pressing 'A' on it in the mask menu.");
    AddWidget(path, "No Blast Mask Cooldown", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Masks.NoBlastMaskCooldown")
        .Tooltip("Eliminates the Cooldown between Blast Mask usage.");

    // Song Enhancements
    path.column = 2;
    AddWidget(path, "Ocarina", WIDGET_SEPARATOR_TEXT);
    AddWidget(path, "Better Song of Double Time", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Songs.BetterSongOfDoubleTime")
        .Tooltip("When playing the Song of Double Time, you can now choose the exact time you want to go "
                 "to, similar to the 3DS version.");
    AddWidget(path, "Enable Sun's Song", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Songs.EnableSunsSong")
        .Tooltip("Enables the partially implemented Sun's Song. RIGHT-DOWN-UP-RIGHT-DOWN-UP to play it. "
                 "This song will make time move very fast until either Link moves to a different scene, "
                 "or when the time switches to a new time period.");
    AddWidget(path, "Dpad Ocarina", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Playback.DpadOcarina")
        .Tooltip("Enables using the Dpad for Ocarina playback.");
    AddWidget(path, "Pause Owl Warp", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Songs.PauseOwlWarp")
        .Tooltip("Allows warping to registered Owl Statues from the pause menu map screen. "
                 "Requires that you can play Song of Soaring normally.\n\n"
                 "Accounts for Index-Warp being active, by presenting all valid warps for the registered "
                 "map points. Great Bay Coast warp is always given for index 0 warp as a convenience.");
    AddWidget(path, "Zora Eggs For Bossa Nova", WIDGET_CVAR_SLIDER_INT)
        .CVar("gEnhancements.Songs.ZoraEggCount")
        .Tooltip("The number of eggs required to unlock new wave bossa nova.")
        .Options(UIWidgets::IntSliderOptions{ .min = 1, .max = 7, .defaultValue = 7 });
    AddWidget(path, "Prevent Dropped Ocarina Inputs", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Playback.NoDropOcarinaInput")
        .Tooltip("Prevent dropping inputs when playing the ocarina quickly.");
    AddWidget(path, "Skip Scarecrow Song", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Playback.SkipScarecrowSong")
        .Tooltip("Pierre appears when the Ocarina is pulled out.");
    AddWidget(path, "Faster Song Playback", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Songs.FasterSongPlayback")
        .Tooltip("Speeds up the playback of songs.");

    // Time Savers
    enhancementsSidebar.emplace("Time Savers", SidebarEntry{ .columnCount = 3 });
    enhancementsOrder.push_back("Time Savers");
    path = { "Enhancements", "Time Savers", 1 };
    // Cutscene Skips
    AddWidget(path, "Cutscenes", WIDGET_SEPARATOR_TEXT);
    AddWidget(path, "Hide Title Cards", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Cutscenes.HideTitleCards")
        .Tooltip("Hides Title Cards when entering areas.");
    AddWidget(path, "Skip Entrance Cutscenes", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Cutscenes.SkipEntranceCutscenes")
        .Tooltip("Skip cutscenes that occur when first entering a new area.");
    AddWidget(path, "Skip to File Select", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Cutscenes.SkipToFileSelect")
        .Tooltip("Skip the opening title sequence and go straight to the file select menu after boot.");
    AddWidget(path, "Skip Intro Sequence", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Cutscenes.SkipIntroSequence")
        .Tooltip("When starting a game you will be taken straight to South Clock Town as Deku Link.");
    AddWidget(path, "Skip First Cycle", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Cutscenes.SkipFirstCycle")
        .Tooltip("When starting a game you will be taken straight to South Clock Town as Human Link "
                 "with Deku Mask, Ocarina, Song of Time, and Song of Healing.")
        .PreFunc([](widgetInfo& info) { info.isHidden = mBenMenu->disabledMap.at(DISABLE_FOR_INTRO_SKIP_OFF).active; });
    AddWidget(path, "Skip Story Cutscenes", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Cutscenes.SkipStoryCutscenes")
        .Tooltip("Disclaimer: This doesn't do much yet, we will be progressively adding more skips over time.");
    AddWidget(path, "Skip Misc Interactions", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Cutscenes.SkipMiscInteractions")
        .Tooltip("Disclaimer: This doesn't do much yet, we will be progressively adding more skips over time.");

    // Dialogue Enhancements
    path.column = 2;
    AddWidget(path, "Dialogue", WIDGET_SEPARATOR_TEXT);
    AddWidget(path, "Fast Bank Selection", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Dialogue.FastBankSelection")
        .Tooltip("Pressing the Z or R buttons while the Deposit/Withdrawal Rupees dialogue is open will set "
                 "the Rupees to Links current Rupees or 0 respectively.");
    AddWidget(path, "Fast Text", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Dialogue.FastText")
        .Tooltip("Speeds up text rendering, and enables holding of B progress to next message.");

    // Fixes
    enhancementsSidebar.emplace("Fixes", SidebarEntry{ .columnCount = 3 });
    enhancementsOrder.push_back("Fixes");
    path = { "Enhancements", "Fixes", 1 };
    AddWidget(path, "Fixes", WIDGET_SEPARATOR_TEXT);
    AddWidget(path, "Fix Ammo Count Color", WIDGET_CVAR_CHECKBOX)
        .CVar("gFixes.FixAmmoCountEnvColor")
        .Tooltip("Fixes a missing gDPSetEnvColor, which causes the ammo count to be "
                 "the wrong color prior to obtaining magic or other conditions.");
    AddWidget(path, "Fix Fierce Deity Z-Target movement", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Fixes.FierceDeityZTargetMovement")
        .Tooltip("Fixes Fierce Deity movement being choppy when Z-targeting");
    AddWidget(path, "Fix Hess and Weirdshot Crash", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Fixes.HessCrash")
        .Tooltip("Fixes a crash that can occur when performing a HESS or Weirdshot.");
    AddWidget(path, "Fix Text Control Characters", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Fixes.ControlCharacters")
        .Tooltip("Fixes certain control characters not functioning properly "
                 "depending on their position within the text.");
    AddWidget(path, "Fix Ikana Great Fairy Fountain Color", WIDGET_CVAR_CHECKBOX)
        .CVar("gFixes.FixIkanaGreatFairyFountainColor")
        .Tooltip("Fixes a bug that results in the Ikana Great Fairy fountain looking green instead of "
                 "yellow, this was fixed in the EU version");
    AddWidget(path, "Fix Texture overflow OOB", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Fixes.FixTexturesOOB")
        .Tooltip("Fixes textures that normally overflow to be patched with the correct size or format")
        .Options(UIWidgets::CheckboxOptions{ .defaultValue = true })
        .Callback([](widgetInfo& info) { GfxPatcher_ApplyOverflowTexturePatches(); });
    AddWidget(path, "Fix Completed Heart Container Audio", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Fixes.CompletedHeartContainerAudio")
        .Tooltip("Fixes a bug that results in the wrong audio playing upon receiving a 4th piece of heart to "
                 "fill a new heart container.");

    // Restorations
    enhancementsSidebar.emplace("Restorations", SidebarEntry{ .columnCount = 3 });
    enhancementsOrder.push_back("Restorations");
    path = { "Enhancements", "Restorations", 1 };
    AddWidget(path, "Restorations", WIDGET_SEPARATOR_TEXT);
    AddWidget(path, "Constant Distance Backflips and Sidehops", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Restorations.ConstantFlipsHops")
        .Tooltip("Backflips and Sidehops travel a constant distance as they did in OoT.");
    AddWidget(path, "Power Crouch Stab", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Restorations.PowerCrouchStab")
        .Tooltip("Crouch stabs will use the power of Link's previous melee attack, as is in MM JP 1.0 and OoT.");
    AddWidget(path, "Side Rolls", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Restorations.SideRoll")
        .Tooltip("Restores side rolling from OoT.");
    AddWidget(path, "Tatl ISG", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Restorations.TatlISG")
        .Tooltip("Restores Navi ISG from OoT, but now with Tatl.");
    AddWidget(path, "Woodfall Mountain Appearance", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Restorations.WoodfallMountainAppearance")
        .Tooltip("Restores the appearance of Woodfall mountain to not look poisoned "
                 "when viewed from Termina Field after clearing Woodfall Temple\n\n"
                 "Requires a scene reload to take effect");

    // Difficulty Options
    enhancementsSidebar.emplace("Difficulty Options", SidebarEntry{ .columnCount = 3 });
    enhancementsOrder.push_back("Difficulty Options");
    path = { "Enhancements", "Difficulty Options", 1 };
    AddWidget(path, "Disable Takkuri Steal", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Cheats.DisableTakkuriSteal")
        .Tooltip("Prevents the Takkuri from stealing key items like bottles and swords. It may still steal "
                 "other items.");
    AddWidget(path, "Deku Guard Search Balls", WIDGET_CVAR_COMBOBOX)
        .CVar("gEnhancements.Cheats.DekuGuardSearchBalls")
        .Tooltip("Choose when to show the Deku Palace Guards' search balls\n"
                 "- Never: Never show the search balls. This matches Majora's Mask 3D behaviour\n"
                 "- Night Only: Only show the search balls at night. This matches original N64 behaviour.\n"
                 "- Always: Always show the search balls.")
        .Options(UIWidgets::ComboboxOptions{ .defaultIndex =
                                                 DekuGuardSearchBallsOptions::DEKU_GUARD_SEARCH_BALLS_NIGHT_ONLY })
        .ComboMap(dekuGuardSearchBallsOptions);

    // HUD Editor
    enhancementsSidebar.emplace("HUD Editor", SidebarEntry{ .columnCount = 1 });
    enhancementsOrder.push_back("HUD Editor");
    path = { "Enhancements", "HUD Editor", 1 };
    AddWidget(path, "Popout HUD Editor", WIDGET_WINDOW_BUTTON)
        .CVar("gWindows.HudEditor")
        .Tooltip("Enables the HUD Editor window, allowing you to modify your HUD")
        .Options(UIWidgets::ButtonOptions{ .size = UIWidgets::Sizes::Inline })
        .WindowName("HUD Editor");

    // Item Tracker Settings
    enhancementsSidebar.emplace("Item Tracker", SidebarEntry{ .columnCount = 1 });
    enhancementsOrder.push_back("Item Tracker");
    path = { "Enhancements", "Item Tracker", 1 };
    AddWidget(path, "Popout Item Tracker", WIDGET_WINDOW_BUTTON)
        .CVar("gWindows.ItemTracker")
        .WindowName("Item Tracker Settings");
}

void BenMenu::AddDevTools() {
    devToolsSidebar.emplace("General", SidebarEntry{ .columnCount = 3 });
    devToolsOrder.push_back("General");
    WidgetPath path = { "Dev Tools", "General", 1 };
    AddWidget(path, "Popout Menu", WIDGET_CVAR_CHECKBOX)
        .CVar("gSettings.Menu.Popout")
        .Tooltip("Changes the menu display from overlay to windowed.");
    AddWidget(path, "Set Warp Point", WIDGET_BUTTON)
        .Tooltip("Creates warp point that you can teleport to later")
        .Callback([](widgetInfo& info) {
            Player* player = GET_PLAYER(gPlayState);

            CVarSetInteger(WARP_POINT_CVAR "Entrance", gSaveContext.save.entrance);
            CVarSetInteger(WARP_POINT_CVAR "Room", gPlayState->roomCtx.curRoom.num);
            CVarSetFloat(WARP_POINT_CVAR "X", player->actor.world.pos.x);
            CVarSetFloat(WARP_POINT_CVAR "Y", player->actor.world.pos.y);
            CVarSetFloat(WARP_POINT_CVAR "Z", player->actor.world.pos.z);
            CVarSetFloat(WARP_POINT_CVAR "Rotation", player->actor.shape.rot.y);
            CVarSetInteger(WARP_POINT_CVAR "Saved", 1);
            Ship::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesOnNextTick();
        })
        .PreFunc(
            [](widgetInfo& info) { info.isHidden = mBenMenu->disabledMap.at(DISABLE_FOR_NULL_PLAY_STATE).active; });
    AddWidget(path, "Scene Room ID", WIDGET_TEXT).PreFunc([](widgetInfo& info) {
        u32 sceneId =
            Entrance_GetSceneIdAbsolute(CVarGetInteger(WARP_POINT_CVAR "Entrance", ENTRANCE(SOUTH_CLOCK_TOWN, 0)));
        info.name = fmt::format("{} Room {}", warpPointSceneList[sceneId], CVarGetInteger(WARP_POINT_CVAR "Room", 0));
        info.isHidden = mBenMenu->disabledMap.at(DISABLE_FOR_NULL_PLAY_STATE).active ||
                        mBenMenu->disabledMap.at(DISABLE_FOR_WARP_POINT_NOT_SET).active;
    });
    AddWidget(path, ICON_FA_TIMES, WIDGET_BUTTON)
        .Tooltip("Clear warp point")
        .Options(UIWidgets::ButtonOptions{ .size = UIWidgets::Sizes::Inline })
        .Callback([](widgetInfo& info) {
            CVarClear(WARP_POINT_CVAR "Entrance");
            CVarClear(WARP_POINT_CVAR "Room");
            CVarClear(WARP_POINT_CVAR "X");
            CVarClear(WARP_POINT_CVAR "Y");
            CVarClear(WARP_POINT_CVAR "Z");
            CVarClear(WARP_POINT_CVAR "Rotation");
            CVarClear(WARP_POINT_CVAR "Saved");
            Ship::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesOnNextTick();
        })
        .PreFunc([](widgetInfo& info) {
            info.isHidden = mBenMenu->disabledMap.at(DISABLE_FOR_NULL_PLAY_STATE).active ||
                            mBenMenu->disabledMap.at(DISABLE_FOR_WARP_POINT_NOT_SET).active;
        })
        .SameLine(true);
    AddWidget(path, "Warp", WIDGET_BUTTON)
        .Tooltip("Teleport to the set warp point")
        .Options(UIWidgets::ButtonOptions{ .size = UIWidgets::Sizes::Inline })
        .Callback([](widgetInfo& info) { Warp(); })
        .PreFunc([](widgetInfo& info) {
            info.isHidden = mBenMenu->disabledMap.at(DISABLE_FOR_NULL_PLAY_STATE).active ||
                            mBenMenu->disabledMap.at(DISABLE_FOR_WARP_POINT_NOT_SET).active;
        })
        .SameLine(true);
    AddWidget(path, "Debug Mode", WIDGET_CVAR_CHECKBOX)
        .CVar("gDeveloperTools.DebugEnabled")
        .Tooltip("Enables Debug Mode, allowing you to select maps with L + R + Z.")
        .Callback([](widgetInfo& info) {
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
        });
    AddWidget(path, "Better Map Select", WIDGET_CVAR_CHECKBOX)
        .CVar("gDeveloperTools.BetterMapSelect.Enabled")
        .Tooltip("Overrides the original map select with a translated, more user-friendly version.")
        .PreFunc([](widgetInfo& info) { info.isHidden = mBenMenu->disabledMap.at(DISABLE_FOR_DEBUG_MODE_OFF).active; });
    AddWidget(path, "Debug Save File Mode", WIDGET_CVAR_COMBOBOX)
        .CVar("gDeveloperTools.DebugSaveFileMode")
        .Tooltip("Change the behavior of creating saves while debug mode is enabled:\n\n"
                 "- Empty Save: The default 3 heart save file in first cycle\n"
                 "- Vanilla Debug Save: Uses the title screen save info (8 hearts, all items and masks)\n"
                 "- 100\% Save: All items, equipment, mask, quest status and bombers notebook complete")
        .ComboMap(debugSaveOptions)
        .Callback([](widgetInfo& info) { RegisterDebugSaveCreate(); })
        .PreFunc([](widgetInfo& info) { info.isHidden = mBenMenu->disabledMap.at(DISABLE_FOR_DEBUG_MODE_OFF).active; });
    AddWidget(path, "Prevent Actor Update", WIDGET_CVAR_CHECKBOX)
        .CVar("gDeveloperTools.PreventActorUpdate")
        .Tooltip("Prevents Actors from updating.")
        .Callback([](widgetInfo& info) { RegisterPreventActorUpdateHooks(); })
        .PreFunc([](widgetInfo& info) { info.isHidden = mBenMenu->disabledMap.at(DISABLE_FOR_DEBUG_MODE_OFF).active; });
    AddWidget(path, "Prevent Actor Draw", WIDGET_CVAR_CHECKBOX)
        .CVar("gDeveloperTools.PreventActorDraw")
        .Tooltip("Prevents Actors from drawing.")
        .Callback([](widgetInfo& info) { RegisterPreventActorDrawHooks(); })
        .PreFunc([](widgetInfo& info) { info.isHidden = mBenMenu->disabledMap.at(DISABLE_FOR_DEBUG_MODE_OFF).active; });
    AddWidget(path, "Prevent Actor Init", WIDGET_CVAR_CHECKBOX)
        .CVar("gDeveloperTools.PreventActorInit")
        .Tooltip("Prevents Actors from initializing.")
        .Callback([](widgetInfo& info) { RegisterPreventActorInitHooks(); })
        .PreFunc([](widgetInfo& info) { info.isHidden = mBenMenu->disabledMap.at(DISABLE_FOR_DEBUG_MODE_OFF).active; });
    AddWidget(path, "Disable Object Dependency", WIDGET_CVAR_CHECKBOX)
        .CVar("gDeveloperTools.DisableObjectDependency")
        .Tooltip("Disables dependencies when loading objects.")
        .PreFunc([](widgetInfo& info) { info.isHidden = mBenMenu->disabledMap.at(DISABLE_FOR_DEBUG_MODE_OFF).active; });
    AddWidget(path, "Log Level", WIDGET_CVAR_COMBOBOX)
        .CVar("gDeveloperTools.LogLevel")
        .Tooltip("The log level determines which messages are printed to the "
                 "console. This does not affect the log file output")
        .ComboMap(logLevels)
        .Callback([](widgetInfo& info) {
            Ship::Context::GetInstance()->GetLogger()->set_level(
                (spdlog::level::level_enum)CVarGetInteger("gDeveloperTools.LogLevel", 1));
        })
        .PreFunc([](widgetInfo& info) { info.isHidden = mBenMenu->disabledMap.at(DISABLE_FOR_DEBUG_MODE_OFF).active; });
    AddWidget(path, "Frame Advance", WIDGET_CHECKBOX)
        .Tooltip("This allows you to advance through the game one frame at a time on command. "
                 "To advance a frame, hold Z and tap R on the second controller. Holding Z "
                 "and R will advance a frame every half second. You can also use the buttons below.")
        .PreFunc([](widgetInfo& info) {
            info.isHidden = mBenMenu->disabledMap.at(DISABLE_FOR_NULL_PLAY_STATE).active ||
                            mBenMenu->disabledMap.at(DISABLE_FOR_DEBUG_MODE_OFF).active;
            if (gPlayState != nullptr) {
                info.valuePointer = (bool*)&gPlayState->frameAdvCtx.enabled;
            } else {
                info.valuePointer = (bool*)nullptr;
            }
        });
    AddWidget(path, "Advance 1", WIDGET_BUTTON)
        .Tooltip("Advance 1 frame.")
        .Options(UIWidgets::ButtonOptions{ .size = UIWidgets::Sizes::Inline })
        .Callback([](widgetInfo& info) { CVarSetInteger("gDeveloperTools.FrameAdvanceTick", 1); })
        .PreFunc([](widgetInfo& info) {
            info.isHidden = mBenMenu->disabledMap.at(DISABLE_FOR_FRAME_ADVANCE_OFF).active ||
                            mBenMenu->disabledMap.at(DISABLE_FOR_DEBUG_MODE_OFF).active;
        });
    AddWidget(path, "Advance (Hold)", WIDGET_BUTTON)
        .Tooltip("Advance frames while the button is held.")
        .Options(UIWidgets::ButtonOptions{ .size = UIWidgets::Sizes::Inline })
        .PreFunc([](widgetInfo& info) {
            info.isHidden = mBenMenu->disabledMap.at(DISABLE_FOR_FRAME_ADVANCE_OFF).active ||
                            mBenMenu->disabledMap.at(DISABLE_FOR_DEBUG_MODE_OFF).active;
        })
        .PostFunc([](widgetInfo& info) {
            if (ImGui::IsItemActive()) {
                CVarSetInteger("gDeveloperTools.FrameAdvanceTick", 1);
            }
        })
        .SameLine(true);

    // dev tools windows
    devToolsSidebar.emplace("Collision Viewer", SidebarEntry{ .columnCount = 1 });
    devToolsOrder.push_back("Collision Viewer");
    path = { "Dev Tools", "Collision Viewer", 1 };
    AddWidget(path, "Popout Collision Viewer", WIDGET_WINDOW_BUTTON)
        .CVar("gWindows.CollisionViewer")
        .Tooltip("Makes collision visible on screen")
        .Options(UIWidgets::ButtonOptions{ .size = UIWidgets::Sizes::Inline })
        .WindowName("Collision Viewer");

    devToolsSidebar.emplace("Stats", SidebarEntry{ .columnCount = 1 });
    devToolsOrder.push_back("Stats");
    path = { "Dev Tools", "Stats", 1 };
    AddWidget(path, "Popout Stats", WIDGET_WINDOW_BUTTON)
        .CVar("gOpenWindows.Stats")
        .Tooltip("Shows the stats window, with your FPS and frametimes, and the OS you're playing on")
        .WindowName("Stats");

    devToolsSidebar.emplace("Console", SidebarEntry{ .columnCount = 1 });
    devToolsOrder.push_back("Console");
    path = { "Dev Tools", "Console", 1 };
    AddWidget(path, "Popout Console", WIDGET_WINDOW_BUTTON)
        .CVar("gOpenWindows.Console")
        .Tooltip("Enables the console window, allowing you to input commands. Type help for some examples")
        .WindowName("Console");

    devToolsSidebar.emplace("Gfx Debugger", SidebarEntry{ .columnCount = 1 });
    devToolsOrder.push_back("Gfx Debugger");
    path = { "Dev Tools", "Gfx Debugger", 1 };
    AddWidget(path, "Popout Gfx Debugger", WIDGET_WINDOW_BUTTON)
        .CVar("gOpenWindows.GfxDebugger")
        .Tooltip("Enables the Gfx Debugger window, allowing you to input commands, type help for some examples")
        .WindowName("GfxDebuggerWindow");

    devToolsSidebar.emplace("Save Editor", SidebarEntry{ .columnCount = 1 });
    devToolsOrder.push_back("Save Editor");
    path = { "Dev Tools", "Save Editor", 1 };
    AddWidget(path, "Popout Save Editor", WIDGET_WINDOW_BUTTON)
        .CVar("gWindows.SaveEditor")
        .Tooltip("Enables the Save Editor window, allowing you to edit your save file")
        .WindowName("Save Editor");

    devToolsSidebar.emplace("Actor Viewer", SidebarEntry{ .columnCount = 1 });
    devToolsOrder.push_back("Actor Viewer");
    path = { "Dev Tools", "Actor Viewer", 1 };
    AddWidget(path, "Popout Actor Viewer", WIDGET_WINDOW_BUTTON)
        .CVar("gWindows.ActorViewer")
        .Tooltip("Enables the Actor Viewer window, allowing you to view actors in the world.")
        .WindowName("Actor Viewer");

    devToolsSidebar.emplace("Event Log", SidebarEntry{ .columnCount = 1 });
    devToolsOrder.push_back("Event Log");
    path = { "Dev Tools", "Event Log", 1 };
    AddWidget(path, "Popout Event Log", WIDGET_WINDOW_BUTTON)
        .CVar("gWindows.EventLog")
        .Tooltip("Enables the event log window")
        .WindowName("Event Log");
}

BenMenu::BenMenu(const std::string& consoleVariable, const std::string& name) : Menu(consoleVariable, name) {
}

void BenMenu::InitElement() {
    Ship::Menu::InitElement();
    AddSettings();
    AddEnhancements();
    AddDevTools();

    menuEntries = { { "Settings", settingsSidebar, "gSettings.Menu.SettingsSidebarSection", settingsOrder },
                    { "Enhancements", enhancementsSidebar, "gSettings.Menu.EnhancementsSidebarSection",
                      enhancementsOrder },
                    { "Developer Tools", devToolsSidebar, "gSettings.Menu.DevToolsSidebarSection", devToolsOrder } };

    if (CVarGetInteger("gSettings.SidebarSearch", 0)) {
        InsertSidebarSearch();
    }

    disabledMap = {
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
          { [](disabledInfo& info) -> bool {
               return !CVarGetInteger("gEnhancements.Camera.FirstPerson.GyroEnabled", 0);
           },
            "Gyro Aiming is Disabled" } },
        { DISABLE_FOR_GYRO_ON,
          { [](disabledInfo& info) -> bool {
               return CVarGetInteger("gEnhancements.Camera.FirstPerson.GyroEnabled", 0);
           },
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
          { [](disabledInfo& info) -> bool {
               return !Ship::Context::GetInstance()->GetWindow()->CanDisableVerticalSync();
           },
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
}

void BenMenu::UpdateElement() {
    Ship::Menu::UpdateElement();
}

void BenMenu::Draw() {
    Ship::Menu::Draw();
}

void BenMenu::DrawElement() {
    Ship::Menu::DrawElement();
}
} // namespace BenGui
