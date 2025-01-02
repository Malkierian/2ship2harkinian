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

void BenMenu::AddSettings() {
    // General Settings
    settingsSidebar.push_back(
        { .label = "General",
          .columnCount = 3,
          .columnWidgets = { {
              { .name = "Menu Theme",
                .cVar = "gSettings.MenuTheme",
                .tooltip = "Changes the Theme of the Menu Widgets.",
                .type = WIDGET_CVAR_COMBOBOX,
                .options = UIWidgets::ComboboxOptions{ .defaultIndex = UIWidgets::Color::DarkBlue },
                .comboMap = menuThemeOptions },
#if not defined(__SWITCH__) and not defined(__WIIU__)
              { .name = "Menu Controller Navigation",
                .cVar = CVAR_IMGUI_CONTROLLER_NAV,
                .tooltip = "Allows controller navigation of the 2Ship menu (Settings, Enhancements,...)\nCAUTION: "
                           "This will disable game inputs while the menu is visible.\n\nD-pad to move between "
                           "items, A to select, B to move up in scope.",
                .type = WIDGET_CVAR_CHECKBOX,
                .options = UIWidgets::CheckboxOptions{} },
              { .name = "Cursor Always Visible",
                .cVar = "gSettings.CursorVisibility",
                .tooltip = "Makes the cursor always visible, even in full screen.",
                .type = WIDGET_CVAR_CHECKBOX,
                .options = UIWidgets::CheckboxOptions{},
                .callback =
                    [](widgetInfo& info) {
                        Ship::Context::GetInstance()->GetWindow()->SetForceCursorVisibility(
                            CVarGetInteger("gSettings.CursorVisibility", 0));
                    } },
#endif
              { .name = "Search In Sidebar",
                .cVar = "gSettings.SidebarSearch",
                .tooltip = "Displays the Search menu as a sidebar entry in Settings instead of in the header.",
                .type = WIDGET_CVAR_CHECKBOX,
                .options = UIWidgets::CheckboxOptions{},
                .callback =
                    [](widgetInfo& info) {
                        if (CVarGetInteger("gSettings.SidebarSearch", 0)) {
                            mBenMenu->InsertSidebarSearch();
                        } else {
                            mBenMenu->RemoveSidebarSearch();
                        }
                    } },
              { .name = "Search Input Autofocus",
                .cVar = "gSettings.SearchAutofocus",
                .tooltip = "Search input box gets autofocus when visible. Does not affect using other widgets.",
                .type = WIDGET_CVAR_CHECKBOX,
                .options = UIWidgets::CheckboxOptions{} },
              { .name = "Alt Assets Tab hotkey",
                .cVar = "gEnhancements.Mods.AlternateAssetsHotkey",
                .tooltip = "Allows pressing the Tab key to toggle alternate assets",
                .type = WIDGET_CVAR_CHECKBOX,
                .options = UIWidgets::CheckboxOptions{ .defaultValue = true } },
              { .name = "Open App Files Folder",
                .cVar = "",
                .tooltip = "Opens the folder that contains the save and mods folders, etc.",
                .type = WIDGET_BUTTON,
                .options = UIWidgets::ButtonOptions{},
                .callback =
                    [](widgetInfo& info) {
                        std::string filesPath = Ship::Context::GetInstance()->GetAppDirectoryPath();
                        SDL_OpenURL(std::string("file:///" + std::filesystem::absolute(filesPath).string()).c_str());
                    } },
          } } });
    // Audio Settings
    settingsSidebar.push_back(
        { .label = "Audio",
          .columnCount = 3,
          .columnWidgets = {
              { { .name = "Master Volume: %.0f%%",
                  .cVar = "gSettings.Audio.MasterVolume",
                  .tooltip = "Adjust overall sound volume.",
                  .type = WIDGET_CVAR_SLIDER_FLOAT,
                  .options = UIWidgets::FloatSliderOptions{ .showButtons = false,
                                                            .format = "",
                                                            .min = 0.0f,
                                                            .max = 1.0f,
                                                            .defaultValue = 1.0f,
                                                            .isPercentage = true } },
                { .name = "Main Music Volume: %.0f%%",
                  .cVar = "gSettings.Audio.MainMusicVolume",
                  .tooltip = "Adjust the Background Music volume.",
                  .type = WIDGET_CVAR_SLIDER_FLOAT,
                  .options =
                      UIWidgets::FloatSliderOptions{
                          .showButtons = false,
                          .format = "",
                          .min = 0.0f,
                          .max = 1.0f,
                          .defaultValue = 1.0f,
                          .isPercentage = true,
                      },
                  .callback =
                      [](widgetInfo& info) {
                          AudioSeq_SetPortVolumeScale(SEQ_PLAYER_BGM_MAIN,
                                                      CVarGetFloat("gSettings.Audio.MainMusicVolume", 1.0f));
                      } },
                { .name = "Sub Music Volume: %.0f%%",
                  .cVar = "gSettings.Audio.SubMusicVolume",
                  .tooltip = "Adjust the Sub Music volume.",
                  .type = WIDGET_CVAR_SLIDER_FLOAT,
                  .options = UIWidgets::FloatSliderOptions{ .showButtons = false,
                                                            .format = "",
                                                            .min = 0.0f,
                                                            .max = 1.0f,
                                                            .defaultValue = 1.0f,
                                                            .isPercentage = true },
                  .callback =
                      [](widgetInfo& info) {
                          AudioSeq_SetPortVolumeScale(SEQ_PLAYER_BGM_SUB,
                                                      CVarGetFloat("gSettings.Audio.SubMusicVolume", 1.0f));
                      } },
                { .name = "Sound Effects Volume: %.0f%%",
                  .cVar = "gSettings.Audio.SoundEffectsVolume",
                  .tooltip = "Adjust the Sound Effects volume.",
                  .type = WIDGET_CVAR_SLIDER_FLOAT,
                  .options = UIWidgets::FloatSliderOptions{ .showButtons = false,
                                                            .format = "",
                                                            .min = 0.0f,
                                                            .max = 1.0f,
                                                            .defaultValue = 1.0f,
                                                            .isPercentage = true },
                  .callback =
                      [](widgetInfo& info) {
                          AudioSeq_SetPortVolumeScale(SEQ_PLAYER_SFX,
                                                      CVarGetFloat("gSettings.Audio.SoundEffectsVolume", 1.0f));
                      } },
                { .name = "Fanfare Volume: %.0f%%",
                  .cVar = "gSettings.Audio.FanfareVolume",
                  .tooltip = "Adjust the Fanfare volume.",
                  .type = WIDGET_CVAR_SLIDER_FLOAT,
                  .options = UIWidgets::FloatSliderOptions{ .showButtons = false,
                                                            .format = "",
                                                            .min = 0.0f,
                                                            .max = 1.0f,
                                                            .defaultValue = 1.0f,
                                                            .isPercentage = true },
                  .callback =
                      [](widgetInfo& info) {
                          AudioSeq_SetPortVolumeScale(SEQ_PLAYER_FANFARE,
                                                      CVarGetFloat("gSettings.Audio.FanfareVolume", 1.0f));
                      } },
                { .name = "Ambience Volume: %.0f%%",
                  .cVar = "gSettings.Audio.AmbienceVolume",
                  .tooltip = "Adjust the Ambient Sound volume.",
                  .type = WIDGET_CVAR_SLIDER_FLOAT,
                  .options = UIWidgets::FloatSliderOptions{ .showButtons = false,
                                                            .format = "",
                                                            .min = 0.0f,
                                                            .max = 1.0f,
                                                            .defaultValue = 1.0f,
                                                            .isPercentage = true },
                  .callback =
                      [](widgetInfo& info) {
                          AudioSeq_SetPortVolumeScale(SEQ_PLAYER_AMBIENCE,
                                                      CVarGetFloat("gSettings.Audio.AmbienceVolume", 1.0f));
                      } },
                { .name = "Audio API",
                  .cVar = NULL,
                  .tooltip = "Sets the audio API used by the game. Requires a relaunch to take effect.",
                  .type = WIDGET_AUDIO_BACKEND } } } });
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
        { .label = "Graphics",
          .columnCount = 3,
          .columnWidgets = {
              { { .name = "Toggle Fullscreen",
                  .cVar = "gSettings.Fullscreen",
                  .tooltip = "Toggles Fullscreen On/Off.",
                  .type = WIDGET_CVAR_CHECKBOX,
                  .options = UIWidgets::CheckboxOptions{},
                  .callback = [](widgetInfo& info) { Ship::Context::GetInstance()->GetWindow()->ToggleFullscreen(); } },
#ifndef __APPLE__
                { .name = "Internal Resolution: %.0f%%",
                  .cVar = CVAR_INTERNAL_RESOLUTION,
                  .tooltip =
                      "Multiplies your output resolution by the value inputted, as a more intensive but effective "
                      "form of anti-aliasing.",
                  .type = WIDGET_CVAR_SLIDER_FLOAT,
                  .options = UIWidgets::FloatSliderOptions{ .showButtons = false,
                                                            .format = "",
                                                            .min = 0.5f,
                                                            .max = 2.0f,
                                                            .defaultValue = 1.0f,
                                                            .isPercentage = true },
                  .callback =
                      [](widgetInfo& info) {
                          Ship::Context::GetInstance()->GetWindow()->SetResolutionMultiplier(
                              CVarGetFloat(CVAR_INTERNAL_RESOLUTION, 1));
                      } },
#endif
#ifndef __WIIU__
                { .name = "Anti-aliasing (MSAA): %d",
                  .cVar = CVAR_MSAA_VALUE,
                  .tooltip =
                      "Activates MSAA (multi-sample anti-aliasing) from 2x up to 8x, to smooth the edges of rendered "
                      "geometry.\n"
                      "Higher sample count will result in smoother edges on models, but may reduce performance.",
                  .type = WIDGET_CVAR_SLIDER_INT,
                  .options = UIWidgets::IntSliderOptions{ .min = 1, .max = 8, .defaultValue = 1 },
                  .callback =
                      [](widgetInfo& info) {
                          Ship::Context::GetInstance()->GetWindow()->SetMsaaLevel(CVarGetInteger(CVAR_MSAA_VALUE, 1));
                      } },
#endif

                { .name = "Current FPS: %d",
                  .cVar = "gInterpolationFPS",
                  .tooltip = tooltip,
                  .type = WIDGET_CVAR_SLIDER_INT,
                  .options = UIWidgets::IntSliderOptions{ .min = 20, .max = maxFps, .defaultValue = 20 },
                  .callback =
                      [](widgetInfo& info) {
                          int32_t defaultValue = std::get<UIWidgets::IntSliderOptions>(info.options).defaultValue;
                          if (CVarGetInteger(info.cVar, defaultValue) == defaultValue) {
                              info.name = "Current FPS: Original (%d)";
                          } else {
                              info.name = "Current FPS: %d";
                          }
                      },
                  .preFunc =
                      [](widgetInfo& info) {
                          if (mBenMenu->disabledMap.at(DISABLE_FOR_MATCH_REFRESH_RATE_ON).active)
                              info.activeDisables.push_back(DISABLE_FOR_MATCH_REFRESH_RATE_ON);
                      } },
                { .name = "Match Refresh Rate",
                  .tooltip = "Matches interpolation value to the current game's window refresh rate.",
                  .type = WIDGET_BUTTON,
                  .options = UIWidgets::ButtonOptions{},
                  .callback =
                      [](widgetInfo& info) {
                          int hz = Ship::Context::GetInstance()->GetWindow()->GetCurrentRefreshRate();
                          if (hz >= 20 && hz <= 360) {
                              CVarSetInteger("gInterpolationFPS", hz);
                              Ship::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesOnNextTick();
                          }
                      },
                  .preFunc =
                      [](widgetInfo& info) {
                          info.isHidden = mBenMenu->disabledMap.at(DISABLE_FOR_NOT_DIRECTX).active;
                      } },
                { .name = "Match Refresh Rate",
                  .cVar = "gMatchRefreshRate",
                  .tooltip = "Matches interpolation value to the current game's window refresh rate.",
                  .type = WIDGET_CVAR_CHECKBOX,
                  .options = UIWidgets::CheckboxOptions{},
                  .preFunc =
                      [](widgetInfo& info) { info.isHidden = mBenMenu->disabledMap.at(DISABLE_FOR_DIRECTX).active; } },
                { .name = "Jitter fix : >= % d FPS",
                  .cVar = "gExtraLatencyThreshold",
                  .tooltip = "When Interpolation FPS setting is at least this threshold, add one frame of input "
                             "lag (e.g. 16.6 ms for 60 FPS) in order to avoid jitter. This setting allows the "
                             "CPU to work on one frame while GPU works on the previous frame.\nThis setting "
                             "should be used when your computer is too slow to do CPU + GPU work in time.",
                  .type = WIDGET_CVAR_SLIDER_INT,
                  .options = UIWidgets::IntSliderOptions{ .min = 0, .max = 360, .defaultValue = 80 },
                  .preFunc =
                      [](widgetInfo& info) {
                          info.isHidden = mBenMenu->disabledMap.at(DISABLE_FOR_NOT_DIRECTX).active;
                      } },
                { .name = "Renderer API (Needs reload)",
                  .cVar = NULL,
                  .tooltip = "Sets the renderer API used by the game.",
                  .type = WIDGET_VIDEO_BACKEND,
                  .options = UIWidgets::CheckboxOptions{} },
                { .name = "Enable Vsync",
                  .cVar = CVAR_VSYNC_ENABLED,
                  .tooltip = "Enables Vsync.",
                  .type = WIDGET_CVAR_CHECKBOX,
                  .options = UIWidgets::CheckboxOptions{},
                  .preFunc =
                      [](widgetInfo& info) { info.isHidden = mBenMenu->disabledMap.at(DISABLE_FOR_NO_VSYNC).active; } },
                { .name = "Windowed Fullscreen",
                  .cVar = CVAR_SDL_WINDOWED_FULLSCREEN,
                  .tooltip = "Enables Windowed Fullscreen Mode.",
                  .type = WIDGET_CVAR_CHECKBOX,
                  .options = UIWidgets::CheckboxOptions{},
                  .preFunc =
                      [](widgetInfo& info) {
                          info.isHidden = mBenMenu->disabledMap.at(DISABLE_FOR_NO_WINDOWED_FULLSCREEN).active;
                      } },
                { .name = "Allow multi-windows",
                  .cVar = CVAR_ENABLE_MULTI_VIEWPORTS,
                  .tooltip = "Allows multiple windows to be opened at once. Requires a reload to take effect.",
                  .type = WIDGET_CVAR_CHECKBOX,
                  .options = UIWidgets::CheckboxOptions{},
                  .preFunc =
                      [](widgetInfo& info) {
                          info.isHidden = mBenMenu->disabledMap.at(DISABLE_FOR_NO_MULTI_VIEWPORT).active;
                      } },
                { .name = "Texture Filter (Needs reload)",
                  .cVar = CVAR_TEXTURE_FILTER,
                  .tooltip = "Sets the applied Texture Filtering.",
                  .type = WIDGET_CVAR_COMBOBOX,
                  .options = UIWidgets::ComboboxOptions{},
                  .comboMap = textureFilteringMap } } } });
    // Input Editor
    settingsSidebar.push_back(
        { .label = "Input Editor",
          .columnCount = 1,
          .columnWidgets = { { { .name = "Popout Input Editor",
                                 .cVar = "gWindows.BenInputEditor",
                                 .tooltip = "Enables the separate Input Editor window.",
                                 .type = WIDGET_WINDOW_BUTTON,
                                 .options = UIWidgets::ButtonOptions{ .size = UIWidgets::Sizes::Inline },
                                 .windowName = "2S2H Input Editor" } } } });

    settingsSidebar.push_back(
        { .label = "Notifications",
          .columnCount = 1,
          .columnWidgets = { {
              { .name = "Position",
                .cVar = "gNotifications.Position",
                .tooltip = "Which corner of the screen notifications appear in.",
                .type = WIDGET_CVAR_COMBOBOX,
                .options = UIWidgets::ComboboxOptions{ .defaultIndex = 3 },
                .comboMap = notificationPosition },
              { .name = "Duration: %.0f seconds",
                .cVar = "gNotifications.Duration",
                .tooltip = "How long notifications are displayed for.",
                .type = WIDGET_CVAR_SLIDER_FLOAT,
                .options =
                    UIWidgets::FloatSliderOptions{
                        .format = "%.1f", .step = 0.1f, .min = 3.0f, .max = 30.0f, .defaultValue = 10.0f } },
              { .name = "Background Opacity: %.0f%%",
                .cVar = "gNotifications.BgOpacity",
                .tooltip = "How opaque the background of notifications is.",
                .type = WIDGET_CVAR_SLIDER_FLOAT,
                .options =
                    UIWidgets::FloatSliderOptions{
                        .format = "%.0f%%", .min = 0.0f, .max = 1.0f, .defaultValue = 0.5f, .isPercentage = true } },
              { .name = "Size %.1f",
                .cVar = "gNotifications.Size",
                .tooltip = "How large notifications are.",
                .type = WIDGET_CVAR_SLIDER_FLOAT,
                .options =
                    UIWidgets::FloatSliderOptions{
                        .format = "%.1f", .step = 0.1f, .min = 1.0f, .max = 5.0f, .defaultValue = 1.8f } },
              { .name = "Test Notification",
                .tooltip = "Displays a test notification.",
                .type = WIDGET_BUTTON,
                .options = UIWidgets::ButtonOptions{},
                .callback =
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

void BenMenu::AddEnhancements() {
    // Camera Snap Fix
    enhancementsSidebar.push_back(
        { .label = "Camera",
          .columnCount = 3,
          .columnWidgets = {
              {
                  { .name = "Fixes", .type = WIDGET_SEPARATOR_TEXT, .options = UIWidgets::TextOptions{} },
                  { .name = "Fix Targeting Camera Snap",
                    .cVar = "gEnhancements.Camera.FixTargettingCameraSnap",
                    .tooltip = "Fixes the camera snap that occurs when you are moving and press the targeting button.",
                    .type = WIDGET_CVAR_CHECKBOX,
                    .options = UIWidgets::CheckboxOptions{} },
                  { .name = "First Person", .type = WIDGET_SEPARATOR_TEXT, .options = UIWidgets::TextOptions{} },
                  { .name = "Disable Auto-Centering",
                    .cVar = "gEnhancements.Camera.FirstPerson.DisableFirstPersonAutoCenterView",
                    .tooltip = "Disables the auto-centering of the camera in first person mode.",
                    .type = WIDGET_CVAR_CHECKBOX,
                    .options = UIWidgets::CheckboxOptions{},
                    .preFunc =
                        [](widgetInfo& info) {
                            if (mBenMenu->disabledMap.at(DISABLE_FOR_GYRO_ON).active)
                                info.activeDisables.push_back(DISABLE_FOR_GYRO_ON);
                        } },
                  { .name = "Invert X Axis",
                    .cVar = "gEnhancements.Camera.FirstPerson.InvertX",
                    .tooltip = "Inverts the X Axis of the Camera in First Person Mode.",
                    .type = WIDGET_CVAR_CHECKBOX,
                    .options = UIWidgets::CheckboxOptions{} },
                  { .name = "Invert Y Axis",
                    .cVar = "gEnhancements.Camera.FirstPerson.InvertY",
                    .tooltip = "Inverts the Y Axis of the Camera in First Person Mode.",
                    .type = WIDGET_CVAR_CHECKBOX,
                    .options = UIWidgets::CheckboxOptions{ .defaultValue = true } },
                  { .name = "X Axis Sensitivity: %.0f%%",
                    .cVar = "gEnhancements.Camera.FirstPerson.SensitivityX",
                    .tooltip = "Adjusts the Sensitivity of the X Axis in First Person Mode.",
                    .type = WIDGET_CVAR_SLIDER_FLOAT,
                    .options = UIWidgets::FloatSliderOptions{ .format = "%.0f%%",
                                                              .min = 0.01f,
                                                              .max = 2.0f,
                                                              .defaultValue = 1.0f,
                                                              .isPercentage = true } },
                  { .name = "Y Axis Sensitivity: %.0f%%",
                    .cVar = "gEnhancements.Camera.FirstPerson.SensitivityY",
                    .tooltip = "Adjusts the Sensitivity of the Y Axis in First Person Mode.",
                    .type = WIDGET_CVAR_SLIDER_FLOAT,
                    .options = UIWidgets::FloatSliderOptions{ .format = "%.0f%%",
                                                              .min = 0.01f,
                                                              .max = 2.0f,
                                                              .defaultValue = 1.0f,
                                                              .isPercentage = true } },
                  { .name = "Gyro Aiming",
                    .cVar = "gEnhancements.Camera.FirstPerson.GyroEnabled",
                    .tooltip = "Enables Gyro Aiming in First Person Mode.",
                    .type = WIDGET_CVAR_CHECKBOX,
                    .options = UIWidgets::CheckboxOptions{} },
                  { .name = "Invert Gyro X Axis",
                    .cVar = "gEnhancements.Camera.FirstPerson.GyroInvertX",
                    .tooltip = "Inverts the X Axis of the Gyro in First Person Mode.",
                    .type = WIDGET_CVAR_CHECKBOX,
                    .options = UIWidgets::CheckboxOptions{},
                    .preFunc =
                        [](widgetInfo& info) {
                            info.isHidden = mBenMenu->disabledMap.at(DISABLE_FOR_GYRO_OFF).active;
                        } },
                  { .name = "Invert Gyro Y Axis",
                    .cVar = "gEnhancements.Camera.FirstPerson.GyroInvertY",
                    .tooltip = "Inverts the Y Axis of the Gyro in First Person Mode.",
                    .type = WIDGET_CVAR_CHECKBOX,
                    .options = UIWidgets::CheckboxOptions{},
                    .preFunc =
                        [](widgetInfo& info) {
                            info.isHidden = mBenMenu->disabledMap.at(DISABLE_FOR_GYRO_OFF).active;
                        } },
                  { .name = "Gyro X Axis Sensitivity: %.0f%%",
                    .cVar = "gEnhancements.Camera.FirstPerson.GyroSensitivityX",
                    .tooltip = "Adjusts the Sensitivity of the X Axis of the Gyro in First Person Mode.",
                    .type = WIDGET_CVAR_SLIDER_FLOAT,
                    .options =
                        UIWidgets::FloatSliderOptions{
                            .format = "%.0f%%", .min = 0.01f, .max = 2.0f, .defaultValue = 1.0f, .isPercentage = true },
                    .preFunc =
                        [](widgetInfo& info) {
                            info.isHidden = mBenMenu->disabledMap.at(DISABLE_FOR_GYRO_OFF).active;
                        } },
                  { .name = "Gyro Y Axis Sensitivity: %.0f%%",
                    .cVar = "gEnhancements.Camera.FirstPerson.GyroSensitivityY",
                    .tooltip = "Adjusts the Sensitivity of the Y Axis of the Gyro in First Person Mode.",
                    .type = WIDGET_CVAR_SLIDER_FLOAT,
                    .options =
                        UIWidgets::FloatSliderOptions{
                            .format = "%.0f%%", .min = 0.01f, .max = 2.0f, .defaultValue = 1.0f, .isPercentage = true },
                    .preFunc =
                        [](widgetInfo& info) {
                            info.isHidden = mBenMenu->disabledMap.at(DISABLE_FOR_GYRO_OFF).active;
                        } },
                  { .name = "Right Stick Aiming",
                    .cVar = "gEnhancements.Camera.FirstPerson.RightStickEnabled",
                    .tooltip = "Enables Right Stick Aiming in First Person Mode.",
                    .type = WIDGET_CVAR_CHECKBOX,
                    .options = UIWidgets::CheckboxOptions{} },
                  { .name = "Move while aiming",
                    .cVar = "gEnhancements.Camera.FirstPerson.MoveInFirstPerson",
                    .tooltip = "Allows movement with the left stick while in first person mode.",
                    .type = WIDGET_CVAR_CHECKBOX,
                    .options = UIWidgets::CheckboxOptions{},
                    .preFunc =
                        [](widgetInfo& info) {
                            if (mBenMenu->disabledMap.at(DISABLE_FOR_RIGHT_STICK_OFF).active)
                                info.activeDisables.push_back(DISABLE_FOR_RIGHT_STICK_OFF);
                        } },
                  { .name = "Invert Right Stick X Axis",
                    .cVar = "gEnhancements.Camera.FirstPerson.RightStickInvertX",
                    .tooltip = "Inverts the X Axis of the Right Stick in First Person Mode.",
                    .type = WIDGET_CVAR_CHECKBOX,
                    .options = UIWidgets::CheckboxOptions{},
                    .preFunc =
                        [](widgetInfo& info) {
                            info.isHidden = mBenMenu->disabledMap.at(DISABLE_FOR_RIGHT_STICK_OFF).active;
                        } },
                  { .name = "Invert Right Stick Y Axis",
                    .cVar = "gEnhancements.Camera.FirstPerson.RightStickInvertY",
                    .tooltip = "Inverts the Y Axis of the Right Stick in First Person Mode.",
                    .type = WIDGET_CVAR_CHECKBOX,
                    .options = UIWidgets::CheckboxOptions{ .defaultValue = true },
                    .preFunc =
                        [](widgetInfo& info) {
                            info.isHidden = mBenMenu->disabledMap.at(DISABLE_FOR_RIGHT_STICK_OFF).active;
                        } },
                  { .name = "Right Stick X Axis Sensitivity: %.0f%%",
                    .cVar = "gEnhancements.Camera.FirstPerson.RightStickSensitivityX",
                    .tooltip = "Adjusts the Sensitivity of the X Axis of the Right Stick in First Person Mode.",
                    .type = WIDGET_CVAR_SLIDER_FLOAT,
                    .options =
                        UIWidgets::FloatSliderOptions{
                            .format = "%.0f%%", .min = 0.01f, .max = 2.0f, .defaultValue = 1.0f, .isPercentage = true },
                    .preFunc =
                        [](widgetInfo& info) {
                            info.isHidden = mBenMenu->disabledMap.at(DISABLE_FOR_RIGHT_STICK_OFF).active;
                        } },
                  { .name = "Right Stick Y Axis Sensitivity: %.0f%%",
                    .cVar = "gEnhancements.Camera.FirstPerson.RightStickSensitivityY",
                    .tooltip = "Adjusts the Sensitivity of the Y Axis of the Right Stick in First Person Mode.",
                    .type = WIDGET_CVAR_SLIDER_FLOAT,
                    .options =
                        UIWidgets::FloatSliderOptions{
                            .format = "%.0f%%", .min = 0.01f, .max = 2.0f, .defaultValue = 1.0f, .isPercentage = true },
                    .preFunc =
                        [](widgetInfo& info) {
                            info.isHidden = mBenMenu->disabledMap.at(DISABLE_FOR_RIGHT_STICK_OFF).active;
                        } },
              },
              // Camera Enhancements
              { { .name = "Cameras", .type = WIDGET_SEPARATOR_TEXT, .options = UIWidgets::TextOptions{} },
                { .name = "Free Look",
                  .cVar = "gEnhancements.Camera.FreeLook.Enable",
                  .tooltip = "Enables free look camera control\nNote: You must remap C buttons off of the right "
                             "stick in the controller config menu, and map the camera stick to the right stick.",
                  .type = WIDGET_CVAR_CHECKBOX,
                  .options = UIWidgets::CheckboxOptions{},
                  .preFunc =
                      [](widgetInfo& info) {
                          if (mBenMenu->disabledMap.at(DISABLE_FOR_DEBUG_CAM_ON).active)
                              info.activeDisables.push_back(DISABLE_FOR_DEBUG_CAM_ON);
                      } },
                { .name = "Camera Distance: %d",
                  .cVar = "gEnhancements.Camera.FreeLook.MaxCameraDistance",
                  .tooltip = "Maximum Camera Distance for Free Look.",
                  .type = WIDGET_CVAR_SLIDER_INT,
                  .options = UIWidgets::IntSliderOptions{ .min = 100, .max = 900, .defaultValue = 185 },
                  .preFunc =
                      [](widgetInfo& info) {
                          info.isHidden = mBenMenu->disabledMap.at(DISABLE_FOR_FREE_LOOK_OFF).active;
                      } },
                { .name = "Camera Transition Speed: %d",
                  .cVar = "gEnhancements.Camera.FreeLook.TransitionSpeed",
                  .tooltip = "Can someone help me?",
                  .type = WIDGET_CVAR_SLIDER_INT,
                  .options = UIWidgets::IntSliderOptions{ .min = 1, .max = 900, .defaultValue = 25 },
                  .preFunc =
                      [](widgetInfo& info) {
                          info.isHidden = mBenMenu->disabledMap.at(DISABLE_FOR_FREE_LOOK_OFF).active;
                      } },
                { .name = "Max Camera Height Angle: %.0f\xC2\xB0",
                  .cVar = "gEnhancements.Camera.FreeLook.MaxPitch",
                  .tooltip = "Maximum Height of the Camera.",
                  .type = WIDGET_CVAR_SLIDER_FLOAT,
                  .options =
                      UIWidgets::FloatSliderOptions{
                          .format = "%.0f\xC2\xB0", .min = -89.0f, .max = 89.0f, .defaultValue = 72.0f },
                  .callback = [](widgetInfo& info) { FreeLookPitchMinMax(); },
                  .preFunc =
                      [](widgetInfo& info) {
                          info.isHidden = mBenMenu->disabledMap.at(DISABLE_FOR_FREE_LOOK_OFF).active;
                      } },
                { .name = "Min Camera Height Angle: %.0f\xC2\xB0",
                  .cVar = "gEnhancements.Camera.FreeLook.MinPitch",
                  .tooltip = "Minimum Height of the Camera.",
                  .type = WIDGET_CVAR_SLIDER_FLOAT,
                  .options =
                      UIWidgets::FloatSliderOptions{
                          .format = "%.0f\xC2\xB0", .min = -89.0f, .max = 89.0f, .defaultValue = -49.0f },
                  .callback = [](widgetInfo& info) { FreeLookPitchMinMax(); },
                  .preFunc =
                      [](widgetInfo& info) {
                          info.isHidden = mBenMenu->disabledMap.at(DISABLE_FOR_FREE_LOOK_OFF).active;
                      } },
                { .name = "Debug Camera",
                  .cVar = "gEnhancements.Camera.DebugCam.Enable",
                  .tooltip = "Enables free camera control.",
                  .type = WIDGET_CVAR_CHECKBOX,
                  .options = UIWidgets::CheckboxOptions{},
                  .preFunc =
                      [](widgetInfo& info) {
                          if (mBenMenu->disabledMap.at(DISABLE_FOR_FREE_LOOK_ON).active) {
                              info.activeDisables.push_back(DISABLE_FOR_FREE_LOOK_ON);
                          }
                      } },
                { .name = "Invert Camera X Axis",
                  .cVar = "gEnhancements.Camera.RightStick.InvertXAxis",
                  .tooltip = "Inverts the Camera X Axis",
                  .type = WIDGET_CVAR_CHECKBOX,
                  .options = UIWidgets::CheckboxOptions{},
                  .preFunc =
                      [](widgetInfo& info) {
                          if (mBenMenu->disabledMap.at(DISABLE_FOR_CAMERAS_OFF).active) {
                              info.activeDisables.push_back(DISABLE_FOR_CAMERAS_OFF);
                          }
                      } },
                { .name = "Invert Camera Y Axis",
                  .cVar = "gEnhancements.Camera.RightStick.InvertYAxis",
                  .tooltip = "Inverts the Camera Y Axis",
                  .type = WIDGET_CVAR_CHECKBOX,
                  .options = UIWidgets::CheckboxOptions{},
                  .preFunc =
                      [](widgetInfo& info) {
                          if (mBenMenu->disabledMap.at(DISABLE_FOR_CAMERAS_OFF).active) {
                              info.activeDisables.push_back(DISABLE_FOR_CAMERAS_OFF);
                          }
                      } },
                { .name = "Third-Person Horizontal Sensitivity: %.0f",
                  .cVar = "gEnhancements.Camera.RightStick.CameraSensitivity.X",
                  .tooltip = "Adjust the Sensitivity of the x axis when in Third Person.",
                  .type = WIDGET_CVAR_SLIDER_FLOAT,
                  .options = UIWidgets::FloatSliderOptions{ .format = "%.0f%%",
                                                            .min = 0.01f,
                                                            .max = 5.0f,
                                                            .defaultValue = 1.0f,
                                                            .isPercentage = true },
                  .preFunc =
                      [](widgetInfo& info) {
                          if (mBenMenu->disabledMap.at(DISABLE_FOR_CAMERAS_OFF).active) {
                              info.activeDisables.push_back(DISABLE_FOR_CAMERAS_OFF);
                          }
                      } },
                { .name = "Third-Person Vertical Sensitivity: %.0f",
                  .cVar = "gEnhancements.Camera.RightStick.CameraSensitivity.Y",
                  .tooltip = "Adjust the Sensitivity of the x axis when in Third Person.",
                  .type = WIDGET_CVAR_SLIDER_FLOAT,
                  .options = UIWidgets::FloatSliderOptions{ .format = "%.0f%%",
                                                            .min = 0.01f,
                                                            .max = 5.0f,
                                                            .defaultValue = 1.0f,
                                                            .isPercentage = true },
                  .preFunc =
                      [](widgetInfo& info) {
                          if (mBenMenu->disabledMap.at(DISABLE_FOR_CAMERAS_OFF).active) {
                              info.activeDisables.push_back(DISABLE_FOR_CAMERAS_OFF);
                          }
                      } },
                { .name = "Enable Roll (6\xC2\xB0 of Freedom)",
                  .cVar = "gEnhancements.Camera.DebugCam.6DOF",
                  .tooltip = "This allows for all six degrees of movement with the camera, NOTE: Yaw will work "
                             "differently in this system, instead rotating around the focal point"
                             ", rather than a polar axis.",
                  .type = WIDGET_CVAR_CHECKBOX,
                  .options = UIWidgets::CheckboxOptions{},
                  .preFunc =
                      [](widgetInfo& info) {
                          info.isHidden = mBenMenu->disabledMap.at(DISABLE_FOR_DEBUG_CAM_OFF).active;
                      } },
                { .name = "Camera Speed: %.0f",
                  .cVar = "gEnhancements.Camera.DebugCam.CameraSpeed",
                  .tooltip = "Adjusts the speed of the Camera.",
                  .type = WIDGET_CVAR_SLIDER_FLOAT,
                  .options =
                      UIWidgets::FloatSliderOptions{
                          .format = "%.0f%%", .min = 0.1f, .max = 3.0f, .defaultValue = 0.5f, .isPercentage = true },
                  .preFunc = [](widgetInfo& info) {
                      info.isHidden = mBenMenu->disabledMap.at(DISABLE_FOR_DEBUG_CAM_OFF).active;
                  } } } } });
    // Cheats
    enhancementsSidebar.push_back(
        { .label = "Cheats",
          .columnCount = 3,
          .columnWidgets = {
              { { .name = "Infinite Health",
                  .cVar = "gCheats.InfiniteHealth",
                  .tooltip = "Always have full Hearts.",
                  .type = WIDGET_CVAR_CHECKBOX,
                  .options = UIWidgets::CheckboxOptions{} },
                { .name = "Infinite Magic",
                  .cVar = "gCheats.InfiniteMagic",
                  .tooltip = "Always have full Magic.",
                  .type = WIDGET_CVAR_CHECKBOX,
                  .options = UIWidgets::CheckboxOptions{} },
                { .name = "Infinite Rupees",
                  .cVar = "gCheats.InfiniteRupees",
                  .tooltip = "Always have a full Wallet.",
                  .type = WIDGET_CVAR_CHECKBOX,
                  .options = UIWidgets::CheckboxOptions{} },
                { .name = "Infinite Consumables",
                  .cVar = "gCheats.InfiniteConsumables",
                  .tooltip = "Always have max Consumables, you must have collected the consumables first.",
                  .type = WIDGET_CVAR_CHECKBOX,
                  .options = UIWidgets::CheckboxOptions{} },
                { .name = "Longer Deku Flower Glide",
                  .cVar = "gCheats.LongerFlowerGlide",
                  .tooltip = "Allows Deku Link to glide longer, no longer dropping after a certain distance.",
                  .type = WIDGET_CVAR_CHECKBOX,
                  .options = UIWidgets::CheckboxOptions{} },
                { .name = "No Clip",
                  .cVar = "gCheats.NoClip",
                  .tooltip = "Allows Link to phase through collision.",
                  .type = WIDGET_CVAR_CHECKBOX,
                  .options = UIWidgets::CheckboxOptions{} },
                { .name = "Unbreakable Razor Sword",
                  .cVar = "gCheats.UnbreakableRazorSword",
                  .tooltip = "Allows to Razor Sword to be used indefinitely without dulling its blade.",
                  .type = WIDGET_CVAR_CHECKBOX,
                  .options = UIWidgets::CheckboxOptions{} },
                { .name = "Unrestricted Items",
                  .cVar = "gCheats.UnrestrictedItems",
                  .tooltip = "Allows all Forms to use all Items.",
                  .type = WIDGET_CVAR_CHECKBOX,
                  .options = UIWidgets::CheckboxOptions{} },
                { .name = "Hookshot Anywhere",
                  .cVar = "gCheats.HookshotAnywhere",
                  .tooltip = "Allows most surfaces to be hookshot-able",
                  .type = WIDGET_CVAR_CHECKBOX,
                  .options = UIWidgets::CheckboxOptions{} },
                { .name = "Moon Jump on L",
                  .cVar = "gCheats.MoonJumpOnL",
                  .tooltip = "Holding L makes you float into the air.",
                  .type = WIDGET_CVAR_CHECKBOX,
                  .options = UIWidgets::CheckboxOptions{} },
                { .name = "Elegy of Emptiness Anywhere",
                  .cVar = "gCheats.ElegyAnywhere",
                  .tooltip = "Allows Elegy of Emptiness outside of Ikana",
                  .type = WIDGET_CVAR_CHECKBOX,
                  .options = UIWidgets::CheckboxOptions{} },
                { .name = "Stop Time in Dungeons",
                  .cVar = "gCheats.TempleTimeStop",
                  .tooltip = "Stops time from advancing in selected areas. Requires a room change to update.\n\n"
                             "- Off: Vanilla behaviour.\n"
                             "- Temples: Stops time in Woodfall, Snowhead, Great Bay, and Stone Tower Temples.\n"
                             "- Temples + Mini Dungeons: In addition to the above temples, stops time in both Spider "
                             "Houses, Pirate's Fortress, Beneath the Well, Ancient Castle of Ikana, and Secret Shrine.",
                  .type = WIDGET_CVAR_COMBOBOX,
                  .options = UIWidgets::CheckboxOptions{},
                  .comboMap = timeStopOptions } } } });
    // Gameplay Enhancements
    enhancementsSidebar.push_back(
        { .label = "Gameplay",
          .columnCount = 3,
          .columnWidgets = {
              { { .name = "Player", .type = WIDGET_SEPARATOR_TEXT, .options = UIWidgets::TextOptions{} },
                { .name = "Fast Deku Flower Launch",
                  .cVar = "gEnhancements.Player.FastFlowerLaunch",
                  .tooltip = "Speeds up the time it takes to be able to get maximum height from launching out of a "
                             "deku flower",
                  .type = WIDGET_CVAR_CHECKBOX,
                  .options = UIWidgets::CheckboxOptions{} },
                { .name = "Instant Putaway",
                  .cVar = "gEnhancements.Player.InstantPutaway",
                  .tooltip = "Allows Link to instantly puts away held item without waiting.",
                  .type = WIDGET_CVAR_CHECKBOX,
                  .options = UIWidgets::CheckboxOptions{} },
                { .name = "Fierce Deity Putaway",
                  .cVar = "gEnhancements.Player.FierceDeityPutaway",
                  .tooltip = "Allows Fierce Deity Link to put away his sword.",
                  .type = WIDGET_CVAR_CHECKBOX,
                  .options = UIWidgets::CheckboxOptions{} },
                { .name = "Climb speed",
                  .cVar = "gEnhancements.Player.ClimbSpeed",
                  .tooltip = "Increases the speed at which Link climbs vines and ladders.",
                  .type = WIDGET_CVAR_SLIDER_INT,
                  .options = UIWidgets::IntSliderOptions{ .min = 1, .max = 5, .defaultValue = 1 } },
                { .name = "Dpad Equips",
                  .cVar = "gEnhancements.Dpad.DpadEquips",
                  .tooltip = "Allows you to equip items to your d-pad",
                  .type = WIDGET_CVAR_CHECKBOX,
                  .options = UIWidgets::CheckboxOptions{} },
                { .name = "Always Win Doggy Race",
                  .cVar = "gEnhancements.Minigames.AlwaysWinDoggyRace",
                  .tooltip = "Makes the Doggy Race easier to win.",
                  .type = WIDGET_CVAR_COMBOBOX,
                  .options = UIWidgets::ComboboxOptions{},
                  .comboMap = alwaysWinDoggyraceOptions },
                { .name = "Milk Run Reward Options",
                  .cVar = "gEnhancements.Minigames.CremiaHugs",
                  .tooltip = "Choose what reward you get for winning the Milk Run minigame after the first time. \n"
                             "-Vanilla: Reward is Random\n"
                             "-Hug: Get the hugging cutscene\n"
                             "-Rupee: Get the rupee reward",
                  .type = WIDGET_CVAR_COMBOBOX,
                  .options = UIWidgets::ComboboxOptions{},
                  .comboMap = cremiaRewardOptions },
                { .name = "Cucco Shack Cucco Count",
                  .cVar = "gEnhancements.Minigames.CuccoShackCuccoCount",
                  .tooltip = "Choose how many cuccos you need to raise to make Grog happy.",
                  .type = WIDGET_CVAR_SLIDER_INT,
                  .options = UIWidgets::IntSliderOptions{ .min = 1, .max = 10, .defaultValue = 10 } },
                { .name = "Swordsman School Winning Score",
                  .cVar = "gEnhancements.Minigames.SwordsmanSchoolScore",
                  .tooltip = "Sets the score required to win the Swordsman School.",
                  .type = WIDGET_CVAR_SLIDER_INT,
                  .options = UIWidgets::IntSliderOptions{ .min = 1, .max = 30, .defaultValue = 30 } },
                { .name = "Fast Magic Arrow Equip Animation",
                  .cVar = "gEnhancements.Equipment.MagicArrowEquipSpeed",
                  .tooltip = "Removes the animation for equipping Magic Arrows.",
                  .type = WIDGET_CVAR_CHECKBOX },
                { .name = "Instant Fin Boomerangs Recall",
                  .cVar = "gEnhancements.PlayerActions.InstantRecall",
                  .tooltip =
                      "Pressing B will instantly recall the fin boomerang back to Zora Link after they are thrown.",
                  .type = WIDGET_CVAR_CHECKBOX,
                  .options = UIWidgets::CheckboxOptions{} },
                { .name = "Two-Handed Sword Spin Attack",
                  .cVar = "gEnhancements.Equipment.TwoHandedSwordSpinAttack",
                  .tooltip = "Enables magic spin attacks for the Fierce Deity Sword and Great Fairy's Sword.",
                  .type = WIDGET_CVAR_CHECKBOX,
                  .options = UIWidgets::CheckboxOptions{} } },
              { { .name = "Modes", .type = WIDGET_SEPARATOR_TEXT, .options = UIWidgets::TextOptions{} },
                { .name = "Play as Kafei",
                  .cVar = "gModes.PlayAsKafei",
                  .tooltip = "Requires scene reload to take effect.",
                  .type = WIDGET_CVAR_CHECKBOX,
                  .options = UIWidgets::CheckboxOptions{} },
                { .name = "Hyrule Warriors Styled Link",
                  .cVar = "gModes.HyruleWarriorsStyledLink",
                  .tooltip = "When acquired, places the Keaton and Fierce Deity masks on Link similarly to how he "
                             "wears them in "
                             "Hyrule Warriors",
                  .type = WIDGET_CVAR_CHECKBOX,
                  .options = UIWidgets::CheckboxOptions{} },
                { .name = "Time Moves when you Move",
                  .cVar = "gModes.TimeMovesWhenYouMove",
                  .tooltip = "Time only moves when Link is not standing still.",
                  .type = WIDGET_CVAR_CHECKBOX,
                  .options = UIWidgets::CheckboxOptions{} },
                { .name = "Mirrored World",
                  .cVar = "gModes.MirroredWorld.Mode",
                  .tooltip = "Mirrors the world horizontally.",
                  .type = WIDGET_CVAR_CHECKBOX,
                  .options = UIWidgets::CheckboxOptions{},
                  .callback =
                      [](widgetInfo& info) {
                          if (CVarGetInteger("gModes.MirroredWorld.Mode", 0)) {
                              CVarSetInteger("gModes.MirroredWorld.State", 1);
                          } else {
                              CVarClear("gModes.MirroredWorld.State");
                          }
                      } } },
              { { .name = "Saving", .type = WIDGET_SEPARATOR_TEXT, .options = UIWidgets::TextOptions{} },
                { .name = "Persistent Owl Saves",
                  .cVar = "gEnhancements.Saving.PersistentOwlSaves",
                  .tooltip = "Continuing a save will not remove the owl save. Playing Song of "
                             "Time, allowing the moon to crash or finishing the "
                             "game will remove the owl save and become the new last save.",
                  .type = WIDGET_CVAR_CHECKBOX,
                  .options = UIWidgets::CheckboxOptions{} },
                { .name = "Pause Menu Save",
                  .cVar = "gEnhancements.Saving.PauseSave",
                  .tooltip = "Re-introduce the pause menu save system. Pressing B in the pause menu will give you the "
                             "option to create a persistent Owl Save from your current location.\n\nWhen loading back "
                             "into the game, you will be placed either at the entrance of the dungeon you saved in, or "
                             "in South Clock Town.",
                  .type = WIDGET_CVAR_CHECKBOX,
                  .options = UIWidgets::CheckboxOptions{} },
                { .name = "Autosave",
                  .cVar = "gEnhancements.Saving.Autosave",
                  .tooltip = "Automatically create a persistent Owl Save on the chosen interval.\n\nWhen loading "
                             "back into the game, you will be placed either at the entrance of the dungeon you "
                             "saved in, or in South Clock Town.",
                  .type = WIDGET_CVAR_CHECKBOX,
                  .options = UIWidgets::CheckboxOptions{},
                  .callback = [](widgetInfo& info) { RegisterAutosave(); } },
                { .name = "Autosave Interval: %d minutes",
                  .cVar = "gEnhancements.Saving.AutosaveInterval",
                  .tooltip = "Sets the interval between Autosaves.",
                  .type = WIDGET_CVAR_SLIDER_INT,
                  .options = UIWidgets::IntSliderOptions{ .min = 1, .max = 60, .defaultValue = 5 },
                  .preFunc =
                      [](widgetInfo& info) {
                          if (mBenMenu->disabledMap.at(DISABLE_FOR_AUTO_SAVE_OFF).active) {
                              info.activeDisables.push_back(DISABLE_FOR_AUTO_SAVE_OFF);
                          }
                      } },
                { .name = "Time Cycle", .type = WIDGET_SEPARATOR_TEXT, .options = UIWidgets::TextOptions{} },
                { .name = "Do not reset Bottle content",
                  .cVar = "gEnhancements.Cycle.DoNotResetBottleContent",
                  .tooltip = "Playing the Song Of Time will not reset the bottles' content.",
                  .type = WIDGET_CVAR_CHECKBOX,
                  .options = UIWidgets::CheckboxOptions{} },
                { .name = "Do not reset Consumables",
                  .cVar = "gEnhancements.Cycle.DoNotResetConsumables",
                  .tooltip = "Playing the Song Of Time will not reset the consumables.",
                  .type = WIDGET_CVAR_CHECKBOX,
                  .options = UIWidgets::CheckboxOptions{} },
                { .name = "Do not reset Razor Sword",
                  .cVar = "gEnhancements.Cycle.DoNotResetRazorSword",
                  .tooltip = "Playing the Song Of Time will not reset the Sword back to Kokiri Sword.",
                  .type = WIDGET_CVAR_CHECKBOX,
                  .options = UIWidgets::CheckboxOptions{} },
                { .name = "Do not reset Rupees",
                  .cVar = "gEnhancements.Cycle.DoNotResetRupees",
                  .tooltip = "Playing the Song Of Time will not reset the your rupees.",
                  .type = WIDGET_CVAR_CHECKBOX,
                  .options = UIWidgets::CheckboxOptions{} },
                { .name = "Do not reset Time Speed",
                  .cVar = "gEnhancements.Cycle.DoNotResetTimeSpeed",
                  .tooltip =
                      "Playing the Song Of Time will not reset the current time speed set by Inverted Song of Time.",
                  .type = WIDGET_CVAR_CHECKBOX,
                  .options = UIWidgets::CheckboxOptions{} },
                { .name = "Keep Express Mail",
                  .cVar = "gEnhancements.Cycle.KeepExpressMail",
                  .tooltip = "Allows the player to keep the Express Mail in their inventory after delivering it "
                             "the first time, so that both deliveries can be done within one cycle",
                  .type = WIDGET_CVAR_CHECKBOX,
                  .options = UIWidgets::CheckboxOptions{},
                  .callback = [](widgetInfo& info) { RegisterKeepExpressMail(); } },
                { .name = "Unstable",
                  .type = WIDGET_SEPARATOR_TEXT,
                  .options = UIWidgets::TextOptions{ .color = UIWidgets::Color::Orange } },
                { .name = "Disable Save Delay",
                  .cVar = "gEnhancements.Saving.DisableSaveDelay",
                  .tooltip = "Removes the arbitrary 2 second timer for saving from the original game. This is known to "
                             "cause issues when attempting the 0th Day Glitch",
                  .type = WIDGET_CVAR_CHECKBOX,
                  .options = UIWidgets::CheckboxOptions{} } } } });
    // Graphics Enhancements
    enhancementsSidebar.push_back(
        { .label = "Graphics",
          .columnCount = 3,
          .columnWidgets = { {
              { .name = "Clock", .type = WIDGET_SEPARATOR_TEXT, .options = UIWidgets::TextOptions{} },
              { .name = "Clock Type",
                .cVar = "gEnhancements.Graphics.ClockType",
                .tooltip = "Swaps between Graphical and Text only Clock types.",
                .type = WIDGET_CVAR_COMBOBOX,
                .options = UIWidgets::ComboboxOptions{},
                .comboMap = clockTypeOptions },
              { .name = "24 Hours Clock",
                .cVar = "gEnhancements.Graphics.24HoursClock",
                .tooltip = "Changes from a 12 Hour to a 24 Hour Clock",
                .type = WIDGET_CVAR_CHECKBOX,
                .options = UIWidgets::CheckboxOptions{} },
              { .name = "Mods", .type = WIDGET_SEPARATOR_TEXT, .options = UIWidgets::TextOptions{} },
              { .name = "Use Alternate Assets",
                .cVar = "gEnhancements.Mods.AlternateAssets",
                .tooltip = "Toggle between standard assets and alternate assets. Usually mods will indicate if "
                           "this setting has to be used or not.",
                .type = WIDGET_CVAR_CHECKBOX,
                .options = UIWidgets::CheckboxOptions{} },
              { .name = "Motion Blur", .type = WIDGET_SEPARATOR_TEXT, .options = UIWidgets::TextOptions{} },
              { .name = "Motion Blur Mode",
                .cVar = "gEnhancements.Graphics.MotionBlur.Mode",
                .tooltip = "Selects the Mode for Motion Blur.",
                .type = WIDGET_CVAR_COMBOBOX,
                .options = UIWidgets::ComboboxOptions{ .labelPosition = UIWidgets::LabelPosition::None },
                .comboMap = motionBlurOptions },
              { .name = "Interpolate",
                .cVar = "gEnhancements.Graphics.MotionBlur.Interpolate",
                .tooltip =
                    "Change motion blur capture to also happen on interpolated frames instead of only on game frames.\n"
                    "This notably reduces the overall motion blur strength but smooths out the trails.",
                .type = WIDGET_CVAR_CHECKBOX,
                .options = UIWidgets::CheckboxOptions{},
                .preFunc =
                    [](widgetInfo& info) {
                        info.isHidden = mBenMenu->disabledMap.at(DISABLE_FOR_MOTION_BLUR_MODE).value == 1;
                    } },
              { .name = "On/Off",
                .tooltip = "Enables Motion Blur.",
                .type = WIDGET_CHECKBOX,
                .options = UIWidgets::CheckboxOptions{},
                .valuePointer = (bool*)&R_MOTION_BLUR_ENABLED,
                .preFunc =
                    [](widgetInfo& info) {
                        info.valuePointer = (bool*)&R_MOTION_BLUR_ENABLED;
                        info.isHidden = mBenMenu->disabledMap.at(DISABLE_FOR_MOTION_BLUR_MODE).value != 0;
                    } },
              { .name = "Strength",
                .cVar = "gEnhancements.Graphics.MotionBlur.Strength",
                .tooltip = "Motion Blur strength.",
                .type = WIDGET_CVAR_SLIDER_INT,
                .options = UIWidgets::IntSliderOptions{ .min = 0, .max = 255, .defaultValue = 180 },
                .preFunc =
                    [](widgetInfo& info) {
                        info.isHidden = mBenMenu->disabledMap.at(DISABLE_FOR_MOTION_BLUR_MODE).value != 2;
                    } },
              { .name = "Strength",
                .tooltip = "Motion Blur strength.",
                .type = WIDGET_SLIDER_INT,
                .options = UIWidgets::IntSliderOptions{ .min = 0, .max = 255, .defaultValue = 180 },
                .valuePointer = &motionBlurStrength,
                .callback = [](widgetInfo& info) { R_MOTION_BLUR_ALPHA = motionBlurStrength; },
                .preFunc =
                    [](widgetInfo& info) {
                        motionBlurStrength = R_MOTION_BLUR_ALPHA;
                        info.isHidden = mBenMenu->disabledMap.at(DISABLE_FOR_MOTION_BLUR_MODE).value != 0 ||
                                        mBenMenu->disabledMap.at(DISABLE_FOR_MOTION_BLUR_OFF).active;
                    } },
              { .name = "Other", .type = WIDGET_SEPARATOR_TEXT, .options = UIWidgets::TextOptions{} },
              { .name = "3D Item Drops",
                .cVar = "gEnhancements.Graphics.3DItemDrops",
                .tooltip = "Makes item drops 3D",
                .type = WIDGET_CVAR_CHECKBOX,
                .options = UIWidgets::CheckboxOptions{} },
              { .name = "Authentic Logo",
                .cVar = "gEnhancements.Graphics.AuthenticLogo",
                .tooltip = "Hide the game version and build details and display the authentic "
                           "model and texture on the boot logo start screen",
                .type = WIDGET_CVAR_CHECKBOX,
                .options = UIWidgets::CheckboxOptions{} },
              { .name = "Bow Reticle",
                .cVar = "gEnhancements.Graphics.BowReticle",
                .tooltip = "Gives the bow a reticle when you draw an arrow.",
                .type = WIDGET_CVAR_CHECKBOX,
                .options = UIWidgets::CheckboxOptions{} },
              { .name = "Disable Black Bar Letterboxes",
                .cVar = "gEnhancements.Graphics.DisableBlackBars",
                .tooltip = "Disables Black Bar Letterboxes during cutscenes and Z-targeting\nNote: there may be "
                           "minor visual glitches that were covered up by the black bars\nPlease disable this "
                           "setting before reporting a bug.",
                .type = WIDGET_CVAR_CHECKBOX,
                .options = UIWidgets::CheckboxOptions{} },
              { .name = "Unstable",
                .type = WIDGET_SEPARATOR_TEXT,
                .options = UIWidgets::TextOptions{ .color = UIWidgets::Color::Orange } },
              { .name = "Disable Scene Geometry Distance Check",
                .cVar = "gEnhancements.Graphics.DisableSceneGeometryDistanceCheck",
                .tooltip = "Disables the distance check for scene geometry, allowing it to be drawn no matter how far "
                           "away it is from the player. This may have unintended side effects.",
                .type = WIDGET_CVAR_CHECKBOX,
                .options = UIWidgets::CheckboxOptions{},
                .callback = [](widgetInfo& info) { GfxPatcher_ApplyGeometryIssuePatches(); } },
              { .name = "Widescreen Actor Culling",
                .cVar = "gEnhancements.Graphics.ActorCullingAccountsForWidescreen",
                .tooltip = "Adjusts the culling planes to account for widescreen resolutions. "
                           "This may have unintended side effects.",
                .type = WIDGET_CVAR_CHECKBOX,
                .options = UIWidgets::CheckboxOptions{} },
              { .name = "Increase Actor Draw Distance: %dx",
                .cVar = "gEnhancements.Graphics.IncreaseActorDrawDistance",
                .tooltip = "Increase the range in which Actors are drawn. This may have unintended side effects.",
                .type = WIDGET_CVAR_SLIDER_INT,
                .options = UIWidgets::IntSliderOptions{ .min = 1, .max = 5, .defaultValue = 1 } },
          } } });
    enhancementsSidebar.push_back(
        { .label = "Items/Songs",
          .columnCount = 3,
          .columnWidgets = {
              // Mask Enhancements
              { { .name = "Masks", .type = WIDGET_SEPARATOR_TEXT, .options = UIWidgets::TextOptions{} },
                { .name = "Blast Mask has Powder Keg Force",
                  .cVar = "gEnhancements.Masks.BlastMaskKeg",
                  .tooltip = "Blast Mask can also destroy objects only the Powder Keg can.",
                  .type = WIDGET_CVAR_CHECKBOX,
                  .options = UIWidgets::CheckboxOptions{} },
                { .name = "Fast Transformation",
                  .cVar = "gEnhancements.Masks.FastTransformation",
                  .tooltip = "Removes the delay when using transformation masks.",
                  .type = WIDGET_CVAR_CHECKBOX,
                  .options = UIWidgets::CheckboxOptions{} },
                { .name = "Fierce Deity's Mask Anywhere",
                  .cVar = "gEnhancements.Masks.FierceDeitysAnywhere",
                  .tooltip = "Allow using Fierce Deity's mask outside of boss rooms.",
                  .type = WIDGET_CVAR_CHECKBOX,
                  .options = UIWidgets::CheckboxOptions{} },
                { .name = "Persistent Bunny Hood",
                  .cVar = "gEnhancements.Masks.PersistentBunnyHood.Enabled",
                  .tooltip = "Permanently toggle a speed boost from the bunny hood by pressing "
                             "'A' on it in the mask menu.",
                  .type = WIDGET_CVAR_CHECKBOX,
                  .options = UIWidgets::CheckboxOptions{} },
                { .name = "No Blast Mask Cooldown",
                  .cVar = "gEnhancements.Masks.NoBlastMaskCooldown",
                  .tooltip = "Eliminates the Cooldown between Blast Mask usage.",
                  .type = WIDGET_CVAR_CHECKBOX,
                  .options = UIWidgets::CheckboxOptions{} } },
              // Song Enhancements
              { { .name = "Ocarina", .type = WIDGET_SEPARATOR_TEXT, .options = UIWidgets::TextOptions{} },
                { .name = "Better Song of Double Time",
                  .cVar = "gEnhancements.Songs.BetterSongOfDoubleTime",
                  .tooltip = "When playing the Song of Double Time, you can now choose the exact time you want to go "
                             "to, similar to "
                             "the 3DS version.",
                  .type = WIDGET_CVAR_CHECKBOX,
                  .options = UIWidgets::CheckboxOptions{} },
                { .name = "Enable Sun's Song",
                  .cVar = "gEnhancements.Songs.EnableSunsSong",
                  .tooltip = "Enables the partially implemented Sun's Song. RIGHT-DOWN-UP-RIGHT-DOWN-UP to play it. "
                             "This song will make time move very fast until either Link moves to a different scene, "
                             "or when the time switches to a new time period.",
                  .type = WIDGET_CVAR_CHECKBOX,
                  .options = UIWidgets::CheckboxOptions{} },
                { .name = "Dpad Ocarina",
                  .cVar = "gEnhancements.Playback.DpadOcarina",
                  .tooltip = "Enables using the Dpad for Ocarina playback.",
                  .type = WIDGET_CVAR_CHECKBOX,
                  .options = UIWidgets::CheckboxOptions{} },
                { .name = "Pause Owl Warp",
                  .cVar = "gEnhancements.Songs.PauseOwlWarp",
                  .tooltip = "Allows warping to registered Owl Statues from the pause menu map screen. "
                             "Requires that you can play Song of Soaring normally.\n\n"
                             "Accounts for Index-Warp being active, by presenting all valid warps for the registered "
                             "map points. "
                             "Great Bay Coast warp is always given for index 0 warp as a convenience.",
                  .type = WIDGET_CVAR_CHECKBOX,
                  .options = UIWidgets::CheckboxOptions{} },
                { .name = "Zora Eggs For Bossa Nova",
                  .cVar = "gEnhancements.Songs.ZoraEggCount",
                  .tooltip = "The number of eggs required to unlock new wave bossa nova.",
                  .type = WIDGET_CVAR_SLIDER_INT,
                  .options = UIWidgets::IntSliderOptions{ .min = 1, .max = 7, .defaultValue = 7 } },
                { .name = "Prevent Dropped Ocarina Inputs",
                  .cVar = "gEnhancements.Playback.NoDropOcarinaInput",
                  .tooltip = "Prevent dropping inputs when playing the ocarina quickly.",
                  .type = WIDGET_CVAR_CHECKBOX,
                  .options = UIWidgets::CheckboxOptions{} },
                { .name = "Skip Scarecrow Song",
                  .cVar = "gEnhancements.Playback.SkipScarecrowSong",
                  .tooltip = "Pierre appears when the Ocarina is pulled out.",
                  .type = WIDGET_CVAR_CHECKBOX,
                  .options = UIWidgets::CheckboxOptions{} },
                { .name = "Faster Song Playback",
                  .cVar = "gEnhancements.Songs.FasterSongPlayback",
                  .tooltip = "Speeds up the playback of songs.",
                  .type = WIDGET_CVAR_CHECKBOX,
                  .options = UIWidgets::CheckboxOptions{} } } } });
    enhancementsSidebar.push_back(
        { .label = "Time Savers",
          .columnCount = 3,
          .columnWidgets = {
              // Cutscene Skips
              { { .name = "Cutscenes", .type = WIDGET_SEPARATOR_TEXT, .options = UIWidgets::TextOptions{} },
                { .name = "Hide Title Cards",
                  .cVar = "gEnhancements.Cutscenes.HideTitleCards",
                  .tooltip = "Hides Title Cards when entering areas.",
                  .type = WIDGET_CVAR_CHECKBOX,
                  .options = UIWidgets::CheckboxOptions{} },
                { .name = "Skip Entrance Cutscenes",
                  .cVar = "gEnhancements.Cutscenes.SkipEntranceCutscenes",
                  .tooltip = "Skip cutscenes that occur when first entering a new area.",
                  .type = WIDGET_CVAR_CHECKBOX,
                  .options = UIWidgets::CheckboxOptions{} },
                { .name = "Skip to File Select",
                  .cVar = "gEnhancements.Cutscenes.SkipToFileSelect",
                  .tooltip = "Skip the opening title sequence and go straight to the file select menu after boot.",
                  .type = WIDGET_CVAR_CHECKBOX,
                  .options = UIWidgets::CheckboxOptions{} },
                { .name = "Skip Intro Sequence",
                  .cVar = "gEnhancements.Cutscenes.SkipIntroSequence",
                  .tooltip = "When starting a game you will be taken straight to South Clock Town as Deku Link.",
                  .type = WIDGET_CVAR_CHECKBOX,
                  .options = UIWidgets::CheckboxOptions{} },
                { .name = "Skip First Cycle",
                  .cVar = "gEnhancements.Cutscenes.SkipFirstCycle",
                  .tooltip = "When starting a game you will be taken straight to South Clock Town as Human Link "
                             "with Deku Mask, Ocarina, Song of Time, and Song of Healing.",
                  .type = WIDGET_CVAR_CHECKBOX,
                  .options = UIWidgets::CheckboxOptions{},
                  .preFunc =
                      [](widgetInfo& info) {
                          info.isHidden = mBenMenu->disabledMap.at(DISABLE_FOR_INTRO_SKIP_OFF).active;
                      } },
                { .name = "Skip Story Cutscenes",
                  .cVar = "gEnhancements.Cutscenes.SkipStoryCutscenes",
                  .tooltip =
                      "Disclaimer: This doesn't do much yet, we will be progressively adding more skips over time.",
                  .type = WIDGET_CVAR_CHECKBOX,
                  .options = UIWidgets::CheckboxOptions{} },
                { .name = "Skip Misc Interactions",
                  .cVar = "gEnhancements.Cutscenes.SkipMiscInteractions",
                  .tooltip =
                      "Disclaimer: This doesn't do much yet, we will be progressively adding more skips over time.",
                  .type = WIDGET_CVAR_CHECKBOX,
                  .options = UIWidgets::CheckboxOptions{} } },
              // Dialogue Enhancements
              { { .name = "Dialogue", .type = WIDGET_SEPARATOR_TEXT, .options = UIWidgets::TextOptions{} },
                { .name = "Fast Bank Selection",
                  .cVar = "gEnhancements.Dialogue.FastBankSelection",
                  .tooltip =
                      "Pressing the Z or R buttons while the Deposit/Withdrawal Rupees dialogue is open will set "
                      "the Rupees to Links current Rupees or 0 respectively.",
                  .type = WIDGET_CVAR_CHECKBOX,
                  .options = UIWidgets::CheckboxOptions{} },
                { .name = "Fast Text",
                  .cVar = "gEnhancements.Dialogue.FastText",
                  .tooltip = "Speeds up text rendering, and enables holding of B progress to next message.",
                  .type = WIDGET_CVAR_CHECKBOX,
                  .options = UIWidgets::CheckboxOptions{} } } } });
    enhancementsSidebar.push_back(
        { .label = "Fixes",
          .columnCount = 3,
          .columnWidgets = {
              // Fixes
              { { .name = "Fixes", .type = WIDGET_SEPARATOR_TEXT, .options = UIWidgets::TextOptions{} },
                { .name = "Fix Ammo Count Color",
                  .cVar = "gFixes.FixAmmoCountEnvColor",
                  .tooltip = "Fixes a missing gDPSetEnvColor, which causes the ammo count to be "
                             "the wrong color prior to obtaining magic or other conditions.",
                  .type = WIDGET_CVAR_CHECKBOX,
                  .options = UIWidgets::CheckboxOptions{} },
                { .name = "Fix Fierce Deity Z-Target movement",
                  .cVar = "gEnhancements.Fixes.FierceDeityZTargetMovement",
                  .tooltip = "Fixes Fierce Deity movement being choppy when Z-targeting",
                  .type = WIDGET_CVAR_CHECKBOX,
                  .options = UIWidgets::CheckboxOptions{} },
                { .name = "Fix Hess and Weirdshot Crash",
                  .cVar = "gEnhancements.Fixes.HessCrash",
                  .tooltip = "Fixes a crash that can occur when performing a HESS or Weirdshot.",
                  .type = WIDGET_CVAR_CHECKBOX,
                  .options = UIWidgets::CheckboxOptions{} },
                { .name = "Fix Text Control Characters",
                  .cVar = "gEnhancements.Fixes.ControlCharacters",
                  .tooltip = "Fixes certain control characters not functioning properly "
                             "depending on their position within the text.",
                  .type = WIDGET_CVAR_CHECKBOX,
                  .options = UIWidgets::CheckboxOptions{} },
                { .name = "Fix Ikana Great Fairy Fountain Color",
                  .cVar = "gFixes.FixIkanaGreatFairyFountainColor",
                  .tooltip = "Fixes a bug that results in the Ikana Great Fairy fountain looking green instead of "
                             "yellow, this was "
                             "fixed in the EU version",
                  .type = WIDGET_CVAR_CHECKBOX,
                  .options = UIWidgets::CheckboxOptions{} },
                { .name = "Fix Texture overflow OOB",
                  .cVar = "gEnhancements.Fixes.FixTexturesOOB",
                  .tooltip = "Fixes textures that normally overflow to be patched with the correct size or format",
                  .type = WIDGET_CVAR_CHECKBOX,
                  .options = UIWidgets::CheckboxOptions{ .defaultValue = true },
                  .callback = [](widgetInfo& info) { GfxPatcher_ApplyOverflowTexturePatches(); } },
                { .name = "Fix Completed Heart Container Audio",
                  .cVar = "gEnhancements.Fixes.CompletedHeartContainerAudio",
                  .tooltip =
                      "Fixes a bug that results in the wrong audio playing upon receiving a 4th piece of heart to "
                      "fill a new heart container.",
                  .type = WIDGET_CVAR_CHECKBOX,
                  .options = UIWidgets::CheckboxOptions{} } } } });
    enhancementsSidebar.push_back(
        { .label = "Restorations",
          .columnCount = 3,
          .columnWidgets = {
              // Restorations
              { { .name = "Restorations", .type = WIDGET_SEPARATOR_TEXT, .options = UIWidgets::TextOptions{} },
                { .name = "Constant Distance Backflips and Sidehops",
                  .cVar = "gEnhancements.Restorations.ConstantFlipsHops",
                  .tooltip = "Backflips and Sidehops travel a constant distance as they did in OoT.",
                  .type = WIDGET_CVAR_CHECKBOX,
                  .options = UIWidgets::CheckboxOptions{} },
                { .name = "Power Crouch Stab",
                  .cVar = "gEnhancements.Restorations.PowerCrouchStab",
                  .tooltip =
                      "Crouch stabs will use the power of Link's previous melee attack, as is in MM JP 1.0 and OoT.",
                  .type = WIDGET_CVAR_CHECKBOX,
                  .options = UIWidgets::CheckboxOptions{} },
                { .name = "Side Rolls",
                  .cVar = "gEnhancements.Restorations.SideRoll",
                  .tooltip = "Restores side rolling from OoT.",
                  .type = WIDGET_CVAR_CHECKBOX,
                  .options = UIWidgets::CheckboxOptions{} },
                { .name = "Tatl ISG",
                  .cVar = "gEnhancements.Restorations.TatlISG",
                  .tooltip = "Restores Navi ISG from OoT, but now with Tatl.",
                  .type = WIDGET_CVAR_CHECKBOX,
                  .options = UIWidgets::CheckboxOptions{} },
                { .name = "Woodfall Mountain Appearance",
                  .cVar = "gEnhancements.Restorations.WoodfallMountainAppearance",
                  .tooltip = "Restores the appearance of Woodfall mountain to not look poisoned "
                             "when viewed from Termina Field after clearing Woodfall Temple\n\n"
                             "Requires a scene reload to take effect",
                  .type = WIDGET_CVAR_CHECKBOX,
                  .options = UIWidgets::CheckboxOptions{} } } } });

    enhancementsSidebar.push_back(
        { .label = "Difficulty Options",
          .columnCount = 3,
          .columnWidgets = {
              { { .name = "Disable Takkuri Steal",
                  .cVar = "gEnhancements.Cheats.DisableTakkuriSteal",
                  .tooltip = "Prevents the Takkuri from stealing key items like bottles and swords. It may still steal "
                             "other items.",
                  .type = WIDGET_CVAR_CHECKBOX,
        .options = UIWidgets::CheckboxOptions{} },
                { .name = "Deku Guard Search Balls",
                  .cVar = "gEnhancements.Cheats.DekuGuardSearchBalls",
                  .tooltip = "Choose when to show the Deku Palace Guards' search balls\n"
                             "- Never: Never show the search balls. This matches Majora's Mask 3D behaviour\n"
                             "- Night Only: Only show the search balls at night. This matches original N64 behaviour.\n"
                             "- Always: Always show the search balls.",
                  .type = WIDGET_CVAR_COMBOBOX,
                  .options =
                      UIWidgets::ComboboxOptions{ .defaultIndex =
                                                      DekuGuardSearchBallsOptions::DEKU_GUARD_SEARCH_BALLS_NIGHT_ONLY },
                  .comboMap = dekuGuardSearchBallsOptions } } } });
    // HUD Editor
    enhancementsSidebar.push_back(
        { .label = "HUD Editor",
          .columnCount = 1,
          .columnWidgets = { { { .name = "Popout HUD Editor",
                                 .cVar = "gWindows.HudEditor",
                                 .tooltip = "Enables the HUD Editor window, allowing you to modify your HUD",
                                 .type = WIDGET_WINDOW_BUTTON,
                                 .options = UIWidgets::ButtonOptions{ .size = UIWidgets::Sizes::Inline },
                                 .windowName = "HUD Editor" } } } });
    // Item Tracker Settings
    enhancementsSidebar.push_back(
        { .label = "Item Tracker",
          .columnCount = 1,
          .columnWidgets = { { { .name = "Popout Item Tracker Settings",
                                 .cVar = "gWindows.ItemTrackerSettings",
                                 .type = WIDGET_WINDOW_BUTTON,
                                 .options = UIWidgets::ButtonOptions{ .size = UIWidgets::Sizes::Inline },
                                 .windowName = "Item Tracker Settings" } } } });
}

void BenMenu::AddDevTools() {
    devToolsSidebar.push_back(
        { .label = "General",
          .columnCount = 3,
          .columnWidgets = {
              { { .name = "Popout Menu",
                  .cVar = "gSettings.Menu.Popout",
                  .tooltip = "Changes the menu display from overlay to windowed.",
                  .type = WIDGET_CVAR_CHECKBOX,
                  .options = UIWidgets::CheckboxOptions{} },
                { .name = "Set Warp Point",
                  .tooltip = "Creates warp point that you can teleport to later",
                  .type = WIDGET_BUTTON,
                  .options = UIWidgets::ButtonOptions{},
                  .callback =
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
                  .preFunc =
                      [](widgetInfo& info) {
                          info.isHidden = mBenMenu->disabledMap.at(DISABLE_FOR_NULL_PLAY_STATE).active;
                      } },
                { .name = "Scene Room ID",
                  .type = WIDGET_TEXT,
                  .options = UIWidgets::TextOptions{},
                  .preFunc =
                      [](widgetInfo& info) {
                          u32 sceneId = Entrance_GetSceneIdAbsolute(
                              CVarGetInteger(WARP_POINT_CVAR "Entrance", ENTRANCE(SOUTH_CLOCK_TOWN, 0)));
                          info.name = fmt::format("{} Room {}", warpPointSceneList[sceneId],
                                                  CVarGetInteger(WARP_POINT_CVAR "Room", 0));
                          info.isHidden = mBenMenu->disabledMap.at(DISABLE_FOR_NULL_PLAY_STATE).active ||
                                          mBenMenu->disabledMap.at(DISABLE_FOR_WARP_POINT_NOT_SET).active;
                      } },
                { .name = ICON_FA_TIMES,
                  .tooltip = "Clear warp point",
                  .type = WIDGET_BUTTON,
                  .options = UIWidgets::ButtonOptions{ .size = UIWidgets::Sizes::Inline },
                  .callback =
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
                  .preFunc =
                      [](widgetInfo& info) {
                          info.isHidden = mBenMenu->disabledMap.at(DISABLE_FOR_NULL_PLAY_STATE).active ||
                                          mBenMenu->disabledMap.at(DISABLE_FOR_WARP_POINT_NOT_SET).active;
                      },
                  .sameLine = true },
                { .name = "Warp",
                  .tooltip = "Teleport to the set warp point",
                  .type = WIDGET_BUTTON,
                  .options = UIWidgets::ButtonOptions{ .size = UIWidgets::Sizes::Inline },
                  .callback = [](widgetInfo& info) { Warp(); },
                  .preFunc =
                      [](widgetInfo& info) {
                          info.isHidden = mBenMenu->disabledMap.at(DISABLE_FOR_NULL_PLAY_STATE).active ||
                                          mBenMenu->disabledMap.at(DISABLE_FOR_WARP_POINT_NOT_SET).active;
                      },
                  .sameLine = true } },
              { { .name = "Debug Mode",
                  .cVar = "gDeveloperTools.DebugEnabled",
                  .tooltip = "Enables Debug Mode, allowing you to select maps with L + R + Z.",
                  .type = WIDGET_CVAR_CHECKBOX,
                  .options = UIWidgets::CheckboxOptions{},
                  .callback =
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
                { .name = "Better Map Select",
                  .cVar = "gDeveloperTools.BetterMapSelect.Enabled",
                  .tooltip = "Overrides the original map select with a translated, more user-friendly version.",
                  .type = WIDGET_CVAR_CHECKBOX,
                  .options = UIWidgets::CheckboxOptions{},
                  .preFunc =
                      [](widgetInfo& info) {
                          info.isHidden = mBenMenu->disabledMap.at(DISABLE_FOR_DEBUG_MODE_OFF).active;
                      } },
                { .name = "Debug Save File Mode",
                  .cVar = "gDeveloperTools.DebugSaveFileMode",
                  .tooltip = "Change the behavior of creating saves while debug mode is enabled:\n\n"
                             "- Empty Save: The default 3 heart save file in first cycle\n"
                             "- Vanilla Debug Save: Uses the title screen save info (8 hearts, all items and masks)\n"
                             "- 100\% Save: All items, equipment, mask, quest status and bombers notebook complete",
                  .type = WIDGET_CVAR_COMBOBOX,
                  .options = UIWidgets::ComboboxOptions{},
                  .comboMap = debugSaveOptions,
                  .callback = [](widgetInfo& info) { RegisterDebugSaveCreate(); },
                  .preFunc =
                      [](widgetInfo& info) {
                          info.isHidden = mBenMenu->disabledMap.at(DISABLE_FOR_DEBUG_MODE_OFF).active;
                      } },
                { .name = "Prevent Actor Update",
                  .cVar = "gDeveloperTools.PreventActorUpdate",
                  .tooltip = "Prevents Actors from updating.",
                  .type = WIDGET_CVAR_CHECKBOX,
                  .options = UIWidgets::ButtonOptions{},
                  .callback = [](widgetInfo& info) { RegisterPreventActorUpdateHooks(); },
                  .preFunc =
                      [](widgetInfo& info) {
                          info.isHidden = mBenMenu->disabledMap.at(DISABLE_FOR_DEBUG_MODE_OFF).active;
                      } },
                { .name = "Prevent Actor Draw",
                  .cVar = "gDeveloperTools.PreventActorDraw",
                  .tooltip = "Prevents Actors from drawing.",
                  .type = WIDGET_CVAR_CHECKBOX,
                  .options = UIWidgets::CheckboxOptions{},
                  .callback = [](widgetInfo& info) { RegisterPreventActorDrawHooks(); },
                  .preFunc =
                      [](widgetInfo& info) {
                          info.isHidden = mBenMenu->disabledMap.at(DISABLE_FOR_DEBUG_MODE_OFF).active;
                      } },
                { .name = "Prevent Actor Init",
                  .cVar = "gDeveloperTools.PreventActorInit",
                  .tooltip = "Prevents Actors from initializing.",
                  .type = WIDGET_CVAR_CHECKBOX,
                  .options = UIWidgets::CheckboxOptions{},
                  .callback = [](widgetInfo& info) { RegisterPreventActorInitHooks(); },
                  .preFunc =
                      [](widgetInfo& info) {
                          info.isHidden = mBenMenu->disabledMap.at(DISABLE_FOR_DEBUG_MODE_OFF).active;
                      } },
                { .name = "Disable Object Dependency",
                  .cVar = "gDeveloperTools.DisableObjectDependency",
                  .tooltip = "Disables dependencies when loading objects.",
                  .type = WIDGET_CVAR_CHECKBOX,
                  .options = UIWidgets::ButtonOptions{},
                  .preFunc =
                      [](widgetInfo& info) {
                          info.isHidden = mBenMenu->disabledMap.at(DISABLE_FOR_DEBUG_MODE_OFF).active;
                      } },
                { .name = "Log Level",
                  .cVar = "gDeveloperTools.LogLevel",
                  .tooltip = "The log level determines which messages are printed to the "
                             "console. This does not affect the log file output",
                  .type = WIDGET_CVAR_COMBOBOX,
                  .options = UIWidgets::ComboboxOptions{},
                  .comboMap = logLevels,
                  .callback =
                      [](widgetInfo& info) {
                          Ship::Context::GetInstance()->GetLogger()->set_level(
                              (spdlog::level::level_enum)CVarGetInteger("gDeveloperTools.LogLevel", 1));
                      },
                  .preFunc =
                      [](widgetInfo& info) {
                          info.isHidden = mBenMenu->disabledMap.at(DISABLE_FOR_DEBUG_MODE_OFF).active;
                      } },
                { .name = "Frame Advance",
                  .tooltip = "This allows you to advance through the game one frame at a time on command. "
                             "To advance a frame, hold Z and tap R on the second controller. Holding Z "
                             "and R will advance a frame every half second. You can also use the buttons below.",
                  .type = WIDGET_CHECKBOX,
                  .options = UIWidgets::CheckboxOptions{},
                  .preFunc =
                      [](widgetInfo& info) {
                          info.isHidden = mBenMenu->disabledMap.at(DISABLE_FOR_NULL_PLAY_STATE).active ||
                                          mBenMenu->disabledMap.at(DISABLE_FOR_DEBUG_MODE_OFF).active;
                          if (gPlayState != nullptr) {
                              info.valuePointer = (bool*)&gPlayState->frameAdvCtx.enabled;
                          } else {
                              info.valuePointer = (bool*)nullptr;
                          }
                      } },
                { .name = "Advance 1",
                  .tooltip = "Advance 1 frame.",
                  .type = WIDGET_BUTTON,
                  .options = UIWidgets::ButtonOptions{ .size = UIWidgets::Sizes::Inline },
                  .callback = [](widgetInfo& info) { CVarSetInteger("gDeveloperTools.FrameAdvanceTick", 1); },
                  .preFunc =
                      [](widgetInfo& info) {
                          info.isHidden = mBenMenu->disabledMap.at(DISABLE_FOR_FRAME_ADVANCE_OFF).active ||
                                          mBenMenu->disabledMap.at(DISABLE_FOR_DEBUG_MODE_OFF).active;
                      } },
                { .name = "Advance (Hold)",
                  .tooltip = "Advance frames while the button is held.",
                  .type = WIDGET_BUTTON,
                  .options = UIWidgets::ButtonOptions{ .size = UIWidgets::Sizes::Inline },
                  .preFunc =
                      [](widgetInfo& info) {
                          info.isHidden = mBenMenu->disabledMap.at(DISABLE_FOR_FRAME_ADVANCE_OFF).active ||
                                          mBenMenu->disabledMap.at(DISABLE_FOR_DEBUG_MODE_OFF).active;
                      },
                  .postFunc =
                      [](widgetInfo& info) {
                          if (ImGui::IsItemActive()) {
                              CVarSetInteger("gDeveloperTools.FrameAdvanceTick", 1);
                          }
                      },
                  .sameLine = true } } } });
    // dev tools windows
    devToolsSidebar.push_back(
        { .label = "Collision Viewer",
          .columnCount = 1,
          .columnWidgets = { { { .name = "Popout Collision Viewer",
                                 .cVar = "gWindows.CollisionViewer",
                                 .tooltip = "Makes collision visible on screen",
                                 .type = WIDGET_WINDOW_BUTTON,
                                 .options = UIWidgets::ButtonOptions{ .size = UIWidgets::Sizes::Inline },
                                 .windowName = "Collision Viewer" } } } });
    devToolsSidebar.push_back(
        { .label = "Stats",
          .columnCount = 1,
          .columnWidgets = {
              { { .name = "Popout Stats",
                  .cVar = "gOpenWindows.Stats",
                  .tooltip = "Shows the stats window, with your FPS and frametimes, and the OS you're playing on",
                  .type = WIDGET_WINDOW_BUTTON,
                  .options = UIWidgets::ButtonOptions{ .size = UIWidgets::Sizes::Inline },
                  .windowName = "Stats" } } } });
    devToolsSidebar.push_back(
        { .label = "Console",
          .columnCount = 1,
          .columnWidgets = {
              { { .name = "Popout Console",
                  .cVar = "gOpenWindows.Console",
                  .tooltip = "Enables the console window, allowing you to input commands. Type help for some examples",
                  .type = WIDGET_WINDOW_BUTTON,
                  .options = UIWidgets::ButtonOptions{ .size = UIWidgets::Sizes::Inline },
                  .windowName = "Console" } } } });
    devToolsSidebar.push_back(
        { .label = "Gfx Debugger",
          .columnCount = 1,
          .columnWidgets = {
              { { .name = "Popout Gfx Debugger",
                  .cVar = "gOpenWindows.GfxDebugger",
                  .tooltip =
                      "Enables the Gfx Debugger window, allowing you to input commands, type help for some examples",
                  .type = WIDGET_WINDOW_BUTTON,
                  .options = UIWidgets::ButtonOptions{ .size = UIWidgets::Sizes::Inline },
                  .windowName = "GfxDebuggerWindow" } } } });
    devToolsSidebar.push_back(
        { .label = "Save Editor",
          .columnCount = 1,
          .columnWidgets = { { { .name = "Popout Save Editor",
                                 .cVar = "gWindows.SaveEditor",
                                 .tooltip = "Enables the Save Editor window, allowing you to edit your save file",
                                 .type = WIDGET_WINDOW_BUTTON,
                                 .options = UIWidgets::ButtonOptions{ .size = UIWidgets::Sizes::Inline },
                                 .windowName = "Save Editor" } } } });
    devToolsSidebar.push_back(
        { .label = "Actor Viewer",
          .columnCount = 1,
          .columnWidgets = {
              { { .name = "Popout Actor Viewer",
                  .cVar = "gWindows.ActorViewer",
                  .tooltip = "Enables the Actor Viewer window, allowing you to view actors in the world.",
                  .type = WIDGET_WINDOW_BUTTON,
                  .options = UIWidgets::ButtonOptions{ .size = UIWidgets::Sizes::Inline },
                  .windowName = "Actor Viewer" } } } });
    devToolsSidebar.push_back(
        { .label = "Event Log",
          .columnCount = 1,
          .columnWidgets = { { { .name = "Popout Event Log",
                                 .cVar = "gWindows.EventLog",
                                 .tooltip = "Enables the event log window",
                                 .type = WIDGET_WINDOW_BUTTON,
                                 .options = UIWidgets::ButtonOptions{ .size = UIWidgets::Sizes::Inline },
                                 .windowName = "Event Log" } } } });
}

BenMenu::BenMenu(const std::string& consoleVariable, const std::string& name) : Menu(consoleVariable, name) {
}

void BenMenu::InitElement() {
    Ship::Menu::InitElement();
    AddSettings();
    AddEnhancements();
    AddDevTools();

    menuEntries = { { "Settings", settingsSidebar, "gSettings.Menu.SettingsSidebarIndex" },
                    { "Enhancements", enhancementsSidebar, "gSettings.Menu.EnhancementsSidebarIndex" },
                    { "Developer Tools", devToolsSidebar, "gSettings.Menu.DevToolsSidebarIndex" } };

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
