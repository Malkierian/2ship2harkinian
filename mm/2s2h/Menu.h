#pragma once

#include <libultraship/libultraship.h>
#include <2s2h/BenGui/UIWidgets.hpp>

namespace BenGui {
class BenMenu : public Ship::GuiWindow {
public:
    using Ship::GuiWindow::GuiWindow;

    void InitElement() override;
    void DrawElement() override;
    void UpdateElement() override;

private:
    bool allowPopout = true;  // PortNote: should be set to false on small screen ports
    bool popped;
    ImVec2 poppedSize;
    ImVec2 poppedPos;
    float windowHeight;
    float windowWidth;
    UIWidgets::SidebarEntry audioSettingsEntry;
    UIWidgets::MainMenuEntry settingsEntry;
};
}