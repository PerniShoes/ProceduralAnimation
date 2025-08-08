#include "ChainLink.h"
#include "PrettyColors.h"
#include "structs.h"

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
        SetColor(GetColor(dRed));
        DrawEllipse(*m_Center,*m_ConstraintR,*m_ConstraintR,lineWidth);
        if(m_Constrained==true){
            if (m_MousePos != nullptr)
                return;
            if (auto shared = m_TargetCenter.lock())
            {
                SetColor(GetColor(white));
                DrawLine(*m_Center,*shared);
            }
        }
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
            Point2f target{*shared - direction* *sharedRadius};
            *m_Center = target;
        }
    }
}
void ChainLink::UnConstrain()
{
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
Circlef ChainLink::GetBounds() const
{
    return Circlef{*m_Center,m_Radius};
}
bool ChainLink::IsConstrained() const
{
    return m_Constrained;
}