#ifndef BenGui_hpp
#define BenGui_hpp

#include <BenGui/UIWidgets.hpp>
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
