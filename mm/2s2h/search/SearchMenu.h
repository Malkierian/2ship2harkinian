#include "2s2h/Enhancements/Enhancements.h"

extern "C" {
#include "functions.h"
extern PlayState* gPlayState;
}

using WidgetFunc = void (*)();
std::string disabledTempTooltip;
const char* disabledTooltip;
bool disabledValue = false;

std::vector<ImVec4> menuTheme = {
    ImVec4(1.0f, 1.0f, 1.0f, 1.0f),    ImVec4(0.4f, 0.4f, 0.4f, 1.0f), ImVec4(0.1f, 0.1f, 0.1f, 1.0f),
    ImVec4(0.24f, 0.31f, 0.71f, 1.0f), ImVec4(0.5f, 0.0f, 0.0f, 1.0f), ImVec4(0.3f, 0.0f, 0.0f, 1.0f),
    ImVec4(0.0f, 0.7f, 0.0f, 1.0f),    ImVec4(0.0f, 0.5f, 0.0f, 1.0f), ImVec4(0.0f, 0.3f, 0.0f, 1.0f),
    ImVec4(1.0f, 0.627f, 0.0f, 1.0f),
};

typedef enum {
    COLOR_WHITE,
    COLOR_GRAY,
    COLOR_DARK_GRAY,
    COLOR_INDIGO,
    COLOR_RED,
    COLOR_DARK_RED,
    COLOR_LIGHT_GREEN,
    COLOR_GREEN,
    COLOR_DARK_GREEN,
    COLOR_YELLOW,
} colorOptions;

typedef enum {
    CHECKBOX,
    COMBOBOX,
    SLIDER_INT,
    SLIDER_FLOAT,
    BUTTON,
};

typedef enum {
    MOTION_BLUR_DYNAMIC,
    MOTION_BLUR_ALWAYS_OFF,
    MOTION_BLUR_ALWAYS_ON,
};

typedef enum {
    DEBUG_LOG_TRACE,
    DEBUG_LOG_DEBUG,
    DEBUG_LOG_INFO,
    DEBUG_LOG_WARN,
    DEBUG_LOG_ERROR,
    DEBUG_LOG_CRITICAL,
    DEBUG_LOG_OFF,
};

struct widgetOptions {
    int32_t min;
    int32_t max;
    int32_t defaultValue;
    std::unordered_map<int32_t, const char*> comboBoxOptions;
};

struct cvarObject {
    uint32_t cVarIndex;
    const char* cVarText;
    const char* cVarName;
    const char* cVarTooltip;
    uint32_t widgetType;
    widgetOptions cVarOptions;
    WidgetFunc cVarFunction;
    std::vector<const char*> disabledName;
};

std::unordered_map<int32_t, const char*> menuThemeOptions = {
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

static const std::unordered_map<int32_t, const char*> clockTypeOptions = {
    { CLOCK_TYPE_ORIGINAL, "Original" },
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

std::unordered_map<const char*, bool> disabledMap = {
    { "Debug Camera is Enabled", false },
    { "Free Look is Enabled", false },
    { "AutoSave is Disabled", false },
};

typedef enum {
    MENU_ITEM_MENU_THEME,
    MENU_ITEM_MENUBAR_CONTROLLER_NAV,
    MENU_ITEM_CURSOR_VISIBILITY,
    MENU_ITEM_HOTKEY_TEXT,
    MENU_ITEM_MASTER_VOLUME,
    MENU_ITEM_MAIN_MUSIC_VOLUME,
    MENU_ITEM_SUB_MUSIC_VOLUME,
    MENU_ITEM_SOUND_EFFECT_VOLUME,
    MENU_ITEM_FANFARE_VOLUME,
    MENU_ITEM_AMBIENT_VOLUME,
    // MENU_ITEM_AUDIO_BACKEND, (Unused placeholder)
    MENU_ITEM_TOGGLE_FULLSCREEN,
    MENU_ITEM_INTERNAL_RESOLUTION,
    MENU_ITEM_MSAA,
    MENU_ITEM_FRAME_RATE,
    MENU_ITEM_MATCH_REFRESH_RATE,
    MENU_ITEM_JITTER_FIX,
    // MENU_ITEM_RENDERER_API, (Unused placeholder)
    MENU_ITEM_ENABLE_VSYNC,
    MENU_ITEM_ENABLE_WINDOWED_FULLSCREEN,
    MENU_ITEM_ENABLE_MULTI_VIEWPORT,
    MENU_ITEM_TEXTURE_FILTER,
    MENU_ITEM_INPUT_EDITOR,
    MENU_ITEM_FIX_TARGET_CAMERA_SNAP,
    MENU_ITEM_ENABLE_FREE_LOOK,
    MENU_ITEM_FREE_LOOK_CAMERA_DISTANCE,
    MENU_ITEM_FREE_LOOK_TRANSITION_SPEED,
    MENU_ITEM_FREE_LOOK_MAX_PITCH,
    MENU_ITEM_FREE_LOOK_MIN_PITCH,
    MENU_ITEM_ENABLE_DEBUG_CAMERA,
    MENU_ITEM_INVERT_CAMERA_X_AXIS,
    MENU_ITEM_INVERT_CAMERA_Y_AXIS,
    MENU_ITEM_THIRD_PERSON_CAMERA_X_SENSITIVITY,
    MENU_ITEM_THIRD_PERSON_CAMERA_Y_SENSITIVITY,
    MENU_ITEM_ENABLE_CAMERA_ROLL,
    MENU_ITEM_CAMERA_SPEED,
    MENU_ITEM_CHEATS_INFINITE_HEALTH,
    MENU_ITEM_CHEATS_INFINITE_MAGIC,
    MENU_ITEM_CHEATS_INFINITE_RUPEES,
    MENU_ITEM_CHEATS_INFINITE_CONSUMABLES,
    MENU_ITEM_CHEATS_LONG_FLOWER_GLIDE,
    MENU_ITEM_CHEATS_NO_CLIP,
    MENU_ITEM_CHEATS_INFINITE_RAZOR_SWORD,
    MENU_ITEM_CHEATS_UNRESTRICTED_ITEMS,
    MENU_ITEM_CHEATS_MOON_JUMP_ON_L,
    MENU_ITEM_FAST_FLOWER_LAUNCH,
    MENU_ITEM_INSTANT_PUTAWAY,
    MENU_ITEM_CLIMB_SPEED,
    MENU_ITEM_DPAD_EQUIPS,
    MENU_ITEM_ALWAYS_WIN_DOGGY_RACE,
    MENU_ITEM_PLAY_AS_KAFEI,
    MENU_ITEM_TIME_MOVES_WHEN_YOU_MOVE,
    MENU_ITEM_PERSIST_OWL_SAVES,
    MENU_ITEM_PAUSE_MENU_SAVE,
    MENU_ITEM_AUTOSAVE,
    MENU_ITEM_AUTOSAVE_INTERVAL,
    MENU_ITEM_DISABLE_BOTTLE_RESET,
    MENU_ITEM_DISABLE_CONSUMABLE_RESET,
    MENU_ITEM_DISABLE_RAZOR_SWORD_RESET,
    MENU_ITEM_DISABLE_RUPEE_RESET,
    MENU_ITEM_DISABLE_SAVE_DELAY,
    MENU_ITEM_CLOCK_OPTIONS,
    MENU_ITEM_MILITARY_CLOCK,
    MENU_ITEM_MOTION_BLUR_MODE,
    MENU_ITEM_MOTION_BLUE_INTERPOLATE,
    MENU_ITEM_MOTION_BLUR_ENABLE,
    MENU_ITEM_MOTION_BLUR_STRENGTH,
    MENU_ITEM_AUTHENTIC_LOGO,
    MENU_ITEM_BOW_RETICLE,
    MENU_ITEM_DISABLE_BLACK_BARS,
    MENU_ITEM_GEOMETRY_DISTANCE_CHECK,
    MENU_ITEM_ACTOR_DRAW_DISTANCE,
    MENU_ITEM_ACTOR_UPDATE_DISTANCE,
    MENU_ITEM_BLAST_MASK_KEG_FORCE,
    MENU_ITEM_FAST_TRANSFORMATION,
    MENU_ITEM_FIERCE_DEITY_ANYWHERE,
    MENU_ITEM_NO_BLAST_MASK_COOLDOWN,
    MENU_ITEM_ENABLE_SUNS_SONG,
    MENU_ITEM_DPAD_OCARINA,
    MENU_ITEM_PREVENT_DROPPED_OCARINA_INPUTS,
    MENU_ITEM_HIDE_TITLE_CARDS,
    MENU_ITEM_SKIP_ENTRANCE_CUTSCENES,
    MENU_ITEM_SKIP_TO_FILE_SELECT,
    MENU_ITEM_SKIP_INTRO_SEQUENCE,
    MENU_ITEM_SKIP_STORY_CUTSCENES,
    MENU_ITEM_SKIP_MISC_INTERACTIONS,
    MENU_ITEM_FAST_BANK_SELECTION,
    MENU_ITEM_FAST_TEXT,
    MENU_ITEM_FAST_MAGIC_ARROW_ANIM,
    MENU_ITEM_FIX_AMMO_COUNT_COLOR,
    MENU_ITEM_FIX_HESS_WEIRDSHOT,
    MENU_ITEM_FIX_TEXT_CONTROL_CHAR,
    MENU_ITEM_RESTORE_DISTANCE_FLIPS_HOPS,
    MENU_ITEM_RESTORE_POWER_CROUCH_STAB,
    MENU_ITEM_RESTORE_SIDEROLLS,
    MENU_ITEM_RESTORE_TATL_ISG,
    // MENU_ITEM_HUD_EDITOR, Unused placeholder
    MENU_ITEM_MODERN_MENU_POPOUT,
    MENU_ITEM_OPEN_APP_FILES,
    MENU_ITEM_DEBUG_MODE_ENABLE,
    MENU_ITEM_DEBUG_BETTER_MAP_SELECT,
    MENU_ITEM_DEBUG_SAVE_FILE_MODE,
    MENU_ITEM_PREVENT_ACTOR_UPDATE,
    MENU_ITEM_PREVENT_ACTOR_DRAW,
    MENU_ITEM_PREVENT_ACTOR_INIT,
    MENU_ITEM_DISABLE_OBJECT_DEPENDECY,
    MENU_ITEM_DEBUG_LOG_LEVEL,
    MENU_ITEM_FRAME_ADVANCE_ENABLE,
    MENU_ITEM_FRAME_ADVANCE_SINGLE,
    MENU_ITEM_FRAME_ADVANCE_HOLD,
};

cvarObject enhancementList[] = {
    // Menu Theme
    { MENU_ITEM_MENU_THEME,
      "Menu Theme",
      "gSettings.MenuTheme",
      "Changes the Theme of the Menu Widgets.",
      COMBOBOX,
      { 0, 0, 0, menuThemeOptions },
      nullptr },
    // General Settings
    { MENU_ITEM_MENUBAR_CONTROLLER_NAV,
      "Menubar Controller Navigation",
      CVAR_IMGUI_CONTROLLER_NAV,
      "Allows controller navigation of the SOH menu bar (Settings, Enhancements,...)\nCAUTION: "
      "This will disable game inputs while the menubar is visible.\n\nD-pad to move between "
      "items, A to select, and X to grab focus on the menu bar",
      CHECKBOX,
      {},
      nullptr },
    { MENU_ITEM_CURSOR_VISIBILITY,
      "Cursor Always Visible",
      "gSettings.CursorVisibility",
      "Makes the cursor always visible, even in full screen.",
      CHECKBOX,
      {},
      ([]() {
          Ship::Context::GetInstance()->GetWindow()->SetForceCursorVisibility(
              CVarGetInteger("gSettings.CursorVisibility", 0));
      }) },
    { MENU_ITEM_HOTKEY_TEXT,
      "Hide Menu Hotkey Text",
      "gSettings.DisableMenuShortcutNotify",
      "Prevents showing the text telling you the shortcut to open the menu\n"
      "when the menu isn't open.",
      CHECKBOX,
      {},
      nullptr },
    // Audio Settings
    { MENU_ITEM_MASTER_VOLUME,
      "Master Volume: %.2f%%",
      "gSettings.Audio.MasterVolume",
      "Adjust overall sound volume.",
      SLIDER_FLOAT,
      { 0, 100, 100 },
      nullptr },
    { MENU_ITEM_MAIN_MUSIC_VOLUME,
      "Main Music Volume: %.2f%%",
      "gSettings.Audio.MainMusicVolume",
      "Adjust the Background Music volume.",
      SLIDER_FLOAT,
      { 0, 100, 100 },
      ([]() {
          AudioSeq_SetPortVolumeScale(SEQ_PLAYER_BGM_MAIN, CVarGetFloat("gSettings.Audio.MainMusicVolume", 1.0f));
      }) },
    { MENU_ITEM_SUB_MUSIC_VOLUME,
      "Sub Music Volume: %.2f%%",
      "gSettings.Audio.SubMusicVolume",
      "Adjust the Sub Music volume.",
      SLIDER_FLOAT,
      { 0, 100, 100 },
      ([]() {
          AudioSeq_SetPortVolumeScale(SEQ_PLAYER_BGM_SUB, CVarGetFloat("gSettings.Audio.SubMusicVolume", 1.0f));
      }) },
    { MENU_ITEM_SOUND_EFFECT_VOLUME,
      "Sound Effects Volume: %.2f%%",
      "gSettings.Audio.SoundEffectsVolume",
      "Adjust the Sound Effects volume.",
      SLIDER_FLOAT,
      { 0, 100, 100 },
      ([]() {
          AudioSeq_SetPortVolumeScale(SEQ_PLAYER_SFX, CVarGetFloat("gSettings.Audio.SoundEffectsVolume", 1.0f));
      }) },
    { MENU_ITEM_FANFARE_VOLUME,
      "Fanfare Volume: %.2f%%",
      "gSettings.Audio.FanfareVolume",
      "Adjust the Fanfare volume.",
      SLIDER_FLOAT,
      { 0, 100, 100 },
      ([]() {
          AudioSeq_SetPortVolumeScale(SEQ_PLAYER_FANFARE, CVarGetFloat("gSettings.Audio.FanfareVolume", 1.0f));
      }) },
    { MENU_ITEM_AMBIENT_VOLUME,
      "Ambience Volume: %.2f%%",
      "gSettings.Audio.AmbienceVolume",
      "Adjust the Ambient Sound volume.",
      SLIDER_FLOAT,
      { 0, 100, 100 },
      ([]() {
          AudioSeq_SetPortVolumeScale(SEQ_PLAYER_AMBIENCE, CVarGetFloat("gSettings.Audio.AmbienceVolume", 1.0f));
      }) },
    //{ MENU_ITEM_AUDIO_BACKEND }, Unused placeholder
    // Graphics Settings
    { MENU_ITEM_TOGGLE_FULLSCREEN,
      "Toggle Fullscreen",
      "gSettings.Fullscreen",
      "Toggles Fullscreen On/Off.",
      CHECKBOX,
      {},
      ([]() { Ship::Context::GetInstance()->GetWindow()->ToggleFullscreen(); }) },
    { MENU_ITEM_INTERNAL_RESOLUTION,
      "Internal Resolution: %f%%",
      CVAR_INTERNAL_RESOLUTION,
      "Multiplies your output resolution by the value inputted, as a more intensive but effective "
      "form of anti-aliasing.",
      SLIDER_FLOAT,
      { 50, 200, 100 },
      ([]() {
          Ship::Context::GetInstance()->GetWindow()->SetResolutionMultiplier(CVarGetFloat(CVAR_INTERNAL_RESOLUTION, 1));
      }) },
    { MENU_ITEM_MSAA,
      "Anti-aliasing (MSAA): %d",
      CVAR_MSAA_VALUE,
      "Activates MSAA (multi-sample anti-aliasing) from 2x up to 8x, to smooth the edges of rendered "
      "geometry.\n"
      "Higher sample count will result in smoother edges on models, but may reduce performance.",
      SLIDER_INT,
      { 1, 8, 1 },
      ([]() { Ship::Context::GetInstance()->GetWindow()->SetMsaaLevel(CVarGetInteger(CVAR_MSAA_VALUE, 1)); }) },
    { MENU_ITEM_FRAME_RATE,
      "Current FPS: %d",
      "gInterpolationFPS",
      "Uses Matrix Interpolation to create extra frames, resulting in smoother graphics. This is "
      "purely visual and does not impact game logic, execution of glitches etc.\n\n A higher target "
      "FPS than your monitor's refresh rate will waste resources, and might give a worse result.",
      SLIDER_INT,
      { 20, 360, 20 },
      nullptr },
    { MENU_ITEM_MATCH_REFRESH_RATE,
      "Match Refresh Rate",
      "",
      "Matches interpolation value to the current game's window refresh rate.",
      BUTTON,
      { },
      ([]() {
          int hz = Ship::Context::GetInstance()->GetWindow()->GetCurrentRefreshRate();
          if (hz >= 20 && hz <= 360) {
              CVarSetInteger("gInterpolationFPS", hz);
              Ship::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesOnNextTick();
          }
      }) },
    { MENU_ITEM_JITTER_FIX,
      "Jitter fix : >= % d FPS",
      "gExtraLatencyThreshold",
      "When Interpolation FPS setting is at least this threshold, add one frame of input "
      "lag (e.g. 16.6 ms for 60 FPS) in order to avoid jitter. This setting allows the "
      "CPU to work on one frame while GPU works on the previous frame.\nThis setting "
      "should be used when your computer is too slow to do CPU + GPU work in time.",
      SLIDER_INT,
      { 0, 360, 80 },
      nullptr },
    //{ MENU_ITEM_RENDERER_API }, Unused placeholder
    { MENU_ITEM_ENABLE_VSYNC, "Enable Vsync", CVAR_VSYNC_ENABLED, "Enables Vsync.", CHECKBOX, {}, nullptr },
    { MENU_ITEM_ENABLE_WINDOWED_FULLSCREEN,
      "Windowed Fullscreen",
      CVAR_SDL_WINDOWED_FULLSCREEN,
      "Enables Windowed Fullscreen Mode.",
      CHECKBOX,
      {},
      nullptr },
    { MENU_ITEM_ENABLE_MULTI_VIEWPORT,
      "Allow multi-windows",
      CVAR_ENABLE_MULTI_VIEWPORTS,
      "Allows multiple windows to be opened at once. Requires a reload to take effect.",
      CHECKBOX,
      {},
      nullptr },
    { MENU_ITEM_TEXTURE_FILTER,
      "Texture Filter (Needs reload)",
      CVAR_TEXTURE_FILTER,
      "Sets the applied Texture Filtering.",
      COMBOBOX,
      { 0, 0, 0, textureFilteringMap },
      nullptr },
    // Input Editor
    { MENU_ITEM_INPUT_EDITOR,
      "Popout Input Editor",
      "gWindows.BenInputEditor",
      "Enables the separate Input Editor window.",
      BUTTON,
      {},
      nullptr },
    // Camera Snap Fix
    { MENU_ITEM_FIX_TARGET_CAMERA_SNAP,
      "Fix Targetting Camera Snap",
      "gEnhancements.Camera.FixTargettingCameraSnap",
      "Fixes the camera snap that occurs when you are moving and press the targetting button.",
      CHECKBOX,
      {},
      nullptr },
    // Free Look Settings
    { MENU_ITEM_ENABLE_FREE_LOOK,
      "Free Look",
      "gEnhancements.Camera.FreeLook.Enable",
      "Enables free look camera control\nNote: You must remap C buttons off of the right "
      "stick in the controller config menu, and map the camera stick to the right stick.",
      CHECKBOX,
      {},
      ([]() { RegisterCameraFreeLook(); }), { "Debug Camera is Enabled" } },
    { MENU_ITEM_FREE_LOOK_CAMERA_DISTANCE,
      "Camera Distance: %d",
      "gEnhancements.Camera.FreeLook.MaxCameraDistance",
      "Maximum Camera Distance for Free Look.",
      SLIDER_INT,
      { 100, 900, 185 },
      nullptr },
    { MENU_ITEM_FREE_LOOK_TRANSITION_SPEED,
      "Camera Transition Speed: %d",
      "gEnhancements.Camera.FreeLook.TransitionSpeed",
      "Can someone help me?",
      SLIDER_INT,
      { 1, 900, 25 },
      nullptr },
    { MENU_ITEM_FREE_LOOK_MAX_PITCH,
      "Max Camera Height Angle: %.0f\xC2\xB0",
      "gEnhancements.Camera.FreeLook.MaxPitch",
      "Maximum Height of the Camera.",
      SLIDER_FLOAT,
      { -8900, 8900, 7200 },
      nullptr },
    { MENU_ITEM_FREE_LOOK_MIN_PITCH,
      "Min Camera Height Angle: %.0f\xC2\xB0",
      "gEnhancements.Camera.FreeLook.MinPitch",
      "Minimum Height of the Camera.",
      SLIDER_FLOAT,
      { -8900, 8900, -4900 },
      nullptr },
    // Camera Enhancements
    { MENU_ITEM_ENABLE_DEBUG_CAMERA,
      "Debug Camera",
      "gEnhancements.Camera.DebugCam.Enable",
      "Enables free camera control.",
      CHECKBOX,
      {},
      ([]() { RegisterDebugCam(); }), { "Free Look is Enabled" } },
    { MENU_ITEM_INVERT_CAMERA_X_AXIS,
      "Invert Camera X Axis",
      "gEnhancements.Camera.RightStick.InvertXAxis",
      "Inverts the Camera X Axis",
      CHECKBOX,
      {},
      nullptr },
    { MENU_ITEM_INVERT_CAMERA_Y_AXIS,
      "Invert Camera Y Axis",
      "gEnhancements.Camera.RightStick.InvertYAxis",
      "Inverts the Camera Y Axis",
      CHECKBOX,
      {},
      nullptr },
    { MENU_ITEM_THIRD_PERSON_CAMERA_X_SENSITIVITY,
      "Third-Person Horizontal Sensitivity: %.0f",
      "gEnhancements.Camera.RightStick.CameraSensitivity.X",
      "Adjust the Sensitivity of the x axis when in Third Person.",
      SLIDER_FLOAT,
      { 1, 500, 100 },
      nullptr },
    { MENU_ITEM_THIRD_PERSON_CAMERA_Y_SENSITIVITY,
      "Third-Person Vertical Sensitivity: %.0f",
      "gEnhancements.Camera.RightStick.CameraSensitivity.Y",
      "Adjust the Sensitivity of the x axis when in Third Person.",
      SLIDER_FLOAT,
      { 1, 500, 100 },
      nullptr },
    { MENU_ITEM_ENABLE_CAMERA_ROLL,
      "Enable Roll (6\xC2\xB0 of Freedom)",
      "gEnhancements.Camera.DebugCam.6DOF",
      "This allows for all six degrees of movement with the camera, NOTE: Yaw will work "
      "differently in this system, instead rotating around the focal point"
      ", rather than a polar axis.",
      CHECKBOX,
      {},
      nullptr },
    { MENU_ITEM_CAMERA_SPEED,
      "Camera Speed: %.0f",
      "gEnhancements.Camera.DebugCam.CameraSpeed",
      "Adjusts the speed of the Camera.",
      SLIDER_FLOAT,
      { 10, 300, 50 },
      nullptr },
    // Cheats
    { MENU_ITEM_CHEATS_INFINITE_HEALTH,
      "Infinite Health",
      "gCheats.InfiniteHealth",
      "Always have full Hearts.",
      CHECKBOX,
      {},
      nullptr },
    { MENU_ITEM_CHEATS_INFINITE_MAGIC,
      "Infinite Magic",
      "gCheats.InfiniteMagic",
      "Always have full Magic.",
      CHECKBOX,
      {},
      nullptr },
    { MENU_ITEM_CHEATS_INFINITE_RUPEES,
      "Infinite Rupees",
      "gCheats.InfiniteRupees",
      "Always have a full Wallet.",
      CHECKBOX,
      {},
      nullptr },
    { MENU_ITEM_CHEATS_INFINITE_CONSUMABLES,
      "Infinite Consumables",
      "gCheats.InfiniteConsumables",
      "Always have max Consumables, you must have collected the consumables first.",
      CHECKBOX,
      {},
      nullptr },
    { MENU_ITEM_CHEATS_LONG_FLOWER_GLIDE,
      "Longer Deku Flower Glide",
      "gCheats.LongerFlowerGlide",
      "Allows Deku Link to glide longer, no longer dropping after a certain distance.",
      CHECKBOX,
      {},
      ([]() { RegisterLongerFlowerGlide(); }) },
    { MENU_ITEM_CHEATS_NO_CLIP,
      "No Clip",
      "gCheats.NoClip",
      "Allows Link to phase through collision.",
      CHECKBOX,
      {},
      nullptr },
    { MENU_ITEM_CHEATS_INFINITE_RAZOR_SWORD,
      "Unbreakable Razor Sword",
      "gCheats.UnbreakableRazorSword",
      "Allows to Razor Sword to be used indefinitely without dulling its blade.",
      CHECKBOX,
      {},
      nullptr },
    { MENU_ITEM_CHEATS_UNRESTRICTED_ITEMS,
      "Unrestricted Items",
      "gCheats.UnrestrictedItems",
      "Allows all Forms to use all Items.",
      CHECKBOX,
      {},
      nullptr },
    { MENU_ITEM_CHEATS_MOON_JUMP_ON_L,
      "Moon Jump on L",
      "gCheats.MoonJumpOnL",
      "Holding L makes you float into the air.",
      CHECKBOX,
      {},
      ([]() { RegisterMoonJumpOnL(); }) },
    // Gameplay Enhancements
    { MENU_ITEM_FAST_FLOWER_LAUNCH,
      "Fast Deku Flower Launch",
      "gEnhancements.Player.FastFlowerLaunch",
      "Speeds up the time it takes to be able to get maximum height from launching out of a deku flower",
      CHECKBOX,
      {},
      ([]() { RegisterFastFlowerLaunch(); }) },
    { MENU_ITEM_INSTANT_PUTAWAY,
      "Instant Putaway",
      "gEnhancements.Player.InstantPutaway",
      "Allows Link to instantly puts away held item without waiting.",
      CHECKBOX,
      {},
      nullptr },
    { MENU_ITEM_CLIMB_SPEED,
      "Climb speed",
      "gEnhancements.PlayerMovement.ClimbSpeed",
      "Increases the speed at which Link climbs vines and ladders.",
      SLIDER_INT,
      { 1, 5, 1 },
      nullptr },
    { MENU_ITEM_DPAD_EQUIPS,
      "Dpad Equips",
      "gEnhancements.Dpad.DpadEquips",
      "Allows you to equip items to your d-pad",
      CHECKBOX,
      {},
      nullptr },
    { MENU_ITEM_ALWAYS_WIN_DOGGY_RACE,
      "Always Win Doggy Race",
      "gEnhancements.Minigames.AlwaysWinDoggyRace",
      "Makes the Doggy Race easier to win.",
      COMBOBOX,
      { 0, 0, 0, alwaysWinDoggyraceOptions },
      nullptr },
    // Game Modes
    { MENU_ITEM_PLAY_AS_KAFEI,
      "Play as Kafei",
      "gModes.PlayAsKafei",
      "Requires scene reload to take effect.",
      CHECKBOX,
      {},
      nullptr },
    { MENU_ITEM_TIME_MOVES_WHEN_YOU_MOVE,
      "Time Moves when you Move",
      "gModes.TimeMovesWhenYouMove",
      "Time only moves when Link is not standing still.",
      CHECKBOX,
      {},
      ([]() { RegisterTimeMovesWhenYouMove(); }) },
    // Saving Enhancements
    { MENU_ITEM_PERSIST_OWL_SAVES,
      "Persistent Owl Saves",
      "gEnhancements.Saving.PersistentOwlSaves",
      "Continuing a save will not remove the owl save. Playing Song of "
      "Time, allowing the moon to crash or finishing the "
      "game will remove the owl save and become the new last save.",
      CHECKBOX,
      {},
      nullptr },
    { MENU_ITEM_PAUSE_MENU_SAVE,
      "Pause Menu Save",
      "gEnhancements.Saving.PauseSave",
      "Re-introduce the pause menu save system. Pressing B in the pause menu will give you the "
      "option to create a persistent Owl Save from your current location.\n\nWhen loading back "
      "into the game, you will be placed either at the entrance of the dungeon you saved in, or "
      "in South Clock Town.",
      CHECKBOX,
      {},
      nullptr },
    { MENU_ITEM_AUTOSAVE,
      "Autosave",
      "gEnhancements.Saving.Autosave",
      "Automatically create a persistent Owl Save on the chosen interval.\n\nWhen loading "
      "back into the game, you will be placed either at the entrance of the dungeon you "
      "saved in, or in South Clock Town.",
      CHECKBOX,
      {},
      ([]() { RegisterAutosave(); }) },
    { MENU_ITEM_AUTOSAVE_INTERVAL,
      "Autosave Interval: %d minutes",
      "gEnhancements.Saving.AutosaveInterval",
      "Sets the interval between Autosaves.",
      SLIDER_INT,
      { 1, 60, 5 },
      nullptr, { "AutoSave is Disabled" } },
    { MENU_ITEM_DISABLE_BOTTLE_RESET,
      "Do not reset Bottle content",
      "gEnhancements.Cycle.DoNotResetBottleContent",
      "Playing the Song Of Time will not reset the bottles' content.",
      CHECKBOX,
      {},
      nullptr },
    { MENU_ITEM_DISABLE_CONSUMABLE_RESET,
      "Do not reset Consumables",
      "gEnhancements.Cycle.DoNotResetConsumables",
      "Playing the Song Of Time will not reset the consumables.",
      CHECKBOX,
      {},
      nullptr },
    { MENU_ITEM_DISABLE_RAZOR_SWORD_RESET,
      "Do not reset Razor Sword",
      "gEnhancements.Cycle.DoNotResetRazorSword",
      "Playing the Song Of Time will not reset the Sword back to Kokiri Sword.",
      CHECKBOX,
      {},
      nullptr },
    { MENU_ITEM_DISABLE_RUPEE_RESET,
      "Do not reset Rupees",
      "gEnhancements.Cycle.DoNotResetRupees",
      "Playing the Song Of Time will not reset the your rupees.",
      CHECKBOX,
      {},
      nullptr },
    { MENU_ITEM_DISABLE_SAVE_DELAY,
      "Disable Save Delay",
      "gEnhancements.Saving.DisableSaveDelay",
      "Removes the arbitrary 2 second timer for saving from the original game. This is known to "
      "cause issues when attempting the 0th Day Glitch",
      CHECKBOX,
      {},
      nullptr },
    // Graphics Enhancements
    { MENU_ITEM_CLOCK_OPTIONS,
      "Clock Type",
      "gEnhancements.Graphics.ClockType",
      "Swaps between Graphical and Text only Clock types.",
      COMBOBOX,
      { 0, 0, 0, clockTypeOptions },
      nullptr },
    { MENU_ITEM_MILITARY_CLOCK,
      "24 Hours Clock",
      "gEnhancements.Graphics.24HoursClock",
      "Changes from a 12 Hour to a 24 Hour Clock",
      CHECKBOX,
      {},
      nullptr },
    { MENU_ITEM_MOTION_BLUR_MODE,
      "Motion Blur Mode",
      "gEnhancements.Graphics.MotionBlur.Mode",
      "Selects the Mode for Motion Blur.",
      COMBOBOX,
      { 0, 0, 0, motionBlurOptions },
      nullptr },
    { MENU_ITEM_MOTION_BLUE_INTERPOLATE,
      "Interpolate",
      "gEnhancements.Graphics.MotionBlur.Interpolate",
      "Change motion blur capture to also happen on interpolated frames instead of only on game frames.\n"
      "This notably reduces the overall motion blur strength but smooths out the trails.",
      CHECKBOX,
      {},
      nullptr },
    { MENU_ITEM_MOTION_BLUR_ENABLE,
      "On/Off",
      "gEnhancements.Graphics.MotionBlur.Toggle",
      "Enables Motion Blur.",
      CHECKBOX,
      {},
      ([]() {
          if (CVarGetInteger("gEnhancements.Graphics.MotionBlur.Toggle", 0) == 0) {
              R_MOTION_BLUR_ENABLED;
              R_MOTION_BLUR_ALPHA = CVarGetInteger("gEnhancements.Graphics.MotionBlur.Strength", 0);
          } else {
              !R_MOTION_BLUR_ENABLED;
          }
      }) },
    { MENU_ITEM_MOTION_BLUR_STRENGTH,
      "Strength",
      "gEnhancements.Graphics.MotionBlur.Strength",
      "Motion Blur strength.",
      SLIDER_INT,
      { 0, 255, 180 },
      nullptr },
    { MENU_ITEM_AUTHENTIC_LOGO,
      "Authentic Logo",
      "gEnhancements.Graphics.AuthenticLogo",
      "Hide the game version and build details and display the authentic "
      "model and texture on the boot logo start screen",
      CHECKBOX,
      {},
      nullptr },
    { MENU_ITEM_BOW_RETICLE,
      "Bow Reticle",
      "gEnhancements.Graphics.BowReticle",
      "Gives the bow a reticle when you draw an arrow.",
      CHECKBOX,
      {},
      nullptr },
    { MENU_ITEM_DISABLE_BLACK_BARS,
      "Disable Black Bar Letterboxes",
      "gEnhancements.Graphics.DisableBlackBars",
      "Disables Black Bar Letterboxes during cutscenes and Z-targeting\nNote: there may be "
      "minor visual glitches that were covered up by the black bars\nPlease disable this "
      "setting before reporting a bug.",
      CHECKBOX,
      {},
      nullptr },
    { MENU_ITEM_GEOMETRY_DISTANCE_CHECK,
      "Disable Scene Geometry Distance Check",
      "gEnhancements.Graphics.DisableSceneGeometryDistanceCheck",
      "Disables the distance check for scene geometry, allowing it to be drawn no matter how far "
      "away it is from the player. This may have unintended side effects.",
      CHECKBOX,
      {},
      nullptr },
    { MENU_ITEM_ACTOR_DRAW_DISTANCE,
      "Increase Actor Draw Distance: %dx",
      "gEnhancements.Graphics.IncreaseActorDrawDistance",
      "Increase the range in which Actors are drawn. This may have unintended side effects.",
      SLIDER_INT,
      { 1, 5, 1 },
      ([]() {
          CVarSetInteger("gEnhancements.Graphics.IncreaseActorUpdateDistance",
                         MIN(CVarGetInteger("gEnhancements.Graphics.IncreaseActorDrawDistance", 1),
                             CVarGetInteger("gEnhancements.Graphics.IncreaseActorUpdateDistance", 1)));
      }) },
    { MENU_ITEM_ACTOR_UPDATE_DISTANCE,
      "Increase Actor Update Distance: %dx",
      "gEnhancements.Graphics.IncreaseActorUpdateDistance",
      "Increase the range in which Actors are updated. This may have unintended side effects.",
      SLIDER_INT,
      { 1, 5, 1 },
      ([]() {
          CVarSetInteger("gEnhancements.Graphics.IncreaseActorDrawDistance",
                         MAX(CVarGetInteger("gEnhancements.Graphics.IncreaseActorDrawDistance", 1),
                             CVarGetInteger("gEnhancements.Graphics.IncreaseActorUpdateDistance", 1)));
      }) },
    // Mask Enhancements
    { MENU_ITEM_BLAST_MASK_KEG_FORCE,
      "Blast Mask has Powder Keg Force",
      "gEnhancements.Masks.BlastMaskKeg",
      "Blast Mask can also destroy objects only the Powder Keg can.",
      CHECKBOX,
      {},
      nullptr },
    { MENU_ITEM_FAST_TRANSFORMATION,
      "Fast Transformation",
      "gEnhancements.Masks.FastTransformation",
      "Removes the delay when using transormation masks.",
      CHECKBOX,
      {},
      nullptr },
    { MENU_ITEM_FIERCE_DEITY_ANYWHERE,
      "Fierce Deity's Mask Anywhere",
      "gEnhancements.Masks.FierceDeitysAnywhere",
      "Allow using Fierce Deity's mask outside of boss rooms.",
      CHECKBOX,
      {},
      nullptr },
    { MENU_ITEM_NO_BLAST_MASK_COOLDOWN,
      "No Blast Mask Cooldown",
      "gEnhancements.Masks.NoBlastMaskCooldown",
      "Eliminates the Cooldown between Blast Mask usage.",
      CHECKBOX,
      {},
      nullptr },
    // Song Enhancements
    { MENU_ITEM_ENABLE_SUNS_SONG,
      "Enable Sun's Song",
      "gEnhancements.Songs.EnableSunsSong",
      "Enables the partially implemented Sun's Song. RIGHT-DOWN-UP-RIGHT-DOWN-UP to play it. "
      "This song will make time move very fast until either Link moves to a different scene, "
      "or when the time switches to a new time period.",
      CHECKBOX,
      {},
      nullptr },
    { MENU_ITEM_DPAD_OCARINA,
      "Dpad Ocarina",
      "gEnhancements.Playback.DpadOcarina",
      "Enables using the Dpad for Ocarina playback.",
      CHECKBOX,
      {},
      nullptr },
    { MENU_ITEM_PREVENT_DROPPED_OCARINA_INPUTS,
      "Prevent Dropped Ocarina Inputs",
      "gEnhancements.Playback.NoDropOcarinaInput",
      "Prevent dropping inputs when playing the ocarina quickly.",
      CHECKBOX,
      {},
      nullptr },
    // Cutscene Skips
    { MENU_ITEM_HIDE_TITLE_CARDS,
      "Hide Title Cards",
      "gEnhancements.Cutscenes.HideTitleCards",
      "Hides Title Cards when entering areas.",
      CHECKBOX,
      {},
      nullptr },
    { MENU_ITEM_SKIP_ENTRANCE_CUTSCENES,
      "Skip Entrance Cutscenes",
      "gEnhancements.Cutscenes.SkipEntranceCutscenes",
      "Skip cutscenes that occur when first entering a new area.",
      CHECKBOX,
      {},
      nullptr },
    { MENU_ITEM_SKIP_TO_FILE_SELECT,
      "Skip to File Select",
      "gEnhancements.Cutscenes.SkipToFileSelect",
      "Skip the opening title sequence and go straight to the file select menu after boot.",
      CHECKBOX,
      {},
      nullptr },
    { MENU_ITEM_SKIP_INTRO_SEQUENCE,
      "Skip Intro Sequence",
      "gEnhancements.Cutscenes.SkipIntroSequence",
      "When starting a game you will be taken straight to South Clock Town as Deku Link.",
      CHECKBOX,
      {},
      nullptr },
    { MENU_ITEM_SKIP_STORY_CUTSCENES,
      "Skip Story Cutscenes",
      "gEnhancements.Cutscenes.SkipStoryCutscenes",
      "Disclaimer: This doesn't do much yet, we will be progressively adding more skips over time.",
      CHECKBOX,
      {},
      nullptr },
    { MENU_ITEM_SKIP_MISC_INTERACTIONS,
      "Skip Misc Interactions",
      "gEnhancements.Cutscenes.SkipMiscInteractions",
      "Disclaimer: This doesn't do much yet, we will be progressively adding more skips over time.",
      CHECKBOX,
      {},
      nullptr },
    // Dialogue Enhancements
    { MENU_ITEM_FAST_BANK_SELECTION,
      "Fast Bank Selection",
      "gEnhancements.Dialogue.FastBankSelection",
      "Pressing the Z or R buttons while the Deposit/Withdrawl Rupees dialogue is open will set "
      "the Rupees to Links current Rupees or 0 respectively.",
      CHECKBOX,
      {},
      nullptr },
    { MENU_ITEM_FAST_TEXT,
      "Fast Text",
      "gEnhancements.Dialogue.FastText",
      "Speeds up text rendering, and enables holding of B progress to next message.",
      CHECKBOX,
      {},
      nullptr },
    { MENU_ITEM_FAST_MAGIC_ARROW_ANIM,
      "Fast Magic Arrow Equip Animation",
      "gEnhancements.Equipment.MagicArrowEquipSpeed",
      "Removes the animation for equipping Magic Arrows.",
      CHECKBOX,
      {},
      nullptr },
    // Fixes
    { MENU_ITEM_FIX_AMMO_COUNT_COLOR,
      "Fix Ammo Count Color",
      "gFixes.FixAmmoCountEnvColor",
      "Fixes a missing gDPSetEnvColor, which causes the ammo count to be "
      "the wrong color prior to obtaining magic or other conditions.",
      CHECKBOX,
      {},
      nullptr },
    { MENU_ITEM_FIX_HESS_WEIRDSHOT,
      "Fix Hess and Weirdshot Crash",
      "gEnhancements.Fixes.HessCrash",
      "Fixes a crash that can occur when performing a HESS or Weirdshot.",
      CHECKBOX,
      {},
      nullptr },
    { MENU_ITEM_FIX_TEXT_CONTROL_CHAR,
      "Fix Text Control Characters",
      "gEnhancements.Fixes.ControlCharacters",
      "Fixes certain control characters not functioning properly "
      "depending on their position within the text.",
      CHECKBOX,
      {},
      nullptr },
    // Restorations
    { MENU_ITEM_RESTORE_DISTANCE_FLIPS_HOPS,
      "Constant Distance Backflips and Sidehops",
      "gEnhancements.Restorations.ConstantFlipsHops",
      "Backflips and Sidehops travel a constant distance as they did in OoT.",
      CHECKBOX,
      {},
      nullptr },
    { MENU_ITEM_RESTORE_POWER_CROUCH_STAB,
      "Power Crouch Stab",
      "gEnhancements.Restorations.PowerCrouchStab",
      "Crouch stabs will use the power of Link's previous melee attack, as is in MM JP 1.0 and OoT.",
      CHECKBOX,
      {},
      nullptr },
    { MENU_ITEM_RESTORE_SIDEROLLS,
      "Side Rolls",
      "gEnhancements.Restorations.SideRoll",
      "Restores side rolling from OoT.",
      CHECKBOX,
      {},
      nullptr },
    { MENU_ITEM_RESTORE_TATL_ISG,
      "Tatl ISG",
      "gEnhancements.Restorations.TatlISG",
      "Restores Navi ISG from OoT, but now with Tatl.",
      CHECKBOX,
      {},
      nullptr },
    // HUD Editor
    // { MENU_ITEM_HUD_EDITOR }, Unused placeholder
    { MENU_ITEM_MODERN_MENU_POPOUT,
      "Popout Menu",
      "gSettings.Menu.Popout",
      "Changes the menu display from overlay to windowed.",
      CHECKBOX,
      {},
      nullptr },
    { MENU_ITEM_OPEN_APP_FILES,
      "Open App Files Folder",
      "",
      "Opens the folder that contains the save and mods folders, etc,",
      BUTTON,
      {},
      ([]() {
          std::string filesPath = Ship::Context::GetInstance()->GetAppDirectoryPath();
          SDL_OpenURL(std::string("file:///" + std::filesystem::absolute(filesPath).string()).c_str());
      }) },
    { MENU_ITEM_DEBUG_MODE_ENABLE,
      "Debug Mode",
      "gDeveloperTools.DebugEnabled",
      "Enables Debug Mode, allowing you to select maps with L + R + Z.",
      CHECKBOX,
      {},
      ([]() {
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
      }) },
    { MENU_ITEM_DEBUG_BETTER_MAP_SELECT,
      "Better Map Select",
      "gDeveloperTools.BetterMapSelect.Enabled",
      "Overrides the original map select with a translated, more user-friendly version.",
      CHECKBOX,
      {},
      nullptr },
    { MENU_ITEM_DEBUG_SAVE_FILE_MODE,
      "Debug Save File Mode",
      "gDeveloperTools.DebugSaveFileMode",
      "Change the behavior of creating saves while debug mode is enabled:\n\n"
      "- Empty Save: The default 3 heart save file in first cycle\n"
      "- Vanilla Debug Save: Uses the title screen save info (8 hearts, all items and masks)\n"
      "- 100\% Save: All items, equipment, mask, quast status and bombers notebook complete",
      COMBOBOX,
      { 0, 0, 0, debugSaveOptions },
      ([]() { RegisterDebugSaveCreate(); }) },
    { MENU_ITEM_PREVENT_ACTOR_UPDATE,
      "Prevent Actor Update",
      "gDeveloperTools.PreventActorUpdate",
      "Prevents Actors from updating.",
      CHECKBOX,
      {},
      ([]() { RegisterPreventActorUpdateHooks(); }) },
    { MENU_ITEM_PREVENT_ACTOR_DRAW,
      "Prevent Actor Draw",
      "gDeveloperTools.PreventActorDraw",
      "Prevents Actors from drawing.",
      CHECKBOX,
      {},
      ([]() { RegisterPreventActorDrawHooks(); }) },
    { MENU_ITEM_PREVENT_ACTOR_INIT,
      "Prevent Actor Init",
      "gDeveloperTools.PreventActorInit",
      "Prevents Actors from initializing.",
      CHECKBOX,
      {},
      ([]() { RegisterPreventActorInitHooks(); }) },
    { MENU_ITEM_DISABLE_OBJECT_DEPENDECY,
      "Disable Object Dependency",
      "gDeveloperTools.DisableObjectDependency",
      "Disables dependencies when loading objects.",
      CHECKBOX,
      {},
      nullptr },
    { MENU_ITEM_DEBUG_LOG_LEVEL,
      "Log Level",
      "gDeveloperTools.LogLevel",
      "The log level determines which messages are printed to the "
      "console. This does not affect the log file output",
      COMBOBOX,
      { 0, 0, 0, logLevels },
      ([]() {
          Ship::Context::GetInstance()->GetLogger()->set_level(
              (spdlog::level::level_enum)CVarGetInteger("gDeveloperTools.LogLevel", 1));
      }) },
    { MENU_ITEM_FRAME_ADVANCE_ENABLE,
      "Frame Advance",
      "gDeveloperTools.FrameAdvance",
      "This allows you to advance through the game one frame at a time on command. "
      "To advance a frame, hold Z and tap R on the second controller. Holding Z "
      "and R will advance a frame every half second. You can also use the buttons below.",
      CHECKBOX,
      {},
      ([]() {
          if (CVarGetInteger("gDeveloperTools.FrameAdvance", 0) == 1) {
              gPlayState->frameAdvCtx.enabled = true;
          } else {
              gPlayState->frameAdvCtx.enabled = false;
          };
      }) },
    { MENU_ITEM_FRAME_ADVANCE_SINGLE, "Advance 1", "", "Advance 1 frame.", BUTTON, {}, ([]() {
          CVarSetInteger("gDeveloperTools.FrameAdvanceTick", 1);
      }) },
    { MENU_ITEM_FRAME_ADVANCE_HOLD,
      "Advance (Hold)",
      "",
      "Advance frames while the button is held.",
      BUTTON,
      {},
      nullptr },
};

void SearchMenuUpdateDisabled() {
    disabledMap.at("Debug Camera is Enabled") = CVarGetInteger("gEnhancements.Camera.DebugCam.Enable", 0);
    disabledMap.at("Free Look is Enabled") = CVarGetInteger("gEnhancements.Camera.FreeLook.Enable", 0);
    disabledMap.at("AutoSave is Disabled") = !CVarGetInteger("gEnhancements.Saving.Autosave", 0);
}

void SearchMenuGetItem(uint32_t index) {
    disabledTempTooltip = "This setting is disabled because the following is set: \n\n";
    disabledValue = false;
    disabledTooltip = " ";

    if (!enhancementList[index].disabledName.empty()) {
        for (int i = 0; i < enhancementList[index].disabledName.size(); i++) {
            if (disabledMap.at(enhancementList[index].disabledName[i]) == true) {
                disabledValue = true;
                disabledTempTooltip += std::string("- ") + enhancementList[index].disabledName[i] + std::string("\n");
            }
        }
        disabledTooltip = disabledTempTooltip.c_str();
    } else {
        disabledValue = false;
        disabledTooltip= " ";
    }

    float floatMin;
    float floatMax;
    float floatDefault;

    switch (enhancementList[index].widgetType) {
        case CHECKBOX:
            if (UIWidgets::CVarCheckbox(enhancementList[index].cVarText, enhancementList[index].cVarName,
                                        {
                                            .color = menuTheme[CVarGetInteger("gSettings.MenuTheme", 0)],
                                            .tooltip = enhancementList[index].cVarTooltip,
                                            .disabled = disabledValue,
                                            .disabledTooltip = disabledTooltip,
                                        })) {
                if (enhancementList[index].cVarFunction != nullptr) {
                    enhancementList[index].cVarFunction();
                }
            };
            break;
        case COMBOBOX:
            if (UIWidgets::CVarCombobox(enhancementList[index].cVarText, enhancementList[index].cVarName,
                                        enhancementList[index].cVarOptions.comboBoxOptions,
                                        {
                                            .color = menuTheme[CVarGetInteger("gSettings.MenuTheme", 0)],
                                            .tooltip = enhancementList[index].cVarTooltip,
                                            .disabled = disabledValue,
                                            .disabledTooltip = disabledTooltip,
                                        })) {
                if (enhancementList[index].cVarFunction != nullptr) {
                    enhancementList[index].cVarFunction();
                }
            }
            break;
        case SLIDER_INT:
            if (UIWidgets::CVarSliderInt(enhancementList[index].cVarText, enhancementList[index].cVarName,
                                         enhancementList[index].cVarOptions.min, enhancementList[index].cVarOptions.max,
                                         enhancementList[index].cVarOptions.defaultValue,
                                         {
                                             .color = menuTheme[CVarGetInteger("gSettings.MenuTheme", 0)],
                                             .tooltip = enhancementList[index].cVarTooltip,
                                             .disabled = disabledValue,
                                             .disabledTooltip = disabledTooltip,
                                         })) {
                if (enhancementList[index].cVarFunction != nullptr) {
                    enhancementList[index].cVarFunction();
                }
            };
            break;
        case SLIDER_FLOAT:
            floatMin = (static_cast<float>(enhancementList[index].cVarOptions.min) / 100);
            floatMax = (static_cast<float>(enhancementList[index].cVarOptions.max) / 100);
            floatDefault = (static_cast<float>(enhancementList[index].cVarOptions.defaultValue) / 100);
            if (UIWidgets::CVarSliderFloat(enhancementList[index].cVarText, enhancementList[index].cVarName, floatMin,
                                           floatMax, floatDefault,
                                           {
                                               .color = menuTheme[CVarGetInteger("gSettings.MenuTheme", 0)],
                                               .tooltip = enhancementList[index].cVarTooltip,
                                               .disabled = disabledValue,
                                               .disabledTooltip = disabledTooltip,
                                           })) {
                if (enhancementList[index].cVarFunction != nullptr) {
                    enhancementList[index].cVarFunction();
                }
            }
            break;
        case BUTTON:
            if (UIWidgets::Button(enhancementList[index].cVarText,
                                  {
                                      .color = menuTheme[CVarGetInteger("gSettings.MenuTheme", 0)],
                                      .tooltip = enhancementList[index].cVarTooltip,
                                      .disabled = disabledValue,
                                      .disabledTooltip = disabledTooltip,
                                  })) {
                if (enhancementList[index].cVarFunction != nullptr) {
                    enhancementList[index].cVarFunction();
                }
            }
            break;
        default:
            break;
    }
}
