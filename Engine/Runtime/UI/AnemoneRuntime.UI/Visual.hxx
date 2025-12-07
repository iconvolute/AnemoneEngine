#pragma once
#include "AnemoneRuntime.Interop/Headers.hxx"
#include "AnemoneRuntime.Math/Transform2.hxx"
#include "AnemoneRuntime.UI/UIElement.hxx"

namespace Anemone::UI
{
    class ANEMONE_RUNTIME_UI_API Visual : public UIElement
    {
    private:
        Transform2F _transform{};
        float _opacity{};
        RectF _bounds{};
        ThicknessF _padding{};
        ThicknessF _margin{};
        Matrix3x2F _localTransform{};

    public:
        Transform2F const& Transform() const;
        void Transform(Transform2F const& value);

        TranslateTransform2F const& TranslateTransform() const;
        void TranslateTransform(TranslateTransform2F const& value);

        RotateTransform2F const& RotateTransform() const;
        void RotateTransform(RotateTransform2F const& value);

        ScaleTransform2F const& ScaleTransform() const;
        void ScaleTransform(ScaleTransform2F const& value);

        SkewTransform2F const& SkewTransform() const;
        void SkewTransform(SkewTransform2F const& value);

        Matrix3x2F const& TransformMatrix() const;

        float Opacity() const;
        void Opacity(float value);

        SizeF Size() const;
        void Size(SizeF value);

        PointF Location() const;
        void Location(PointF value);

        RectF Bounds() const;
        void Bounds(RectF value);

        ThicknessF Padding() const;
        void Padding(ThicknessF value);

        ThicknessF Margin() const;
        void Margin(ThicknessF value);
    };
}
