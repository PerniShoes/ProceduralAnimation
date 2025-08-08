#include <cmath>
#include <iostream>
#include <print>

#include "Time.h"
#include "Texture.h"
#include "OrientationManager.h"
#include "ChainLink.h"

#include "ProceduralAnimation.h" // Project

ProceduralAnimation::ProceduralAnimation()
    :m_LastMousePos{0.0f,0.0f}
    ,m_LBHeldTimer{std::make_unique<Time>(0.1f)}
    ,m_ConstrainedToMouse{-1}
    ,m_DrawContraint{false}

{
    using namespace std;
    float linkRadius{10.0f};
    float linkConstraintR{50.0f};

    int add{10};
    int rows{5};
    for (int index{0}; index < rows; index++)
    {
        for (int i{0}; i < add; i++)
        {
            m_Links.push_back(make_unique<ChainLink>(Circlef{Point2f{50.0f + (linkRadius * 4) * i,50.0f+ (linkRadius * 4)*index},linkRadius},linkConstraintR));
        }
    }

}

ProceduralAnimation::~ProceduralAnimation()
{
    Cleanup();
}
void ProceduralAnimation::Cleanup()
{

}

void ProceduralAnimation::Draw() const
{
    glPushMatrix();

    glTranslatef(0.0f,0.0f,0.0f);
    glScalef(1.0f,1.0f,0.0f);

    for (int i{0}; i < m_Links.size(); i++)
    {
        m_Links[i]->Draw(m_DrawContraint);
    }
  
    glPopMatrix();
}
void ProceduralAnimation::DrawUI(Rectf viewport) const
{

}
void ProceduralAnimation::DrawLinks() const
{




}

void ProceduralAnimation::Update(float elapsedTime)
{

    m_LBHeldTimer->Update(elapsedTime);

    for (int i{0}; i < m_Links.size(); i++)
    {
        m_Links[i]->Update(); // Have manager class and things like UpdateAll()
    }
    
   
}
void ProceduralAnimation::UpdateTextures() 
{

}
void ProceduralAnimation::UnConstrainAll()
{
    for (int i{0}; i < m_Links.size(); i++)
    {
        m_Links[i]->UnConstrain();
    }
    m_MainLinks.clear();
    UpdateLinkColors();
}

void ProceduralAnimation::OnMouseDown()
{
    m_LBHeldTimer->Start();

    SelectLink();
    UpdateLinkColors();
}
void ProceduralAnimation::OnMouseUp()
{
    m_LBHeldTimer->RestartAndPause();


}
void ProceduralAnimation::OnMouseMotion(const Point2f& mousePos)
{
    m_LastMousePos = mousePos;

}

void ProceduralAnimation::SelectLink()
{
    using namespace utils;

    for (int i{0}; i < m_Links.size(); i++)
    {
        if (IsPointInCircle(m_LastMousePos,m_Links[i]->GetBounds()))
        {
            if (m_LinksSelected.size() == 0)
            {
                m_LinksSelected.push_back(i);
                break;
            }
            // Duplicate check
            for (int index{0}; index < m_LinksSelected.size(); index++)
            {
                // Last Link
                if (index == m_LinksSelected.size() - 1)
                    if (m_LinksSelected[index] != i)
                    {
                        m_LinksSelected.push_back(i);
                        break;
                    }
                // Erase duplicate
                if (m_LinksSelected[index] == i)
                {
                    m_LinksSelected.erase(m_LinksSelected.begin() + index);
                    break;
                }
            }
            break; // Makes it so only one Link can be clicked on at once
        }
    }
}
void ProceduralAnimation::UpdateLinkColors()
{
    using namespace PrettyColors;

    int mainLinksFound{0};
    for (int i{0}; i < m_Links.size(); i++)
    {
        // Color to white
        m_Links[i]->SetLinkColor(GetColor(white));

        // Color Main links to rose
        if (mainLinksFound < m_MainLinks.size())
        {
            for (int index{0}; index < m_MainLinks.size(); index++)
            {
                if (i == m_MainLinks[index])
                {
                    m_Links[i]->SetLinkColor(GetColor(rose));
                    mainLinksFound += 1;
                }
            }
        }

        if (m_LinksSelected.size() == 0)
            continue;
        // Color Selected links to red
        for (int index{0}; index < m_LinksSelected.size(); index++)
        {
            if (i == m_LinksSelected[index])
            {
                m_Links[i]->SetLinkColor(GetColor(red));
            }
        }
    }
}
void ProceduralAnimation::UpdateMainLinks()
{
    for (int i{0}; i < m_MainLinks.size(); i++)
    {
        if (m_Links[m_MainLinks[i]]->IsConstrained())
        {
            m_MainLinks[i] = -1;
        }
    }
    std::erase(m_MainLinks,-1);
}
void ProceduralAnimation::ConstrainSelectedLinks()
{
    for (int i{0}; i < m_LinksSelected.size(); i++)
    {
        if (i == m_LinksSelected.size() - 1) // Last Link has nothing to Constrain to it
            break;
        // Constrain second link to first // third to second...
        m_Links[m_LinksSelected[i+1]]->ConstrainTo(m_Links[m_LinksSelected[i]]->GetCenter(),m_Links[m_LinksSelected[i]]->GetConstraintR());
    }
    m_MainLinks.push_back(m_LinksSelected[0]);
    UpdateMainLinks();

    m_LinksSelected.clear();
    UpdateLinkColors();
}
void ProceduralAnimation::ConstrainToMouse()
{
    using namespace utils;
    if (m_ConstrainedToMouse >= 0) // Something already constrained
    {
        m_Links[m_ConstrainedToMouse]->UnConstrain();
        m_ConstrainedToMouse = -1;
        return;
    }
    for (int i{0}; i < m_Links.size(); i++)
    {
        if (IsPointInCircle(m_LastMousePos,m_Links[i]->GetBounds()))
        {
            m_Links[i]->ConstrainTo(m_LastMousePos);
            m_ConstrainedToMouse = i;
            break; 
        }
    }
}
void ProceduralAnimation::SwitchContraintDrawing()
{
    if (m_DrawContraint == true)
    {
        m_DrawContraint = false;
    }
    else
    {
        m_DrawContraint = true;
    }
}