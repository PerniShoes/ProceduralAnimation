#include "ChainLink.h"
#include "PrettyColors.h"
#include "structs.h"
#include <cmath>
#define M_PI 3.14159265358979323846
#include <algorithm>

using namespace std;

ChainLink::ChainLink(Circlef bounds,float constraintRadius)
    :m_Radius{bounds.radius}
    ,m_Color{PrettyColors::GetColor(PrettyColors::white)}
    ,m_Constrained{false}
    ,m_TargetConstraintR{}
    ,m_MousePos{}
{
    m_Center = std::make_shared<Point2f>(bounds.center);
    m_ConstraintR = std::make_shared<float>(constraintRadius);
  /*  m_DirectionAngle = {};
    *m_ParentAngle = {};*/
}
ChainLink::~ChainLink()
{

}

void ChainLink::Draw(bool drawConstrain) const
{
    using namespace utils;
    using namespace PrettyColors;
    float lineWidth{2.0f};
    //SetColor(m_Color);
    //DrawEllipse(*m_Center,m_Radius,m_Radius,lineWidth);
    SetColor(GetColor(dGreen));
    FillEllipse(*m_Center,m_Radius,m_Radius);

    if (drawConstrain == true)
    {
        DrawSidePoints();
        SetColor(GetColor(dRed));
        DrawEllipse(*m_Center,*m_ConstraintR,*m_ConstraintR,lineWidth);
        if (m_Constrained == true)
        {
            if (m_MousePos != nullptr)
                return;
            if (auto shared = m_TargetCenter.lock())
            {
                SetColor(GetColor(white));
                DrawLine(*m_Center,*shared,1.0f);
            }
        }
    }
    if (isHead)
    {
        if (!m_LeftSidePoints.empty() && !m_RightSidePoints.empty())
        {
            Vector2f leftPoint{m_LeftSidePoints.back().x,  m_LeftSidePoints.back().y};
            Vector2f rightPoint{m_RightSidePoints.back().x, m_RightSidePoints.back().y};
            Vector2f center = (leftPoint + rightPoint) * 0.5f;
            Vector2f leftDir = (leftPoint - center).Normalized();
            Vector2f rightDir = (rightPoint - center).Normalized();
       
            // radius
            float r = (*m_ConstraintR)/1.5f;
            float padding = 8.0f;        
            float offset = padding + r;   
            Vector2f leftEyePos = leftPoint - leftDir * offset;
            Vector2f rightEyePos = rightPoint - rightDir * offset;

            SetColor(GetColor(white));
            FillEllipse(Point2f(rightEyePos),r,r);

            SetColor(GetColor(white));
            FillEllipse(Point2f(leftEyePos),r,r);
        }


    }

}
void ChainLink::DrawSidePoints() const
{
    using namespace utils;
    using namespace PrettyColors;
    float radius{5.0f};

    // Actually left = right 
    SetColor(GetColor(orange));
    for (int i{0}; i < m_LeftSidePoints.size(); i++)
    {
        FillEllipse(m_LeftSidePoints[i],radius,radius);
    }
    SetColor(GetColor(yellow));
    for (int i{0}; i < m_RightSidePoints.size(); i++)
    {
        FillEllipse(m_RightSidePoints[i],radius,radius);
    }

}

void ChainLink::Update()
{
    // Later take radius into account and vector and stuff
    if (m_Constrained == true && m_MousePos != nullptr)
    {
        *m_Center = *m_MousePos;
    }
    else if (m_Constrained == true)
    {

        if (auto shared = m_TargetCenter.lock();
            auto sharedRadius = m_TargetConstraintR.lock())
        {

            float fullTurn{6.28318f}; // Radians
            float maxDeviation{fullTurn / 48};
            float lerpSpeed{1.0f}; // Adjust 0.0 - 1.0 for smoothness

            // Compute direction to target
            Vector2f direction{*shared - *m_Center};
            direction = direction.Normalized();
            float desiredAngle = atan2(direction.y,direction.x);

            // Clamp relative to parent with angle wrapping
            float delta = desiredAngle - *m_ParentAngle;
            while (delta > M_PI)  delta -= 2 * M_PI;
            while (delta < -M_PI) delta += 2 * M_PI;

            delta = std::clamp(delta,-maxDeviation,maxDeviation);
            float targetAngle = *m_ParentAngle + delta;

            // Smoothly move current angle toward target using lerp
            m_DirectionAngle += (targetAngle - m_DirectionAngle) * lerpSpeed;
            while (m_DirectionAngle > M_PI)  m_DirectionAngle -= 2 * M_PI;
            while (m_DirectionAngle < -M_PI) m_DirectionAngle += 2 * M_PI;

            // Compute new position along constrained direction
            Vector2f constrainedDir{cos(m_DirectionAngle), sin(m_DirectionAngle)};
            Point2f target{*shared - constrainedDir * *sharedRadius};
            *m_Center = target;


        }

    }
    UpdateSidePoints();
}
void ChainLink::UnConstrain()
{
    m_TargetConstraintR.reset();
    m_TargetCenter.reset();
    m_MousePos = nullptr;
    m_Constrained = false;
}
void ChainLink::ConstrainTo(std::weak_ptr<const Point2f> target,std::weak_ptr<const float> targetConstraintR,const float& targetAngle)
{
    m_ParentAngle = &targetAngle;
    m_TargetCenter = target;
    m_TargetConstraintR = targetConstraintR;
    m_Constrained = true;
}
void ChainLink::ConstrainTo(const Point2f& targetPos)
{
    m_MousePos = &targetPos;
    m_Constrained = true;
}

void ChainLink::CalculateSidePoints(int amountOfPairs)
{
    int pairAmount = amountOfPairs;
    float fullTurn{6.28318f}; // Radians
    float halfTurn{fullTurn/2};
    float quarterTurn{fullTurn/4};
    float angleChange{quarterTurn / amountOfPairs};

    for (int i{0}; i < amountOfPairs; i++)
    {
        float rightX{m_Center->x + m_Radius * cos(m_DirectionAngle + quarterTurn-angleChange * (amountOfPairs-1-i))};
        float rightY{m_Center->y + m_Radius * sin(m_DirectionAngle + quarterTurn-angleChange * (amountOfPairs-1 - i))};

        float leftX{m_Center->x + m_Radius * cos(m_DirectionAngle - quarterTurn+angleChange * (amountOfPairs-1 - i))};
        float leftY{m_Center->y + m_Radius * sin(m_DirectionAngle - quarterTurn+angleChange * (amountOfPairs-1 - i))};

        m_LeftSidePoints.push_back(Point2f{leftX,leftY});
        m_RightSidePoints.push_back(Point2f{rightX,rightY});

    }
}
void ChainLink::UpdateSidePoints()
{
    float fullTurn{6.28318f}; // Radians
    float halfTurn{fullTurn / 2};
    float quarterTurn{fullTurn / 4};
    float angleChange{};

    angleChange = quarterTurn / m_RightSidePoints.size();
    for (int i{0}; i < m_RightSidePoints.size(); i++)
    {
        float rightX{m_Center->x + m_Radius * cos(m_DirectionAngle +quarterTurn - angleChange * (m_RightSidePoints.size() -1- i))};
        float rightY{m_Center->y + m_Radius * sin(m_DirectionAngle +quarterTurn - angleChange * (m_RightSidePoints.size() -1- i))};
        Point2f rightSide{rightX,rightY};

        m_RightSidePoints[i] = rightSide;
    }

    angleChange = quarterTurn / m_LeftSidePoints.size();
    for (int i{0}; i < m_LeftSidePoints.size(); i++)
    {
        float leftX{m_Center->x + m_Radius * cos(m_DirectionAngle -quarterTurn + angleChange * (m_LeftSidePoints.size()-1- i))};
        float leftY{m_Center->y + m_Radius * sin(m_DirectionAngle -quarterTurn + angleChange * (m_LeftSidePoints.size()-1- i))};

        Point2f leftSide{leftX,leftY};

        m_LeftSidePoints[i] = leftSide;
    }
}

void ChainLink::SetLinkColor(Color4f color)
{
    m_Color = color;
}

std::weak_ptr<const float> ChainLink::GetConstraintR() const
{
    return std::weak_ptr<const float>(m_ConstraintR);
}
std::weak_ptr<const Point2f> ChainLink::GetCenter() const
{
    return std::weak_ptr<const Point2f>(m_Center);
}
std::weak_ptr<const Point2f> ChainLink::GetTarget() const
{
    return m_TargetCenter;
}

Circlef ChainLink::GetBounds() const
{
    return Circlef{*m_Center,m_Radius};
}
bool ChainLink::IsConstrained() const
{
    return m_Constrained;
}
float& ChainLink::GetAngle() 
{
    return m_DirectionAngle;
}
void ChainLink::SetAngle(float angle)
{
    m_DirectionAngle = angle;
}

std::vector<Point2f>& ChainLink::GetLeftPoints()
{
    // Right and left are swapped
    return m_RightSidePoints;
}

std::vector<Point2f>& ChainLink::GetRightPoints()
{
    // Right and left are swapped
    return m_LeftSidePoints;
}

void ChainLink::SetParentAsHead()
{
    isParentHead = true;
}
void ChainLink::SetHead()
{
    isHead = true;
}

