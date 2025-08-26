#include "ChainLink.h"
#include "PrettyColors.h"
#include "structs.h"
#include <cmath>

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
    m_DirectionAngle={};
}
ChainLink::~ChainLink()
{

}

void ChainLink::Draw(bool drawConstrain) const
{
    using namespace utils;
    using namespace PrettyColors;
    float lineWidth{2.0f};
    SetColor(m_Color);
    DrawEllipse(*m_Center,m_Radius,m_Radius,lineWidth);

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

}
void ChainLink::DrawSidePoints() const
{
    using namespace utils;
    using namespace PrettyColors;
    float radius{5.0f};

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
            Vector2f direction{*shared-*m_Center};
            direction = direction.Normalized();
            m_DirectionAngle = atan2(direction.y,direction.x);
            Point2f target{*shared - direction* *sharedRadius};
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
void ChainLink::ConstrainTo(std::weak_ptr<const Point2f> target,std::weak_ptr<const float> targetConstraintR)
{
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
        float rightX{m_Center->x + m_Radius * cos(m_DirectionAngle + quarterTurn-angleChange*i)};
        float rightY{m_Center->y + m_Radius * sin(m_DirectionAngle + quarterTurn-angleChange*i)};

        float leftX{m_Center->x + m_Radius * cos(m_DirectionAngle - quarterTurn+angleChange*i)};
        float leftY{m_Center->y + m_Radius * sin(m_DirectionAngle - quarterTurn+angleChange*i)};

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
        float rightX{m_Center->x + m_Radius * cos(m_DirectionAngle +quarterTurn - angleChange * i)};
        float rightY{m_Center->y + m_Radius * sin(m_DirectionAngle +quarterTurn - angleChange * i)};
        Point2f rightSide{rightX,rightY};

        m_RightSidePoints[i] = rightSide;
    }

    angleChange = quarterTurn / m_LeftSidePoints.size();
    for (int i{0}; i < m_LeftSidePoints.size(); i++)
    {
        float leftX{m_Center->x + m_Radius * cos(m_DirectionAngle -quarterTurn + angleChange * i)};
        float leftY{m_Center->y + m_Radius * sin(m_DirectionAngle -quarterTurn + angleChange * i)};

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
float ChainLink::GetAngle() const
{
    return m_DirectionAngle;
}
void ChainLink::SetAngle(float angle)
{
    m_DirectionAngle = angle;
}


