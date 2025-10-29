#include "AnemoneMath/Transform2.hxx"
#include "AnemoneMath/Matrix3x2.hxx"

namespace Anemone::Math
{
    inline Matrix3x2F ToMatrix3x2(Transform2F const& transform)
    {
        Matrix3x2F result = Matrix3x2F::CreateSkew(transform.Skew.RadiansX, transform.Skew.RadiansY);
        result = Multiply(result, Matrix3x2F::CreateScale(transform.Scale.X, transform.Scale.Y));
        result = Multiply(result, Matrix3x2F::CreateRotation(transform.Rotate.Radians));
        result = Multiply(result, Matrix3x2F::CreateTranslation(transform.Translate.X, transform.Translate.Y));
        return result;
    }

    inline Matrix3x2F ToMatrix3x2(Transform2F const& transform, PointF origin)
    {
        Matrix3x2F result = Matrix3x2F::CreateTranslation(-origin.X, -origin.Y);
        result = Multiply(result, Matrix3x2F::CreateSkew(transform.Skew.RadiansX, transform.Skew.RadiansY));
        result = Multiply(result, Matrix3x2F::CreateScale(transform.Scale.X, transform.Scale.Y));
        result = Multiply(result, Matrix3x2F::CreateRotation(transform.Rotate.Radians));
        result = Multiply(result, Matrix3x2F::CreateTranslation(transform.Translate.X, transform.Translate.Y));
        result = Multiply(result, Matrix3x2F::CreateTranslation(origin.X, origin.Y));
        return result;
    }
}
