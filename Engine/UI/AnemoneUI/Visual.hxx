#pragma once
#include "AnemoneInterop/Headers.hxx"
#include "AnemoneMath/Transform2.hxx"

#include "AnemoneUI/UIElement.hxx"

namespace Anemone::UI
{
    class UI_API Visual : public UIElement
    {
    private:
        Math::Transform2F _transform{};
        float _opacity{};
        Math::RectF _bounds{};
        Math::ThicknessF _padding{};
        Math::ThicknessF _margin{};
        Math::Matrix3x2F _localTransform{};

    public:
        Math::Transform2F const& Transform() const;
        void Transform(Math::Transform2F const& value);

        Math::TranslateTransform2F const& TranslateTransform() const;
        void TranslateTransform(Math::TranslateTransform2F const& value);

        Math::RotateTransform2F const& RotateTransform() const;
        void RotateTransform(Math::RotateTransform2F const& value);

        Math::ScaleTransform2F const& ScaleTransform() const;
        void ScaleTransform(Math::ScaleTransform2F const& value);

        Math::SkewTransform2F const& SkewTransform() const;
        void SkewTransform(Math::SkewTransform2F const& value);

        Math::Matrix3x2F const& TransformMatrix() const;

        float Opacity() const;
        void Opacity(float value);

        Math::SizeF Size() const;
        void Size(Math::SizeF value);

        Math::PointF Location() const;
        void Location(Math::PointF value);

        Math::RectF Bounds() const;
        void Bounds(Math::RectF value);

        Math::ThicknessF Padding() const;
        void Padding(Math::ThicknessF value);

        Math::ThicknessF Margin() const;
        void Margin(Math::ThicknessF value);
    };
}
