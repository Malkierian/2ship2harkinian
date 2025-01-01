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

namespace BenGui {}

namespace Ship {
std::string disabledTempTooltip;
const char* disabledTooltip;
bool disabledValue = false;

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

std::unordered_map<ColorOption, ImVec4> menuTheme = { { COLOR_WHITE, UIWidgets::Colors::White },
                                                      { COLOR_GRAY, UIWidgets::Colors::Gray },
                                                      { COLOR_DARK_GRAY, UIWidgets::Colors::DarkGray },
                                                      { COLOR_INDIGO, UIWidgets::Colors::Indigo },
                                                      { COLOR_RED, UIWidgets::Colors::Red },
                                                      { COLOR_DARK_RED, UIWidgets::Colors::DarkRed },
                                                      { COLOR_LIGHT_GREEN, UIWidgets::Colors::LightGreen },
                                                      { COLOR_GREEN, UIWidgets::Colors::Green },
                                                      { COLOR_DARK_GREEN, UIWidgets::Colors::DarkGreen },
                                                      { COLOR_YELLOW, UIWidgets::Colors::Yellow } };

void Menu::InsertSidebarSearch() {
    menuEntries[0].sidebarEntries.insert(menuEntries[0].sidebarEntries.begin() + searchSidebarIndex,
                                         searchSidebarEntry);
    CVarSetInteger(menuEntries[0].sidebarCvar, CVarGetInteger(menuEntries[0].sidebarCvar, 0) + 1);
}

void Menu::RemoveSidebarSearch() {
    std::erase_if(menuEntries[0].sidebarEntries, [this](SidebarEntry entry) {
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

void Menu::UpdateWindowBackendObjects() {
    Ship::WindowBackend runningWindowBackend = Ship::Context::GetInstance()->GetWindow()->GetWindowBackend();
    int32_t configWindowBackendId = Ship::Context::GetInstance()->GetConfig()->GetInt("Window.Backend.Id", -1);
    if (Ship::Context::GetInstance()->GetWindow()->IsAvailableWindowBackend(configWindowBackendId)) {
        configWindowBackend = static_cast<Ship::WindowBackend>(configWindowBackendId);
    } else {
        configWindowBackend = runningWindowBackend;
    }

    availableWindowBackends = Ship::Context::GetInstance()->GetWindow()->GetAvailableWindowBackends();
    for (auto& backend : *availableWindowBackends) {
        availableWindowBackendsMap[backend] = windowBackendsMap.at(backend);
    }
}

Menu::Menu(const std::string& cVar, const std::string& name, uint8_t searchSidebarIndex_, ColorOption menuThemeIndex_)
    : GuiWindow(cVar, name), searchSidebarIndex(searchSidebarIndex_), menuThemeIndex(menuThemeIndex_) {
}

void Menu::InitElement() {
    popped = CVarGetInteger("gSettings.Menu.Popout", 0);
    poppedSize.x = CVarGetInteger("gSettings.Menu.PoppedWidth", 1280);
    poppedSize.y = CVarGetInteger("gSettings.Menu.PoppedHeight", 800);
    poppedPos.x = CVarGetInteger("gSettings.Menu.PoppedPos.x", 0);
    poppedPos.y = CVarGetInteger("gSettings.Menu.PoppedPos.y", 0);

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

void Menu::MenuDrawItem(widgetInfo& widget) {
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

    if (widget.sameLine) {
        ImGui::SameLine();
    }

    try {
        switch (widget.type) {
            case WIDGET_CHECKBOX: {
                bool* pointer = std::get<bool*>(widget.valuePointer);
                if (pointer == nullptr) {
                    SPDLOG_ERROR("Checkbox Widget requires a value pointer, currently nullptr");
                    assert(false);
                    return;
                }
                if (UIWidgets::Checkbox(widget.name.c_str(), pointer,
                                        std::get<UIWidgets::CheckboxOptions>(widget.options))) {
                    if (widget.callback != nullptr) {
                        widget.callback(widget);
                    }
                }
            } break;
            case WIDGET_CVAR_CHECKBOX: {
                if (UIWidgets::CVarCheckbox(widget.name.c_str(), widget.cVar,
                                            std::get<UIWidgets::CheckboxOptions>(widget.options))) {
                    if (widget.callback != nullptr) {
                        widget.callback(widget);
                    }
                };
            } break;
            case WIDGET_AUDIO_BACKEND: {
                auto currentAudioBackend = Ship::Context::GetInstance()->GetAudio()->GetAudioBackend();
                if (UIWidgets::Combobox(
                        "Audio API", &currentAudioBackend, audioBackendsMap,
                        { .color = menuTheme[menuThemeIndex],
                          .tooltip = widget.tooltip,
                          .disabled =
                              Ship::Context::GetInstance()->GetAudio()->GetAvailableAudioBackends()->size() <= 1,
                          .disabledTooltip = "Only one audio API is available on this platform." })) {
                    Ship::Context::GetInstance()->GetAudio()->SetAudioBackend(currentAudioBackend);
                }
            } break;
            case WIDGET_VIDEO_BACKEND: {
                if (UIWidgets::Combobox(
                        "Renderer API (Needs reload)", &configWindowBackend, availableWindowBackendsMap,
                        { .color = menuTheme[menuThemeIndex],
                          .tooltip = widget.tooltip,
                          .disabled = availableWindowBackends->size() <= 1,
                          .disabledTooltip = "Only one renderer API is available on this platform." })) {
                    Ship::Context::GetInstance()->GetConfig()->SetInt("Window.Backend.Id",
                                                                      static_cast<int32_t>(configWindowBackend));
                    Ship::Context::GetInstance()->GetConfig()->SetString("Window.Backend.Name",
                                                                         windowBackendsMap.at(configWindowBackend));
                    Ship::Context::GetInstance()->GetConfig()->Save();
                    UpdateWindowBackendObjects();
                }
            } break;
            case WIDGET_SEPARATOR: {
                ImGui::Separator();
            } break;
            case WIDGET_SEPARATOR_TEXT: {
                UIWidgets::SeparatorOptions options = std::get<UIWidgets::SeparatorOptions>(widget.options);
                if (options.color != UIWidgets::COLOR_NONE) {
                    ImGui::PushStyleColor(ImGuiCol_Text, options.color);
                }
                ImGui::SeparatorText(widget.name.c_str());
                if (options.color != UIWidgets::COLOR_NONE) {
                    ImGui::PopStyleColor();
                }
            } break;
            case WIDGET_TEXT: {
                ImGui::AlignTextToFramePadding();
                ImGui::Text(widget.name.c_str());
            } break;
            case WIDGET_COMBOBOX: {
                int32_t* pointer = std::get<int32_t*>(widget.valuePointer);
                if (pointer == nullptr) {
                    SPDLOG_ERROR("Combobox Widget requires a value pointer, currently nullptr");
                    assert(false);
                    return;
                }
                if (UIWidgets::Combobox(widget.name.c_str(), pointer, widget.comboMap,
                                        std::get<UIWidgets::ComboboxOptions>(widget.options))) {
                    if (widget.callback != nullptr) {
                        widget.callback(widget);
                    }
                };
            } break;
            case WIDGET_CVAR_COMBOBOX: {
                if (UIWidgets::CVarCombobox(widget.name.c_str(), widget.cVar, widget.comboMap,
                                            std::get<UIWidgets::ComboboxOptions>(widget.options))) {
                    if (widget.callback != nullptr) {
                        widget.callback(widget);
                    }
                }
            } break;
            case WIDGET_SLIDER_INT: {
                int32_t* pointer = std::get<int32_t*>(widget.valuePointer);
                if (pointer == nullptr) {
                    SPDLOG_ERROR("int32 Slider Widget requires a value pointer, currently nullptr");
                    assert(false);
                    return;
                }
                if (UIWidgets::SliderInt(widget.name.c_str(), pointer,
                                         std::get<UIWidgets::IntSliderOptions>(widget.options))) {
                    if (widget.callback != nullptr) {
                        widget.callback(widget);
                    }
                };
            } break;
            case WIDGET_SLIDER_FLOAT: {
                float* pointer = std::get<float*>(widget.valuePointer);

                if (pointer == nullptr) {
                    SPDLOG_ERROR("float Slider Widget requires a value pointer, currently nullptr");
                    assert(false);
                    return;
                }
                if (UIWidgets::SliderFloat(widget.name.c_str(), pointer,
                                           std::get<UIWidgets::FloatSliderOptions>(widget.options))) {
                    if (widget.callback != nullptr) {
                        widget.callback(widget);
                    }
                }
            } break;
            case WIDGET_CVAR_SLIDER_INT: {
                if (UIWidgets::CVarSliderInt(widget.name.c_str(), widget.cVar,
                                             std::get<UIWidgets::IntSliderOptions>(widget.options))) {
                    if (widget.callback != nullptr) {
                        widget.callback(widget);
                    }
                };
            } break;
            case WIDGET_CVAR_SLIDER_FLOAT: {
                if (UIWidgets::CVarSliderFloat(widget.name.c_str(), widget.cVar,
                                               std::get<UIWidgets::FloatSliderOptions>(widget.options))) {
                    if (widget.callback != nullptr) {
                        widget.callback(widget);
                    }
                }
            } break;
            case WIDGET_BUTTON: {
                if (UIWidgets::Button(widget.name.c_str(), std::get<UIWidgets::ButtonOptions>(widget.options))) {
                    if (widget.callback != nullptr) {
                        widget.callback(widget);
                    }
                }
            } break;
            case WIDGET_WINDOW_BUTTON: {
                if (widget.windowName == nullptr || widget.windowName[0] == '\0') {
                    std::string msg =
                        fmt::format("Error drawing window contents for {}: windowName not defined", widget.name);
                    SPDLOG_ERROR(msg.c_str());
                    break;
                }
                auto window = Ship::Context::GetInstance()->GetWindow()->GetGui()->GetGuiWindow(widget.windowName);
                if (!window) {
                    std::string msg =
                        fmt::format("Error drawing window contents: windowName {} does not exist", widget.windowName);
                    SPDLOG_ERROR(msg.c_str());
                    break;
                }
                UIWidgets::WindowButton(widget.name.c_str(), widget.cVar, window,
                                        std::get<UIWidgets::ButtonOptions>(widget.options));
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
                                if (info.type == WIDGET_SEARCH || info.type == WIDGET_SEPARATOR ||
                                    info.type == WIDGET_SEPARATOR_TEXT || info.isHidden) {
                                    continue;
                                }
                                std::string widgetStr =
                                    std::string(info.name) + std::string(info.tooltip != NULL ? info.tooltip : "");
                                std::transform(menuSearchText.begin(), menuSearchText.end(), menuSearchText.begin(),
                                               ::tolower);
                                menuSearchText.erase(std::remove(menuSearchText.begin(), menuSearchText.end(), ' '),
                                                     menuSearchText.end());
                                std::transform(widgetStr.begin(), widgetStr.end(), widgetStr.begin(), ::tolower);
                                widgetStr.erase(std::remove(widgetStr.begin(), widgetStr.end(), ' '), widgetStr.end());
                                if (widgetStr.find(menuSearchText) != std::string::npos) {
                                    MenuDrawItem(info);
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
        SPDLOG_ERROR("Failed to draw menu item \"{}\" due to: {}", widget.name, e.what());
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
                        if (info.type == WIDGET_SEPARATOR || info.type == WIDGET_SEPARATOR_TEXT || info.isHidden) {
                            continue;
                        }
                        std::string widgetStr =
                            std::string(info.name) + std::string(info.tooltip != NULL ? info.tooltip : "");
                        std::transform(menuSearchText.begin(), menuSearchText.end(), menuSearchText.begin(), ::tolower);
                        std::transform(widgetStr.begin(), widgetStr.end(), widgetStr.begin(), ::tolower);
                        widgetStr.erase(std::remove(widgetStr.begin(), widgetStr.end(), ' '), widgetStr.end());
                        if (widgetStr.find(menuSearchText) != std::string::npos) {
                            MenuDrawItem(info);
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
                MenuDrawItem(entry);
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
} // namespace Ship
