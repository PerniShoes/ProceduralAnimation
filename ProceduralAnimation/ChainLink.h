#pragma once
#include "utils.h"
#include <memory>

class ChainLink
{
public:
    
    ChainLink(Circlef bounds, float constraintRadius);
    ChainLink(const ChainLink& gol) = delete;
    ChainLink& operator=(const ChainLink& gol) = delete;
    ChainLink(ChainLink&& gol) = delete;
    ChainLink& operator=(ChainLink&& gol) = delete;

    ~ChainLink();

    void Draw(bool drawConstrain = true) const;
    void DrawSidePoints() const;
    void Update();

    void UnConstrain();
    void ConstrainTo(const Point2f& targetPos);
    void ConstrainTo(std::weak_ptr<const Point2f> target,std::weak_ptr<const float> targetConstraintR,const float& targetAngle);
    void SetLinkColor(Color4f color);
    
    bool IsConstrained() const;
    Circlef GetBounds() const;
    std::weak_ptr<const float> GetConstraintR() const;
    std::weak_ptr<const Point2f> GetCenter() const;
    std::weak_ptr<const Point2f> GetTarget() const;
    float& GetAngle();
    void SetAngle(float angle); // In case of being head
    void CalculateSidePoints(int amountOfPairs);
    void SetParentAsHead();
    void UpdateSidePoints();
    std::vector<Point2f>& GetRightPoints();
    std::vector<Point2f>& GetLeftPoints();
    void SetHead();

private:

    // Basic
    std::shared_ptr<Point2f> m_Center;
    std::shared_ptr<float> m_ConstraintR;
    float m_Radius;
    Color4f m_Color;
    float m_DirectionAngle;
    const float* m_ParentAngle;
    std::vector<Point2f> m_LeftSidePoints; 
    std::vector<Point2f> m_RightSidePoints;
    bool isParentHead{false};
    bool isHead{false};

    // Constrained:
    bool m_Constrained;
    std::weak_ptr<const Point2f> m_TargetCenter;
    std::weak_ptr<const float> m_TargetConstraintR;

    const Point2f* m_MousePos; // For constraining to mouse
 
};

