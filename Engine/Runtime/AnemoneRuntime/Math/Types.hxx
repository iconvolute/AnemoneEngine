#pragma once
#include "AnemoneRuntime/Interop/Headers.hxx"
#include "AnemoneRuntime/Math/Aligned.hxx"
#include "AnemoneRuntime/Math/Packed.hxx"
#include "AnemoneRuntime/Math/Detail/SimdFloat.hxx"

//
//  API design note:
//
//  We tried to create some fancy template-based API for loading and storing from packed/aligned types, but it had no real benefits:
//
//    - it over-complicated the API
//    - it was no shorter nor more efficient
//
//  Compare:
//
//  ```c++
//  RotorF const r = RotorF::Load(some-address);
// // vs
//  RotorF const r = Load<RotorF>(some-address);
//  ```
//
//  There is no real benefit in the latter, and it is more complex to implement.
//

namespace Anemone::Math
{
    struct Matrix4x4F;
    struct Vector2F;
    struct Vector3F;
    struct Vector4F;
    struct QuaternionF;
    struct RotorF;


    struct Matrix4x4F
    {
        Detail::SimdMatrix4x4F Inner;

        [[nodiscard]] static Matrix4x4F anemone_vectorcall CreateTranslation(Vector3F v);
    };

    struct Mask2F
    {
        Detail::SimdMask4F Inner;
    };

    struct Mask3F
    {
        Detail::SimdMask4F Inner;
    };

    struct Mask4F
    {
        Detail::SimdMask4F Inner;

        [[nodiscard]] static Mask4F anemone_vectorcall Create(bool x, bool y, bool z, bool w);
        [[nodiscard]] static inline Mask4F anemone_vectorcall CreateReplicated(bool value);
        [[nodiscard]] static inline Mask4F anemone_vectorcall CreateTrue();
        [[nodiscard]] static inline Mask4F anemone_vectorcall CreateFalse();
    };

    struct Vector2F
    {
        Detail::SimdVector4F Inner;
    };

    struct Vector3F
    {
        Detail::SimdVector4F Inner;

        [[nodiscard]] static Vector3F anemone_vectorcall Create(float x, float y, float z);
    };

    struct Vector4F
    {
        Detail::SimdVector4F Inner;
    };

    struct PlaneF
    {
        Detail::SimdVector4F Inner;
    };

    struct RotorF
    {
        Detail::SimdVector4F Inner;

        [[nodiscard]] static RotorF anemone_vectorcall Create(float xy, float xz, float yz, float scalar);
        [[nodiscard]] static RotorF anemone_vectorcall CreateFromBivector(Vector4F bivector, float scalar);
        [[nodiscard]] static RotorF anemone_vectorcall Identity();
        [[nodiscard]] static RotorF anemone_vectorcall FromAxisAngle(Vector3F axis, float angle);
        [[nodiscard]] static RotorF anemone_vectorcall FromNormalAngle(Vector3F normal, float angle);
        [[nodiscard]] static RotorF anemone_vectorcall RotationBetween(Vector3F from, Vector3F to);

        [[nodiscard]] static RotorF anemone_vectorcall Load(Packed::RotorF const& value);
        [[nodiscard]] static RotorF anemone_vectorcall Load(Aligned::RotorF const& value);
        static void anemone_vectorcall Store(Packed::RotorF& destination, RotorF source);
        static void anemone_vectorcall Store(Aligned::RotorF& destination, RotorF source);
    };

    struct QuaternionF
    {
        Detail::SimdVector4F Inner;
    };

    inline Matrix4x4F anemone_vectorcall Matrix4x4F::CreateTranslation(Vector3F v)
    {
        return {Detail::Matrix4x4F_CreateTranslation(v.Inner)};
    }

    inline Vector3F anemone_vectorcall Transform(Matrix4x4F matrix, Vector3F v)
    {
        return {Detail::Vector4F_Transform3(v.Inner, matrix.Inner)};
    }

    inline Vector2F anemone_vectorcall Add(Vector2F left, Vector2F right)
    {
        return {Detail::Vector4F_Add(left.Inner, right.Inner)};
    }

    inline Vector3F anemone_vectorcall Add(Vector3F left, Vector3F right)
    {
        return {Detail::Vector4F_Add(left.Inner, right.Inner)};
    }

    inline Vector4F anemone_vectorcall Add(Vector4F left, Vector4F right)
    {
        return {Detail::Vector4F_Add(left.Inner, right.Inner)};
    }
}

// Mask4F
namespace Anemone::Math
{
    inline Mask4F anemone_vectorcall Mask4F::Create(bool x, bool y, bool z, bool w)
    {
        return {Detail::Mask4F_Create(x, y, z, w)};
    }

    inline Mask4F anemone_vectorcall Mask4F::CreateReplicated(bool value)
    {
        return {Detail::Mask4F_Replicate(value)};
    }

    inline Mask4F anemone_vectorcall Mask4F::CreateTrue()
    {
        return {Detail::Mask4F_True()};
    }

    inline Mask4F anemone_vectorcall Mask4F::CreateFalse()
    {
        return {Detail::Mask4F_False()};
    }

    template <size_t N>
    inline bool anemone_vectorcall Extract(Mask4F mask)
        requires(N < 4)
    {
        return Detail::Mask4F_Extract<N>(mask.Inner);
    }

    template <size_t N>
    inline Mask4F anemone_vectorcall Insert(Mask4F mask, bool value)
    {
        return {Detail::Mask4F_Insert<N>(mask.Inner, value)};
    }

    inline Mask4F anemone_vectorcall CompareEqual(Mask4F left, Mask4F right)
    {
        return {Detail::Mask4F_CompareEqual(left.Inner, right.Inner)};
    }

    inline Mask4F anemone_vectorcall CompareNotEqual(Mask4F left, Mask4F right)
    {
        return {Detail::Mask4F_CompareNotEqual(left.Inner, right.Inner)};
    }

    inline Mask4F anemone_vectorcall And(Mask4F left, Mask4F right)
    {
        return {Detail::Mask4F_And(left.Inner, right.Inner)};
    }

    inline Mask4F anemone_vectorcall AndNot(Mask4F left, Mask4F right)
    {
        return {Detail::Mask4F_AndNot(left.Inner, right.Inner)};
    }

    inline Mask4F anemone_vectorcall Or(Mask4F left, Mask4F right)
    {
        return {Detail::Mask4F_Or(left.Inner, right.Inner)};
    }

    inline Mask4F anemone_vectorcall Xor(Mask4F left, Mask4F right)
    {
        return {Detail::Mask4F_Xor(left.Inner, right.Inner)};
    }

    inline Mask4F anemone_vectorcall Not(Mask4F mask)
    {
        return {Detail::Mask4F_Not(mask.Inner)};
    }

    inline Mask4F anemone_vectorcall Select(Mask4F mask, Mask4F whenFalse, Mask4F whenTrue)
    {
        return {Detail::Mask4F_Select(mask.Inner, whenFalse.Inner, whenTrue.Inner)};
    }

    template <bool X, bool Y, bool Z, bool W>
    inline Mask4F anemone_vectorcall Select(Mask4F whenFalse, Mask4F whenTrue)
    {
        return {Detail::Mask4F_Select<X, Y, Z, W>(whenFalse.Inner, whenTrue.Inner)};
    }

    inline bool anemone_vectorcall All(Mask4F mask)
    {
        return Detail::Mask4F_All4(mask.Inner);
    }

    inline bool anemone_vectorcall Any(Mask4F mask)
    {
        return Detail::Mask4F_Any4(mask.Inner);
    }

    inline bool anemone_vectorcall None(Mask4F mask)
    {
        return Detail::Mask4F_None4(mask.Inner);
    }
}

// Vector3F
namespace Anemone::Math
{
    inline Vector3F anemone_vectorcall Vector3F::Create(float x, float y, float z)
    {
        return {Detail::Vector4F_Create(x, y, z, 0.0f)};
    }
}

// RotorF
namespace Anemone::Math
{
    inline RotorF anemone_vectorcall RotorF::Create(float xy, float xz, float yz, float scalar)
    {
        return {Detail::RotorF_Create(xy, xz, yz, scalar)};
    }

    inline RotorF anemone_vectorcall RotorF::CreateFromBivector(Vector4F bivector, float scalar)
    {
        return {Detail::RotorF_Create(bivector.Inner, scalar)};
    }

    inline RotorF anemone_vectorcall RotorF::Identity()
    {
        return {Detail::RotorF_Identity()};
    }

    inline RotorF anemone_vectorcall RotorF::FromAxisAngle(Vector3F axis, float angle)
    {
        return {Detail::RotorF_CreateFromAxisAngle(axis.Inner, angle)};
    }

    inline RotorF anemone_vectorcall RotorF::FromNormalAngle(Vector3F normal, float angle)
    {
        return {Detail::RotorF_CreateFromNormalAngle(normal.Inner, angle)};
    }

    inline RotorF anemone_vectorcall RotorF::RotationBetween(Vector3F from, Vector3F to)
    {
        return {Detail::RotorF_RotationBetween(from.Inner, to.Inner)};
    }

    inline RotorF anemone_vectorcall RotorF::Load(Packed::RotorF const& value)
    {
        return {Detail::Vector4F_LoadUnalignedFloat4(reinterpret_cast<float const*>(&value))};
    }

    inline RotorF anemone_vectorcall RotorF::Load(Aligned::RotorF const& value)
    {
        return {Detail::Vector4F_LoadAlignedFloat4(reinterpret_cast<float const*>(&value))};
    }

    inline void anemone_vectorcall RotorF::Store(Packed::RotorF& destination, RotorF source)
    {
        Detail::Vector4F_StoreUnalignedFloat4(reinterpret_cast<float*>(&destination), source.Inner);
    }

    inline void anemone_vectorcall RotorF::Store(Aligned::RotorF& destination, RotorF source)
    {
        Detail::Vector4F_StoreAlignedFloat4(reinterpret_cast<float*>(&destination), source.Inner);
    }

    inline Vector3F anemone_vectorcall Rotate(RotorF rotor, Vector3F v)
    {
        return {Detail::RotorF_Rotate3(rotor.Inner, v.Inner)};
    }

    inline Vector3F anemone_vectorcall InverseRotate(RotorF rotor, Vector3F v)
    {
        return {Detail::RotorF_InverseRotate3(rotor.Inner, v.Inner)};
    }

    inline RotorF anemone_vectorcall Dot(RotorF a, RotorF b)
    {
        return {Detail::RotorF_Dot(a.Inner, b.Inner)};
    }

    inline RotorF anemone_vectorcall Multiply(RotorF a, RotorF b)
    {
        return {Detail::RotorF_Multiply(a.Inner, b.Inner)};
    }

    inline RotorF anemone_vectorcall Rotate(RotorF rotor, RotorF rotation)
    {
        return {Detail::RotorF_RotateRotor(rotor.Inner, rotation.Inner)};
    }

    inline RotorF anemone_vectorcall Reverse(RotorF rotor)
    {
        return {Detail::RotorF_Reverse(rotor.Inner)};
    }

    inline Vector4F anemone_vectorcall Length(RotorF rotor)
    {
        return {Detail::RotorF_Length(rotor.Inner)};
    }

    inline Vector4F anemone_vectorcall LengthSquared(RotorF rotor)
    {
        return {Detail::RotorF_LengthSquared(rotor.Inner)};
    }

    inline RotorF anemone_vectorcall Normalize(RotorF rotor)
    {
        return {Detail::RotorF_Normalize(rotor.Inner)};
    }

    inline Matrix4x4F anemone_vectorcall ToMatrix4x4F(RotorF rotor)
    {
        return {Detail::RotorF_ToMatrix4x4F(rotor.Inner)};
    }

    inline RotorF anemone_vectorcall Nlerp(RotorF from, RotorF to, Vector4F t)
    {
        return {Detail::RotorF_Nlerp(from.Inner, to.Inner, t.Inner)};
    }

    inline RotorF anemone_vectorcall Nlerp(RotorF from, RotorF to, float t)
    {
        return {Detail::RotorF_Nlerp(from.Inner, to.Inner, t)};
    }

    inline RotorF anemone_vectorcall Slerp(RotorF from, RotorF to, Vector4F t)
    {
        return {Detail::RotorF_Slerp(from.Inner, to.Inner, t.Inner)};
    }

    inline RotorF anemone_vectorcall Slerp(RotorF from, RotorF to, float t)
    {
        return {Detail::RotorF_Slerp(from.Inner, to.Inner, t)};
    }

    inline Mask4F anemone_vectorcall CompareEquals(RotorF left, RotorF right)
    {
        return {Detail::RotorF_CompareEqual(left.Inner, right.Inner)};
    }

    inline Mask4F anemone_vectorcall CompareNotEquals(RotorF left, RotorF right)
    {
        return {Detail::RotorF_CompareNotEqual(left.Inner, right.Inner)};
    }
}
