#pragma once

#include <libultraship/libultraship.h>
#include "UIWidgets.hpp"
#include "2s2h/Enhancements/Enhancements.h"
#include "graphic/Fast3D/gfx_rendering_api.h"
#include "2s2h/DeveloperTools/DeveloperTools.h"
#include "MenuTypes.h"

namespace Ship {
class Menu : public GuiWindow {
  public:
    using GuiWindow::GuiWindow;

    Menu(const std::string& cVar, const std::string& name, uint8_t searchSidebarIndex_ = 0,
         ColorOption menuThemeIndex_ = COLOR_INDIGO);

    void InitElement() override;
    void DrawElement() override;
    void UpdateElement() override;
    void Draw() override;
    void InsertSidebarSearch();
    void RemoveSidebarSearch();
    void UpdateWindowBackendObjects();

  protected:
    ImVec2 mOriginalSize;
    std::string mName;
    uint32_t mWindowFlags;
    std::vector<MainMenuEntry> menuEntries;
    void MenuDrawItem(widgetInfo& widget);
    ImGuiTextFilter menuSearch;
    uint8_t searchSidebarIndex;
    ColorOption menuThemeIndex;
    std::shared_ptr<std::vector<Ship::WindowBackend>> availableWindowBackends;
    std::unordered_map<Ship::WindowBackend, const char*> availableWindowBackendsMap;
    Ship::WindowBackend configWindowBackend;

    std::unordered_map<uint32_t, disabledInfo> disabledMap;
    const SidebarEntry searchSidebarEntry = {
        .label = "Search",
        .columnCount = 1,
        .columnWidgets = { { { .name = "Sidebar Search", .tooltip = "Searches all menus for the given text, including tooltips.", .type = WIDGET_SEARCH, .options = UIWidgets::SeparatorOptions{} } } }
    };;

  private:
    bool allowPopout = true; // PortNote: should be set to false on small screen ports
    bool popped;
    ImVec2 poppedSize;
    ImVec2 poppedPos;
    float windowHeight;
    float windowWidth;
};
} // namespace Ship