#pragma once
#include <AnemoneUI/MatrixF.hxx>

namespace Anemone::UI
{
    struct ScaleTransformF
    {
        float X;
        float Y;
    };

    struct ShearTransformF
    {
        float AngleX;
        float AngleY;
    };

    struct TranslateTransformF
    {
        float X;
        float Y;
    };

    struct RotateTransformF
    {
        float Angle;
    };

    struct TransformF
    {
        TranslateTransformF Translate;
        RotateTransformF Rotate;
        ScaleTransformF Scale;
        ShearTransformF Shear;

        MatrixF ToMatrixF() const;
        MatrixF ToMatrixF(Point2F const& origin) const;
    };
}

namespace Anemone::UI
{
    inline MatrixF TransformF::ToMatrixF() const
    {
        MatrixF result = MatrixF::Identity();
        result.Shear(this->Shear.AngleX, this->Shear.AngleY);
        result.Scale(this->Scale.X, this->Scale.Y);
        result.Rotate(this->Rotate.Angle);
        result.Translate(this->Translate.X, this->Translate.Y);
        return result;
    }

    inline MatrixF TransformF::ToMatrixF(Point2F const& origin) const
    {
        MatrixF result = MatrixF::Identity();
        result.Translate(-origin.X, -origin.Y);
        result.Shear(this->Shear.AngleX, this->Shear.AngleY);
        result.Scale(this->Scale.X, this->Scale.Y);
        result.Rotate(this->Rotate.Angle);
        result.Translate(this->Translate.X, this->Translate.Y);
        result.Translate(origin.X, origin.Y);
        return result;
    }
}
