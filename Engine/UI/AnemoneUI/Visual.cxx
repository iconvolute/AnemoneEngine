#include "AnemoneUI/Visual.hxx"

namespace Anemone::UI
{
    Math::Transform2F const& Visual::Transform() const
    {
        return this->_transform;
    }

    void Visual::Transform(Math::Transform2F const& value)
    {
        this->_transform = value;
    }

    Math::TranslateTransform2F const& Visual::TranslateTransform() const
    {
        return this->_transform.Translate;
    }

    void Visual::TranslateTransform(Math::TranslateTransform2F const& value)
    {
        this->_transform.Translate = value;
    }

    Math::RotateTransform2F const& Visual::RotateTransform() const
    {
        return this->_transform.Rotate;
    }

    void Visual::RotateTransform(Math::RotateTransform2F const& value)
    {
        this->_transform.Rotate = value;
    }

    Math::ScaleTransform2F const& Visual::ScaleTransform() const
    {
        return this->_transform.Scale;
    }

    void Visual::ScaleTransform(Math::ScaleTransform2F const& value)
    {
        this->_transform.Scale = value;
    }

    Math::SkewTransform2F const& Visual::SkewTransform() const
    {
        return this->_transform.Skew;
    }

    void Visual::SkewTransform(Math::SkewTransform2F const& value)
    {
        this->_transform.Skew = value;
    }

    Math::Matrix3x2F const& Visual::TransformMatrix() const
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

    Math::SizeF Visual::Size() const
    {
        return this->_bounds.Size();
    }

    void Visual::Size(Math::SizeF value)
    {
        this->_bounds = Math::RectF::FromLocationSize(this->_bounds.Location(), value);
    }

    Math::PointF Visual::Location() const
    {
        return this->_bounds.Location();
    }

    void Visual::Location(Math::PointF value)
    {
        this->_bounds = Math::RectF::FromLocationSize(value, this->_bounds.Size());
    }

    Math::RectF Visual::Bounds() const
    {
        return this->_bounds;
    }

    void Visual::Bounds(Math::RectF value)
    {
        this->_bounds = value;
    }

    Math::ThicknessF Visual::Padding() const
    {
        return this->_padding;
    }

    void Visual::Padding(Math::ThicknessF value)
    {
        this->_padding = value;
    }

    Math::ThicknessF Visual::Margin() const
    {
        return this->_margin;
    }

    void Visual::Margin(Math::ThicknessF value)
    {
        this->_margin = value;
    }
}
