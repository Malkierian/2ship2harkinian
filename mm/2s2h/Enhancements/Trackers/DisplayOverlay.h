#include <libultraship/libultraship.h>

void DisplayOverlayInitSettings();

class DisplayOverlayWindow : public Ship::GuiWindow {
  public:
    using GuiWindow::GuiWindow;

    void InitElement() override;
    void DrawElement() override{};
    void Draw() override;
    void UpdateElement() override{};
};