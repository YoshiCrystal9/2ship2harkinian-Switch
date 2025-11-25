#ifndef BenGui_hpp
#define BenGui_hpp

#include <BenGui/UIWidgets.hpp>
#include "2s2h/BenGui/InputViewer.h"
#include "BenModals.h"

namespace BenGui {
    void SetupHooks();
    void SetupGuiElements();
    void Draw();
    void Destroy();
    UIWidgets::Colors GetMenuThemeColor();
}

#define THEME_COLOR BenGui::GetMenuThemeColor()

#endif /* BenGui_hpp */
