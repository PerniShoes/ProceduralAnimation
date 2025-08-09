#include <cmath>
#include <iostream>
#include <print>
#include <array>

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
    float linkConstraintR{45.0f};

    float startX{10.0f};
    float startY{400.0f};
    float xOffset{0};
   // std::array<float,15> snakeBody{40.0f,50.0f,50.0f,40.0f,35.0f,35.0f,30.0f,25.0f,20.0f,15.0f,10.0f,10.0f,5.0f,5.0f,2.0f};
   
  //  std::array<float,100> snakeBody{  // 25 steps 40 -> 55
  //40.0f, 40.6f, 41.2f, 41.8f, 42.4f, 43.0f, 43.6f, 44.2f, 44.8f, 45.4f,
  //46.0f, 46.6f, 47.2f, 47.8f, 48.4f, 49.0f, 49.6f, 50.2f, 50.8f, 51.4f,
  //52.0f, 52.6f, 53.2f, 53.8f, 54.4f,

  //// 25 steps 55 -> 40
  //55.0f, 54.4f, 53.8f, 53.2f, 52.6f, 52.0f, 51.4f, 50.8f, 50.2f, 49.6f,
  //49.0f, 48.4f, 47.8f, 47.2f, 46.6f, 46.0f, 45.4f, 44.8f, 44.2f, 43.6f,
  //43.0f, 42.4f, 41.8f, 41.2f, 40.6f,

  //// 40 steps 40 -> 5
  //40.0f, 38.6f, 37.2f, 35.8f, 34.4f, 33.0f, 31.6f, 30.2f, 28.8f, 27.4f,
  //26.0f, 24.6f, 23.2f, 21.8f, 20.4f, 19.0f, 17.6f, 16.2f, 14.8f, 13.4f,
  //12.0f, 10.6f, 9.2f, 7.8f, 6.4f, 5.0f, 5.0f, 5.0f, 5.0f, 5.0f,
  //5.0f, 5.0f, 5.0f, 5.0f, 5.0f, 5.0f, 5.0f, 5.0f, 5.0f, 5.0f,

  //// last 5 steps all 5
  //5.0f, 5.0f, 5.0f, 5.0f, 5.0f};
  
    std::array<float,5> snakeBody{50.0f,40.0f,30.0f,20.0f,15.0f};

    int add{int(snakeBody.size())};
    int rows{1};
    for (int index{0}; index < rows; index++)
    {
        for (int i{0}; i < add; i++)
        {
            float radius = snakeBody[i];
            Point2f position{startX + xOffset + radius
                ,startY};

            m_Links.push_back(make_unique<ChainLink>(Circlef{position,snakeBody[i]}
            ,linkConstraintR));
            xOffset += (radius * 2) + 5;
        }
    }
    // Sides setup
    for (int i{0}; i < m_Links.size(); i++)
    {
        if (i != 0)
        {
            m_Links[i]->CalculateSidePoints(1);
        }
        else
        {
            m_Links[i]->CalculateSidePoints(2);
        }
    }

    // Automatic constraining
    for (int i{0}; i < snakeBody.size(); i++)
    {
        if (i == snakeBody.size() - 1) // Last Link has nothing to Constrain to it
            break;
        m_Links[i + 1]->ConstrainTo(m_Links[i]->GetCenter(),m_Links[i]->GetConstraintR());
    }
    m_MainLinks.push_back(0);
    UpdateLinkColors();
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

    // SetAngle of head based on an angle of a link that points to it

    for (int i{0}; i < m_MainLinks.size(); i++)
    {
        auto sharedMain = m_Links[m_MainLinks[i]]->GetCenter().lock();
        for (int index{0}; index < m_Links.size(); index++)
        {
            auto shared = m_Links[index]->GetTarget().lock();
            if (shared == sharedMain)
            {
                m_Links[m_MainLinks[i]]->SetAngle(m_Links[index]->GetAngle());
                break;
            }
        }
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
    if (m_LinksSelected.size() == 0)
        return;
    for (int i{0}; i < m_LinksSelected.size(); i++)
    {
        if (i == m_LinksSelected.size() - 1) // Last Link has nothing to Constrain to it
            break;
        // Constrain second link to first // third to second...
        m_Links[m_LinksSelected[i + 1]]->UnConstrain();
        m_Links[m_LinksSelected[i+1]]->ConstrainTo(m_Links[m_LinksSelected[i]]->GetCenter(),m_Links[m_LinksSelected[i]]->GetConstraintR());
    }
    m_Links[m_LinksSelected[0]]->UnConstrain();
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
            m_Links[i]->UnConstrain();
            m_MainLinks.push_back(i);
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
void ProceduralAnimation::CalculateArcs()
{


}