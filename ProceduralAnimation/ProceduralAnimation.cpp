#include <cmath>
#include <iostream>
#include <print>
#include <array>

#include "Time.h"
#include "Texture.h"
#include "OrientationManager.h"
#include "ChainLink.h"

#include "ProceduralAnimation.h" // Project
#include <algorithm>
#include <vector>
#include <ranges>

ProceduralAnimation::ProceduralAnimation()
    :m_LastMousePos{0.0f,0.0f}
    ,m_LBHeldTimer{std::make_unique<Time>(0.1f)}
    ,m_ConstrainedToMouse{-1}
    ,m_DrawContraint{false}

{

    using namespace std;
    float linkRadius{10.0f};
    float linkConstraintR{15.0f};

    float startX{10.0f};
    float startY{400.0f};
    float xOffset{0};
   // std::array<float,16> snakeBody{40.0f,50.0f,50.0f,40.0f,35.0f,35.0f,30.0f,25.0f,20.0f,15.0f,10.0f,10.0f,5.0f,5.0f,5.0f,5.0f};
   
    std::array<float,200> snakeBody{
        // Segment 1: 40 -> 55 (~55 points)
        40.0f, 40.3f, 40.6f, 40.9f, 41.2f, 41.5f, 41.8f, 42.1f, 42.4f, 42.7f,
        43.0f, 43.3f, 43.6f, 43.9f, 44.2f, 44.5f, 44.8f, 45.1f, 45.4f, 45.7f,
        46.0f, 46.3f, 46.6f, 46.9f, 47.2f, 47.5f, 47.8f, 48.1f, 48.4f, 48.7f,
        49.0f, 49.3f, 49.6f, 49.9f, 50.2f, 50.5f, 50.8f, 51.1f, 51.4f, 51.7f,
        52.0f, 52.3f, 52.6f, 52.9f, 53.2f, 53.5f, 53.8f, 54.1f, 54.4f, 54.7f,
        55.0f, 55.0f, 55.0f, 55.0f, 55.0f,

        // Segment 2: 55 -> 40 (~55 points)
        55.0f, 54.7f, 54.4f, 54.1f, 53.8f, 53.5f, 53.2f, 52.9f, 52.6f, 52.3f,
        52.0f, 51.7f, 51.4f, 51.1f, 50.8f, 50.5f, 50.2f, 49.9f, 49.6f, 49.3f,
        49.0f, 48.7f, 48.4f, 48.1f, 47.8f, 47.5f, 47.2f, 46.9f, 46.6f, 46.3f,
        46.0f, 45.7f, 45.4f, 45.1f, 44.8f, 44.5f, 44.2f, 43.9f, 43.6f, 43.3f,
        43.0f, 42.7f, 42.4f, 42.1f, 41.8f, 41.5f, 41.2f, 40.9f, 40.6f, 40.3f,
        40.0f, 40.0f, 40.0f, 40.0f, 40.0f,

        // Segment 3: 40 -> 10 (~80 points)
        40.0f, 39.6f, 39.2f, 38.8f, 38.4f, 38.0f, 37.6f, 37.2f, 36.8f, 36.4f,
        36.0f, 35.6f, 35.2f, 34.8f, 34.4f, 34.0f, 33.6f, 33.2f, 32.8f, 32.4f,
        32.0f, 31.6f, 31.2f, 30.8f, 30.4f, 30.0f, 29.6f, 29.2f, 28.8f, 28.4f,
        28.0f, 27.6f, 27.2f, 26.8f, 26.4f, 26.0f, 25.6f, 25.2f, 24.8f, 24.4f,
        24.0f, 23.6f, 23.2f, 22.8f, 22.4f, 22.0f, 21.6f, 21.2f, 20.8f, 20.4f,
        20.0f, 19.6f, 19.2f, 18.8f, 18.4f, 18.0f, 17.6f, 17.2f, 16.8f, 16.4f,
        16.0f, 15.6f, 15.2f, 14.8f, 14.4f, 14.0f, 13.6f, 13.2f, 12.8f, 12.4f,
        12.0f, 11.6f, 11.2f, 10.8f, 10.4f, 10.0f, 10.0f, 10.0f, 10.0f, 10.0f,

        // Last 10 points: 10 -> 5
        10.0f, 9.5f, 9.0f, 8.5f, 8.0f, 7.5f, 7.0f, 6.5f, 6.0f, 5.0f
    };
  
   // std::array<float,5> snakeBody{50.0f,40.0f,30.0f,20.0f,15.0f};
   // 
     //std::array<float,3> snakeBody{25.0f,20.0f,15.0f};

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
            m_Links[i]->SetHead();
            m_Links[i]->CalculateSidePoints(5);
        }
    }

    // Automatic constraining
    for (int i{0}; i < snakeBody.size(); i++)
    {
        if (i == snakeBody.size() - 1) // Last Link has nothing to Constrain to it
            break;
        m_Links[i + 1]->ConstrainTo(m_Links[i]->GetCenter(),m_Links[i]->GetConstraintR(),m_Links[i]->GetAngle());
    }
    m_MainLinks.push_back(0);
    UpdateLinkColors();


    m_AllSidePoints.reserve(snakeBody.size()*3); // FIX OR DELETE
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
 
    for (auto& link : std::ranges::reverse_view(m_Links))
    {
        link->Draw(m_DrawContraint);
    }
    DrawPolygon();
    glPopMatrix();
}
void ProceduralAnimation::DrawUI(Rectf viewport) const
{

}
void ProceduralAnimation::DrawLinks() const
{


}

void ProceduralAnimation::DrawPolygon()const
{
    using namespace utils;
    using namespace PrettyColors;

    std::vector<Point2f>polygonVerticies;
    std::vector<Point2f>temp;

    for (size_t i{0}; i < m_AllSidePoints.size() / 2; ++i)
    {
        for (size_t index{0}; index < m_AllSidePoints[i].size(); ++index)
        {
            polygonVerticies.push_back(m_AllSidePoints[i][index]);
        }
    }
    for (size_t i{m_AllSidePoints.size() / 2}; i < m_AllSidePoints.size(); ++i)
    {
        for (size_t index{0}; index < m_AllSidePoints[i].size(); ++index)
        {
            temp.push_back(m_AllSidePoints[i][index]);
        }
    }
    std::reverse_copy(temp.begin(),temp.end(),std::back_inserter(polygonVerticies));

    std::reverse(polygonVerticies.begin(),polygonVerticies.end());
    SetColor(GetColor(lime));
    //tils::DrawPolygon(polygonVerticies, true, 10.0f);

   // drawFilledPolygon(polygonVerticies);
    Point2f lastPoint{0.0f,0.0f};
    Point2f lastRightPoint{0.0f,0.0f};
    Point2f firstRightPoint{0.0f,0.0f};
    if (m_AllSidePoints.size() != 0)
    {
        firstRightPoint = m_AllSidePoints[0][0];
    }
    for (int i{0}; i < m_AllSidePoints.size(); ++i)
    {
        // Side switch (assuming equal right - left side points)
        if (i == m_AllSidePoints.size() / 2)
        {
            lastRightPoint = lastPoint;
            lastPoint = Point2f{0.0f,0.0f};
        }
        for (int index{0}; index < m_AllSidePoints[i].size(); ++index)
        {
            // Connecting last left and last right points
            if (i == m_AllSidePoints.size() - 1 && index == m_AllSidePoints[i].size() - 1)
            {
                SetColor(GetColor(white));
                DrawLine(lastRightPoint,m_AllSidePoints[i][index]);
            }
            // From first left to first right
            else if (i == m_AllSidePoints.size() / 2 && index == 0)
            {
                SetColor(GetColor(white));
                DrawLine(firstRightPoint,m_AllSidePoints[i][0]);
            }

            if (index == m_AllSidePoints[i].size() - 1)
            {
               
                if (lastPoint.x != 0.0f)
                {
                    SetColor(GetColor(white));
                    DrawLine(lastPoint,m_AllSidePoints[i][index]);
                }
                lastPoint = m_AllSidePoints[i][index];
                continue;
            }

            SetColor(GetColor(white));
            DrawLine(m_AllSidePoints[i][index],m_AllSidePoints[i][index+1]);
        }
    }
    

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
    CalculateArcs();

   
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
    change = true;
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
        m_Links[m_LinksSelected[i+1]]->ConstrainTo(m_Links[m_LinksSelected[i]]->GetCenter(),m_Links[m_LinksSelected[i]]->GetConstraintR()
        ,m_Links[m_LinksSelected[i]]->GetAngle());
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

    // It's probably not a good idea to clear the entire vector instead of updating each element, 
    // But I want to be done with this for fun, project already
    m_AllSidePoints.clear();
    for (int i{0}; i < m_Links.size(); ++i)
    {
        m_AllSidePoints.push_back(m_Links[i]->GetRightPoints());
    }

    for (int i{0}; i < m_Links.size(); ++i)
    {
        m_AllSidePoints.push_back(m_Links[i]->GetLeftPoints());
    }
   
 


    //// BROKEN
    //float bulgeAmount{10.0f};

    //// Idk why, Just want to finish this, left and right points are flipped
    //Vector2f p1_right{m_Links[1]->GetLeftPoints()[0].x,m_Links[0]->GetLeftPoints()[0].y};
    //Vector2f p2_right{m_Links[2]->GetLeftPoints()[0].x,m_Links[0]->GetLeftPoints()[0].y};
    //
    //Vector2f dir = p2_right - p1_right;
    //Vector2f mid = p1_right + 0.5f * dir;
    //Vector2f perp(-dir.y,dir.x);  // perpendicular
    //perp = perp.Normalized() * bulgeAmount;
    //Vector2f control = mid + perp;
    //
    //int steps = 10;
    //std::vector<Point2f> sidePoints;
    //for (int i{0}; i <= steps; ++i)
    //{
    //    float t = float(i) / steps;
    //    // Quadratic Bezier: B(t) = (1-t)^2*P0 + 2*(1-t)*t*P1 + t^2*P2
    //    Vector2f pt = (1 - t) * (1 - t) * p1_right      // start point
    //        + 2 * (1 - t) * t * control         // control point
    //        + t * t * p2_right;             // end point (use full x AND y!)
    //    if (m_AllSidePoints.size() <= steps)
    //    {
    //        m_AllSidePoints.push_back(pt);
    //    }
    //    else
    //    {
    //        m_AllSidePoints[i] = pt;
    //    }
    //}
}

// CHATGPT CODE, FIXED ONLY A BIT JUST SO IT WORKS
void ProceduralAnimation::drawFilledPolygon(const std::vector<Point2f>& poly) const
{

    // CHATGPT CODE, FIXED ONLY A BIT JUST SO IT WORKS
    if (poly.size() < 3) return;

    auto cross = [](const Point2f& a,const Point2f& b,const Point2f& c)
        {
            return (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
        };
    auto inside = [&](const Point2f& p,const Point2f& a,const Point2f& b,const Point2f& c)
        {
            float c1 = cross(a,b,p),c2 = cross(b,c,p),c3 = cross(c,a,p);
            return (c1 >= 0 && c2 >= 0 && c3 >= 0) || (c1 <= 0 && c2 <= 0 && c3 <= 0);
        };

    // index list
    std::vector<int> V(poly.size());
    for (int i = 0; i < (int)poly.size(); ++i) V[i] = i;

    glBegin(GL_TRIANGLES);
    while (V.size() > 2)
    {
        bool ear = false;
        for (size_t i = 0; i < V.size(); ++i)
        {
            int i0 = V[(i + V.size() - 1) % V.size()];
            int i1 = V[i];
            int i2 = V[(i + 1) % V.size()];

            if (cross(poly[i0],poly[i1],poly[i2]) <= 0) continue; // reflex

            bool ok = true;
            for (size_t j = 0; j < V.size(); ++j)
            {
                int vi = V[j];
                if (vi == i0 || vi == i1 || vi == i2) continue;
                if (inside(poly[vi],poly[i0],poly[i1],poly[i2])) { ok = false; break; }
            }
            if (!ok) continue;

            glVertex2f(poly[i0].x,poly[i0].y);
            glVertex2f(poly[i1].x,poly[i1].y);
            glVertex2f(poly[i2].x,poly[i2].y);

            V.erase(V.begin() + i); // clip ear
            ear = true;
            break;
        }
        if (!ear) break; // failed (probably bad input)
    }
    glEnd();
}
