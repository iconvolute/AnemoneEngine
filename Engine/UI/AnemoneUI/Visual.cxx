#include "AnemoneUI/Visual.hxx"

namespace Anemone::UI
{
    Transform2F const& Visual::Transform() const
    {
        return this->_transform;
    }

    void Visual::Transform(Transform2F const& value)
    {
        this->_transform = value;
    }

    TranslateTransform2F const& Visual::TranslateTransform() const
    {
        return this->_transform.Translate;
    }

    void Visual::TranslateTransform(TranslateTransform2F const& value)
    {
        this->_transform.Translate = value;
    }

    RotateTransform2F const& Visual::RotateTransform() const
    {
        return this->_transform.Rotate;
    }

    void Visual::RotateTransform(RotateTransform2F const& value)
    {
        this->_transform.Rotate = value;
    }

    ScaleTransform2F const& Visual::ScaleTransform() const
    {
        return this->_transform.Scale;
    }

    void Visual::ScaleTransform(ScaleTransform2F const& value)
    {
        this->_transform.Scale = value;
    }

    SkewTransform2F const& Visual::SkewTransform() const
    {
        return this->_transform.Skew;
    }

    void Visual::SkewTransform(SkewTransform2F const& value)
    {
        this->_transform.Skew = value;
    }

    Matrix3x2F const& Visual::TransformMatrix() const
    {
        return this->_localTransform;
    }

    float Visual::Opacity() const
    {
        return this->_opacity;
    }

    void Visual::Opacity(float value)
    {
        this->_opacity = value;
    }

    SizeF Visual::Size() const
    {
        return this->_bounds.Size();
    }

    void Visual::Size(SizeF value)
    {
        this->_bounds = RectF::FromLocationSize(this->_bounds.Location(), value);
    }

    PointF Visual::Location() const
    {
        return this->_bounds.Location();
    }

    void Visual::Location(PointF value)
    {
        this->_bounds = RectF::FromLocationSize(value, this->_bounds.Size());
    }

    RectF Visual::Bounds() const
    {
        return this->_bounds;
    }

    void Visual::Bounds(RectF value)
    {
        this->_bounds = value;
    }

    ThicknessF Visual::Padding() const
    {
        return this->_padding;
    }

    void Visual::Padding(ThicknessF value)
    {
        this->_padding = value;
    }

    ThicknessF Visual::Margin() const
    {
        return this->_margin;
    }

    void Visual::Margin(ThicknessF value)
    {
        this->_margin = value;
    }
}
