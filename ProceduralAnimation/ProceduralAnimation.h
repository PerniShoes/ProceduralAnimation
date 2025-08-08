#pragma once
#include <SDL.h>
#include <vector>
#include <memory>
#include <array>

#include "utils.h"
#include "PrettyColors.h"

// HOW TO CHANGE PROJECT NAME:
// -Replace all "ProceduralAnimation" in ProceduralAnimation.h and in ProceduralAnimation.cpp (make sure #include in .cpp changes too)
// -Rename both files 
// -Replace all "ProceduralAnimation" in Game.h and in Game.cpp             .h->(Forward declaration, unique_ptr<ProceduralAnimation>) .cpp->(#include, make_unique<ProceduralAnimation>)
// -Replace all "m_ProjectName" in Game.h and in Game.cpp (optional)
// In case of issues all relevant lines, in Game.h/cpp files, are marked with "// Project"

class Time;
class Texture;
class ChainLink;

class ProceduralAnimation
{

public:
    ProceduralAnimation();
    ProceduralAnimation(const ProceduralAnimation& gol) = delete;
    ProceduralAnimation& operator=(const ProceduralAnimation& gol) = delete;
    ProceduralAnimation(ProceduralAnimation&& gol) = delete;
    ProceduralAnimation& operator=(ProceduralAnimation&& gol) = delete;

    ~ProceduralAnimation();

    void Draw() const;
    void DrawLinks() const;
    void DrawLink(ChainLink target) const;
    void DrawUI(Rectf viewport) const; 

    void Update(float elapsedTime);
    void UpdateLinks();
    void UpdateTextures(); 

    void OnMouseUp();
    void OnMouseDown();
    void OnMouseMotion(const Point2f& mousePos);  
    // Instead of having functions fire off at key/button press, have them fire off at release 
    // (like Pyke Q, press = start charge, hold = keep charging, release = fire off)
    // If you don't need charging/holding then you can just do it on press

    void SelectLink();
    void UpdateLinkColors();
    void ConstrainSelectedLinks();
    void SwitchContraintDrawing();

    void UnConstrainAll();
    void ConstrainToMouse();
    void UpdateMainLinks();

private:
    // Add FIX to things that need to be fixed, later check FIX in search tool
    // Same with DELETE
    
    void Cleanup();

    // Mouse
    Point2f m_LastMousePos; 
    int m_ConstrainedToMouse;
    std::unique_ptr<Time> m_LBHeldTimer; 

    std::vector<int> m_MainLinks;
    std::vector<std::unique_ptr<ChainLink>> m_Links;
    std::vector<int> m_LinksSelected;

    bool m_DrawContraint;

};

