#pragma once

#include <libultraship/libultraship.h>
#include "UIWidgets.hpp"

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
} ColorOption;

typedef enum {
    DISABLE_FOR_CAMERAS_OFF,
    DISABLE_FOR_DEBUG_CAM_ON,
    DISABLE_FOR_DEBUG_CAM_OFF,
    DISABLE_FOR_FREE_LOOK_ON,
    DISABLE_FOR_FREE_LOOK_OFF,
    DISABLE_FOR_GYRO_OFF,
    DISABLE_FOR_GYRO_ON,
    DISABLE_FOR_RIGHT_STICK_OFF,
    DISABLE_FOR_AUTO_SAVE_OFF,
    DISABLE_FOR_NULL_PLAY_STATE,
    DISABLE_FOR_DEBUG_MODE_OFF,
    DISABLE_FOR_NO_VSYNC,
    DISABLE_FOR_NO_WINDOWED_FULLSCREEN,
    DISABLE_FOR_NO_MULTI_VIEWPORT,
    DISABLE_FOR_NOT_DIRECTX,
    DISABLE_FOR_DIRECTX,
    DISABLE_FOR_MATCH_REFRESH_RATE_ON,
    DISABLE_FOR_MOTION_BLUR_MODE,
    DISABLE_FOR_MOTION_BLUR_OFF,
    DISABLE_FOR_FRAME_ADVANCE_OFF,
    DISABLE_FOR_WARP_POINT_NOT_SET,
    DISABLE_FOR_INTRO_SKIP_OFF,
} DisableOption;

struct widgetInfo;
struct disabledInfo;
using VoidFunc = void (*)();
using DisableInfoFunc = bool (*)(disabledInfo&);
using DisableVec = std::vector<DisableOption>;
using WidgetFunc = void (*)(widgetInfo&);

typedef enum {
    WIDGET_CHECKBOX,
    WIDGET_COMBOBOX,
    WIDGET_SLIDER_INT,
    WIDGET_SLIDER_FLOAT,
    WIDGET_CVAR_CHECKBOX,
    WIDGET_CVAR_COMBOBOX,
    WIDGET_CVAR_SLIDER_INT,
    WIDGET_CVAR_SLIDER_FLOAT,
    WIDGET_BUTTON,
    WIDGET_COLOR_24, // color picker without alpha
    WIDGET_COLOR_32, // color picker with alpha
    WIDGET_SEARCH,
    WIDGET_SEPARATOR,
    WIDGET_SEPARATOR_TEXT,
    WIDGET_TEXT,
    WIDGET_WINDOW_BUTTON,
    WIDGET_AUDIO_BACKEND, // needed for special operations that can't be handled easily with the normal combobox widget
    WIDGET_VIDEO_BACKEND  // same as above
} WidgetType;

typedef enum {
    MOTION_BLUR_DYNAMIC,
    MOTION_BLUR_ALWAYS_OFF,
    MOTION_BLUR_ALWAYS_ON,
} MotionBlurOption;

typedef enum {
    DEBUG_LOG_TRACE,
    DEBUG_LOG_DEBUG,
    DEBUG_LOG_INFO,
    DEBUG_LOG_WARN,
    DEBUG_LOG_ERROR,
    DEBUG_LOG_CRITICAL,
    DEBUG_LOG_OFF,
} DebugLogOption;

// holds the widget values for a widget, contains all CVar types available from LUS. int32_t is used for boolean
// evaluation
using CVarVariant = std::variant<int32_t, const char*, float, Color_RGBA8, Color_RGB8>;

// All the info needed for display and search of all widgets in the menu.
// `name` is the label displayed,
// `cVar` is the string representation of the CVar used to store the widget value
// `tooltip` is what is displayed when hovering (except when disabled, more on that later)
// `type` is the WidgetType for the widget, which is what determines how the information is used in the draw func
// `options` is a variant that holds the UIWidgetsOptions struct for the widget type
// blank objects need to be initialized with specific typing matching the expected Options struct for the widget
// `callback` is a lambda used for running code on widget change. may need `BenGui::GetMenu()` for specific menu actions
// `preFunc` is a lambda called before drawing code starts. It can be used to determine a widget's status,
// whether disabled or hidden, as well as update pointers for non-CVar widget types.
// `postFunc` is a lambda called after all drawing code is finished, for reacting to states other than
// widgets having been changed, like holding buttons.
// All three lambdas accept a `widgetInfo` reference in case it needs information on the widget for these operations
// `activeDisables` is a vector of DisableOptions for specifying what reasons a widget is disabled, which are displayed
// in the disabledTooltip for the widget. Can display multiple reasons. Handling the reasons is done in `preFunc`.
// It is recommended to utilize `disabledInfo`/`DisableReason` to list out all reasons for disabling and isHidden so
// the info can be shown.
// `windowName` is what is displayed and searched for `windowButton` type and window interactions
// `isHidden` just prevents the widget from being drawn under whatever circumstances you specify in the `preFunc`
// `sameLine` allows for specifying that the widget should be on the same line as the previous widget
struct widgetInfo {
    std::string name; // Used by all widgets
    const char* cVar; // Used by all widgets except
    const char* tooltip;
    WidgetType type;
    std::variant<UIWidgets::ButtonOptions, UIWidgets::CheckboxOptions, UIWidgets::ComboboxOptions,
                 UIWidgets::FloatSliderOptions, UIWidgets::IntSliderOptions, UIWidgets::SeparatorOptions>
        options;
    std::variant<bool*, int32_t*, float*> valuePointer;
    std::unordered_map<int32_t, const char*> comboMap = {};
    CVarVariant min;
    CVarVariant max;
    WidgetFunc callback = nullptr;
    WidgetFunc preFunc = nullptr;
    WidgetFunc postFunc = nullptr;
    DisableVec activeDisables = {};
    const char* windowName = "";
    bool isHidden = false;
    bool sameLine = false;
};

// `disabledInfo` holds information on reasons for hiding or disabling a widget, as well as an evaluation lambda that
// is run once per frame to update its status (this is done to prevent dozens of redundant CVarGets in each frame loop)
// `evaluation` returns a bool which can be determined by whatever code you want that changes its status
// `reason` is the text displayed in the disabledTooltip when a widget is disabled by a particular DisableReason
// `active` is what's referenced when determining disabled status for a widget that uses this This can also be used to
// hold reasons to hide widgets so that their evaluations are also only run once per frame
struct disabledInfo {
    DisableInfoFunc evaluation;
    const char* reason;
    bool active = false;
    int32_t value = 0;
};

// Contains the name displayed in the sidebar (label), the number of columns to use in drawing (columnCount; for visual
// separation, 1-3), and nested vectors of the widgets, grouped by column (columnWidgets). The number of widget vectors
// added to the column groups does not need to match the specified columnCount, e.g. you can have one vector added to
// the sidebar, but still separate the window into 3 columns and display only in the first column
struct SidebarEntry {
    std::string label;
    uint32_t columnCount;
    std::vector<std::vector<widgetInfo>> columnWidgets;
};

// Contains entries for what's listed in the header at the top, including the name displayed on the top bar (label),
// a vector of the SidebarEntries for that header entry, and the name of the cvar used to track what sidebar entry is
// the last viewed for that header.
struct MainMenuEntry {
    std::string label;
    std::vector<SidebarEntry> sidebarEntries;
    const char* sidebarCvar;
};

static const std::unordered_map<Ship::AudioBackend, const char*> audioBackendsMap = {
    { Ship::AudioBackend::WASAPI, "Windows Audio Session API" },
    { Ship::AudioBackend::SDL, "SDL" },
};

static const std::unordered_map<Ship::WindowBackend, const char*> windowBackendsMap = {
    { Ship::WindowBackend::FAST3D_DXGI_DX11, "DirectX" },
    { Ship::WindowBackend::FAST3D_SDL_OPENGL, "OpenGL" },
    { Ship::WindowBackend::FAST3D_SDL_METAL, "Metal" },
};
