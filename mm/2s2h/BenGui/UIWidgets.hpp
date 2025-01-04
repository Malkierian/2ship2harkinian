#ifndef UIWidgets_hpp
#define UIWidgets_hpp

#include <string>
#include <vector>
#include <span>
#include <stdint.h>
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <libultraship/libultraship.h>
#include <unordered_map>
#include "2s2h/ShipUtils.h"
#include "2s2h/ShipInit.hpp"

namespace UIWidgets {

    using SectionFunc = void(*)();

    struct TextFilters {
        static int FilterNumbers(ImGuiInputTextCallbackData* data) {
            if (data->EventChar < 256 && strchr("1234567890", (char)data->EventChar)) {
                return 0;
            }
            return 1;
        }

        static int FilterAlphaNum(ImGuiInputTextCallbackData* data) {
            const char* alphanum = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWYZ0123456789";
            if (data->EventChar < 256 && strchr(alphanum, (char)data->EventChar)) {
                return 0;
            }
            return 1;
        }
    };

    std::string WrappedText(const char* text, unsigned int charactersPerLine = 60);
    std::string WrappedText(const std::string& text, unsigned int charactersPerLine = 60);
    void Tooltip(const char* text);

    // mostly in order for colors usable by the menu without custom text color
    enum Color {
        Pink,
        Red,
        DarkRed,
        Orange,
        LightGreen,
        Green,
        DarkGreen,
        LightBlue,
        Blue,
        DarkBlue,
        Indigo,
        Violet,
        Purple,
        Brown,
        DarkBrown,
        LightGray,
        Gray,
        DarkGray,
        // not suitable for menu theme use
        Yellow,
        Cyan,
        Black,
        White,
        NoColor
    };

    const std::unordered_map<Color, ImVec4> Colors = {
        { Color::Pink, ImVec4(0.87f, 0.3f, 0.87f, 1.0f) },
        { Color::Red, ImVec4(0.5f, 0.0f, 0.0f, 1.0f) },
        { Color::DarkRed, ImVec4(0.3f, 0.0f, 0.0f, 1.0f) },
        { Color::Orange, ImVec4(0.95f, 0.627f, 0.0f, 1.0f) },
        { Color::Yellow, ImVec4(0.95f, 0.95f, 0.0f, 1.0f) },
        { Color::LightGreen, ImVec4(0.0f, 0.7f, 0.0f, 1.0f) },
        { Color::Green, ImVec4(0.0f, 0.5f, 0.0f, 1.0f) },
        { Color::DarkGreen, ImVec4(0.0f, 0.3f, 0.0f, 1.0f) },
        { Color::Cyan, ImVec4(0.0f, 0.9f, 0.9f, 1.0f) },
        { Color::LightBlue, ImVec4(0.0f, 0.39f, 1.0f, 1.0f) },
        { Color::Blue, ImVec4(0.18f, 0.13f, 0.89f, 1.0f) },
        { Color::DarkBlue, ImVec4(0.05f, 0.0f, 0.65f, 1.0f) },
        { Color::Indigo, ImVec4(0.4f, 0.0f, 0.87f, 1.0f) },
        { Color::Violet, ImVec4(0.6f, 0.0f, 1.0f, 1.0f) },
        { Color::Purple, ImVec4(0.31f, 0.0f, 0.67f, 1.0f) },
        { Color::Brown, ImVec4(0.5f, 0.3f, 0.08f, 1.0f) },
        { Color::DarkBrown, ImVec4(0.26f, 0.13f, 0.0f, 1.0f) },
        { Color::LightGray, ImVec4(0.75f, 0.75f, 0.75f, 1.0f) },
        { Color::Gray, ImVec4(0.4f, 0.4f, 0.4f, 1.0f) },
        { Color::DarkGray, ImVec4(0.1f, 0.1f, 0.1f, 1.0f) },
        { Color::Black, ImVec4(0.0f, 0.0f, 0.0f, 1.0f)},
        { Color::White, ImVec4(1.0f, 1.0f, 1.0f, 1.0f) },
        { Color::NoColor, ImVec4(0.0f, 0.0f, 0.0f, 0.0f)},
    };

    namespace Sizes {
        const ImVec2 Inline = ImVec2(0.0f, 0.0f);
        const ImVec2 Fill = ImVec2(-1.0f, 0.0f);
    }

    enum LabelPosition {
        Near,
        Far,
        Above,
        None,
        Within,
    };

    enum ComponentAlignment {
        Left,
        Right,
    };

    struct ButtonOptions {
        Color color = Color::Gray;
        ImVec2 size = Sizes::Fill;
        const char* tooltip = "";
        bool disabled = false;
        const char* disabledTooltip = "";
    };

    struct CheckboxOptions {
        Color color = Color::DarkBlue;
        const char* tooltip = "";
        bool disabled = false;
        const char* disabledTooltip = "";
        bool defaultValue = false; // Only applicable to CVarCheckbox
        ComponentAlignment alignment = ComponentAlignment::Left;
        LabelPosition labelPosition = LabelPosition::Near;
    };

    struct ComboboxOptions {
        Color color = Color::Indigo;
        const char* tooltip = "";
        bool disabled = false;
        const char* disabledTooltip = "";
        uint32_t defaultIndex = 0; // Only applicable to CVarCombobox
        ComponentAlignment alignment = ComponentAlignment::Left;
        LabelPosition labelPosition = LabelPosition::Above;
        ImGuiComboFlags flags = 0;
    };

    struct IntSliderOptions {
        Color color = Color::Gray;
        const char* tooltip = "";
        bool disabled = false;
        const char* disabledTooltip = "";
        bool showButtons = true;
        ImGuiSliderFlags flags = 0;
        const char* format = "%d";
        int32_t step = 1;
        int32_t min = 1;
        int32_t max = 10;
        int32_t defaultValue = 1;
        ComponentAlignment alignment = ComponentAlignment::Left;
        LabelPosition labelPosition = LabelPosition::Above;
    };

    struct FloatSliderOptions {
        Color color = Color::Gray;
        const char* tooltip = "";
        bool disabled = false;
        const char* disabledTooltip = "";
        bool showButtons = true;
        ImGuiSliderFlags flags = 0;
        const char* format = "%f";
        float step = 0.01f;
        float min = 0.01f;
        float max = 10.0f;
        float defaultValue = 1.0f;
        bool isPercentage = false; // Multiplies visual value by 100
        ComponentAlignment alignment = ComponentAlignment::Left;
        LabelPosition labelPosition = LabelPosition::Above;
    };

    struct TextOptions {
        Color color = Color::NoColor;
        const char* tooltip = "";
    };

    void PushStyleMenu(const ImVec4& color);
    void PushStyleMenu(Color color = Color::DarkBlue);
    void PopStyleMenu();
    bool BeginMenu(const char* label, Color color = Color::DarkBlue);

    void PushStyleMenuItem(const ImVec4& color);
    void PushStyleMenuItem(Color color = Color::DarkBlue);
    void PopStyleMenuItem();
    bool MenuItem(const char* label, const char* shortcut = NULL, Color color = Color::DarkBlue);

    void PushStyleButton(const ImVec4& color);
    void PushStyleButton(Color color = Color::Gray);
    void PopStyleButton();
    bool Button(const char* label, const ButtonOptions& options = {});
    bool WindowButton(const char* label, const char* cvarName, std::shared_ptr<Ship::GuiWindow> windowPtr, const ButtonOptions& options = {});

    void PushStyleCheckbox(const ImVec4& color);
    void PushStyleCheckbox(Color color = Color::DarkBlue);
    void PopStyleCheckbox();
    void RenderText(ImVec2 pos, const char* text, const char* text_end, bool hide_text_after_hash);
    bool Checkbox(const char* label, bool* v, const CheckboxOptions& options = {});
    bool CVarCheckbox(const char* label, const char* cvarName, const CheckboxOptions& options = {});

    void PushStyleCombobox(const ImVec4& color);
    void PushStyleCombobox(Color color = Color::DarkBlue);
    void PopStyleCombobox();

    template <typename T>
    bool Combobox(const char* label, T* value, const std::unordered_map<T, const char*>& comboMap, const ComboboxOptions& options = {}) {
        bool dirty = false;
        float startX = ImGui::GetCursorPosX();
        std::string invisibleLabelStr = "##" + std::string(label);
        const char* invisibleLabel = invisibleLabelStr.c_str();
        ImGui::PushID(label);
        ImGui::BeginGroup();
        ImGui::BeginDisabled(options.disabled);
        PushStyleCombobox(options.color);
        if (options.alignment == ComponentAlignment::Left) {
            if (options.labelPosition == LabelPosition::Above) {
                ImGui::Text("%s", label);
                ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
            } else if (options.labelPosition == LabelPosition::Near) {
                ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - ImGui::CalcTextSize(label).x - ImGui::GetStyle().ItemSpacing.x * 2);
            } else if (options.labelPosition == LabelPosition::Far || options.labelPosition == LabelPosition::None) {
                ImGui::SetNextItemWidth(ImGui::CalcTextSize(comboMap.at(*value)).x + ImGui::GetStyle().FramePadding.x * 4 + ImGui::GetStyle().ItemSpacing.x);
            }
        } else if (options.alignment == ComponentAlignment::Right) {
            if (options.labelPosition == LabelPosition::Above) {
                ImGui::NewLine();
                ImGui::SameLine(ImGui::GetContentRegionAvail().x - ImGui::CalcTextSize(label).x);
                ImGui::Text("%s", label);
                ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
            } else if (options.labelPosition == LabelPosition::Near) {
                ImGui::SameLine(ImGui::CalcTextSize(label).x + ImGui::GetStyle().ItemSpacing.x * 2);
                ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
            } else if (options.labelPosition == LabelPosition::Far || options.labelPosition == LabelPosition::None) {
                float width = ImGui::CalcTextSize(comboMap.at(*value)).x + ImGui::GetStyle().FramePadding.x * 4;
                ImGui::SameLine(ImGui::GetContentRegionAvail().x - width);
                ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
            }
        }
        if (ImGui::BeginCombo(invisibleLabel, comboMap.at(*value), options.flags)) {
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(10.0f, 10.0f));
            for (const auto& pair : comboMap) {
                if (strlen(pair.second) > 1) {
                    if (ImGui::Selectable(pair.second, pair.first == *value)) {
                        *value = pair.first;
                        dirty = true;
                    }
                }
            }
            ImGui::PopStyleVar();
            ImGui::EndCombo();
        }
        if (options.alignment == ComponentAlignment::Left) {
            if (options.labelPosition == LabelPosition::Near) {
                ImGui::SameLine();
                ImGui::Text("%s", label);
            } else if (options.labelPosition == LabelPosition::Far) {
                ImGui::SameLine(ImGui::GetContentRegionAvail().x - ImGui::CalcTextSize(label).x);
                ImGui::Text("%s", label);
            }
        } else if (options.alignment == ComponentAlignment::Right) {
            if (options.labelPosition == LabelPosition::Near || options.labelPosition == LabelPosition::Far) {
                ImGui::SameLine(startX);
                ImGui::Text("%s", label);
            }
        }
        PopStyleCombobox();
        ImGui::EndDisabled();
        ImGui::EndGroup();
        if (options.disabled && ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled) && !Ship_IsCStringEmpty(options.disabledTooltip)) {
            ImGui::SetTooltip("%s", WrappedText(options.disabledTooltip).c_str());
        } else if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled) && !Ship_IsCStringEmpty(options.tooltip)) {
            ImGui::SetTooltip("%s", WrappedText(options.tooltip).c_str());
        }
        ImGui::PopID();
        return dirty;
    }

    template <typename T = size_t>
    bool Combobox(const char* label, T* value, const std::vector<const char*>& comboVector, const ComboboxOptions& options = {}) {
        bool dirty = false;
        float startX = ImGui::GetCursorPosX();
        size_t currentValueIndex = static_cast<size_t>(*value);
        std::string invisibleLabelStr = "##" + std::string(label);
        const char* invisibleLabel = invisibleLabelStr.c_str();
        ImGui::PushID(label);
        ImGui::BeginGroup();
        ImGui::BeginDisabled(options.disabled);
        PushStyleCombobox(options.color);
        if (options.alignment == ComponentAlignment::Left) {
            if (options.labelPosition == LabelPosition::Above) {
                ImGui::Text(label);
                ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
            } else if (options.labelPosition == LabelPosition::Near) {
                ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - ImGui::CalcTextSize(label).x - ImGui::GetStyle().ItemSpacing.x * 2);
            } else if (options.labelPosition == LabelPosition::Far || options.labelPosition == LabelPosition::None) {
                ImGui::SetNextItemWidth(ImGui::CalcTextSize(comboVector.at(currentValueIndex)).x + ImGui::GetStyle().FramePadding.x * 4 + ImGui::GetStyle().ItemSpacing.x);
            }
        } else if (options.alignment == ComponentAlignment::Right) {
            if (options.labelPosition == LabelPosition::Above) {
                ImGui::NewLine();
                ImGui::SameLine(ImGui::GetContentRegionAvail().x - ImGui::CalcTextSize(label).x);
                ImGui::Text(label);
                ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
            } else if (options.labelPosition == LabelPosition::Near) {
                ImGui::SameLine(ImGui::CalcTextSize(label).x + ImGui::GetStyle().ItemSpacing.x * 2);
                ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
            } else if (options.labelPosition == LabelPosition::Far || options.labelPosition == LabelPosition::None) {
                float width = ImGui::CalcTextSize(comboVector.at(currentValueIndex)).x + ImGui::GetStyle().FramePadding.x * 4;
                ImGui::SameLine(ImGui::GetContentRegionAvail().x - width);
                ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
            }
        }
        if (ImGui::BeginCombo(invisibleLabel, comboVector.at(currentValueIndex), options.flags)) {
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(10.0f, 10.0f));
            for (size_t i = 0; i < comboVector.size(); ++i) {
                auto newValue = static_cast<T>(i);
                if (strlen(comboVector.at(i)) > 1) {
                    if (ImGui::Selectable(comboVector.at(i), newValue == *value)) {
                        *value = newValue;
                        dirty = true;
                    }
                }
            }
            ImGui::PopStyleVar();
            ImGui::EndCombo();
        }
        if (options.alignment == ComponentAlignment::Left) {
            if (options.labelPosition == LabelPosition::Near) {
                ImGui::SameLine();
                ImGui::Text(label);
            } else if (options.labelPosition == LabelPosition::Far) {
                ImGui::SameLine(ImGui::GetContentRegionAvail().x - ImGui::CalcTextSize(label).x);
                ImGui::Text(label);
            }
        } else if (options.alignment == ComponentAlignment::Right) {
            if (options.labelPosition == LabelPosition::Near || options.labelPosition == LabelPosition::Far) {
                ImGui::SameLine(startX);
                ImGui::Text(label);
            }
        }
        PopStyleCombobox();
        ImGui::EndDisabled();
        ImGui::EndGroup();
        if (options.disabled && ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled) && !Ship_IsCStringEmpty(options.disabledTooltip)) {
            ImGui::SetTooltip("%s", WrappedText(options.disabledTooltip).c_str());
        } else if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled) && !Ship_IsCStringEmpty(options.tooltip)) {
            ImGui::SetTooltip("%s", WrappedText(options.tooltip).c_str());
        }
        ImGui::PopID();
        return dirty;
    }

    template <typename T = size_t, size_t N>
    bool Combobox(const char* label, T* value, const char* (&comboArray)[N], const ComboboxOptions& options = {}) {
        bool dirty = false;
        float startX = ImGui::GetCursorPosX();
        size_t currentValueIndex = static_cast<size_t>(*value);
        if (currentValueIndex >= N) {
            currentValueIndex = 0;
        }
        std::string invisibleLabelStr = "##" + std::string(label);
        const char* invisibleLabel = invisibleLabelStr.c_str();
        ImGui::PushID(label);
        ImGui::BeginGroup();
        ImGui::BeginDisabled(options.disabled);
        PushStyleCombobox(options.color);
        if (options.alignment == ComponentAlignment::Left) {
            if (options.labelPosition == LabelPosition::Above) {
                ImGui::Text("%s", label);
                ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
            } else if (options.labelPosition == LabelPosition::Near) {
                ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - ImGui::CalcTextSize(label).x - ImGui::GetStyle().ItemSpacing.x * 2);
            } else if (options.labelPosition == LabelPosition::Far || options.labelPosition == LabelPosition::None) {
                ImGui::SetNextItemWidth(ImGui::CalcTextSize(comboArray[currentValueIndex]).x + ImGui::GetStyle().FramePadding.x * 4 + ImGui::GetStyle().ItemSpacing.x);
            }
        } else if (options.alignment == ComponentAlignment::Right) {
            if (options.labelPosition == LabelPosition::Above) {
                ImGui::NewLine();
                ImGui::SameLine(ImGui::GetContentRegionAvail().x - ImGui::CalcTextSize(label).x);
                ImGui::Text("%s", label);
                ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
            } else if (options.labelPosition == LabelPosition::Near) {
                ImGui::SameLine(ImGui::CalcTextSize(label).x + ImGui::GetStyle().ItemSpacing.x * 2);
                ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
            } else if (options.labelPosition == LabelPosition::Far || options.labelPosition == LabelPosition::None) {
                float width = ImGui::CalcTextSize(comboArray[currentValueIndex]).x + ImGui::GetStyle().FramePadding.x * 4;
                ImGui::SameLine(ImGui::GetContentRegionAvail().x - width);
                ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
            }
        }
        if (ImGui::BeginCombo(invisibleLabel, comboArray[currentValueIndex], options.flags)) {
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(10.0f, 10.0f));
            for (size_t i = 0; i < N; ++i) {
                auto newValue = static_cast<T>(i);
                if (strlen(comboArray[i]) > 1) {
                    if (ImGui::Selectable(comboArray[i], newValue == *value)) {
                        *value = newValue;
                        dirty = true;
                    }
                }
            }
            ImGui::PopStyleVar();
            ImGui::EndCombo();
        }
        if (options.alignment == ComponentAlignment::Left) {
            if (options.labelPosition == LabelPosition::Near) {
                ImGui::SameLine();
                ImGui::Text("%s", label);
            } else if (options.labelPosition == LabelPosition::Far) {
                ImGui::SameLine(ImGui::GetContentRegionAvail().x - ImGui::CalcTextSize(label).x);
                ImGui::Text("%s", label);
            }
        } else if (options.alignment == ComponentAlignment::Right) {
            if (options.labelPosition == LabelPosition::Near || options.labelPosition == LabelPosition::Far) {
                ImGui::SameLine(startX);
                ImGui::Text("%s", label);
            }
        }
        PopStyleCombobox();
        ImGui::EndDisabled();
        ImGui::EndGroup();
        if (options.disabled && ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled) && !Ship_IsCStringEmpty(options.disabledTooltip)) {
            ImGui::SetTooltip("%s", WrappedText(options.disabledTooltip).c_str());
        } else if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled) && !Ship_IsCStringEmpty(options.tooltip)) {
            ImGui::SetTooltip("%s", WrappedText(options.tooltip).c_str());
        }
        ImGui::PopID();
        return dirty;
    }

    template <typename T = int32_t>
    bool CVarCombobox(const char* label, const char* cvarName, const std::unordered_map<T, const char*>& comboMap, const ComboboxOptions& options = {}) {
        bool dirty = false;
        int32_t value = CVarGetInteger(cvarName, options.defaultIndex);
        if (Combobox<T>(label, &value, comboMap, options)) {
            CVarSetInteger(cvarName, value);
            Ship::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesOnNextTick();
            ShipInit::Init(cvarName);
            dirty = true;
        }
        return dirty;
    }

    template <typename T = int32_t>
    bool CVarCombobox(const char* label, const char* cvarName, const std::vector<const char*>& comboVector, const ComboboxOptions& options = {}) {
        bool dirty = false;
        int32_t value = CVarGetInteger(cvarName, options.defaultIndex);
        if (Combobox<T>(label, &value, comboVector, options)) {
            CVarSetInteger(cvarName, value);
            Ship::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesOnNextTick();
            ShipInit::Init(cvarName);
            dirty = true;
        }
        return dirty;
    }

    template <typename T = int32_t, size_t N>
    bool CVarCombobox(const char* label, const char* cvarName, const char* (&comboArray)[N], const ComboboxOptions& options = {}) {
        bool dirty = false;
        int32_t value = CVarGetInteger(cvarName, options.defaultIndex);
        if (Combobox<T>(label, &value, comboArray, options)) {
            CVarSetInteger(cvarName, value);
            Ship::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesOnNextTick();
            ShipInit::Init(cvarName);
            dirty = true;
        }
        return dirty;
    }

    void PushStyleSlider(Color color = Color::DarkBlue);
    void PopStyleSlider();
    bool SliderInt(const char* label, int32_t* value, const IntSliderOptions& options = {});
    bool CVarSliderInt(const char* label, const char* cvarName, const IntSliderOptions& options = {});
    bool SliderFloat(const char* label, float* value, const FloatSliderOptions& options = {});
    bool CVarSliderFloat(const char* label, const char* cvarName, const FloatSliderOptions& options = {});
    bool CVarColorPicker(const char* label, const char* cvarName, Color_RGBA8 defaultColor);
    void DrawFlagArray32(const std::string& name, uint32_t& flags);
    void DrawFlagArray16(const std::string& name, uint16_t& flags);
    void DrawFlagArray8(const std::string& name, uint8_t& flags);
    void DrawFlagArray8Mask(const std::string& name, uint8_t& flags);
}

#endif /* UIWidgets_hpp */
