// ReSharper disable CppClangTidyClangDiagnosticFloatEqual
// ReSharper disable CppPassValueParameterByConstReference
#pragma once
#include "AnemoneRuntime/Numerics/Simd.hxx"

// Implementation details
namespace Anemone::Numerics::Private
{
    template <size_t Lane0, size_t Lane1>
    float32x2_t anemone_vectorcall NeonShuffle(float32x2_t v)
        requires((Lane0 < 2) and (Lane1 < 2));

    template <>
    inline float32x2_t anemone_vectorcall NeonShuffle<0, 1>(float32x2_t v)
    {
        return v;
    }

    template <>
    inline float32x2_t anemone_vectorcall NeonShuffle<1, 0>(float32x2_t v)
    {
        return vrev64_f32(v);
    }

    template <>
    inline float32x2_t anemone_vectorcall NeonShuffle<0, 0>(float32x2_t v)
    {
        return vdup_lane_f32(v, 0);
    }

    template <>
    inline float32x2_t anemone_vectorcall NeonShuffle<1, 1>(float32x2_t v)
    {
        return vdup_lane_f32(v, 1);
    }

    template <size_t X, size_t Y>
    inline float32x2_t anemone_vectorcall NeonShuffle(float32x2_t xy, float32x2_t zw)
        requires((X < 4) and (Y < 4));

    template <>
    inline float32x2_t anemone_vectorcall NeonShuffle<0, 0>(float32x2_t xy, float32x2_t zw)
    {
        (void)zw;
        return vdup_lane_f32(xy, 0);
    }

    template <>
    inline float32x2_t anemone_vectorcall NeonShuffle<0, 1>(float32x2_t xy, float32x2_t zw)
    {
        (void)zw;
        return xy;
    }

    template <>
    inline float32x2_t anemone_vectorcall NeonShuffle<0, 2>(float32x2_t xy, float32x2_t zw)
    {
        return vtrn1_f32(xy, zw);
    }

    template <>
    inline float32x2_t anemone_vectorcall NeonShuffle<0, 3>(float32x2_t xy, float32x2_t zw)
    {
        return vcopy_lane_f32(xy, 1, zw, 1);
    }

    template <>
    inline float32x2_t anemone_vectorcall NeonShuffle<1, 0>(float32x2_t xy, float32x2_t zw)
    {
        (void)zw;
        return vrev64_f32(xy);
    }

    template <>
    inline float32x2_t anemone_vectorcall NeonShuffle<1, 1>(float32x2_t xy, float32x2_t zw)
    {
        (void)zw;
        return vdup_lane_f32(xy, 1);
    }

    template <>
    inline float32x2_t anemone_vectorcall NeonShuffle<1, 2>(float32x2_t xy, float32x2_t zw)
    {
        return vext_f32(xy, zw, 1);
    }

    template <>
    inline float32x2_t anemone_vectorcall NeonShuffle<1, 3>(float32x2_t xy, float32x2_t zw)
    {
        return vtrn2_f32(xy, zw);
    }

    template <>
    inline float32x2_t anemone_vectorcall NeonShuffle<2, 0>(float32x2_t xy, float32x2_t zw)
    {
        return vtrn1_f32(zw, xy);
    }

    template <>
    inline float32x2_t anemone_vectorcall NeonShuffle<2, 1>(float32x2_t xy, float32x2_t zw)
    {
        return vcopy_lane_f32(zw, 1, xy, 1);
    }

    template <>
    inline float32x2_t anemone_vectorcall NeonShuffle<2, 2>(float32x2_t xy, float32x2_t zw)
    {
        (void)xy;
        return vdup_lane_f32(zw, 0);
    }
    template <>
    inline float32x2_t anemone_vectorcall NeonShuffle<2, 3>(float32x2_t xy, float32x2_t zw)
    {
        (void)xy;
        return zw;
    }

    template <>
    inline float32x2_t anemone_vectorcall NeonShuffle<3, 0>(float32x2_t xy, float32x2_t zw)
    {
        return vext_f32(zw, xy, 1);
    }

    template <>
    inline float32x2_t anemone_vectorcall NeonShuffle<3, 1>(float32x2_t xy, float32x2_t zw)
    {
        return vtrn2_f32(zw, xy);
    }

    template <>
    inline float32x2_t anemone_vectorcall NeonShuffle<3, 2>(float32x2_t xy, float32x2_t zw)
    {
        (void)xy;
        return vrev64_f32(zw);
    }

    template <>
    inline float32x2_t anemone_vectorcall NeonShuffle<3, 3>(float32x2_t xy, float32x2_t zw)
    {
        (void)xy;
        return vdup_lane_f32(zw, 1);
    }

    template <size_t Lane0, size_t Lane1, size_t Lane2, size_t Lane3>
    float32x4_t NeonShuffle(float32x4_t v)
        requires((Lane0 < 4) and (Lane1 < 4) and (Lane2 < 4) and (Lane3 < 4))
    {
        float32x4_t const r0 = vmovq_n_f32(vgetq_lane_f32(v, Lane0));
        float32x4_t const r1 = vsetq_lane_f32(vgetq_lane_f32(v, Lane1), r0, 1);
        float32x4_t const r2 = vsetq_lane_f32(vgetq_lane_f32(v, Lane2), r1, 2);
        float32x4_t const result = vsetq_lane_f32(vgetq_lane_f32(v, Lane3), r2, 3);
        return result;
    }

    template <>
    inline float32x4_t NeonShuffle<0, 1, 2, 3>(float32x4_t v) { return v; }

    template <>
    inline float32x4_t NeonShuffle<0, 0, 0, 0>(float32x4_t v) { return vdupq_laneq_f32(v, 0); }

    template <>
    inline float32x4_t NeonShuffle<1, 1, 1, 1>(float32x4_t v) { return vdupq_laneq_f32(v, 1); }

    template <>
    inline float32x4_t NeonShuffle<2, 2, 2, 2>(float32x4_t v) { return vdupq_laneq_f32(v, 2); }

    template <>
    inline float32x4_t NeonShuffle<3, 3, 3, 3>(float32x4_t v) { return vdupq_laneq_f32(v, 3); }

    template <>
    inline float32x4_t NeonShuffle<0, 1, 0, 1>(float32x4_t v)
    {
        float32x2_t const vxy = vget_low_f32(v);
        return vcombine_f32(vxy, vxy);
    }

    template <>
    inline float32x4_t NeonShuffle<2, 3, 2, 3>(float32x4_t v)
    {
        float32x2_t const vzw = vget_high_f32(v);
        return vcombine_f32(vzw, vzw);
    }

    template <>
    inline float32x4_t NeonShuffle<1, 0, 3, 2>(float32x4_t v) { return vrev64q_f32(v); }

    template <>
    inline float32x4_t NeonShuffle<0, 2, 0, 2>(float32x4_t v) { return vuzp1q_f32(v, v); }

    template <>
    inline float32x4_t NeonShuffle<1, 3, 1, 3>(float32x4_t v) { return vuzp2q_f32(v, v); }


    template <>
    inline float32x4_t NeonShuffle<1, 0, 1, 0>(float32x4_t v)
    {
        float32x2_t vt = vrev64_f32(vget_low_f32(v));
        return vcombine_f32(vt, vt);
    }

    template <>
    inline float32x4_t NeonShuffle<3, 2, 3, 2>(float32x4_t v)
    {
        float32x2_t vt = vrev64_f32(vget_high_f32(v));
        return vcombine_f32(vt, vt);
    }

    template <>
    inline float32x4_t NeonShuffle<0, 1, 3, 2>(float32x4_t v) { return vcombine_f32(vget_low_f32(v), vrev64_f32(vget_high_f32(v))); }
    template <>
    inline float32x4_t NeonShuffle<1, 0, 2, 3>(float32x4_t v) { return vcombine_f32(vrev64_f32(vget_low_f32(v)), vget_high_f32(v)); }
    template <>
    inline float32x4_t NeonShuffle<2, 3, 1, 0>(float32x4_t v) { return vcombine_f32(vget_high_f32(v), vrev64_f32(vget_low_f32(v))); }
    template <>
    inline float32x4_t NeonShuffle<3, 2, 0, 1>(float32x4_t v) { return vcombine_f32(vrev64_f32(vget_high_f32(v)), vget_low_f32(v)); }
    template <>
    inline float32x4_t NeonShuffle<3, 2, 1, 0>(float32x4_t v) { return vcombine_f32(vrev64_f32(vget_high_f32(v)), vrev64_f32(vget_low_f32(v))); }

    template <>
    inline float32x4_t NeonShuffle<0, 0, 2, 2>(float32x4_t v) { return vtrn1q_f32(v, v); }
    template <>
    inline float32x4_t NeonShuffle<1, 1, 3, 3>(float32x4_t v) { return vtrn2q_f32(v, v); }

    template <>
    inline float32x4_t NeonShuffle<0, 0, 1, 1>(float32x4_t v) { return vzip1q_f32(v, v); }
    template <>
    inline float32x4_t NeonShuffle<2, 2, 3, 3>(float32x4_t v) { return vzip2q_f32(v, v); }

    template <>
    inline float32x4_t NeonShuffle<1, 2, 3, 0>(float32x4_t v) { return vextq_f32(v, v, 1); }
    template <>
    inline float32x4_t NeonShuffle<2, 3, 0, 1>(float32x4_t v) { return vextq_f32(v, v, 2); }
    template <>
    inline float32x4_t NeonShuffle<3, 0, 1, 2>(float32x4_t v) { return vextq_f32(v, v, 3); }

    template <>
    inline float32x4_t NeonShuffle<1, 0, 0, 0>(float32x4_t v) { return vsetq_lane_f32(vgetq_lane_f32(v, 1), vdupq_n_f32(vgetq_lane_f32(v, 0)), 0); }
    template <>
    inline float32x4_t NeonShuffle<2, 0, 0, 0>(float32x4_t v) { return vsetq_lane_f32(vgetq_lane_f32(v, 2), vdupq_n_f32(vgetq_lane_f32(v, 0)), 0); }
    template <>
    inline float32x4_t NeonShuffle<3, 0, 0, 0>(float32x4_t v) { return vsetq_lane_f32(vgetq_lane_f32(v, 3), vdupq_n_f32(vgetq_lane_f32(v, 0)), 0); }

    template <>
    inline float32x4_t NeonShuffle<0, 1, 1, 1>(float32x4_t v) { return vsetq_lane_f32(vgetq_lane_f32(v, 0), vdupq_n_f32(vgetq_lane_f32(v, 1)), 0); }
    template <>
    inline float32x4_t NeonShuffle<2, 1, 1, 1>(float32x4_t v) { return vsetq_lane_f32(vgetq_lane_f32(v, 2), vdupq_n_f32(vgetq_lane_f32(v, 1)), 0); }
    template <>
    inline float32x4_t NeonShuffle<3, 1, 1, 1>(float32x4_t v) { return vsetq_lane_f32(vgetq_lane_f32(v, 3), vdupq_n_f32(vgetq_lane_f32(v, 1)), 0); }

    template <>
    inline float32x4_t NeonShuffle<0, 2, 2, 2>(float32x4_t v) { return vsetq_lane_f32(vgetq_lane_f32(v, 0), vdupq_n_f32(vgetq_lane_f32(v, 2)), 0); }
    template <>
    inline float32x4_t NeonShuffle<1, 2, 2, 2>(float32x4_t v) { return vsetq_lane_f32(vgetq_lane_f32(v, 1), vdupq_n_f32(vgetq_lane_f32(v, 2)), 0); }
    template <>
    inline float32x4_t NeonShuffle<3, 2, 2, 2>(float32x4_t v) { return vsetq_lane_f32(vgetq_lane_f32(v, 3), vdupq_n_f32(vgetq_lane_f32(v, 2)), 0); }

    template <>
    inline float32x4_t NeonShuffle<0, 3, 3, 3>(float32x4_t v) { return vsetq_lane_f32(vgetq_lane_f32(v, 0), vdupq_n_f32(vgetq_lane_f32(v, 3)), 0); }
    template <>
    inline float32x4_t NeonShuffle<1, 3, 3, 3>(float32x4_t v) { return vsetq_lane_f32(vgetq_lane_f32(v, 1), vdupq_n_f32(vgetq_lane_f32(v, 3)), 0); }
    template <>
    inline float32x4_t NeonShuffle<2, 3, 3, 3>(float32x4_t v) { return vsetq_lane_f32(vgetq_lane_f32(v, 2), vdupq_n_f32(vgetq_lane_f32(v, 3)), 0); }

    template <>
    inline float32x4_t NeonShuffle<0, 1, 0, 0>(float32x4_t v) { return vsetq_lane_f32(vgetq_lane_f32(v, 1), vdupq_n_f32(vgetq_lane_f32(v, 0)), 1); }
    template <>
    inline float32x4_t NeonShuffle<0, 2, 0, 0>(float32x4_t v) { return vsetq_lane_f32(vgetq_lane_f32(v, 2), vdupq_n_f32(vgetq_lane_f32(v, 0)), 1); }
    template <>
    inline float32x4_t NeonShuffle<0, 3, 0, 0>(float32x4_t v) { return vsetq_lane_f32(vgetq_lane_f32(v, 3), vdupq_n_f32(vgetq_lane_f32(v, 0)), 1); }

    template <>
    inline float32x4_t NeonShuffle<1, 0, 1, 1>(float32x4_t v) { return vsetq_lane_f32(vgetq_lane_f32(v, 0), vdupq_n_f32(vgetq_lane_f32(v, 1)), 1); }
    template <>
    inline float32x4_t NeonShuffle<1, 2, 1, 1>(float32x4_t v) { return vsetq_lane_f32(vgetq_lane_f32(v, 2), vdupq_n_f32(vgetq_lane_f32(v, 1)), 1); }
    template <>
    inline float32x4_t NeonShuffle<1, 3, 1, 1>(float32x4_t v) { return vsetq_lane_f32(vgetq_lane_f32(v, 3), vdupq_n_f32(vgetq_lane_f32(v, 1)), 1); }

    template <>
    inline float32x4_t NeonShuffle<2, 0, 2, 2>(float32x4_t v) { return vsetq_lane_f32(vgetq_lane_f32(v, 0), vdupq_n_f32(vgetq_lane_f32(v, 2)), 1); }
    template <>
    inline float32x4_t NeonShuffle<2, 1, 2, 2>(float32x4_t v) { return vsetq_lane_f32(vgetq_lane_f32(v, 1), vdupq_n_f32(vgetq_lane_f32(v, 2)), 1); }
    template <>
    inline float32x4_t NeonShuffle<2, 3, 2, 2>(float32x4_t v) { return vsetq_lane_f32(vgetq_lane_f32(v, 3), vdupq_n_f32(vgetq_lane_f32(v, 2)), 1); }

    template <>
    inline float32x4_t NeonShuffle<3, 0, 3, 3>(float32x4_t v) { return vsetq_lane_f32(vgetq_lane_f32(v, 0), vdupq_n_f32(vgetq_lane_f32(v, 3)), 1); }
    template <>
    inline float32x4_t NeonShuffle<3, 1, 3, 3>(float32x4_t v) { return vsetq_lane_f32(vgetq_lane_f32(v, 1), vdupq_n_f32(vgetq_lane_f32(v, 3)), 1); }
    template <>
    inline float32x4_t NeonShuffle<3, 2, 3, 3>(float32x4_t v) { return vsetq_lane_f32(vgetq_lane_f32(v, 2), vdupq_n_f32(vgetq_lane_f32(v, 3)), 1); }

    template <>
    inline float32x4_t NeonShuffle<0, 0, 1, 0>(float32x4_t v) { return vsetq_lane_f32(vgetq_lane_f32(v, 1), vdupq_n_f32(vgetq_lane_f32(v, 0)), 2); }
    template <>
    inline float32x4_t NeonShuffle<0, 0, 2, 0>(float32x4_t v) { return vsetq_lane_f32(vgetq_lane_f32(v, 2), vdupq_n_f32(vgetq_lane_f32(v, 0)), 2); }
    template <>
    inline float32x4_t NeonShuffle<0, 0, 3, 0>(float32x4_t v) { return vsetq_lane_f32(vgetq_lane_f32(v, 3), vdupq_n_f32(vgetq_lane_f32(v, 0)), 2); }

    template <>
    inline float32x4_t NeonShuffle<1, 1, 0, 1>(float32x4_t v) { return vsetq_lane_f32(vgetq_lane_f32(v, 0), vdupq_n_f32(vgetq_lane_f32(v, 1)), 2); }
    template <>
    inline float32x4_t NeonShuffle<1, 1, 2, 1>(float32x4_t v) { return vsetq_lane_f32(vgetq_lane_f32(v, 2), vdupq_n_f32(vgetq_lane_f32(v, 1)), 2); }
    template <>
    inline float32x4_t NeonShuffle<1, 1, 3, 1>(float32x4_t v) { return vsetq_lane_f32(vgetq_lane_f32(v, 3), vdupq_n_f32(vgetq_lane_f32(v, 1)), 2); }

    template <>
    inline float32x4_t NeonShuffle<2, 2, 0, 2>(float32x4_t v) { return vsetq_lane_f32(vgetq_lane_f32(v, 0), vdupq_n_f32(vgetq_lane_f32(v, 2)), 2); }
    template <>
    inline float32x4_t NeonShuffle<2, 2, 1, 2>(float32x4_t v) { return vsetq_lane_f32(vgetq_lane_f32(v, 1), vdupq_n_f32(vgetq_lane_f32(v, 2)), 2); }
    template <>
    inline float32x4_t NeonShuffle<2, 2, 3, 2>(float32x4_t v) { return vsetq_lane_f32(vgetq_lane_f32(v, 3), vdupq_n_f32(vgetq_lane_f32(v, 2)), 2); }

    template <>
    inline float32x4_t NeonShuffle<3, 3, 0, 3>(float32x4_t v) { return vsetq_lane_f32(vgetq_lane_f32(v, 0), vdupq_n_f32(vgetq_lane_f32(v, 3)), 2); }
    template <>
    inline float32x4_t NeonShuffle<3, 3, 1, 3>(float32x4_t v) { return vsetq_lane_f32(vgetq_lane_f32(v, 1), vdupq_n_f32(vgetq_lane_f32(v, 3)), 2); }
    template <>
    inline float32x4_t NeonShuffle<3, 3, 2, 3>(float32x4_t v) { return vsetq_lane_f32(vgetq_lane_f32(v, 2), vdupq_n_f32(vgetq_lane_f32(v, 3)), 2); }

    template <>
    inline float32x4_t NeonShuffle<0, 0, 0, 1>(float32x4_t v) { return vsetq_lane_f32(vgetq_lane_f32(v, 1), vdupq_n_f32(vgetq_lane_f32(v, 0)), 3); }
    template <>
    inline float32x4_t NeonShuffle<0, 0, 0, 2>(float32x4_t v) { return vsetq_lane_f32(vgetq_lane_f32(v, 2), vdupq_n_f32(vgetq_lane_f32(v, 0)), 3); }
    template <>
    inline float32x4_t NeonShuffle<0, 0, 0, 3>(float32x4_t v) { return vsetq_lane_f32(vgetq_lane_f32(v, 3), vdupq_n_f32(vgetq_lane_f32(v, 0)), 3); }

    template <>
    inline float32x4_t NeonShuffle<1, 1, 1, 0>(float32x4_t v) { return vsetq_lane_f32(vgetq_lane_f32(v, 0), vdupq_n_f32(vgetq_lane_f32(v, 1)), 3); }
    template <>
    inline float32x4_t NeonShuffle<1, 1, 1, 2>(float32x4_t v) { return vsetq_lane_f32(vgetq_lane_f32(v, 2), vdupq_n_f32(vgetq_lane_f32(v, 1)), 3); }
    template <>
    inline float32x4_t NeonShuffle<1, 1, 1, 3>(float32x4_t v) { return vsetq_lane_f32(vgetq_lane_f32(v, 3), vdupq_n_f32(vgetq_lane_f32(v, 1)), 3); }

    template <>
    inline float32x4_t NeonShuffle<2, 2, 2, 0>(float32x4_t v) { return vsetq_lane_f32(vgetq_lane_f32(v, 0), vdupq_n_f32(vgetq_lane_f32(v, 2)), 3); }
    template <>
    inline float32x4_t NeonShuffle<2, 2, 2, 1>(float32x4_t v) { return vsetq_lane_f32(vgetq_lane_f32(v, 1), vdupq_n_f32(vgetq_lane_f32(v, 2)), 3); }
    template <>
    inline float32x4_t NeonShuffle<2, 2, 2, 3>(float32x4_t v) { return vsetq_lane_f32(vgetq_lane_f32(v, 3), vdupq_n_f32(vgetq_lane_f32(v, 2)), 3); }

    template <>
    inline float32x4_t NeonShuffle<3, 3, 3, 0>(float32x4_t v) { return vsetq_lane_f32(vgetq_lane_f32(v, 0), vdupq_n_f32(vgetq_lane_f32(v, 3)), 3); }
    template <>
    inline float32x4_t NeonShuffle<3, 3, 3, 1>(float32x4_t v) { return vsetq_lane_f32(vgetq_lane_f32(v, 1), vdupq_n_f32(vgetq_lane_f32(v, 3)), 3); }
    template <>
    inline float32x4_t NeonShuffle<3, 3, 3, 2>(float32x4_t v) { return vsetq_lane_f32(vgetq_lane_f32(v, 2), vdupq_n_f32(vgetq_lane_f32(v, 3)), 3); }

    inline float32x2_t anemone_vectorcall NeonDot2(float32x2_t v1, float32x2_t v2)
    {
        float32x2_t const vxy = vmul_f32(v1, v2);
        return vpadd_f32(vxy, vxy);
    }

    inline float32x2_t anemone_vectorcall NeonDot2(float32x2_t v)
    {
        float32x2_t const vxy = vmul_f32(v, v);
        return vpadd_f32(vxy, vxy);
    }

    inline float32x2_t anemone_vectorcall NeonDot3(float32x4_t v1, float32x4_t v2)
    {
        // = [xm, ym, zm, _]
        float32x4_t const vxyz = vmulq_f32(v1, v2);
        // = [xm, ym]
        float32x2_t const vxy = vget_low_f32(vxyz);
        // = [zm, _]
        float32x2_t const vzw = vget_high_f32(vxyz);
        // = [x+y, x+y]
        float32x2_t const vxy_xy = vpadd_f32(vxy, vxy);
        // = [z, z]
        float32x2_t const vzz_zz = vdup_lane_f32(vzw, 0);
        // = [x+y+z, x+y+z]
        float32x2_t const vr = vadd_f32(vxy_xy, vzz_zz);
        // = [x+y+z, ...]
        return vr;
    }

    inline float32x2_t anemone_vectorcall NeonDot3(float32x4_t v)
    {
        // = [xm, ym, zm, _]
        float32x4_t const vxyz = vmulq_f32(v, v);
        // = [xm, ym]
        float32x2_t const vxy = vget_low_f32(vxyz);
        // = [zm, _]
        float32x2_t const vzw = vget_high_f32(vxyz);
        // = [x+y, x+y]
        float32x2_t const vxy_xy = vpadd_f32(vxy, vxy);
        // = [z, z]
        float32x2_t const vzz_zz = vdup_lane_f32(vzw, 0);
        // = [x+y+z, x+y+z]
        float32x2_t const vr = vadd_f32(vxy_xy, vzz_zz);
        // = [x+y+z, ...]
        return vr;
    }

    inline float32x2_t anemone_vectorcall NeonDot4(float32x4_t v1, float32x4_t v2)
    {
        // = [x, y, z, w]
        float32x4_t const vxyzw = vmulq_f32(v1, v2);
        // = [x+y, z+w]
        float32x2_t const vxy = vget_low_f32(vpaddq_f32(vxyzw, vxyzw));
        // = [x+y+z+w, ...]
        float32x2_t const vr = vpadd_f32(vxy, vxy);
        return vr;
    }

    inline float32x2_t anemone_vectorcall NeonDot4(float32x4_t v)
    {
        // = [x, y, z, w]
        float32x4_t const vxyzw = vmulq_f32(v, v);
        // = [x+y, z+w]
        float32x2_t const vxy = vget_low_f32(vpaddq_f32(vxyzw, vxyzw));
        // = [x+y+z+w, ...]
        float32x2_t const vr = vpadd_f32(vxy, vxy);
        return vr;
    }

    inline float32x2_t anemone_vectorcall NeonSquareRoot2xNewtonRhapson(float32x2_t v)
    {
        // = [0, 0]
        float32x2_t const zero = vdup_n_f32(0.0f);
        // = [v.x == 0, v.y == 0]
        uint32x2_t const mask_zero = vceq_f32(v, zero);
        // = sqrt_est(v)
        float32x2_t const s0 = vrsqrte_f32(v);
        float32x2_t const p0 = vmul_f32(v, s0);
        float32x2_t const r0 = vrsqrts_f32(p0, s0);
        float32x2_t const s1 = vmul_f32(s0, r0);
        float32x2_t const p1 = vmul_f32(v, s1);
        float32x2_t const r1 = vrsqrts_f32(p1, s1);
        float32x2_t const s2 = vmul_f32(s1, r1);
        float32x2_t const result = vmul_f32(v, s2);
        return vbsl_f32(mask_zero, zero, result);
    }

    inline float32x4_t anemone_vectorcall NeonSquareRoot2xNewtonRhapson(float32x4_t v)
    {
        // = [0, 0]
        float32x4_t const zero = vdupq_n_f32(0.0f);
        // = [v.x == 0, v.y == 0]
        uint32x4_t const mask_zero = vceqq_f32(v, zero);
        // = sqrt_est(v)
        float32x4_t const s0 = vrsqrteq_f32(v);
        float32x4_t const p0 = vmulq_f32(v, s0);
        float32x4_t const r0 = vrsqrtsq_f32(p0, s0);
        float32x4_t const s1 = vmulq_f32(s0, r0);
        float32x4_t const p1 = vmulq_f32(v, s1);
        float32x4_t const r1 = vrsqrtsq_f32(p1, s1);
        float32x4_t const s2 = vmulq_f32(s1, r1);
        float32x4_t const result = vmulq_f32(v, s2);
        return vbslq_f32(mask_zero, zero, result);
    }

    inline float32x2_t anemone_vectorcall NeonReciprocalSquareRoot2xNewtonRhapson(float32x2_t v)
    {
        float32x2_t const s0 = vrsqrte_f32(v);
        float32x2_t const p0 = vmul_f32(v, s0);
        float32x2_t const r0 = vrsqrts_f32(p0, s0);

        float32x2_t const s1 = vmul_f32(s0, r0);
        float32x2_t const p1 = vmul_f32(v, s1);
        float32x2_t const r1 = vrsqrts_f32(p1, s1);
        return vmul_f32(s1, r1);
    }

    inline float32x4_t anemone_vectorcall NeonReciprocalSquareRoot2xNewtonRhapson(float32x4_t v)
    {
        float32x4_t const s0 = vrsqrteq_f32(v);
        float32x4_t const p0 = vmulq_f32(v, s0);
        float32x4_t const r0 = vrsqrtsq_f32(p0, s0);

        float32x4_t const s1 = vmulq_f32(s0, r0);
        float32x4_t const p1 = vmulq_f32(v, s1);
        float32x4_t const r1 = vrsqrtsq_f32(p1, s1);
        return vmulq_f32(s1, r1);
    }

    inline uint32_t anemone_vectorcall NeonExtractMask4(uint32x4_t m)
    {
        // = [v & 1 << 31]
        uint32x4_t const masked = vandq_u32(m, vld1q_u32(F32x4_SignMask_XXXX.As<uint32_t>()));
        // = [v >> 31]
        uint32x4_t const shifted = vshlq_u32(masked, vld1q_s32(I32x4_MoveMaskShifts.As<int32_t>()));
        return vaddvq_u32(shifted);
    }

    inline uint32_t anemone_vectorcall NeonExtractMask2(uint32x2_t m)
    {
        // = [v & 1 << 31]
        uint32x2_t const masked = vand_u32(m, vld1_u32(F32x4_SignMask_XXXX.As<uint32_t>()));
        // = [v >> 31]
        uint32x2_t const shifted = vshl_u32(masked, vld1_s32(I32x4_MoveMaskShifts.As<int32_t>()));
        return vaddv_u32(shifted);
    }

    anemone_forceinline uint32x4_t NeonCreateMask(bool x, bool y, bool z, bool w)
    {
        // = [x, y, z, w] as int32x4
        int32x4_t const r{
            static_cast<int>(x),
            static_cast<int>(y),
            static_cast<int>(z),
            static_cast<int>(w),
        };

        // = [r > 0]
        return vcgtzq_s32(r);
    }

    anemone_forceinline uint32x4_t anemone_vectorcall NeonCompareSign(int32x4_t a, int32x4_t b)
    {
        return vceqq_s32(vshrq_n_s32(a, 31), vshrq_n_s32(b, 31));
    }

    anemone_forceinline uint32x2_t anemone_vectorcall NeonCompareSign(int32x2_t a, int32x2_t b)
    {
        return vceq_s32(vshr_n_s32(a, 31), vshr_n_s32(b, 31));
    }

    anemone_forceinline uint64x2_t anemone_vectorcall NeonCompareSign(int64x2_t a, int64x2_t b)
    {
        return vceqq_s64(vshrq_n_s64(a, 63), vshrq_n_s64(b, 63));
    }

    anemone_forceinline uint32x4_t anemone_vectorcall NeonCompareNearZeroPrecise(int32x4_t v)
    {
        int32x4_t const ulpTolerance = vdupq_n_s32(4);
        int32x4_t const ulpValue = vabsq_s32(v);
        return vcleq_s32(ulpValue, ulpTolerance);
    }

    anemone_forceinline uint32x2_t anemone_vectorcall NeonCompareNearZeroPrecise(int32x2_t v)
    {
        int32x2_t const ulpTolerance = vdup_n_s32(4);
        int32x2_t const ulpValue = vabs_s32(v);
        return vcle_s32(ulpValue, ulpTolerance);
    }

    anemone_forceinline uint64x2_t anemone_vectorcall NeonCompareNearZeroPrecise(int64x2_t v)
    {
        int64x2_t const ulpTolerance = vdupq_n_s64(4);
        int64x2_t const ulpValue = vabsq_s64(v);
        return vcleq_s64(ulpValue, ulpTolerance);
    }

    anemone_forceinline uint32x4_t anemone_vectorcall NeonCompareInBounds(float32x4_t v, float32x4_t lower, float32x4_t upper)
    {
        uint32x4_t const maskLower = vcleq_f32(lower, v);
        uint32x4_t const maskUpper = vcleq_f32(v, upper);
        return vandq_u32(maskLower, maskUpper);
    }

    anemone_forceinline uint32x2_t anemone_vectorcall NeonCompareInBounds(float32x2_t v, float32x2_t lower, float32x2_t upper)
    {
        uint32x2_t const maskLower = vcle_f32(lower, v);
        uint32x2_t const maskUpper = vcle_f32(v, upper);
        return vand_u32(maskLower, maskUpper);
    }

    anemone_forceinline uint64x2_t anemone_vectorcall NeonCompareInBounds(float64x2_t v, float64x2_t lower, float64x2_t upper)
    {
        uint64x2_t const maskLower = vcleq_f64(lower, v);
        uint64x2_t const maskUpper = vcleq_f64(v, upper);
        return vandq_u64(maskLower, maskUpper);
    }

    anemone_forceinline uint32x4_t anemone_vectorcall NeonCompareInBounds(float32x4_t v, float32x4_t bounds)
    {
        uint32x4_t const maskUpper = vcleq_f32(v, bounds);
        uint32x4_t const maskLower = vcleq_f32(vnegq_f32(bounds), v);
        return vandq_u32(maskLower, maskUpper);
    }

    anemone_forceinline uint32x2_t anemone_vectorcall NeonCompareInBounds(float32x2_t v, float32x2_t bounds)
    {
        uint32x2_t const maskUpper = vcle_f32(v, bounds);
        uint32x2_t const maskLower = vcle_f32(vneg_f32(bounds), v);
        return vand_u32(maskLower, maskUpper);
    }

    inline constexpr uint32_t NeonMaskComponentUInt32{0xFFFF'FFFFu};
}

// Vector functions
namespace Anemone::Numerics::Private
{
    inline SimdVector4F anemone_vectorcall Vector4F_LoadAlignedFloat4(float const* source)
    {
        return vld1q_f32(source);
    }

    inline SimdVector4F anemone_vectorcall Vector4F_LoadAlignedFloat3(float const* source)
    {
        return vld1q_f32(source);
    }

    inline SimdVector4F anemone_vectorcall Vector4F_LoadAlignedFloat2(float const* source)
    {
        return vld1q_f32(source);
    }

    inline SimdVector4F anemone_vectorcall Vector4F_LoadAlignedFloat1(float const* source)
    {
        return vld1q_f32(source);
    }

    inline SimdVector4F anemone_vectorcall Vector4F_LoadUnalignedFloat4(float const* source)
    {
        return vld1q_f32(source);
    }

    inline SimdVector4F anemone_vectorcall Vector4F_LoadUnalignedFloat3(float const* source)
    {
        // = [x, y]
        float32x2_t const xy = vld1_f32(source);
        // = [z, 0]
        float32x2_t const z0 = vld1_lane_f32(source + 2, vdup_n_f32(0.0f), 0);
        return vcombine_f32(xy, z0);
    }

    inline SimdVector4F anemone_vectorcall Vector4F_LoadUnalignedFloat2(float const* source)
    {
        // = [x, y]
        float32x2_t const xy = vld1_f32(source);
        // = [0, 0]
        float32x2_t const zero = vdup_n_f32(0.0f);
        return vcombine_f32(xy, zero);
    }

    inline SimdVector4F anemone_vectorcall Vector4F_LoadUnalignedFloat1(float const* source)
    {
        // = [x, 0, 0, 0]
        return vld1q_lane_f32(source, vdupq_n_f32(0.0f), 0);
    }

    inline void anemone_vectorcall Vector4F_StoreAlignedFloat4(float* destination, SimdVector4F source)
    {
        vst1q_f32(destination, source);
    }

    inline void anemone_vectorcall Vector4F_StoreAlignedFloat3(float* destination, SimdVector4F source)
    {
        vst1q_f32(destination, source);
    }

    inline void anemone_vectorcall Vector4F_StoreAlignedFloat2(float* destination, SimdVector4F source)
    {
        vst1q_f32(destination, source);
    }

    inline void anemone_vectorcall Vector4F_StoreAlignedFloat1(float* destination, SimdVector4F source)
    {
        vst1q_f32(destination, source);
    }

    inline void anemone_vectorcall Vector4F_StoreUnalignedFloat4(float* destination, SimdVector4F source)
    {
        vst1q_f32(destination, source);
    }

    inline void anemone_vectorcall Vector4F_StoreUnalignedFloat3(float* destination, SimdVector4F source)
    {
        // = [x, y]
        vst1_f32(destination, vget_low_f32(source));
        // = [z]
        vst1q_lane_f32(destination + 2, source, 2);
    }

    inline void anemone_vectorcall Vector4F_StoreUnalignedFloat2(float* destination, SimdVector4F source)
    {
        // = [x, y]
        vst1_f32(destination, vget_low_f32(source));
    }

    inline void anemone_vectorcall Vector4F_StoreUnalignedFloat1(float* destination, SimdVector4F source)
    {
        // = [x]
        vst1q_lane_f32(destination, source, 0);
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Create(float x, float y, float z, float w)
    {
        return float32x4_t{x, y, z, w};
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Replicate(float f)
    {
        return vdupq_n_f32(f);
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Zero()
    {
        return vdupq_n_f32(0.0f);
    }

    inline SimdVector4F anemone_vectorcall Vector4F_NaN()
    {
        return vld1q_f32(F32x4_PositiveQNaN_XXXX.As<float>());
    }

    inline SimdVector4F anemone_vectorcall Vector4F_PositiveInfinity()
    {
        return vld1q_f32(F32x4_PositiveInfinity_XXXX.As<float>());
    }

    inline SimdVector4F anemone_vectorcall Vector4F_NegativeInfinity()
    {
        return vld1q_f32(F32x4_NegativeInfinity_XXXX.As<float>());
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Epsilon()
    {
        return vld1q_f32(F32x4_Epsilon_XXXX.As<float>());
    }

    inline SimdVector4F anemone_vectorcall Vector4F_PositiveUnitX()
    {
        return vld1q_f32(F32x4_PositiveUnitX.Elements);
    }

    inline SimdVector4F anemone_vectorcall Vector4F_PositiveUnitY()
    {
        return vld1q_f32(F32x4_PositiveUnitY.Elements);
    }

    inline SimdVector4F anemone_vectorcall Vector4F_PositiveUnitZ()
    {
        return vld1q_f32(F32x4_PositiveUnitZ.Elements);
    }

    inline SimdVector4F anemone_vectorcall Vector4F_PositiveUnitW()
    {
        return vld1q_f32(F32x4_PositiveUnitW.Elements);
    }

    inline SimdVector4F anemone_vectorcall Vector4F_NegativeUnitX()
    {
        return vld1q_f32(F32x4_NegativeUnitX.Elements);
    }

    inline SimdVector4F anemone_vectorcall Vector4F_NegativeUnitY()
    {
        return vld1q_f32(F32x4_NegativeUnitY.Elements);
    }

    inline SimdVector4F anemone_vectorcall Vector4F_NegativeUnitZ()
    {
        return vld1q_f32(F32x4_NegativeUnitZ.Elements);
    }

    inline SimdVector4F anemone_vectorcall Vector4F_NegativeUnitW()
    {
        return vld1q_f32(F32x4_NegativeUnitW.Elements);
    }


    template <size_t N>
    inline float anemone_vectorcall Vector4F_Extract(SimdVector4F v)
        requires(N < 4)
    {
        return vgetq_lane_f32(v, N);
    }

    template <size_t N>
    inline SimdVector4F anemone_vectorcall Vector4F_Insert(SimdVector4F v, float f)
        requires(N < 4)
    {
        return vsetq_lane_f32(f, v, N);
    }

    template <size_t N>
    inline SimdVector4F anemone_vectorcall Vector4F_Broadcast(SimdVector4F v)
        requires(N < 4)
    {
        return vdupq_laneq_f32(v, N);
    }

    template <size_t X, size_t Y, size_t Z, size_t W>
    inline SimdVector4F anemone_vectorcall Vector4F_Permute(SimdVector4F v)
        requires((X < 4) and (Y < 4) and (Z < 4) and (W < 4))
    {
        return NeonShuffle<X, Y, Z, W>(v);
    }

    template <size_t X, size_t Y, size_t Z, size_t W>
    inline SimdVector4F anemone_vectorcall Vector4F_Permute(SimdVector4F a, SimdVector4F b)
        requires((X < 8) and (Y < 8) and (Z < 8) and (W < 8))
    {
        float32x4_t r;

        if constexpr (X < 4)
        {
            r = vmovq_n_f32(vgetq_lane_f32(a, X));
        }
        else
        {
            r = vmovq_n_f32(vgetq_lane_f32(b, X - 4));
        }

        if constexpr (Y < 4)
        {
            r = vcopyq_laneq_f32(r, 1, a, Y);
        }
        else
        {
            r = vcopyq_laneq_f32(r, 1, b, Y - 4);
        }

        if constexpr (Z < 4)
        {
            r = vcopyq_laneq_f32(r, 2, a, Z);
        }
        else
        {
            r = vcopyq_laneq_f32(r, 2, b, Z - 4);
        }

        if constexpr (W < 4)
        {
            r = vcopyq_laneq_f32(r, 3, a, W);
        }
        else
        {
            r = vcopyq_laneq_f32(r, 3, b, W - 4);
        }

        return r;
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Select(SimdMask4F mask, SimdVector4F whenFalse, SimdVector4F whenTrue)
    {
        return vbslq_f32(mask, whenTrue, whenFalse);
    }

    template <bool X, bool Y, bool Z, bool W>
    inline SimdVector4F anemone_vectorcall Vector4F_Select(SimdVector4F whenFalse, SimdVector4F whenTrue)
    {
        static constexpr uint32x4_t mask{
            X ? NeonMaskComponentUInt32 : 0,
            Y ? NeonMaskComponentUInt32 : 0,
            Z ? NeonMaskComponentUInt32 : 0,
            W ? NeonMaskComponentUInt32 : 0,
        };

        return vbslq_f32(mask, whenTrue, whenFalse);
    }

    inline SimdVector4F anemone_vectorcall Vector4F_MultiplyAdd(SimdVector4F a, SimdVector4F b, SimdVector4F c)
    {
        return vmlaq_f32(c, a, b);
    }

    inline SimdVector4F anemone_vectorcall Vector4F_MultiplySubtract(SimdVector4F a, SimdVector4F b, SimdVector4F c)
    {
        return vmlaq_f32(vnegq_f32(c), a, b);
    }

    inline SimdVector4F anemone_vectorcall Vector4F_NegateMultiplyAdd(SimdVector4F a, SimdVector4F b, SimdVector4F c)
    {
        return vmlsq_f32(c, a, b);
    }

    inline SimdVector4F anemone_vectorcall Vector4F_NegateMultiplySubtract(SimdVector4F a, SimdVector4F b, SimdVector4F c)
    {
        return vmlsq_f32(vnegq_f32(c), a, b);
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Add(SimdVector4F a, SimdVector4F b)
    {
        return vaddq_f32(a, b);
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Subtract(SimdVector4F a, SimdVector4F b)
    {
        return vsubq_f32(a, b);
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Multiply(SimdVector4F a, SimdVector4F b)
    {
        return vmulq_f32(a, b);
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Divide(SimdVector4F a, SimdVector4F b)
    {
        return vdivq_f32(a, b);
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Negate(SimdVector4F v)
    {
        return vnegq_f32(v);
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Reciprocal(SimdVector4F v)
    {
        return vdivq_f32(vdupq_n_f32(1.0f), v);
    }

    inline SimdVector4F anemone_vectorcall Vector4F_ReciprocalEst(SimdVector4F v)
    {
        return vrecpeq_f32(v);
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Min(SimdVector4F a, SimdVector4F b)
    {
        return vminq_f32(a, b);
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Max(SimdVector4F a, SimdVector4F b)
    {
        return vmaxq_f32(a, b);
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Abs(SimdVector4F v)
    {
        return vabsq_f32(v);
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Square(SimdVector4F v)
    {
        return vmulq_f32(v, v);
    }

    inline SimdVector4F anemone_vectorcall Vector4F_SignedSquare(SimdVector4F v)
    {
        return vmulq_f32(v, vabsq_f32(v));
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Clamp(SimdVector4F v, SimdVector4F lower, SimdVector4F upper)
    {
        return vminq_f32(upper, vmaxq_f32(lower, v));
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Saturate(SimdVector4F v)
    {
        float32x4_t const lower = vdupq_n_f32(0.0f);
        float32x4_t const upper = vdupq_n_f32(1.0f);
        return vminq_f32(upper, vmaxq_f32(lower, v));
    }

    inline SimdVector4F anemone_vectorcall Vector4F_SquareRoot(SimdVector4F v)
    {
        return vsqrtq_f32(v);
    }

    inline SimdVector4F anemone_vectorcall Vector4F_SquareRootEst(SimdVector4F v)
    {
        // Use NeonSquareRoot2xNewtonRhapson(v.Inner);?
        return vsqrtq_f32(v);
    }

    inline SimdVector4F anemone_vectorcall Vector4F_ReciprocalSquareRoot(SimdVector4F v)
    {
        return NeonReciprocalSquareRoot2xNewtonRhapson(v);
    }

    inline SimdVector4F anemone_vectorcall Vector4F_ReciprocalSquareRootEst(SimdVector4F v)
    {
        return vrsqrteq_f32(v);
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Ceil(SimdVector4F v)
    {
        return vrndpq_f32(v);
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Floor(SimdVector4F v)
    {
        return vrndmq_f32(v);
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Truncate(SimdVector4F v)
    {
        return vrndq_f32(v);
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Round(SimdVector4F v)
    {
        return vrndaq_f32(v);
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Fraction(SimdVector4F v)
    {
        return vsubq_f32(v, vrndmq_f32(v));
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Remainder(SimdVector4F x, SimdVector4F y)
    {
        // = [x / y]
        float32x4_t const r0 = vdivq_f32(x, y);
        // = [trunc(x / y)]
        float32x4_t const r1 = vrndq_f32(r0);
        // = [x - (trunc(x / y) * y)]
        return vfmsq_f32(x, r1, y);
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Repeat(SimdVector4F v, SimdVector4F length)
    {
        float32x4_t const scaled = Vector4F_Fraction(vdivq_f32(v, length));
        return vmulq_f32(scaled, length);
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Wrap(SimdVector4F v, SimdVector4F lower, SimdVector4F upper)
    {
        float32x4_t const range = vsubq_f32(upper, lower);
        float32x4_t const wrapped = vsubq_f32(v, lower);
        float32x4_t const scaled = Vector4F_Fraction(vdivq_f32(wrapped, range));
        return vmlaq_f32(lower, scaled, range);
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Power(SimdVector4F x, SimdVector4F y)
    {
        return float32x4_t{
            Power<float>(vgetq_lane_f32(x, 0), vgetq_lane_f32(y, 0)),
            Power<float>(vgetq_lane_f32(x, 1), vgetq_lane_f32(y, 1)),
            Power<float>(vgetq_lane_f32(x, 2), vgetq_lane_f32(y, 2)),
            Power<float>(vgetq_lane_f32(x, 3), vgetq_lane_f32(y, 3)),
        };
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Exp(SimdVector4F v)
    {
        return float32x4_t{
            Exp<float>(vgetq_lane_f32(v, 0)),
            Exp<float>(vgetq_lane_f32(v, 1)),
            Exp<float>(vgetq_lane_f32(v, 2)),
            Exp<float>(vgetq_lane_f32(v, 3)),
        };
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Exp2(SimdVector4F v)
    {
        return float32x4_t{
            Exp2<float>(vgetq_lane_f32(v, 0)),
            Exp2<float>(vgetq_lane_f32(v, 1)),
            Exp2<float>(vgetq_lane_f32(v, 2)),
            Exp2<float>(vgetq_lane_f32(v, 3)),
        };
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Exp10(SimdVector4F v)
    {
        return float32x4_t{
            Exp10<float>(vgetq_lane_f32(v, 0)),
            Exp10<float>(vgetq_lane_f32(v, 1)),
            Exp10<float>(vgetq_lane_f32(v, 2)),
            Exp10<float>(vgetq_lane_f32(v, 3)),
        };
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Log(SimdVector4F v)
    {
        return float32x4_t{
            Log<float>(vgetq_lane_f32(v, 0)),
            Log<float>(vgetq_lane_f32(v, 1)),
            Log<float>(vgetq_lane_f32(v, 2)),
            Log<float>(vgetq_lane_f32(v, 3)),
        };
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Log(SimdVector4F x, SimdVector4F base)
    {
        return float32x4_t{
            Log<float>(vgetq_lane_f32(x, 0), vgetq_lane_f32(base, 0)),
            Log<float>(vgetq_lane_f32(x, 1), vgetq_lane_f32(base, 1)),
            Log<float>(vgetq_lane_f32(x, 2), vgetq_lane_f32(base, 2)),
            Log<float>(vgetq_lane_f32(x, 3), vgetq_lane_f32(base, 3)),
        };
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Log2(SimdVector4F v)
    {
        return float32x4_t{
            Log2<float>(vgetq_lane_f32(v, 0)),
            Log2<float>(vgetq_lane_f32(v, 1)),
            Log2<float>(vgetq_lane_f32(v, 2)),
            Log2<float>(vgetq_lane_f32(v, 3)),
        };
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Log10(SimdVector4F v)
    {
        return float32x4_t{
            Log10<float>(vgetq_lane_f32(v, 0)),
            Log10<float>(vgetq_lane_f32(v, 1)),
            Log10<float>(vgetq_lane_f32(v, 2)),
            Log10<float>(vgetq_lane_f32(v, 3)),
        };
    }

    inline void anemone_vectorcall Vector4F_SinCos(SimdVector4F& sin, SimdVector4F& cos, SimdVector4F v)
    {
        sin = float32x4_t{
            Sin<float>(vgetq_lane_f32(v, 0)),
            Sin<float>(vgetq_lane_f32(v, 1)),
            Sin<float>(vgetq_lane_f32(v, 2)),
            Sin<float>(vgetq_lane_f32(v, 3)),
        };
        cos = float32x4_t{
            Cos<float>(vgetq_lane_f32(v, 0)),
            Cos<float>(vgetq_lane_f32(v, 1)),
            Cos<float>(vgetq_lane_f32(v, 2)),
            Cos<float>(vgetq_lane_f32(v, 3)),
        };
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Sin(SimdVector4F v)
    {
        return float32x4_t{
            Sin<float>(vgetq_lane_f32(v, 0)),
            Sin<float>(vgetq_lane_f32(v, 1)),
            Sin<float>(vgetq_lane_f32(v, 2)),
            Sin<float>(vgetq_lane_f32(v, 3)),
        };
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Cos(SimdVector4F v)
    {
        return float32x4_t{
            Cos<float>(vgetq_lane_f32(v, 0)),
            Cos<float>(vgetq_lane_f32(v, 1)),
            Cos<float>(vgetq_lane_f32(v, 2)),
            Cos<float>(vgetq_lane_f32(v, 3)),
        };
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Tan(SimdVector4F v)
    {
        return float32x4_t{
            Tan<float>(vgetq_lane_f32(v, 0)),
            Tan<float>(vgetq_lane_f32(v, 1)),
            Tan<float>(vgetq_lane_f32(v, 2)),
            Tan<float>(vgetq_lane_f32(v, 3)),
        };
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Asin(SimdVector4F v)
    {
        return float32x4_t{
            Asin<float>(vgetq_lane_f32(v, 0)),
            Asin<float>(vgetq_lane_f32(v, 1)),
            Asin<float>(vgetq_lane_f32(v, 2)),
            Asin<float>(vgetq_lane_f32(v, 3)),
        };
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Acos(SimdVector4F v)
    {
        return float32x4_t{
            Acos<float>(vgetq_lane_f32(v, 0)),
            Acos<float>(vgetq_lane_f32(v, 1)),
            Acos<float>(vgetq_lane_f32(v, 2)),
            Acos<float>(vgetq_lane_f32(v, 3)),
        };
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Atan(SimdVector4F v)
    {
        return float32x4_t{
            Atan<float>(vgetq_lane_f32(v, 0)),
            Atan<float>(vgetq_lane_f32(v, 1)),
            Atan<float>(vgetq_lane_f32(v, 2)),
            Atan<float>(vgetq_lane_f32(v, 3)),
        };
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Atan2(SimdVector4F y, SimdVector4F x)
    {
        return float32x4_t{
            Atan2<float>(vgetq_lane_f32(y, 0), vgetq_lane_f32(x, 0)),
            Atan2<float>(vgetq_lane_f32(y, 1), vgetq_lane_f32(x, 1)),
            Atan2<float>(vgetq_lane_f32(y, 2), vgetq_lane_f32(x, 2)),
            Atan2<float>(vgetq_lane_f32(y, 3), vgetq_lane_f32(x, 3)),
        };
    }

    inline SimdVector4F anemone_vectorcall Vector4F_PreciseLerp(SimdVector4F a, SimdVector4F b, SimdVector4F t)
    {
        float32x4_t const one = vdupq_n_f32(1.0f);
        float32x4_t const nt = vsubq_f32(one, t);
        return vmlaq_f32(vmulq_f32(b, t), a, nt);
    }

    inline SimdVector4F anemone_vectorcall Vector4F_PreciseLerp(SimdVector4F a, SimdVector4F b, float t)
    {
        float const nt = 1.0f - t;
        return vmlaq_n_f32(vmulq_n_f32(b, t), a, nt);
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Lerp(SimdVector4F a, SimdVector4F b, SimdVector4F t)
    {
        float32x4_t const ba = vsubq_f32(b, a);
        return vmlaq_f32(a, ba, t);
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Lerp(SimdVector4F a, SimdVector4F b, float t)
    {
        float32x4_t const ba = vsubq_f32(b, a);

        return vmlaq_n_f32(a, ba, t);
    }

    inline SimdVector4F anemone_vectorcall Vector4F_PreciseBilinearLerp(SimdVector4F a, SimdVector4F b, SimdVector4F c, SimdVector4F d, SimdVector4F u, SimdVector4F v)
    {
        float32x4_t const ab = Vector4F_PreciseLerp(a, b, u);
        float32x4_t const cd = Vector4F_PreciseLerp(c, d, u);
        return Vector4F_PreciseLerp(ab, cd, v);
    }

    inline SimdVector4F anemone_vectorcall Vector4F_PreciseBilinearLerp(SimdVector4F a, SimdVector4F b, SimdVector4F c, SimdVector4F d, float u, float v)
    {
        float32x4_t const ab = Vector4F_PreciseLerp(a, b, u);
        float32x4_t const cd = Vector4F_PreciseLerp(c, d, u);
        return Vector4F_PreciseLerp(ab, cd, v);
    }

    inline SimdVector4F anemone_vectorcall Vector4F_BilinearLerp(SimdVector4F a, SimdVector4F b, SimdVector4F c, SimdVector4F d, SimdVector4F u, SimdVector4F v)
    {
        float32x4_t const ab = Vector4F_Lerp(a, b, u);
        float32x4_t const cd = Vector4F_Lerp(c, d, u);
        return Vector4F_Lerp(ab, cd, v);
    }

    inline SimdVector4F anemone_vectorcall Vector4F_BilinearLerp(SimdVector4F a, SimdVector4F b, SimdVector4F c, SimdVector4F d, float u, float v)
    {
        float32x4_t const ab = Vector4F_Lerp(a, b, u);
        float32x4_t const cd = Vector4F_Lerp(c, d, u);
        return Vector4F_Lerp(ab, cd, v);
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Barycentric(SimdVector4F a, SimdVector4F b, SimdVector4F c, SimdVector4F u, SimdVector4F v)
    {
        float32x4_t const ba = vsubq_f32(b, a);
        float32x4_t const ca = vsubq_f32(c, a);
        float32x4_t const r = vmlaq_f32(a, ba, u);
        return vmlaq_f32(r, ca, v);
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Barycentric(SimdVector4F a, SimdVector4F b, SimdVector4F c, float u, float v)
    {
        float32x4_t const ba = vsubq_f32(b, a);
        float32x4_t const ca = vsubq_f32(c, a);

        float32x4_t const r = vmlaq_n_f32(a, ba, u);
        return vmlaq_n_f32(r, ca, v);
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Unlerp(SimdVector4F value, SimdVector4F lower, SimdVector4F upper)
    {
        float32x4_t const num = vsubq_f32(value, lower);
        float32x4_t const den = vsubq_f32(upper, lower);
        return vdivq_f32(num, den);
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Map(SimdVector4F value, SimdVector4F sourceLower, SimdVector4F sourceUpper, SimdVector4F targetLower, SimdVector4F targetUpper)
    {
        float32x4_t const ratio = Vector4F_Unlerp(value, sourceLower, sourceUpper);
        return Vector4F_Lerp(targetLower, targetUpper, ratio);
    }

    inline SimdVector4F anemone_vectorcall Vector4F_PreciseMap(SimdVector4F value, SimdVector4F sourceLower, SimdVector4F sourceUpper, SimdVector4F targetLower, SimdVector4F targetUpper)
    {
        float32x4_t const ratio = Vector4F_Unlerp(value, sourceLower, sourceUpper);
        return Vector4F_PreciseLerp(targetLower, targetUpper, ratio);
    }

    inline SimdVector4F anemone_vectorcall Vector4F_BezierPosition(SimdVector4F p0, SimdVector4F p1, SimdVector4F p2, SimdVector4F t)
    {
        // https://en.wikipedia.org/wiki/B%C3%A9zier_curve#Cubic_B%C3%A9zier_curves
        // Note: This one uses matrix representation to compress the computation.

        // B(t) = ( 1 * t^2 - 2 * t + 1) * p0
        //      + (-2 * t^2 + 2 * t    ) * p1
        //      + ( 1 * t^2            ) * p2

        // = [t^2, t^2, t^2, t^2]
        float32x4_t const t2 = vmulq_f32(t, t);

        // = [1, -2, 1, 0]
        float32x4_t const c2{1.0f, -2.0f, 1.0f, 0.0f};
        // = [-2, 2, 0, 0]
        float32x4_t const c1{-2.0f, 2.0f, 0.0f, 0.0f};
        // = [1, 0, 0, 0]
        float32x4_t const c0{1.0f, 0.0f, 0.0f, 0.0f};

        // Multiply by coefficients
        // = (c2 * t2 + (c1 * t + c0))
        float32x4_t cc = vmlaq_f32(c0, c1, t);
        cc = vmlaq_f32(cc, c2, t2);

        // Evaluate
        float32x4_t r = vmulq_laneq_f32(p0, cc, 0);
        r = vmlaq_laneq_f32(r, p1, cc, 1);
        r = vmlaq_laneq_f32(r, p2, cc, 2);
        return r;
    }

    inline SimdVector4F anemone_vectorcall Vector4F_BezierPosition(SimdVector4F p0, SimdVector4F p1, SimdVector4F p2, float t)
    {
        return Vector4F_BezierPosition(p0, p1, p2, vdupq_n_f32(t));
    }

    inline SimdVector4F anemone_vectorcall Vector4F_BezierTangent(SimdVector4F p0, SimdVector4F p1, SimdVector4F p2, SimdVector4F t)
    {
        // https://en.wikipedia.org/wiki/B%C3%A9zier_curve#Cubic_B%C3%A9zier_curves
        // Note: This one uses matrix representation to compress the computation.

        // B'(t) = ( 2 * t - 2) * p0
        //       + (-4 * t + 2) * p1
        //       + ( 2 * t    ) * p2

        // = [2, -4, 2, 0]
        float32x4_t const c1{2.0f, -4.0f, 2.0f, 0.0f};
        // = [-2, 2, 0, 0]
        float32x4_t const c0{-2.0f, 2.0f, 0.0f, 0.0f};

        // Multiply by coefficients
        float32x4_t const cc = vmlaq_f32(c0, c1, t);

        // Evaluate
        float32x4_t r = vmulq_laneq_f32(p0, cc, 0);
        r = vmlaq_laneq_f32(r, p1, cc, 1);
        r = vmlaq_laneq_f32(r, p2, cc, 2);
        return r;
    }

    inline SimdVector4F anemone_vectorcall Vector4F_BezierTangent(SimdVector4F p0, SimdVector4F p1, SimdVector4F p2, float t)
    {
        return Vector4F_BezierTangent(p0, p1, p2, vdupq_n_f32(t));
    }

    inline SimdVector4F anemone_vectorcall Vector4F_BezierAcceleration(SimdVector4F p0, SimdVector4F p1, SimdVector4F p2, SimdVector4F t)
    {
        // Time value is not used to compute acceleration at a specific point.
        (void)t;

        // https://en.wikipedia.org/wiki/B%C3%A9zier_curve#Cubic_B%C3%A9zier_curves
        // Note: This one uses matrix representation to compress the computation.

        // B''(t) = ( 2) * p0
        //        + (-4) * p1
        //        + ( 2) * p2

        // = [2, 2, 2, 2]
        float32x4_t const c0 = vdupq_n_f32(2.0f);
        // = [-4, -4, -4, -4]
        float32x4_t const c1 = vdupq_n_f32(-4.0f);

        // Evaluate
        float32x4_t r = vmulq_f32(p0, c0);
        r = vmlaq_f32(r, p1, c1);
        r = vmlaq_f32(r, p2, c0);
        return r;
    }

    inline SimdVector4F anemone_vectorcall Vector4F_BezierAcceleration(SimdVector4F p0, SimdVector4F p1, SimdVector4F p2, float t)
    {
        return Vector4F_BezierAcceleration(p0, p1, p2, vdupq_n_f32(t));
    }

    inline SimdVector4F anemone_vectorcall Vector4F_BezierPosition(SimdVector4F p0, SimdVector4F p1, SimdVector4F p2, SimdVector4F p3, SimdVector4F t)
    {
        // https://en.wikipedia.org/wiki/B%C3%A9zier_curve#Cubic_B%C3%A9zier_curves
        // Note: This one uses matrix representation to compress the computation.

        // B(t) = (-1 * t^3 + 3 * t^2 - 3 * t + 1) * p0
        //      + ( 3 * t^3 - 6 * t^2 + 3 * t    ) * p1
        //      + (-3 * t^3 + 3 * t^2            ) * p2
        //      + ( 1 * t^3                      ) * p3

        // = [t^2, t^2, t^2, t^2]
        float32x4_t const t2 = vmulq_f32(t, t);
        // = [t^3, t^3, t^3, t^3]
        float32x4_t const t3 = vmulq_f32(t2, t);

        // = [-1, 3, -3, 1]
        float32x4_t const c3{-1.0f, 3.0f, -3.0f, 1.0f};
        // = [3, -6, 3, 0]
        float32x4_t const c2{3.0f, -6.0f, 3.0f, 0.0f};
        // = [-3, 3, 0, 0]
        float32x4_t const c1{-3.0f, 3.0f, 0.0f, 0.0f};
        // = [1, 0, 0, 0]
        float32x4_t const c0{1.0f, 0.0f, 0.0f, 0.0f};

        // Multiply by coefficients
        // = (c3 * t3 + (c2 * t2)) + (c1 * t + c0)

        float32x4_t const r0 = vmulq_f32(c2, t2);
        float32x4_t const r1 = vmlaq_f32(r0, c3, t3);
        float32x4_t const r2 = vmlaq_f32(c0, c1, t);
        float32x4_t const cc = vaddq_f32(r1, r2);

        // Evaluate
        float32x4_t r = vmulq_laneq_f32(p0, cc, 0);
        r = vmlaq_laneq_f32(r, p1, cc, 1);
        r = vmlaq_laneq_f32(r, p2, cc, 2);
        r = vmlaq_laneq_f32(r, p3, cc, 3);
        return r;
    }

    inline SimdVector4F anemone_vectorcall Vector4F_BezierPosition(SimdVector4F p0, SimdVector4F p1, SimdVector4F p2, SimdVector4F p3, float t)
    {
        return Vector4F_BezierPosition(p0, p1, p2, p3, vdupq_n_f32(t));
    }

    inline SimdVector4F anemone_vectorcall Vector4F_BezierTangent(SimdVector4F p0, SimdVector4F p1, SimdVector4F p2, SimdVector4F p3, SimdVector4F t)
    {
        // https://en.wikipedia.org/wiki/B%C3%A9zier_curve#Cubic_B%C3%A9zier_curves
        // Note: This one uses matrix representation to compress the computation.

        // B'(t) = (-3 * t^2 +  6 * t - 3) * p0
        //       + ( 9 * t^2 - 12 * t + 3) * p1
        //       + (-9 * t^2 +  6 * t    ) * p2
        //       + ( 3 * t^2             ) * p3

        // = [t^2, t^2, t^2, t^2]
        float32x4_t const t2 = vmulq_f32(t, t);

        // = [-3, 9, -9, 3]
        float32x4_t const c2{-3.0f, 9.0f, -9.0f, 3.0f};
        // = [6, -12, 6, 0]
        float32x4_t const c1{6.0f, -12.0f, 6.0f, 0.0f};
        // = [-3, 3, 0, 0]
        float32x4_t const c0{-3.0f, 3.0f, 0.0f, 0.0f};

        // Multiply by coefficients
        // = (c2 * t2 + (c1 * t + c0))
        float32x4_t cc = vmlaq_f32(c0, c1, t);
        cc = vmlaq_f32(cc, c2, t2);

        // Evaluate
        float32x4_t r = vmulq_laneq_f32(p0, cc, 0);
        r = vmlaq_laneq_f32(r, p1, cc, 1);
        r = vmlaq_laneq_f32(r, p2, cc, 2);
        r = vmlaq_laneq_f32(r, p3, cc, 3);
        return r;
    }

    inline SimdVector4F anemone_vectorcall Vector4F_BezierTangent(SimdVector4F p0, SimdVector4F p1, SimdVector4F p2, SimdVector4F p3, float t)
    {
        return Vector4F_BezierTangent(p0, p1, p2, p3, vdupq_n_f32(t));
    }

    inline SimdVector4F anemone_vectorcall Vector4F_BezierAcceleration(SimdVector4F p0, SimdVector4F p1, SimdVector4F p2, SimdVector4F p3, SimdVector4F t)
    {
        // https://en.wikipedia.org/wiki/B%C3%A9zier_curve#Cubic_B%C3%A9zier_curves
        // Note: This one uses matrix representation to compress the computation.

        // B''(t) = ( -6 * t +  6) * p0
        //        + ( 18 * t - 12) * p1
        //        + (-18 * t +  6) * p2
        //        + (  6 * t     ) * p3

        // = [-6, 18, -18, 6]
        float32x4_t const c1{-6.0f, 18.0f, -18.0f, 6.0f};
        // = [6, -12, 6, 0]
        float32x4_t const c0{6.0f, -12.0f, 6.0f, 0.0f};

        // Multiply by coefficients
        // = (c1 * t + c0)
        float32x4_t const cc = vmlaq_f32(c0, c1, t);

        // Evaluate
        float32x4_t r = vmulq_laneq_f32(p0, cc, 0);
        r = vmlaq_laneq_f32(r, p1, cc, 1);
        r = vmlaq_laneq_f32(r, p2, cc, 2);
        r = vmlaq_laneq_f32(r, p3, cc, 3);
        return r;
    }

    inline SimdVector4F anemone_vectorcall Vector4F_BezierAcceleration(SimdVector4F p0, SimdVector4F p1, SimdVector4F p2, SimdVector4F p3, float t)
    {
        return Vector4F_BezierAcceleration(p0, p1, p2, p3, vdupq_n_f32(t));
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Hermite(SimdVector4F p0, SimdVector4F m0, SimdVector4F p1, SimdVector4F m1, SimdVector4F t)
    {
        // https://en.wikipedia.org/wiki/Cubic_Hermite_spline
        // Note: This one uses matrix representation to compress the computation.

        // H(t) = ( 2 * t^3 - 3 * t^2         + 1) * p0
        //      + ( 1 * t^3 - 2 * t^2 + 1 * t    ) * m0
        //      + (-2 * t^3 + 3 * t^2            ) * p1
        //      + ( 1   t^3 - 1 * t^2            ) * m1

        // = [t^2, t^2, t^2, t^2]
        float32x4_t const t2 = vmulq_f32(t, t);
        // = [t^3, t^3, t^3, t^3]
        float32x4_t const t3 = vmulq_f32(t2, t);

        float32x4_t const c3{2.0f, 1.0f, -2.0f, 1.0f};
        float32x4_t const c2{-3.0f, -2.0f, 3.0f, -1.0f};
        float32x4_t const c0 = vsetq_lane_f32(1.0f, vdupq_n_f32(0.0f), 0);

        // Multiply by coefficients
        // = (c3 * t3 + (c2 * t2)) + ((c1 * t) + c0)
        float32x4_t const r0 = vmulq_f32(c2, t2);
        float32x4_t const r1 = vmlaq_f32(r0, c3, t3);

        // = [0, t, 0, 0]
        float32x4_t const zero = vdupq_n_f32(0.0f);
        float32x4_t const r2 = vcopyq_laneq_f32(zero, 1, t, 0);

        float32x4_t const r3 = vaddq_f32(r2, c0);
        float32x4_t const cc = vaddq_f32(r1, r3);

        // Evaluate
        float32x4_t r = vmulq_laneq_f32(p0, cc, 0);
        r = vmlaq_laneq_f32(r, m0, cc, 1);
        r = vmlaq_laneq_f32(r, p1, cc, 2);
        r = vmlaq_laneq_f32(r, m1, cc, 3);
        return r;
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Hermite(SimdVector4F p0, SimdVector4F m0, SimdVector4F p1, SimdVector4F m1, float t)
    {
        return Vector4F_Hermite(p0, m0, p1, m1, vdupq_n_f32(t));
    }

    inline SimdVector4F anemone_vectorcall Vector4F_CatmullRom(SimdVector4F p0, SimdVector4F p1, SimdVector4F p2, SimdVector4F p3, SimdVector4F t)
    {
        // https://en.wikipedia.org/wiki/Cubic_Hermite_spline#Catmull%E2%80%93Rom_spline

        // C(t) = 0.5 * (
        //         (( -t^3 + 2t^2 - t    ) * p0) +
        //         (( 3t^3 - 5t^2     + 2) * p1) +
        //         ((-3t^3 + 4t^2 + t    ) * p2) +
        //         ((  t^3 -  t^2        ) * p3)
        //      )

        float32x4_t const c3{-1.0f, 3.0f, -3.0f, 1.0f};
        float32x4_t const c2{2.0f, -5.0f, 4.0f, -1.0f};
        float32x4_t const c1{-1.0f, 0.0f, 1.0f, 0.0f};
        float32x4_t const c0{0.0f, 2.0f, 0.0f, 0.0f};

        // = [t^2, t^2, t^2, t^2]
        float32x4_t const t2 = vmulq_f32(t, t);

        // = [t^3, t^3, t^3, t^3]
        float32x4_t const t3 = vmulq_f32(t2, t);

        // Multiply [t^3, t^2, t^1, 1] by coefficients
        // = (c3 * t3 + (c2 * t2)) + (c1 * t + c0)

        float32x4_t const r0 = vmulq_f32(c2, t2);
        float32x4_t const r1 = vmlaq_f32(r0, c3, t3);
        float32x4_t const r2 = vmlaq_f32(c0, c1, t);
        float32x4_t const cc = vaddq_f32(r1, r2);

        float32x4_t r = vmulq_laneq_f32(p0, cc, 0);
        r = vmlaq_laneq_f32(r, p1, cc, 1);
        r = vmlaq_laneq_f32(r, p2, cc, 2);
        r = vmlaq_laneq_f32(r, p3, cc, 3);

        return vmulq_f32(r, vdupq_n_f32(0.5f));
    }

    inline SimdVector4F anemone_vectorcall Vector4F_CatmullRom(SimdVector4F p0, SimdVector4F p1, SimdVector4F p2, SimdVector4F p3, float t)
    {
        return Vector4F_CatmullRom(p0, p1, p2, p3, vdupq_n_f32(t));
    }

    inline SimdVector4F anemone_vectorcall Vector4F_DeCasteljau(SimdVector4F p0, SimdVector4F p1, SimdVector4F p2, SimdVector4F p3, SimdVector4F t)
    {
        // https://en.wikipedia.org/wiki/De_Casteljau%27s_algorithm

        float32x4_t const a0 = Vector4F_Lerp(p0, p1, t);
        float32x4_t const a1 = Vector4F_Lerp(p1, p2, t);
        float32x4_t const a2 = Vector4F_Lerp(p2, p3, t);

        float32x4_t const b0 = Vector4F_Lerp(a0, a1, t);
        float32x4_t const b1 = Vector4F_Lerp(a1, a2, t);

        return Vector4F_Lerp(b0, b1, t);
    }

    inline SimdVector4F anemone_vectorcall Vector4F_UnwindRadians(SimdVector4F v)
    {
        float32x4_t const vToRevolutions = vld1q_f32(F32x4_InvPi2.As<float>());
        float32x4_t const vFromRevolutions = vld1q_f32(F32x4_Pi2.As<float>());

        // Convert radians to full revolutions
        float32x4_t r = vmulq_f32(v, vToRevolutions);
        r = Vector4F_Round(r);
        // Convert full revolutions back to radians and subtract from original
        return Vector4F_NegateMultiplyAdd(r, vFromRevolutions, v);
    }

    inline SimdVector4F anemone_vectorcall Vector4F_UnwindDegrees(SimdVector4F v)
    {
        float32x4_t const vToRevolutions = vdupq_n_f32(Private::Factor_DegreesToRevolutions);
        float32x4_t const vFromRevolutions = vdupq_n_f32(Private::Factor_RevolutionsToDegrees);

        // Convert degrees to full revolutions
        float32x4_t r = vmulq_f32(v, vToRevolutions);
        r = Vector4F_Round(r);
        // Convert full revolutions back to degrees and subtract from original
        return Vector4F_NegateMultiplyAdd(r, vFromRevolutions, v);
    }

    inline SimdVector4F anemone_vectorcall Vector4F_AngleDifferenceRadians(SimdVector4F a, SimdVector4F b)
    {
        return Vector4F_UnwindRadians(vsubq_f32(a, b));
    }

    inline SimdVector4F anemone_vectorcall Vector4F_AngleDifferenceDegrees(SimdVector4F a, SimdVector4F b)
    {
        return Vector4F_UnwindDegrees(vsubq_f32(a, b));
    }

    inline SimdVector4F anemone_vectorcall Vector4F_RevolutionsToDegrees(SimdVector4F v)
    {
        return vmulq_f32(v, vdupq_n_f32(Private::Factor_RevolutionsToDegrees));
    }

    inline SimdVector4F anemone_vectorcall Vector4F_RevolutionsToRadians(SimdVector4F v)
    {
        return vmulq_f32(v, vdupq_n_f32(Private::Factor_RevolutionsToRadians));
    }

    inline SimdVector4F anemone_vectorcall Vector4F_DegreesToRevolutions(SimdVector4F v)
    {
        return vmulq_f32(v, vdupq_n_f32(Private::Factor_DegreesToRevolutions));
    }

    inline SimdVector4F anemone_vectorcall Vector4F_RadiansToRevolutions(SimdVector4F v)
    {
        return vmulq_f32(v, vdupq_n_f32(Private::Factor_RadiansToRevolutions));
    }

    inline SimdVector4F anemone_vectorcall Vector4F_RadiansToDegrees(SimdVector4F v)
    {
        return vmulq_f32(v, vdupq_n_f32(Private::Factor_RadiansToDegrees));
    }

    inline SimdVector4F anemone_vectorcall Vector4F_DegreesToRadians(SimdVector4F v)
    {
        return vmulq_f32(v, vdupq_n_f32(Private::Factor_DegreesToRadians));
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Dot4(SimdVector4F a, SimdVector4F b)
    {
        float32x2_t const r = NeonDot4(a, b);
        return vcombine_f32(r, r);
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Dot3(SimdVector4F a, SimdVector4F b)
    {
        float32x2_t const r = NeonDot3(a, b);
        return vcombine_f32(r, r);
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Dot2(SimdVector4F a, SimdVector4F b)
    {
        float32x2_t const r = NeonDot2(vget_low_f32(a), vget_low_f32(b));
        return vcombine_f32(r, r);
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Cross4(SimdVector4F a, SimdVector4F b, SimdVector4F c)
    {
        // Recombine vectors
        float32x2_t const a_xy = vget_low_f32(a);
        float32x2_t const a_zw = vget_high_f32(a);
        float32x2_t const a_yx = vrev64_f32(a_xy);
        float32x2_t const a_wz = vrev64_f32(a_zw);

        float32x2_t const b_xy = vget_low_f32(b);
        float32x2_t const b_zw = vget_high_f32(b);
        float32x2_t const b_yx = vrev64_f32(b_xy);
        float32x2_t const b_wz = vrev64_f32(b_zw);

        float32x2_t const c_xy = vget_low_f32(c);
        float32x2_t const c_zw = vget_high_f32(c);
        float32x2_t const c_yx = vrev64_f32(c_xy);
        float32x2_t const c_wz = vrev64_f32(c_zw);

        float32x2_t const a_xx = vdup_lane_f32(a_xy, 0);
        float32x2_t const a_yy = vdup_lane_f32(a_xy, 1);
        float32x2_t const a_zz = vdup_lane_f32(a_zw, 0);
        float32x2_t const a_ww = vdup_lane_f32(a_zw, 1);

        float32x2_t const b_yz = vext_f32(b_xy, b_zw, 1);
        float32x2_t const b_yw = vext_f32(b_xy, b_wz, 1);
        float32x2_t const b_xz = vext_f32(b_yx, b_zw, 1);
        float32x2_t const b_wy = vext_f32(b_zw, b_yx, 1);
        float32x2_t const b_wx = vext_f32(b_zw, b_xy, 1);
        float32x2_t const b_zx = vext_f32(b_wz, b_xy, 1);

        float32x2_t const c_yz = vext_f32(c_xy, c_zw, 1);
        float32x2_t const c_xz = vext_f32(c_yx, c_zw, 1);
        float32x2_t const c_wy = vext_f32(c_zw, c_yx, 1);
        float32x2_t const c_wx = vext_f32(c_zw, c_xy, 1);
        float32x2_t const c_yw = vext_f32(c_xy, c_wz, 1);
        float32x2_t const c_zx = vext_f32(c_wz, c_xy, 1);

        // t0 = (((b.zwyz * c.wzwy) - (b.wzwy * c.zwyz)) * a.yxxx)
        float32x4_t const b_zwyz = vcombine_f32(b_zw, b_yz);
        float32x4_t const c_wzwy = vcombine_f32(c_wz, c_wy);
        float32x4_t const b_wzwy = vcombine_f32(b_wz, b_wy);
        float32x4_t const c_zwyz = vcombine_f32(c_zw, c_yz);
        float32x4_t const a_yxxx = vcombine_f32(a_yx, a_xx);
        float32x4_t const t0 = vmulq_f32(vmlsq_f32(vmulq_f32(b_zwyz, c_wzwy), b_wzwy, c_zwyz), a_yxxx);

        // t1 = (((b.ywxz * c.wxwx) - (b.wxwx * c.ywxz)) * a.zzyy)
        float32x4_t const b_ywxz = vcombine_f32(b_yw, b_xz);
        float32x4_t const c_wxwx = vcombine_f32(c_wx, c_wx);
        float32x4_t const b_wxwx = vcombine_f32(b_wx, b_wx);
        float32x4_t const c_ywxz = vcombine_f32(c_yw, c_xz);
        float32x4_t const a_zzyy = vcombine_f32(a_zz, a_yy);
        float32x4_t const t1 = vmulq_f32(vmlsq_f32(vmulq_f32(b_ywxz, c_wxwx), b_wxwx, c_ywxz), a_zzyy);

        // t2 = (((b.yzxy * c.zxyx) - (b.zxyx * c.yzxy)) * a.wwwz)
        float32x4_t const b_yzxy = vcombine_f32(b_yz, b_xy);
        float32x4_t const c_zxyx = vcombine_f32(c_zx, c_yx);
        float32x4_t const b_zxyx = vcombine_f32(b_zx, b_yx);
        float32x4_t const c_yzxy = vcombine_f32(c_yz, c_xy);
        float32x4_t const a_wwwz = vcombine_f32(a_ww, a_wz);
        float32x4_t const t2 = vmulq_f32(vmlsq_f32(vmulq_f32(b_yzxy, c_zxyx), b_zxyx, c_yzxy), a_wwwz);

        // = t0 - t1 + t2
        float32x4_t const r = vaddq_f32(vsubq_f32(t0, t1), t2);
        return r;
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Cross3(SimdVector4F a, SimdVector4F b)
    {
        float32x2_t const a_xy = vget_low_f32(a);
        float32x2_t const a_zz = vdup_lane_f32(vget_high_f32(a), 0);
        float32x2_t const b_xy = vget_low_f32(b);
        float32x2_t const b_zz = vdup_lane_f32(vget_high_f32(b), 0);
        float32x2_t const a_yx = vrev64_f32(a_xy);
        float32x2_t const b_yx = vrev64_f32(b_xy);

        // = [y, x, x, y]
        float32x4_t const a_yxxy = vcombine_f32(a_yx, a_xy);
        float32x4_t const b_yxxy = vcombine_f32(b_yx, b_xy);

        // = [z, z, y, x]
        float32x4_t const a_zzyx = vcombine_f32(a_zz, a_yx);
        float32x4_t const b_zzyx = vcombine_f32(b_zz, b_yx);

        // = [
        //  a.y * b.z,
        //  a.x * b.z,
        //  a.x * b.y,
        //  a.y * b.x,
        // ]
        float32x4_t const m0 = vmulq_f32(a_yxxy, b_zzyx);

        // = [
        //  (a.y * b.z) - (a.z * b.y),
        //  (a.x * b.z) - (a.z * b.x),
        //  (a.x * b.y) - (a.y * b.x),
        //  (a.y * b.x) - (a.x * b.y),
        // ]
        float32x4_t const m1 = vmlsq_f32(m0, a_zzyx, b_yxxy);

        // flip: m2.y = m1.y * -1
        // m1: (a.x * b.z) - (a.z * b.x)
        // m2: -(a.x * b.z) + (a.z * b.x) = (a.z * b.x) - (a.x * b.z)
        uint32x4_t const sign0100 = vld1q_u32(F32x4_SignMask_nXnn.As<uint32_t>());
        uint32x4_t const m2 = veorq_u32(vreinterpretq_u32_f32(m1), sign0100);
        uint32x4_t const mask1110 = vld1q_u32(F32x4_SelectMask_XXXn.As<uint32_t>());
        uint32x4_t const m3 = vandq_u32(m2, mask1110);

        return vreinterpretq_f32_u32(m3);
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Cross2(SimdVector4F a, SimdVector4F b)
    {
        float32x2_t const negate_y{1.0f, -1.0f};

        float32x2_t const a_xy = vget_low_f32(a);
        float32x2_t const b_xy = vget_low_f32(b);
        float32x2_t const b_yx = vrev64_f32(b_xy);
        // = [(a.x * b.y), (a.y * b.x)]
        float32x2_t const r0 = vmul_f32(a_xy, b_yx);
        // = [(a.x * b.y), -(a.y * b.x)]
        float32x2_t const r1 = vmul_f32(r0, negate_y);
        // = [(a.x * b.y) - (a.y * b.x), ...]
        float32x2_t const r2 = vpadd_f32(r1, r1);
        return vcombine_f32(r2, r2);
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Wedge3(SimdVector4F a, SimdVector4F b)
    {
        // = [x,x,y,y]
        float32x2_t const a_xx = vdup_laneq_f32(a, 0);
        float32x2_t const a_yy = vdup_laneq_f32(a, 1);
        float32x2_t const b_xx = vdup_laneq_f32(b, 0);
        float32x2_t const b_yy = vdup_laneq_f32(b, 1);

        float32x4_t const a_xxyy = vcombine_f32(a_xx, a_yy);
        float32x4_t const b_xxyy = vcombine_f32(b_xx, b_yy);

        // = [z, z, z, z]
        float32x4_t const a_zzzz = vdupq_laneq_f32(a, 3);
        float32x4_t const a_yzzz = vcopyq_lane_f32(a_zzzz, 0, a_yy, 0);

        float32x4_t const b_zzzz = vdupq_laneq_f32(b, 3);
        float32x4_t const b_yzzz = vcopyq_lane_f32(b_zzzz, 0, b_yy, 0);

        return vmlsq_f32(vmulq_f32(a_xxyy, b_yzzz), a_yzzz, b_xxyy);
    }

    inline SimdVector4F anemone_vectorcall Vector4F_LengthSquared4(SimdVector4F v)
    {
        float32x2_t const r = NeonDot4(v);
        return vcombine_f32(r, r);
    }

    inline SimdVector4F anemone_vectorcall Vector4F_LengthSquared3(SimdVector4F v)
    {
        float32x2_t const r = NeonDot3(v);
        return vcombine_f32(r, r);
    }

    inline SimdVector4F anemone_vectorcall Vector4F_LengthSquared2(SimdVector4F v)
    {
        float32x2_t const r = NeonDot2(vget_low_f32(v));
        return vcombine_f32(r, r);
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Length4(SimdVector4F v)
    {
        float32x2_t const r0 = NeonDot4(v);
        float32x2_t const r1 = NeonSquareRoot2xNewtonRhapson(r0);
        return vcombine_f32(r1, r1);
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Length3(SimdVector4F v)
    {
        float32x2_t const r0 = NeonDot3(v);
        float32x2_t const r1 = NeonSquareRoot2xNewtonRhapson(r0);
        return vcombine_f32(r1, r1);
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Length2(SimdVector4F v)
    {
        float32x2_t const r0 = NeonDot2(vget_low_f32(v));
        float32x2_t const r1 = NeonSquareRoot2xNewtonRhapson(r0);
        return vcombine_f32(r1, r1);
    }

    inline SimdVector4F anemone_vectorcall Vector4F_ReciprocalLength4(SimdVector4F v)
    {
        float32x2_t const r0 = NeonDot4(v);
        float32x2_t const r1 = NeonReciprocalSquareRoot2xNewtonRhapson(r0);
        return vcombine_f32(r1, r1);
    }

    inline SimdVector4F anemone_vectorcall Vector4F_ReciprocalLength3(SimdVector4F v)
    {
        float32x2_t const r0 = NeonDot3(v);
        float32x2_t const r1 = NeonReciprocalSquareRoot2xNewtonRhapson(r0);
        return vcombine_f32(r1, r1);
    }

    inline SimdVector4F anemone_vectorcall Vector4F_ReciprocalLength2(SimdVector4F v)
    {
        float32x2_t const r0 = NeonDot2(vget_low_f32(v));
        float32x2_t const r1 = NeonReciprocalSquareRoot2xNewtonRhapson(r0);
        return vcombine_f32(r1, r1);
    }

    inline SimdVector4F anemone_vectorcall Vector4F_ReciprocalLengthEst4(SimdVector4F v)
    {
        float32x2_t const r0 = NeonDot4(v);
        float32x2_t const r1 = vrsqrte_f32(r0);
        return vcombine_f32(r1, r1);
    }

    inline SimdVector4F anemone_vectorcall Vector4F_ReciprocalLengthEst3(SimdVector4F v)
    {
        float32x2_t const r0 = NeonDot3(v);
        float32x2_t const r1 = vrsqrte_f32(r0);
        return vcombine_f32(r1, r1);
    }

    inline SimdVector4F anemone_vectorcall Vector4F_ReciprocalLengthEst2(SimdVector4F v)
    {
        float32x2_t const r0 = NeonDot2(vget_low_f32(v));
        float32x2_t const r1 = vrsqrte_f32(r0);
        return vcombine_f32(r1, r1);
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Normalize4(SimdVector4F v)
    {
        // = length
        float32x2_t const length_squared = Private::NeonDot4(v);
        // = 1 / sqrt(length)
        float32x2_t const length_rcp = Private::NeonReciprocalSquareRoot2xNewtonRhapson(length_squared);

        uint32x2_t const mask_zero = vceq_f32(length_squared, vdup_n_f32(0.0f));
        uint32x2_t const mask_inf = vceq_f32(length_squared, vld1_f32(Private::F32x4_PositiveInfinity_XXXX.As<float>()));

        // = v * length_rcp
        float32x4_t const r0 = vmulq_f32(v, vcombine_f32(length_rcp, length_rcp));
        float32x4_t const r1 = vbslq_f32(vcombine_u32(mask_zero, mask_zero), vdupq_n_f32(0.0f), r0);
        float32x4_t const r2 = vbslq_f32(vcombine_u32(mask_inf, mask_inf), vld1q_f32(Private::F32x4_PositiveQNaN_XXXX.As<float>()), r1);
        return r2;
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Normalize3(SimdVector4F v)
    {
        // = length
        float32x2_t const length_squared = NeonDot3(v);
        // = 1 / sqrt(length)
        float32x2_t const length_rcp = NeonReciprocalSquareRoot2xNewtonRhapson(length_squared);

        uint32x2_t const mask_zero = vceq_f32(length_squared, vdup_n_f32(0.0f));
        uint32x2_t const mask_inf = vceq_f32(length_squared, vld1_f32(F32x4_PositiveInfinity_XXXX.As<float>()));

        // = v * length_rcp
        float32x4_t const r0 = vmulq_f32(v, vcombine_f32(length_rcp, length_rcp));
        float32x4_t const r1 = vbslq_f32(vcombine_u32(mask_zero, mask_zero), vdupq_n_f32(0.0f), r0);
        float32x4_t const r2 = vbslq_f32(vcombine_u32(mask_inf, mask_inf), vld1q_f32(F32x4_PositiveQNaN_XXXX.As<float>()), r1);
        return r2;
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Normalize2(SimdVector4F v)
    {
        float32x2_t const v_xy = vget_low_f32(v);
        // = length^2
        float32x2_t const length_squared = NeonDot2(v_xy);
        // = 1 / sqrt(length^2)
        float32x2_t const length_rcp = NeonReciprocalSquareRoot2xNewtonRhapson(length_squared);

        uint32x2_t const mask_zero = vceq_f32(length_squared, vdup_n_f32(0.0f));
        uint32x2_t const mask_inf = vceq_f32(length_squared, vld1_f32(F32x4_PositiveInfinity_XXXX.As<float>()));

        // = v * length_rcp
        float32x2_t const r0 = vmul_f32(v_xy, length_rcp);
        float32x2_t const r1 = vbsl_f32(mask_zero, vdup_n_f32(0.0f), r0);
        float32x2_t const r2 = vbsl_f32(mask_inf, vld1_f32(F32x4_PositiveQNaN_XXXX.As<float>()), r1);

        return vcombine_f32(r2, r2);
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Reflect4(SimdVector4F incident, SimdVector4F normal)
    {
        // = 2 * dot(incident, normal)
        float32x4_t const d = Vector4F_Dot4(incident, normal);
        float32x4_t const r = vaddq_f32(d, d);

        // = incident - (r * normal)
        return vmlsq_f32(incident, r, normal);
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Reflect3(SimdVector4F incident, SimdVector4F normal)
    {
        // = 2 * dot(incident, normal)
        float32x4_t const d = Vector4F_Dot3(incident, normal);
        float32x4_t const r = vaddq_f32(d, d);

        // = incident - (r * normal)
        return vmlsq_f32(incident, r, normal);
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Reflect2(SimdVector4F incident, SimdVector4F normal)
    {
        // = 2 * dot(incident, normal)
        float32x4_t const d = Vector4F_Dot2(incident, normal);
        float32x4_t const r = vaddq_f32(d, d);

        // = incident - (r * normal)
        return vmlsq_f32(incident, r, normal);
    }

    // TODO: Assert that index is not zero
    inline SimdVector4F anemone_vectorcall Vector4F_Refract4(SimdVector4F incident, SimdVector4F normal, SimdVector4F index)
    {
        //
        // k = 1.0 - index^2 * (1.0 - dot(I, N)^2);
        // if (k < 0.0)
        //     R = T(0.0);
        // else
        //     R = index * I - (index * dot(I, N) + sqrt(k)) * N;
        //

        // = dot(I, N)
        float32x4_t const i_dot_n = Vector4F_Dot4(incident, normal);
        float32x4_t const one = vdupq_n_f32(1.0f);

        // = 1 - dot(I, N)^2
        float32x4_t const t0 = Vector4F_NegateMultiplyAdd(i_dot_n, i_dot_n, one);

        // = index * (1 - dot(I, N)^2)
        float32x4_t const t1 = vmulq_f32(t0, index);

        // = 1 - (index^2 * (1 - dot(I, N)^2))
        float32x4_t const k = Vector4F_NegateMultiplyAdd(t1, index, one);

        float32x4_t const zero = vdupq_n_f32(0.0f);
        if (Vector4F_IsLessEqual4(k, zero))
        {
            return zero;
        }

        // = sqrt(t3)
        float32x4_t const r0 = Vector4F_SquareRoot(k);

        // = (index * dot(I, N)) + sqrt(k)
        float32x4_t const r1 = Vector4F_MultiplyAdd(index, i_dot_n, r0);

        // = index * I
        float32x4_t const r2 = Vector4F_Multiply(index, incident);

        // = (index * I) - (N * (index * dot(I, N)) + sqrt(k))
        return Vector4F_NegateMultiplyAdd(normal, r1, r2);
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Refract3(SimdVector4F incident, SimdVector4F normal, SimdVector4F index)
    {
        //
        // k = 1.0 - index^2 * (1.0 - dot(I, N)^2);
        // if (k < 0.0)
        //     R = T(0.0);
        // else
        //     R = index * I - (index * dot(I, N) + sqrt(k)) * N;
        //

        // = dot(I, N)
        float32x4_t const i_dot_n = Vector4F_Dot3(incident, normal);
        float32x4_t const one = vdupq_n_f32(1.0f);

        // = 1 - dot(I, N)^2
        float32x4_t const t0 = Vector4F_NegateMultiplyAdd(i_dot_n, i_dot_n, one);

        // = index * (1 - dot(I, N)^2)
        float32x4_t const t1 = vmulq_f32(t0, index);

        // = 1 - (index^2 * (1 - dot(I, N)^2))
        float32x4_t const k = Vector4F_NegateMultiplyAdd(t1, index, one);

        float32x4_t const zero = vdupq_n_f32(0.0f);
        if (Vector4F_IsLessEqual3(k, zero))
        {
            return zero;
        }

        // = sqrt(t3)
        float32x4_t const r0 = Vector4F_SquareRoot(k);

        // = (index * dot(I, N)) + sqrt(k)
        float32x4_t const r1 = Vector4F_MultiplyAdd(index, i_dot_n, r0);

        // = index * I
        float32x4_t const r2 = Vector4F_Multiply(index, incident);

        // = (index * I) - (N * (index * dot(I, N)) + sqrt(k))
        return Vector4F_NegateMultiplyAdd(normal, r1, r2);
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Refract2(SimdVector4F incident, SimdVector4F normal, SimdVector4F index)
    {
        //
        // k = 1.0 - index^2 * (1.0 - dot(I, N)^2);
        // if (k < 0.0)
        //     R = T(0.0);
        // else
        //     R = index * I - (index * dot(I, N) + sqrt(k)) * N;
        //

        // = dot(I, N)
        float32x4_t const i_dot_n = Vector4F_Dot2(incident, normal);
        float32x4_t const one = vdupq_n_f32(1.0f);

        // = 1 - dot(I, N)^2
        float32x4_t const t0 = Vector4F_NegateMultiplyAdd(i_dot_n, i_dot_n, one);

        // = index * (1 - dot(I, N)^2)
        float32x4_t const t1 = vmulq_f32(t0, index);

        // = 1 - (index^2 * (1 - dot(I, N)^2))
        float32x4_t const k = Vector4F_NegateMultiplyAdd(t1, index, one);

        float32x4_t const zero = vdupq_n_f32(0.0f);
        if (Vector4F_IsLessEqual2(k, zero))
        {
            return zero;
        }

        // = sqrt(t3)
        float32x4_t const r0 = Vector4F_SquareRoot(k);

        // = (index * dot(I, N)) + sqrt(k)
        float32x4_t const r1 = Vector4F_MultiplyAdd(index, i_dot_n, r0);

        // = index * I
        float32x4_t const r2 = Vector4F_Multiply(index, incident);

        // = (index * I) - (N * (index * dot(I, N)) + sqrt(k))
        return Vector4F_NegateMultiplyAdd(normal, r1, r2);
    }


    inline SimdVector4F anemone_vectorcall Vector4F_Refract4(SimdVector4F incident, SimdVector4F normal, float index)
    {
        return Vector4F_Refract4(incident, normal, vdupq_n_f32(index));
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Refract3(SimdVector4F incident, SimdVector4F normal, float index)
    {
        return Vector4F_Refract3(incident, normal, vdupq_n_f32(index));
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Refract2(SimdVector4F incident, SimdVector4F normal, float index)
    {
        return Vector4F_Refract2(incident, normal, vdupq_n_f32(index));
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Transform4(SimdVector4F v, SimdMatrix4x4F m)
    {
        float32x4_t r = vmulq_laneq_f32(m.val[0], v, 0);
        r = vmlaq_laneq_f32(r, m.val[1], v, 1);
        r = vmlaq_laneq_f32(r, m.val[2], v, 2);
        r = vmlaq_laneq_f32(r, m.val[3], v, 3);
        return r;
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Transform3(SimdVector4F v, SimdMatrix4x4F m)
    {
        float32x4_t r = vmlaq_laneq_f32(m.val[3], m.val[0], v, 0);
        r = vmlaq_laneq_f32(r, m.val[1], v, 1);
        r = vmlaq_laneq_f32(r, m.val[2], v, 2);
        return r;
    }

    inline SimdVector4F anemone_vectorcall Vector4F_Transform2(SimdVector4F v, SimdMatrix4x4F m)
    {
        float32x4_t r = vmlaq_laneq_f32(m.val[3], m.val[1], v, 1);
        r = vmlaq_laneq_f32(r, m.val[0], v, 0);
        return r;
    }

    inline SimdMask4F anemone_vectorcall Vector4F_CompareEqual(SimdVector4F a, SimdVector4F b)
    {
        return vceqq_f32(a, b);
    }

    inline bool anemone_vectorcall Vector4F_IsEqual4(SimdVector4F a, SimdVector4F b)
    {
        return NeonExtractMask4(Vector4F_CompareEqual(a, b)) == 0b1111;
    }

    inline bool anemone_vectorcall Vector4F_IsEqual3(SimdVector4F a, SimdVector4F b)
    {
        return (NeonExtractMask4(Vector4F_CompareEqual(a, b)) & 0b0111) == 0b0111;
    }

    inline bool anemone_vectorcall Vector4F_IsEqual2(SimdVector4F a, SimdVector4F b)
    {
        return (NeonExtractMask4(Vector4F_CompareEqual(a, b)) & 0b0011) == 0b0011;
    }

    inline SimdMask4F anemone_vectorcall Vector4F_CompareNotEqual(SimdVector4F a, SimdVector4F b)
    {
        return vmvnq_u32(vceqq_f32(a, b));
    }

    inline bool anemone_vectorcall Vector4F_IsNotEqual4(SimdVector4F a, SimdVector4F b)
    {
        return NeonExtractMask4(Vector4F_CompareNotEqual(a, b)) == 0b1111;
    }

    inline bool anemone_vectorcall Vector4F_IsNotEqual3(SimdVector4F a, SimdVector4F b)
    {
        return (NeonExtractMask4(Vector4F_CompareNotEqual(a, b)) & 0b0111) == 0b0111;
    }

    inline bool anemone_vectorcall Vector4F_IsNotEqual2(SimdVector4F a, SimdVector4F b)
    {
        return (NeonExtractMask4(Vector4F_CompareNotEqual(a, b)) & 0b0011) == 0b0011;
    }

    inline SimdMask4F anemone_vectorcall Vector4F_CompareLessThan(SimdVector4F a, SimdVector4F b)
    {
        return vcltq_f32(a, b);
    }

    inline bool anemone_vectorcall Vector4F_IsLessThan4(SimdVector4F a, SimdVector4F b)
    {
        return NeonExtractMask4(Vector4F_CompareLessThan(a, b)) == 0b1111;
    }

    inline bool anemone_vectorcall Vector4F_IsLessThan3(SimdVector4F a, SimdVector4F b)
    {
        return (NeonExtractMask4(Vector4F_CompareLessThan(a, b)) & 0b0111) == 0b0111;
    }

    inline bool anemone_vectorcall Vector4F_IsLessThan2(SimdVector4F a, SimdVector4F b)
    {
        return (NeonExtractMask4(Vector4F_CompareLessThan(a, b)) & 0b0011) == 0b0011;
    }

    inline SimdMask4F anemone_vectorcall Vector4F_CompareLessEqual(SimdVector4F a, SimdVector4F b)
    {
        return vcleq_f32(a, b);
    }

    inline bool anemone_vectorcall Vector4F_IsLessEqual4(SimdVector4F a, SimdVector4F b)
    {
        return NeonExtractMask4(Vector4F_CompareLessEqual(a, b)) == 0b1111;
    }

    inline bool anemone_vectorcall Vector4F_IsLessEqual3(SimdVector4F a, SimdVector4F b)
    {
        return (NeonExtractMask4(Vector4F_CompareLessEqual(a, b)) & 0b0111) == 0b0111;
    }

    inline bool anemone_vectorcall Vector4F_IsLessEqual2(SimdVector4F a, SimdVector4F b)
    {
        return (NeonExtractMask4(Vector4F_CompareLessEqual(a, b)) & 0b0011) == 0b0011;
    }

    inline SimdMask4F anemone_vectorcall Vector4F_CompareGreaterThan(SimdVector4F a, SimdVector4F b)
    {
        return vcgtq_f32(a, b);
    }

    inline bool anemone_vectorcall Vector4F_IsGreaterThan4(SimdVector4F a, SimdVector4F b)
    {
        return NeonExtractMask4(Vector4F_CompareGreaterThan(a, b)) == 0b1111;
    }

    inline bool anemone_vectorcall Vector4F_IsGreaterThan3(SimdVector4F a, SimdVector4F b)
    {
        return (NeonExtractMask4(Vector4F_CompareGreaterThan(a, b)) & 0b0111) == 0b0111;
    }

    inline bool anemone_vectorcall Vector4F_IsGreaterThan2(SimdVector4F a, SimdVector4F b)
    {
        return (NeonExtractMask4(Vector4F_CompareGreaterThan(a, b)) & 0b0011) == 0b0011;
    }

    inline SimdMask4F anemone_vectorcall Vector4F_CompareGreaterEqual(SimdVector4F a, SimdVector4F b)
    {
        return vcgeq_f32(a, b);
    }

    inline bool anemone_vectorcall Vector4F_IsGreaterEqual4(SimdVector4F a, SimdVector4F b)
    {
        return NeonExtractMask4(Vector4F_CompareGreaterEqual(a, b)) == 0b1111;
    }

    inline bool anemone_vectorcall Vector4F_IsGreaterEqual3(SimdVector4F a, SimdVector4F b)
    {
        return (NeonExtractMask4(Vector4F_CompareGreaterEqual(a, b)) & 0b0111) == 0b0111;
    }

    inline bool anemone_vectorcall Vector4F_IsGreaterEqual2(SimdVector4F a, SimdVector4F b)
    {
        return (NeonExtractMask4(Vector4F_CompareGreaterEqual(a, b)) & 0b0011) == 0b0011;
    }

    anemone_noinline inline SimdMask4F anemone_vectorcall Vector4F_CompareNaN(SimdVector4F v)
    {
        uint32x4_t const mask = vceqq_f32(v, v);
        return vmvnq_u32(mask);
    }

    inline bool anemone_vectorcall Vector4F_IsNaN4(SimdVector4F v)
    {
        return NeonExtractMask4(Vector4F_CompareNaN(v)) == 0b1111;
    }

    inline bool anemone_vectorcall Vector4F_IsNaN3(SimdVector4F v)
    {
        return (NeonExtractMask4(Vector4F_CompareNaN(v)) & 0b0111) == 0b0111;
    }

    inline bool anemone_vectorcall Vector4F_IsNaN2(SimdVector4F v)
    {
        float32x2_t const v_xy = vget_low_f32(v);
        uint32x2_t const mask = vmvn_u32(vceq_f32(v_xy, v_xy));

        return (NeonExtractMask2(mask) & 0b0011) == 0b0011;
    }

    inline SimdMask4F anemone_vectorcall Vector4F_CompareInfinite(SimdVector4F v)
    {
        float32x4_t const vAbs = vabsq_f32(v);
        return vceqq_f32(vAbs, vld1q_f32(F32x4_PositiveInfinity_XXXX.As<float>()));
    }

    inline bool anemone_vectorcall Vector4F_IsInfinite4(SimdVector4F v)
    {
        return NeonExtractMask4(Vector4F_CompareInfinite(v)) == 0b1111;
    }

    inline bool anemone_vectorcall Vector4F_IsInfinite3(SimdVector4F v)
    {
        return (NeonExtractMask4(Vector4F_CompareInfinite(v)) & 0b0111) == 0b0111;
    }

    inline bool anemone_vectorcall Vector4F_IsInfinite2(SimdVector4F v)
    {
        return (NeonExtractMask4(Vector4F_CompareInfinite(v)) & 0b0011) == 0b0011;
    }

    inline SimdMask4F anemone_vectorcall Vector4F_CompareInRange(SimdVector4F v, SimdVector4F lower, SimdVector4F upper)
    {
        uint32x4_t const maskLower = vcleq_f32(lower, v);
        uint32x4_t const maskUpper = vcleq_f32(v, upper);
        return vandq_u32(maskLower, maskUpper);
    }

    inline bool anemone_vectorcall Vector4F_InRange4(SimdVector4F v, SimdVector4F lower, SimdVector4F upper)
    {
        return NeonExtractMask4(Vector4F_CompareInRange(v, lower, upper)) == 0b1111;
    }

    inline bool anemone_vectorcall Vector4F_InRange3(SimdVector4F v, SimdVector4F lower, SimdVector4F upper)
    {
        return (NeonExtractMask4(Vector4F_CompareInRange(v, lower, upper)) & 0b0111) == 0b0111;
    }

    inline bool anemone_vectorcall Vector4F_InRange2(SimdVector4F v, SimdVector4F lower, SimdVector4F upper)
    {
        return (NeonExtractMask4(Vector4F_CompareInRange(v, lower, upper)) & 0b0011) == 0b0011;
    }

    inline SimdMask4F anemone_vectorcall Vector4F_CompareNearZero(SimdVector4F v, SimdVector4F tolerance)
    {
        // = abs(v) <= bounds
        float32x4_t const abs_value = vabsq_f32(v);
        return vcleq_f32(abs_value, tolerance);
    }

    inline SimdMask4F anemone_vectorcall Vector4F_CompareNearZero(SimdVector4F v, float tolerance)
    {
        return Vector4F_CompareNearZero(v, vdupq_n_f32(tolerance));
    }

    inline SimdMask4F anemone_vectorcall Vector4F_CompareNearZero(SimdVector4F v)
    {
        return Vector4F_CompareNearZero(v, vdupq_n_f32(std::numeric_limits<float>::epsilon()));
    }

    inline SimdMask4F anemone_vectorcall Vector4F_CompareNearZeroPrecise(SimdVector4F v)
    {
        return NeonCompareNearZeroPrecise(vreinterpretq_s32_f32(v));
    }

    inline SimdMask4F anemone_vectorcall Vector4F_CompareNearEqual(SimdVector4F a, SimdVector4F b, SimdVector4F tolerance)
    {
        // = abs(a - b) <= tolerance
        float32x4_t const abs_diff = vabsq_f32(vsubq_f32(a, b));
        return vcleq_f32(abs_diff, tolerance);
    }

    inline SimdMask4F anemone_vectorcall Vector4F_CompareNearEqual(SimdVector4F a, SimdVector4F b, float tolerance)
    {
        return Vector4F_CompareNearEqual(a, b, vdupq_n_f32(tolerance));
    }

    inline SimdMask4F anemone_vectorcall Vector4F_CompareNearEqual(SimdVector4F a, SimdVector4F b)
    {
        return Vector4F_CompareNearEqual(a, b, vdupq_n_f32(std::numeric_limits<float>::epsilon()));
    }

    inline SimdMask4F anemone_vectorcall Vector4F_CompareNearEqualPrecise(SimdVector4F a, SimdVector4F b)
    {
        // = abs(a - b) < tolerance
        uint32x4_t const maskWithinEpsilon = Vector4F_CompareNearEqual(a, b);

        // = bit_cast<int32>(...)
        int32x4_t const ulpA = vreinterpretq_s32_f32(a);
        int32x4_t const ulpB = vreinterpretq_s32_f32(b);

        // = (ulpA >> 31) == (ulpB >> 31)
        uint32x4_t const maskSameSignUlp = NeonCompareSign(ulpA, ulpB);

        // = ulp(a - b) < ulpTolerance
        uint32x4_t const maskWithinUlp = NeonCompareNearZeroPrecise(vsubq_s32(ulpA, ulpB));

        return vorrq_u32(maskWithinEpsilon, vandq_u32(maskSameSignUlp, maskWithinUlp));
    }
}

// Mask functions
namespace Anemone::Numerics::Private
{
    inline SimdMask4F anemone_vectorcall Mask4F_Create(bool x, bool y, bool z, bool w)
    {
        return uint32x4_t{
            x ? NeonMaskComponentUInt32 : 0,
            y ? NeonMaskComponentUInt32 : 0,
            z ? NeonMaskComponentUInt32 : 0,
            w ? NeonMaskComponentUInt32 : 0,
        };
    }

    inline SimdMask4F anemone_vectorcall Mask4F_Create(bool x, bool y, bool z)
    {
        return uint32x4_t{
            x ? NeonMaskComponentUInt32 : 0,
            y ? NeonMaskComponentUInt32 : 0,
            z ? NeonMaskComponentUInt32 : 0,
            0,
        };
    }

    inline SimdMask4F anemone_vectorcall Mask4F_Create(bool x, bool y)
    {
        return uint32x4_t{
            x ? NeonMaskComponentUInt32 : 0,
            y ? NeonMaskComponentUInt32 : 0,
            0,
            0,
        };
    }

    inline SimdMask4F anemone_vectorcall Mask4F_Replicate(bool value)
    {
        return vdupq_n_u32(value ? NeonMaskComponentUInt32 : 0);
    }

    template <size_t N>
    inline uint32_t anemone_vectorcall Mask4F_Extract(SimdMask4F v)
        requires(N < 4)
    {
        return vgetq_lane_u32(v, N);
    }

    template <size_t N>
    inline SimdMask4F anemone_vectorcall Mask4F_Insert(SimdMask4F v, uint32_t b)
        requires(N < 4)
    {
        return vsetq_lane_u32(b, v, N);
    }

    inline SimdMask4F anemone_vectorcall Mask4F_True()
    {
        return vdupq_n_u32(NeonMaskComponentUInt32);
    }

    inline SimdMask4F anemone_vectorcall Mask4F_False()
    {
        return vdupq_n_u32(0);
    }

    inline SimdMask4F anemone_vectorcall Mask4F_CompareEqual(SimdMask4F a, SimdMask4F b)
    {
        return vceqq_u32(a, b);
    }

    inline SimdMask4F anemone_vectorcall Mask4F_CompareNotEqual(SimdMask4F a, SimdMask4F b)
    {
        return vmvnq_u32(vceqq_u32(a, b));
    }

    inline SimdMask4F anemone_vectorcall Mask4F_And(SimdMask4F a, SimdMask4F b)
    {
        return vandq_u32(a, b);
    }

    inline SimdMask4F anemone_vectorcall Mask4F_AndNot(SimdMask4F a, SimdMask4F b)
    {
        return vbicq_u32(a, b);
    }

    inline SimdMask4F anemone_vectorcall Mask4F_Or(SimdMask4F a, SimdMask4F b)
    {
        return vorrq_u32(a, b);
    }

    inline SimdMask4F anemone_vectorcall Mask4F_Xor(SimdMask4F a, SimdMask4F b)
    {
        return veorq_u32(a, b);
    }

    inline SimdMask4F anemone_vectorcall Mask4F_Not(SimdMask4F mask)
    {
        return vmvnq_u32(mask);
    }

    inline SimdMask4F anemone_vectorcall Mask4F_Select(SimdMask4F mask, SimdMask4F whenFalse, SimdMask4F whenTrue)
    {
        return vbslq_u32(mask, whenTrue, whenFalse);
    }

    template <bool X, bool Y, bool Z, bool W>
    inline SimdMask4F anemone_vectorcall Mask4F_Select(SimdMask4F whenFalse, SimdMask4F whenTrue)
    {
        static constexpr uint32x4_t mask{
            X ? NeonMaskComponentUInt32 : 0,
            Y ? NeonMaskComponentUInt32 : 0,
            Z ? NeonMaskComponentUInt32 : 0,
            W ? NeonMaskComponentUInt32 : 0,
        };

        return vbslq_u32(mask, whenTrue, whenFalse);
    }

    inline bool anemone_vectorcall Mask4F_All4(SimdMask4F mask)
    {
        return NeonExtractMask4(mask) == 0b1111;
    }

    inline bool anemone_vectorcall Mask4F_All3(SimdMask4F mask)
    {
        return (NeonExtractMask4(mask) & 0b0111) == 0b0111;
    }

    inline bool anemone_vectorcall Mask4F_All2(SimdMask4F mask)
    {
        return (NeonExtractMask4(mask) & 0b0011) == 0b0011;
    }

    inline bool anemone_vectorcall Mask4F_Any4(SimdMask4F mask)
    {
        return NeonExtractMask4(mask) != 0;
    }

    inline bool anemone_vectorcall Mask4F_Any3(SimdMask4F mask)
    {
        return (NeonExtractMask4(mask) & 0b0111) != 0;
    }

    inline bool anemone_vectorcall Mask4F_Any2(SimdMask4F mask)
    {
        return (NeonExtractMask4(mask) & 0b0011) != 0;
    }

    inline bool anemone_vectorcall Mask4F_None4(SimdMask4F mask)
    {
        return NeonExtractMask4(mask) == 0;
    }

    inline bool anemone_vectorcall Mask4F_None3(SimdMask4F mask)
    {
        return (NeonExtractMask4(mask) & 0b0111) == 0;
    }

    inline bool anemone_vectorcall Mask4F_None2(SimdMask4F mask)
    {
        return (NeonExtractMask4(mask) & 0b0011) == 0;
    }
}

// Quaternion functions
namespace Anemone::Numerics::Private
{
    inline SimdMask4F anemone_vectorcall QuaternionF_CompareEqual(SimdVector4F a, SimdVector4F b)
    {
        return Vector4F_CompareEqual(a, b);
    }

    inline bool anemone_vectorcall QuaternionF_IsEqual(SimdVector4F a, SimdVector4F b)
    {
        return Vector4F_IsEqual4(a, b);
    }

    inline SimdMask4F anemone_vectorcall QuaternionF_CompareNotEqual(SimdVector4F a, SimdVector4F b)
    {
        return Vector4F_CompareNotEqual(a, b);
    }

    inline bool anemone_vectorcall QuaternionF_IsNotEqual(SimdVector4F a, SimdVector4F b)
    {
        return Vector4F_IsNotEqual4(a, b);
    }

    SimdMask4F anemone_vectorcall QuaternionF_CompareNaN(SimdVector4F q);
    bool anemone_vectorcall QuaternionF_IsNaN(SimdVector4F q);

    SimdMask4F anemone_vectorcall QuaternionF_CompareInfinite(SimdVector4F q);
    bool anemone_vectorcall QuaternionF_IsInfinite(SimdVector4F q);

    inline SimdMask4F anemone_vectorcall QuaternionF_CompareIdentity(SimdVector4F q)
    {
        return Vector4F_CompareEqual(q, QuaternionF_Identity());
    }

    inline bool anemone_vectorcall QuaternionF_IsIdentity(SimdVector4F q)
    {
        return Vector4F_IsEqual4(q, QuaternionF_Identity());
    }

    inline SimdVector4F anemone_vectorcall QuaternionF_Identity()
    {
        return vld1q_f32(F32x4_PositiveUnitW.As<float>());
    }

    inline SimdVector4F anemone_vectorcall QuaternionF_FromNormalAngle(SimdVector4F normal, float angle)
    {
        auto [fSin, fCos] = SinCos(0.5f * angle);

        // = [fSin, fSin, fSin, ...]
        float32x4_t const vSin = vdupq_n_f32(fSin);
        // = [fSin * n.x, fSin * n.y, fSin * n.z, ...]
        float32x4_t r = vmulq_f32(normal, vSin);
        // = [fSin * n.x, fSin * n.y, fSin * n.z, fCos]
        r = vsetq_lane_f32(fCos, r, 3);
        return r;
    }

    inline SimdVector4F anemone_vectorcall QuaternionF_FromAxisAngle(SimdVector4F axis, float angle)
    {
        // assert (axis is not zero)
        // assert (axis is not infinite)

        SimdVector4F const normal = Vector4F_Normalize3(axis);
        return QuaternionF_FromNormalAngle(normal, angle);
    }

    inline SimdVector4F anemone_vectorcall QuaternionF_FromEulerAngles(SimdVector4F pitchYawRoll)
    {
        float32x4_t vSin;
        float32x4_t vCos;
        Vector4F_SinCos(vSin, vCos, vmulq_n_f32(pitchYawRoll, 0.5f));

        float32x4_t const cp_cp_cp_cp = vdupq_laneq_f32(vCos, 0);
        float32x4_t const cy_cy_cy_cy = vdupq_laneq_f32(vCos, 1);
        float32x4_t const cr_cr_cr_cr = vdupq_laneq_f32(vCos, 2);

        float32x4_t const sp_sp_sp_sp = vdupq_laneq_f32(vSin, 0);
        float32x4_t const sy_sy_sy_sy = vdupq_laneq_f32(vSin, 1);
        float32x4_t const sr_sr_sr_sr = vdupq_laneq_f32(vSin, 2);

        // cr * sp * cy + sr * sy * negate_yz * cp
        // cr * cp * sy + sr * cy * negate_yz * sp
        // sr * cp * cy + cr * sy * negate_yz * sp
        // cr * cp * cy + sr * sy * negate_yz * sp
        //
        // ( r0  )
        //                          (    r1      )
        //                ( r2  )
        //                (          r3          )
        // (              r4                     )

        float32x4_t const cr_cr_sr_cr = vcopyq_laneq_f32(cr_cr_cr_cr, 2, sr_sr_sr_sr, 2);
        float32x4_t const sp_cp_cp_cp = vcopyq_laneq_f32(cp_cp_cp_cp, 0, sp_sp_sp_sp, 0);
        float32x4_t const r0 = vmulq_f32(cr_cr_sr_cr, sp_cp_cp_cp);

        float32x4_t const negate_yz{1.0f, -1.0f, -1.0f, 1.0f};
        float32x4_t const cp_sp_sp_sp = vcopyq_laneq_f32(sp_sp_sp_sp, 0, cp_cp_cp_cp, 0);
        float32x4_t const r1 = vmulq_f32(cp_sp_sp_sp, negate_yz);

        float32x4_t const sr_sr_cr_sr = vcopyq_laneq_f32(sr_sr_sr_sr, 2, cr_cr_cr_cr, 2);
        float32x4_t const sy_cy_sy_sy = vcopyq_laneq_f32(sy_sy_sy_sy, 1, cy_cy_cy_cy, 1);
        float32x4_t const r2 = vmulq_f32(sr_sr_cr_sr, sy_cy_sy_sy);

        float32x4_t const r3 = vmulq_f32(r1, r2);

        float32x4_t const cy_sy_cy_cy = vcopyq_laneq_f32(cy_cy_cy_cy, 1, sy_sy_sy_sy, 1);
        float32x4_t const r4 = vmlaq_f32(r3, cy_sy_cy_cy, r0);
        return r4;
    }

    inline SimdVector4F anemone_vectorcall QuaternionF_FromEulerAngles(float pitch, float yaw, float roll)
    {
        return QuaternionF_FromEulerAngles(float32x4_t{pitch, yaw, roll, 0.0f});
    }

    inline SimdVector4F anemone_vectorcall QuaternionF_Rotate3(SimdVector4F q, SimdVector4F v)
    {
        // = [v.xyz, 0]
        float32x4_t const v_xyz = vsetq_lane_f32(0.0f, v, 3);
        float32x4_t const q_conj = QuaternionF_Conjugate(q);

        // = q' * v.xyz * q
        return QuaternionF_Multiply(QuaternionF_Multiply(q_conj, v_xyz), q);
    }

    inline SimdVector4F anemone_vectorcall QuaternionF_InverseRotate3(SimdVector4F q, SimdVector4F v)
    {
        // = [v.xyz, 0]
        float32x4_t const v_xyz = vsetq_lane_f32(0.0f, v, 3);
        float32x4_t const q_conj = QuaternionF_Conjugate(q);

        // = q * v.xyz * q'
        return QuaternionF_Multiply(QuaternionF_Multiply(q, v_xyz), q_conj);
    }

    inline SimdVector4F anemone_vectorcall QuaternionF_Length(SimdVector4F q)
    {
        return Vector4F_Length4(q);
    }

    inline SimdVector4F anemone_vectorcall QuaternionF_LengthSquared(SimdVector4F q)
    {
        return Vector4F_LengthSquared4(q);
    }

    inline SimdVector4F anemone_vectorcall QuaternionF_Normalize(SimdVector4F q)
    {
        return Vector4F_Normalize4(q);
    }

    inline SimdVector4F anemone_vectorcall QuaternionF_Dot(SimdVector4F a, SimdVector4F b)
    {
        return Vector4F_Dot4(a, b);
    }

    inline SimdVector4F anemone_vectorcall QuaternionF_Multiply(SimdVector4F a, SimdVector4F b)
    {
        float32x4_t const a_xyzw = a;
        float32x4_t const b_xyzw = b;

        float32x4_t const negate_yw{1.0f, -1.0f, 1.0f, -1.0f};
        float32x4_t const negate_zw{1.0f, 1.0f, -1.0f, -1.0f};
        float32x4_t const negate_xw{-1.0f, 1.0f, 1.0f, -1.0f};

        float32x2_t const a_xy = vget_low_f32(a_xyzw);
        float32x2_t const a_zw = vget_high_f32(a_xyzw);
        float32x2_t const a_yx = vrev64_f32(a_xy);
        float32x2_t const a_wz = vrev64_f32(a_zw);

        float32x4_t const a_wzyx = vcombine_f32(a_wz, a_yx);
        float32x4_t const a_zwxy = vcombine_f32(a_zw, a_xy);
        float32x4_t const a_yxwz = vcombine_f32(a_yx, a_wz);

        // (bw * ax) + negate_yw * (bx * aw) + negate_zw * (by * az) + negate_xw * (bz * ay)
        // (bw * ay) + negate_yw * (bx * az) + negate_zw * (by * aw) + negate_xw * (bz * ax)
        // (bw * az) + negate_yw * (bx * ay) + negate_zw * (by * ax) + negate_xw * (bz * aw)
        // (bw * aw) + negate_yw * (bx * ax) + negate_zw * (by * ay) + negate_xw * (bz * az)
        //
        // (  t0   )   negate_yw   (  t1   )
        // (              r0               )
        // (              r0               )   negate_zw   (  t2   )
        // (              r0               )   (       r1          )
        // (              r0               )   (       r1          )   negate_xw   (  t3   )
        // (              r0               )   (                  r2                       )
        // (                                    r2                                         )

        float32x4_t const t0 = vmulq_laneq_f32(a_xyzw, b_xyzw, 3);
        float32x4_t const t1 = vmulq_laneq_f32(a_wzyx, b_xyzw, 0);
        float32x4_t const r0 = vmlaq_f32(t0, negate_yw, t1);

        float32x4_t const t2 = vmulq_laneq_f32(a_zwxy, b_xyzw, 1);
        float32x4_t const r1 = vmulq_f32(negate_zw, t2);
        float32x4_t const t3 = vmulq_laneq_f32(a_yxwz, b_xyzw, 2);
        float32x4_t const r2 = vmlaq_f32(r1, negate_xw, t3);
        float32x4_t const r3 = vaddq_f32(r0, r2);

        return r3;
    }

    inline SimdVector4F anemone_vectorcall QuaternionF_Conjugate(SimdVector4F q)
    {
        float32x4_t const negate_xyz{-1.0f, -1.0f, -1.0f, 1.0f};
        return vmulq_f32(q, negate_xyz);
    }

    inline SimdVector4F anemone_vectorcall QuaternionF_Inverse(SimdVector4F q)
    {
        float32x4_t const lengthSquared = Vector4F_LengthSquared4(q);
        float32x4_t const conj = QuaternionF_Conjugate(q);
        uint32x4_t const mask = vcleq_f32(lengthSquared, vld1q_f32(F32x4_Epsilon_XXXX.As<float>()));
        float32x4_t const result = vdivq_f32(conj, lengthSquared);
        return vreinterpretq_f32_u32(vandq_u32(vreinterpretq_u32_f32(result), mask));
    }

    inline void anemone_vectorcall QuaternionF_ToAxisAngle(SimdVector4F& axis, float& angle, SimdVector4F q)
    {
        // Axis part
        axis = q;

        // Angle part
        angle = 2.0f * Acos<float>(vgetq_lane_f32(q, 3));
    }

    inline SimdVector4F anemone_vectorcall QuaternionF_Nlerp(SimdVector4F from, SimdVector4F to, SimdVector4F t)
    {
        SimdVector4F const r = Vector4F_PreciseLerp(from, to, t);
        return Vector4F_Normalize4(r);
    }

    inline SimdVector4F anemone_vectorcall QuaternionF_Nlerp(SimdVector4F from, SimdVector4F to, float t)
    {
        return QuaternionF_Nlerp(from, to, vdupq_n_f32(t));
    }

    inline SimdVector4F anemone_vectorcall QuaternionF_Slerp(SimdVector4F from, SimdVector4F to, SimdVector4F t)
    {
        // = from * sin((1 - t) * theta) / sin(theta) + to * sin(t * theta) / sin(theta)
        float32x4_t const positive_one = vld1q_f32(F32x4_PositiveOne.As<float>());
        float32x4_t const negative_one = vld1q_f32(F32x4_NegativeOne.As<float>());
        float32x4_t const zero = vdupq_n_f32(0.0f);

        // cos(theta) = dot(from, to)
        float32x4_t cos_theta = Vector4F_Dot4(from, to);
        // = [cos(theta) < 0].xxxx
        uint32x4_t const mask = vcltq_f32(cos_theta, zero);
        // = [cos(theta) < 0 ? -1 : 1]
        float32x4_t const sign = vbslq_f32(mask, positive_one, negative_one); //_mm_blendv_ps(positive_one, negative_one, mask);, BUGGGGGGGGGGGGGGGGG

        // update sign of cos(theta)
        cos_theta = vmulq_f32(cos_theta, sign);

        // sin(theta) = sqrt(1 - cos(theta)^2)
        float32x4_t sin_theta = vmulq_f32(cos_theta, cos_theta);
        sin_theta = vsubq_f32(positive_one, sin_theta);
        sin_theta = vsqrtq_f32(sin_theta);

        // theta = atan2(sin(theta), cos(theta))
        float32x4_t const theta = Vector4F_Atan2(sin_theta, cos_theta);

        float32x4_t const negate_x = vld1q_f32(F32x4_Negate_Xnnn.As<float>());

        // = [-t, t, t, t]
        float32x4_t coefficient_linear = vmulq_f32(t, negate_x);
        // = [1-t, t, t, t]
        coefficient_linear = vaddq_f32(coefficient_linear, vld1q_f32(F32x4_PositiveUnitX.As<float>()));

        // = [
        //     sin((1 - t) * theta) / sin(theta),
        //     sin(t * theta) / sin(theta),
        // ]
        float32x4_t coefficient_spherical = vmulq_f32(coefficient_linear, theta);
        coefficient_spherical = Vector4F_Sin(coefficient_spherical);
        coefficient_spherical = vdivq_f32(coefficient_spherical, sin_theta);

        // = [1 - eps].xxxx
        float32x4_t const one_minus_epsilon = vdupq_n_f32(1.0f - std::numeric_limits<float>::epsilon());
        // = [cos(theta) < (1 - eps)].xxxx
        uint32x4_t const close_to_one = vcgeq_f32(cos_theta, one_minus_epsilon);

        // = close_to_one ? linear : spherical
        float32x4_t const coefficient = vbslq_f32(close_to_one, coefficient_linear, coefficient_spherical);

        // interpolate
        float32x4_t r = vmulq_laneq_f32(to, coefficient, 1);
        r = vmlaq_laneq_f32(r, from, coefficient, 0);
        return r;
    }

    inline SimdVector4F anemone_vectorcall QuaternionF_Slerp(SimdVector4F from, SimdVector4F to, float t)
    {
        return QuaternionF_Slerp(from, to, vdupq_n_f32(t));
    }
}

// Matrix functions
namespace Anemone::Numerics::Private
{
    inline SimdMatrix4x4F anemone_vectorcall Matrix4x4F_Identity()
    {
        return SimdMatrix4x4F{
            vld1q_f32(F32x4_PositiveUnitX.As<float>()),
            vld1q_f32(F32x4_PositiveUnitY.As<float>()),
            vld1q_f32(F32x4_PositiveUnitZ.As<float>()),
            vld1q_f32(F32x4_PositiveUnitW.As<float>()),
        };
    }

    inline SimdMatrix4x4F anemone_vectorcall Matrix4x4F_NaN()
    {
        float32x4_t const lane = vld1q_f32(F32x4_PositiveQNaN_XXXX.As<float>());
        return SimdMatrix4x4F{lane, lane, lane, lane};
    }

    inline SimdMatrix4x4F anemone_vectorcall Matrix4x4F_Infinity()
    {
        float32x4_t const lane = vld1q_f32(F32x4_PositiveInfinity_XXXX.As<float>());
        return SimdMatrix4x4F{lane, lane, lane, lane};
    }

    inline SimdMatrix4x4F anemone_vectorcall Matrix4x4F_CreateTranslation(float x, float y, float z)
    {
        return SimdMatrix4x4F{
            vld1q_f32(F32x4_PositiveUnitX.As<float>()),
            vld1q_f32(F32x4_PositiveUnitY.As<float>()),
            vld1q_f32(F32x4_PositiveUnitZ.As<float>()),
            float32x4_t{x, y, z, 1.0f},
        };
    }

    inline SimdMatrix4x4F anemone_vectorcall Matrix4x4F_CreateTranslation(SimdVector4F translation)
    {
        return SimdMatrix4x4F{
            vld1q_f32(F32x4_PositiveUnitX.As<float>()),
            vld1q_f32(F32x4_PositiveUnitY.As<float>()),
            vld1q_f32(F32x4_PositiveUnitZ.As<float>()),
            vsetq_lane_f32(1.0f, translation, 3),
        };
    }

    inline SimdMatrix4x4F anemone_vectorcall Matrix4x4F_CreateScale(float x, float y, float z)
    {
        float32x4_t const zero = vdupq_n_f32(0.0f);

        return SimdMatrix4x4F{
            vsetq_lane_f32(x, zero, 0),
            vsetq_lane_f32(y, zero, 1),
            vsetq_lane_f32(z, zero, 2),
            vld1q_f32(F32x4_PositiveUnitW.As<float>()),
        };
    }

    inline SimdMatrix4x4F anemone_vectorcall Matrix4x4F_CreateScale(SimdVector4F scale)
    {
        float32x4_t const zero = vdupq_n_f32(0.0f);

        return SimdMatrix4x4F{
            vcopyq_laneq_f32(zero, 0, scale, 0),
            vcopyq_laneq_f32(zero, 1, scale, 1),
            vcopyq_laneq_f32(zero, 2, scale, 2),
            vld1q_f32(F32x4_PositiveUnitW.As<float>()),
        };
    }

    inline SimdMatrix4x4F anemone_vectorcall Matrix4x4F_CreateRotationX(float angle)
    {
        auto [fSin, fCos] = SinCos<float>(angle);

        float32x4_t const zero = vdupq_n_f32(0.0f);

        // = [0, fCos, fSin, 0]
        float32x4_t r1 = vsetq_lane_f32(fCos, zero, 1);
        r1 = vsetq_lane_f32(fSin, r1, 2);

        // = [0, -fSin, fCos, 0]
        float32x4_t r2 = vsetq_lane_f32(-fSin, zero, 1);
        r2 = vsetq_lane_f32(fCos, r2, 2);

        SimdMatrix4x4F result;

        result.val[0] = vld1q_f32(F32x4_PositiveUnitX.As<float>());
        result.val[1] = r1;
        result.val[2] = r2;
        result.val[3] = vld1q_f32(F32x4_PositiveUnitW.As<float>());

        return result;
    }

    inline SimdMatrix4x4F anemone_vectorcall Matrix4x4F_CreateRotationY(float angle)
    {
        auto [fSin, fCos] = SinCos<float>(angle);

        float32x4_t const zero = vdupq_n_f32(0.0f);

        // = [fCos, 0, -fSin, 0]
        float32x4_t r0 = vsetq_lane_f32(fCos, zero, 0);
        r0 = vsetq_lane_f32(-fSin, r0, 2);

        // = [fSin, 0, fCos, 0]
        float32x4_t r2 = vsetq_lane_f32(fSin, zero, 0);
        r2 = vsetq_lane_f32(fCos, r2, 2);

        SimdMatrix4x4F result;

        result.val[0] = r0;
        result.val[1] = vld1q_f32(F32x4_PositiveUnitY.As<float>());
        result.val[2] = r2;
        result.val[3] = vld1q_f32(F32x4_PositiveUnitW.As<float>());

        return result;
    }

    anemone_noinline inline SimdMatrix4x4F anemone_vectorcall Matrix4x4F_CreateRotationZ(float angle)
    {
        auto [fSin, fCos] = SinCos<float>(angle);

        float32x4_t const zero = vdupq_n_f32(0.0f);

        // = [fCos, fSin, 0, 0]
        float32x4_t r0 = vsetq_lane_f32(fCos, zero, 0);
        r0 = vsetq_lane_f32(fSin, r0, 1);

        // = [-fSin, fCos, 0, 0]
        float32x4_t r1 = vsetq_lane_f32(-fSin, zero, 0);
        r1 = vsetq_lane_f32(fCos, r1, 1);

        SimdMatrix4x4F result;

        result.val[0] = r0;
        result.val[1] = r1;
        result.val[2] = vld1q_f32(F32x4_PositiveUnitZ.As<float>());
        result.val[3] = vld1q_f32(F32x4_PositiveUnitW.As<float>());

        return result;
    }

    anemone_noinline inline SimdMatrix4x4F anemone_vectorcall Matrix4x4F_CreateFromQuaternion(SimdVector4F q)
    {
        // = [2x, 2y, 2z, 2w]
        float32x4_t const q2x_q2y_q2z_q2w = vaddq_f32(q, q);
        // = [2xx, 2yy, 2zz, 2ww]
        float32x4_t const q2xx_q2yy_q2zz_q2ww = vmulq_f32(q2x_q2y_q2z_q2w, q);

        // = [2xx, 2yy]
        float32x2_t const q2xx_q2yy = vget_low_f32(q2xx_q2yy_q2zz_q2ww);
        // = [2zz, 2ww]
        float32x2_t const q2zz_q2ww = vget_high_f32(q2xx_q2yy_q2zz_q2ww);
        // = [2zz, 2zz]
        float32x2_t const q2zz_q2zz = vdup_lane_f32(q2zz_q2ww, 0);
        // = [2yy, 2xx]
        float32x2_t const q2yy_q2xx = vrev64_f32(q2xx_q2yy);
        // = [2zz, 2zz, 2yy, 2xx]
        float32x4_t const q2zz_q2zz_q2yy_q2xx = vcombine_f32(q2zz_q2zz, q2yy_q2xx);
        // = [2xx, 2yy, 2xx, 2yy]
        float32x4_t const q2xx_q2yy_q2xx_q2yy = vcombine_f32(q2xx_q2yy, q2xx_q2yy);

        // result[1][1] = 1.0f - q2xx - q2zz;
        // result[0][0] = 1.0f - q2yy - q2zz;
        // result[2][2] = 1.0f - q2xx - q2yy;

        float32x4_t const m22_m11_m33 = vsubq_f32(vsubq_f32(vdupq_n_f32(1.0f), q2xx_q2yy_q2xx_q2yy), q2zz_q2zz_q2yy_q2xx);

        // = [w, w, w, w]
        float32x4_t const qw_qw_qw_qw = vdupq_laneq_f32(q, 3);
        // = [2xw, 2yw, 2zw, 2ww]
        float32x4_t const q2xw_q2yw_q2zw_q2ww = vmulq_f32(q2x_q2y_q2z_q2w, qw_qw_qw_qw);

        // = [x, y]
        float32x2_t const qx_qy = vget_low_f32(q);
        // = [z, w]
        float32x2_t const qz_qw = vget_high_f32(q);
        // = [y, x]
        float32x2_t const qy_qx = vrev64_f32(qx_qy);
        // = [z, z]
        float32x2_t const qz_qz = vdup_lane_f32(qz_qw, 0);
        // = [z, z, y, x]
        float32x4_t const qz_qz_qy_qx = vcombine_f32(qz_qz, qy_qx);
        // = [2x, 2y]
        float32x2_t const q2x_q2y = vget_low_f32(q2x_q2y_q2z_q2w);
        // = [2z, 2w]
        float32x2_t const q2z_q2w = vget_high_f32(q2x_q2y_q2z_q2w);
        // = [2y, 2x]
        float32x2_t const q2y_q2x = vrev64_f32(q2x_q2y);
        // = [2y, 2x, 2x, 2y]
        float32x4_t const q2y_q2x_q2x_q2y = vcombine_f32(q2y_q2x, q2x_q2y);
        // = [2yz, 2xz, 2xy, 2xy]
        float32x4_t const q2yz_q2xz_q2xy_q2xy = vmulq_f32(q2y_q2x_q2x_q2y, qz_qz_qy_qx);

        // result[1][2] = q2yz + q2xw;
        // result[2][0] = q2xz + q2yw;
        // result[0][1] = q2xy + q2zw;
        float32x4_t const m23_m31_m12 = vaddq_f32(q2yz_q2xz_q2xy_q2xy, q2xw_q2yw_q2zw_q2ww);

        // result[2][1] = q2yz - q2xw;
        // result[0][2] = q2xz - q2yw;
        // result[1][0] = q2xy - q2zw;
        float32x4_t const m32_m13_m21 = vsubq_f32(q2yz_q2xz_q2xy_q2xy, q2xw_q2yw_q2zw_q2ww);

        // Swizzle to get the final result
        float32x2_t const m22_m11 = vget_low_f32(m22_m11_m33);
        float32x2_t const m33_xxx = vget_high_f32(m22_m11_m33);
        float32x2_t const m23_m31 = vget_low_f32(m23_m31_m12);
        float32x2_t const m12_xxx = vget_high_f32(m23_m31_m12);
        float32x2_t const m32_m13 = vget_low_f32(m32_m13_m21);
        float32x2_t const m21_xxx = vget_high_f32(m32_m13_m21);

        // = [0, 0]
        float32x2_t const zero = vdup_n_f32(0.0f);
        // = [m11, m12]
        float32x2_t const m11_m12 = vext_f32(m22_m11, m12_xxx, 1);
        // = [m13, 0]
        float32x2_t const m13_000 = vext_f32(m32_m13, zero, 1);
        // = [m21, m22]
        float32x2_t const m21_m22 = vcopy_lane_f32(m21_xxx, 1, m22_m11, 0);
        // = [m23, 0]
        float32x2_t const m23_000 = vcopy_lane_f32(m23_m31, 1, zero, 0);
        // = [m31, m32]
        float32x2_t const m31_m32 = vext_f32(m23_m31, m32_m13, 1);
        // = [m33, 0]
        float32x2_t const m33_000 = vcopy_lane_f32(m33_xxx, 1, zero, 0);

        // = [m11, m12, m13, 0]
        float32x4_t const m11_m12_m13_000 = vcombine_f32(m11_m12, m13_000);
        // = [m21, m22, m23, 0]
        float32x4_t const m21_m22_m23_000 = vcombine_f32(m21_m22, m23_000);
        // = [m31, m32, m33, 0]
        float32x4_t const m31_m32_m33_000 = vcombine_f32(m31_m32, m33_000);

        SimdMatrix4x4F result;

        result.val[0] = m11_m12_m13_000;
        result.val[1] = m21_m22_m23_000;
        result.val[2] = m31_m32_m33_000;
        result.val[3] = vld1q_f32(F32x4_PositiveUnitW.As<float>());

        return result;

        // float const qx = q[0];
        // float const qy = q[1];
        // float const qz = q[2];
        // float const qw = q[3];
        //
        // float const q2x = 2.0f * qx;
        // float const q2y = 2.0f * qy;
        // float const q2z = 2.0f * qz;
        // float const q2w = 2.0f * qw;
        //
        // float const q2yy = q2y * qy;
        // float const q2zz = q2z * qz;
        // float const q2xx = q2x * qx;
        // float const q2xz = q2x * qz;
        // float const q2yw = q2y * qw;
        // float const q2xy = q2x * qy;
        // float const q2zw = q2z * qw;
        // float const q2yz = q2y * qz;
        // float const q2xw = q2x * qw;
        //
        // SimdMatrix4x4F result;
        //
        // result[1][1] = 1.0f - q2xx - q2zz;
        // result[0][0] = 1.0f - q2yy - q2zz;
        // result[2][2] = 1.0f - q2xx - q2yy;
        //
        // result[1][2] = q2yz + q2xw;
        // result[2][0] = q2xz + q2yw;
        // result[0][1] = q2xy + q2zw;
        //
        // result[2][1] = q2yz - q2xw;
        // result[0][2] = q2xz - q2yw;
        // result[1][0] = q2xy - q2zw;
        //
        // result[0][3] = 0.0f;
        // result[1][3] = 0.0f;
        // result[2][3] = 0.0f;
        // result[3][0] = 0.0f;
        // result[3][1] = 0.0f;
        // result[3][2] = 0.0f;
        // result[3][3] = 1.0f;
        //
        // return result;
    }

    inline SimdMatrix4x4F anemone_vectorcall Matrix4x4F_CreateFromPitchYawRoll(float pitch, float yaw, float roll)
    {
        return Matrix4x4F_CreateFromPitchYawRoll(float32x4_t{pitch, yaw, roll, 0.0f});
    }

    inline SimdMatrix4x4F anemone_vectorcall Matrix4x4F_CreateFromPitchYawRoll(SimdVector4F pitchYawRoll)
    {
        float32x4_t sp_sy_sr;
        float32x4_t cp_cy_cr;
        Vector4F_SinCos(sp_sy_sr, cp_cy_cr, pitchYawRoll);

        float32x2_t const sp_sy = vget_low_f32(sp_sy_sr);
        float32x2_t const sr = vget_high_f32(sp_sy_sr);
        float32x2_t const cp_cy = vget_low_f32(cp_cy_cr);
        float32x2_t const cr = vget_high_f32(cp_cy_cr);


        // = [sy, sy]
        float32x2_t const sy_sy = vdup_lane_f32(sp_sy, 1);
        // = [cy, cy]
        float32x2_t const cy_cy = vdup_lane_f32(cp_cy, 1);
        // = [cp, cp]
        float32x2_t const cp_cp = vdup_lane_f32(cp_cy, 0);
        // = [cp, sr]
        float32x2_t const cp_sr = vcopy_lane_f32(cp_cy, 1, sr, 0);
        // = [cr, cp]
        float32x2_t const cr_cp = vcopy_lane_f32(cp_cp, 0, cr, 0);
        // = [sy, cp]
        float32x2_t const sy_cp = vext_f32(sp_sy, cp_cy, 1);
        // = [sr, cr]
        float32x2_t const sr_cr = vext_f32(cp_sr, cr, 1);
        // = [cr, sr]
        float32x2_t const cr_sr = vrev64_f32(sr_cr);

        // = [cp, cp, cr, cp]
        float32x4_t const cp_cp_cr_cp = vcombine_f32(cp_cp, cr_cp);
        // = [cp, sr, cr, cp]
        float32x4_t const cp_sr_cr_cp = vcombine_f32(cp_sr, cr_cp);
        // = [cp, cp, cp, cy]
        float32x4_t const cp_cp_cp_cy = vcombine_f32(cp_cp, cp_cy);
        // = [sy, cp, cp, cy]
        float32x4_t const sy_cp_cp_cy = vcombine_f32(sy_cp, cp_cy);
        // = [cr, sr, cr, sr]
        float32x4_t const cr_sr_cr_sr = vcombine_f32(cr_sr, cr_sr);
        // = [cy, cy, sy, sy]
        float32x4_t const cy_cy_sy_sy = vcombine_f32(cy_cy, sy_sy);
        // = [sr, cr, sr, cr]
        float32x4_t const sr_cr_sr_cr = vcombine_f32(sr_cr, sr_cr);
        // = [sy, sy, cy, cy]
        float32x4_t const sy_sy_cy_cy = vcombine_f32(sy_sy, cy_cy);
        // = [sp, sp, sp, sp]
        float32x4_t const sp_sp_sp_sp = vdupq_lane_f32(sp_sy, 0);

        // = [m32, _, _, _]
        float32x4_t const m32 = vnegq_f32(sp_sy_sr);

        // = [
        //      cp * sy
        //      sr * cp
        //      cr * cp
        //      cp * cy
        // ]
        float32x4_t const m31_m12_m22_m33 = vmulq_f32(cp_sr_cr_cp, sy_cp_cp_cy);

        // = [
        //      cr
        //      -sr
        //      -cr
        //      sr
        // ]
        float32x4_t const sign{1.0f, -1.0f, -1.0f, 1.0f};
        float32x4_t const t0 = vmulq_f32(cr_sr_cr_sr, sign);

        // = [
        //      cr * cy
        //      -sr * cy
        //      -cr * sy
        //      sr * sy
        // ]
        float32x4_t const t1 = vmulq_f32(t0, cy_cy_sy_sy);

        // = [
        //      sr * sp
        //      cr * sp
        //      sr * sp
        //      cr * sp
        // ]
        float32x4_t const t2 = vmulq_f32(sr_cr_sr_cr, sp_sp_sp_sp);

        // = [
        //      sr * sp * sy + cr * cy
        //      cr * sp * sy - sr * cy
        //      sr * sp * cy - cr * sy
        //      cr * sp * cy + sr * sy
        // ]
        float32x4_t const m11_m21_m13_m23 = vmlaq_f32(t1, sy_sy_cy_cy, t2);
        // Vector4F_MultiplyAdd(t2, sy_sy_cy_cy, t1);

        // m11_m21, m13_m23, m32_xxx, m31_m12, m22_m33

        float32x2_t const zero = vdup_n_f32(0.0f);
        float32x2_t const m11_m21 = vget_low_f32(m11_m21_m13_m23);
        float32x2_t const m13_m23 = vget_high_f32(m11_m21_m13_m23);
        float32x2_t const m31_m12 = vget_low_f32(m31_m12_m22_m33);
        float32x2_t const m22_m33 = vget_high_f32(m31_m12_m22_m33);

        // = [m11, m12, m13, 0]
        float32x2_t const m11_m12 = vcopy_lane_f32(m11_m21, 1, m31_m12, 1);
        float32x2_t const m13_000 = vset_lane_f32(0.0f, m13_m23, 1);
        float32x2_t const m21_m22 = vext_f32(m11_m21, m22_m33, 1);
        float32x2_t const m23_000 = vext_f32(m13_m23, zero, 1);
        float32x2_t const m31_m32 = vcopy_laneq_f32(m31_m12, 1, m32, 0);
        float32x2_t const m33_000 = vext_f32(m22_m33, zero, 1);

        SimdMatrix4x4F result;

        result.val[0] = vcombine_f32(m11_m12, m13_000);
        result.val[1] = vcombine_f32(m21_m22, m23_000);
        result.val[2] = vcombine_f32(m31_m32, m33_000);
        result.val[3] = vld1q_f32(F32x4_PositiveUnitW.As<float>());

        return result;
    }

    inline SimdMatrix4x4F anemone_vectorcall Matrix4x4F_CreateAffineTransform2(
        SimdVector4F scaling,
        SimdVector4F rotationOrigin,
        float rotation,
        SimdVector4F translation)
    {
        float32x2_t const ones = vdup_n_f32(1.0f);
        float32x2_t const zero = vdup_n_f32(0.0f);
        // = [scaling.x, scaling.y, 1, 1]
        float32x4_t const vScaling = vcombine_f32(vget_low_f32(scaling), ones);
        // = [rotationOrigin.x, rotationOrigin.y, 0, 0]
        float32x4_t const vRotationOrigin = vcombine_f32(vget_low_f32(rotationOrigin), zero);
        // = [translation.x, translation.y, 0, 0]
        float32x4_t const vTranslation = vcombine_f32(vget_low_f32(translation), zero);

        // 1. Apply scaling
        SimdMatrix4x4F mResult = Matrix4x4F_CreateScale(vScaling);
        // 2. Move to rotation origin
        mResult.val[3] = vsubq_f32(mResult.val[3], vRotationOrigin);
        // 3. Apply rotation
        mResult = Matrix4x4F_Multiply(mResult, Matrix4x4F_CreateRotationZ(rotation));
        // 4. Restore rotation origin
        mResult.val[3] = vaddq_f32(mResult.val[3], vRotationOrigin);
        // 5. Apply translation
        mResult.val[3] = vaddq_f32(mResult.val[3], vTranslation);
        return mResult;
    }

    inline SimdMatrix4x4F anemone_vectorcall Matrix4x4F_CreateAffineTransform3(
        SimdVector4F scaling,
        SimdVector4F rotationOrigin,
        SimdVector4F rotationQuaternion,
        SimdVector4F translation)
    {
        // = [rotationOrigin.x, rotationOrigin.y, rotationOrigin.z, 0]
        float32x4_t const vRotationOrigin = vsetq_lane_f32(0.0f, rotationOrigin, 3);
        // = [translation.x, translation.y, translation.z, 0]
        float32x4_t const vTranslation = vsetq_lane_f32(0.0f, translation, 3);

        SimdMatrix4x4F mRotation = Matrix4x4F_CreateFromQuaternion(rotationQuaternion);

        // 1. Apply scaling
        SimdMatrix4x4F mResult = Matrix4x4F_CreateScale(scaling);
        // 2. Move to rotation origin
        mResult.val[3] = vsubq_f32(mResult.val[3], vRotationOrigin);
        // 3. Apply rotation
        mResult = Matrix4x4F_Multiply(mResult, mRotation);
        // 4. Restore rotation origin
        mResult.val[3] = vaddq_f32(mResult.val[3], vRotationOrigin);
        // 5. Apply translation
        mResult.val[3] = vaddq_f32(mResult.val[3], vTranslation);
        return mResult;
    }

    inline SimdMatrix4x4F anemone_vectorcall Matrix4x4F_CreateLookAtLH(SimdVector4F eye, SimdVector4F focus, SimdVector4F up)
    {
        SimdVector4F const direction = Vector4F_Subtract(focus, eye);
        return Matrix4x4F_CreateLookToLH(eye, direction, up);
    }

    inline SimdMatrix4x4F anemone_vectorcall Matrix4x4F_CreateLookAtRH(SimdVector4F eye, SimdVector4F focus, SimdVector4F up)
    {
        SimdVector4F const direction = Vector4F_Subtract(eye, focus);
        return Matrix4x4F_CreateLookToRH(eye, direction, up);
    }

    inline SimdMatrix4x4F anemone_vectorcall Matrix4x4F_CreateLookToLH(SimdVector4F eye, SimdVector4F direction, SimdVector4F up)
    {
        SimdVector4F const r2 = Vector4F_Normalize3(direction);
        SimdVector4F const r0 = Vector4F_Normalize3(Vector4F_Cross3(up, r2));
        SimdVector4F const r1 = Vector4F_Cross3(r2, r0);
        SimdVector4F const negEye = Vector4F_Negate(eye);

        SimdVector4F const d0 = Vector4F_Dot3(r0, negEye);
        SimdVector4F const d1 = Vector4F_Dot3(r1, negEye);
        SimdVector4F const d2 = Vector4F_Dot3(r2, negEye);

        SimdMatrix4x4F result;

        result.val[0] = Vector4F_Select<true, true, true, false>(d0, r0);
        result.val[1] = Vector4F_Select<true, true, true, false>(d1, r1);
        result.val[2] = Vector4F_Select<true, true, true, false>(d2, r2);
        result.val[3] = Vector4F_PositiveUnitW();

        return result;
    }

    inline SimdMatrix4x4F anemone_vectorcall Matrix4x4F_CreateLookToRH(SimdVector4F eye, SimdVector4F direction, SimdVector4F up)
    {
        SimdVector4F const negDirection = Vector4F_Negate(direction);
        return Matrix4x4F_CreateLookToLH(eye, negDirection, up);
    }

    inline SimdMatrix4x4F anemone_vectorcall Matrix4x4F_CreatePerspectiveLH(float width, float height, float near, float far)
    {
        float const fRange = far / (far - near);
        float const fTwoNearZ = near + near;

        float32x4_t const zero = vdupq_n_f32(0.0f);
        float32x4_t const unitW = vld1q_f32(F32x4_PositiveUnitW.As<float>());

        SimdMatrix4x4F result;

        // = [fTwoNearZ / width, 0, 0, 0]
        result.val[0] = vsetq_lane_f32(fTwoNearZ / width, zero, 0);
        // = [fTwoNearZ / height, 0, 0, 0]
        result.val[1] = vsetq_lane_f32(fTwoNearZ / height, zero, 1);
        // = [fRange, 0, 0, 1]
        result.val[2] = vsetq_lane_f32(fRange, unitW, 2);
        // = [0, 0, fRange * near, 0]
        result.val[3] = vsetq_lane_f32(-fRange * near, zero, 2);

        return result;
    }

    inline SimdMatrix4x4F anemone_vectorcall Matrix4x4F_CreatePerspectiveRH(float width, float height, float near, float far)
    {
        float const fRange = far / (near - far);
        float const fTwoNearZ = near + near;

        float32x4_t const zero = vdupq_n_f32(0.0f);
        float32x4_t const unitW = vld1q_f32(F32x4_NegativeUnitW.As<float>());

        SimdMatrix4x4F result;

        // = [fTwoNearZ / width, 0, 0, 0]
        result.val[0] = vsetq_lane_f32(fTwoNearZ / width, zero, 0);
        // = [0, fTwoNearZ / height, 0, 0]
        result.val[1] = vsetq_lane_f32(fTwoNearZ / height, zero, 1);
        // = [0, 0, fRange, -1]
        result.val[2] = vsetq_lane_f32(fRange, unitW, 2);
        // = [0, 0, fRange * near, 0]
        result.val[3] = vsetq_lane_f32(fRange * near, zero, 2);

        return result;
    }

    inline SimdMatrix4x4F anemone_vectorcall Matrix4x4F_CreatePerspectiveFovLH(float fov, float aspect, float near, float far)
    {
        auto [fSin, fCos] = SinCos<float>(0.5f * fov);

        float const fRange = far / (far - near);
        float const height = fCos / fSin;
        float const width = height / aspect;

        float32x4_t const zero = vdupq_n_f32(0.0f);
        float32x4_t const unitW = vld1q_f32(F32x4_PositiveUnitW.As<float>());

        SimdMatrix4x4F result;

        // = [width, 0, 0, 0]
        result.val[0] = vsetq_lane_f32(width, zero, 0);

        // = [0, height, 0, 0]
        result.val[1] = vsetq_lane_f32(height, zero, 1);

        // = [0, 0, fRange, 1]
        result.val[2] = vsetq_lane_f32(fRange, unitW, 2);

        // = [0, 0, -fRange * near, 0]
        result.val[3] = vsetq_lane_f32(-fRange * near, zero, 2);

        return result;
    }

    inline SimdMatrix4x4F anemone_vectorcall Matrix4x4F_CreatePerspectiveFovRH(float fov, float aspect, float near, float far)
    {
        auto [fSin, fCos] = SinCos<float>(0.5f * fov);

        float const fRange = far / (near - far);
        float const height = fCos / fSin;
        float const width = height / aspect;

        float32x4_t const zero = vdupq_n_f32(0.0f);
        float32x4_t const unitW = vld1q_f32(F32x4_NegativeUnitW.As<float>());

        SimdMatrix4x4F result;

        // = [width, 0, 0, 0]
        result.val[0] = vsetq_lane_f32(width, zero, 0);

        // = [0, height, 0, 0]
        result.val[1] = vsetq_lane_f32(height, zero, 1);

        // = [0, 0, fRange, -1]
        result.val[2] = vsetq_lane_f32(fRange, unitW, 2);

        // = [0, 0, fRange * near, 0]
        result.val[3] = vsetq_lane_f32(fRange * near, zero, 2);

        return result;
    }

    inline SimdMatrix4x4F anemone_vectorcall Matrix4x4F_CreateOrthographicLH(float width, float height, float near, float far)
    {
        float const fRange = 1.0f / (far - near);

        float32x4_t const zero = vdupq_n_f32(0.0f);
        float32x4_t const unitW = vld1q_f32(F32x4_PositiveUnitW.As<float>());

        SimdMatrix4x4F result;

        // = [2 / width, 0, 0, 0]
        result.val[0] = vsetq_lane_f32(2.0f / width, zero, 0);

        // = [0, 2 / height, 0, 0]
        result.val[1] = vsetq_lane_f32(2.0f / height, zero, 1);

        // = [0, 0, fRange, 0]
        result.val[2] = vsetq_lane_f32(fRange, zero, 2);

        // = [0, 0, -fRange * near, 1]
        result.val[3] = vsetq_lane_f32(-fRange * near, unitW, 2);

        return result;
    }

    inline SimdMatrix4x4F anemone_vectorcall Matrix4x4F_CreateOrthographicRH(float width, float height, float near, float far)
    {
        float const fRange = 1.0f / (near - far);

        float32x4_t const zero = vdupq_n_f32(0.0f);
        float32x4_t const unitW = vld1q_f32(F32x4_PositiveUnitW.As<float>());

        SimdMatrix4x4F result;

        // = [2 / width, 0, 0, 0]
        result.val[0] = vsetq_lane_f32(2.0f / width, zero, 0);

        // = [0, 2 / height, 0, 0]
        result.val[1] = vsetq_lane_f32(2.0f / height, zero, 1);

        // = [0, 0, fRange, 0]
        result.val[2] = vsetq_lane_f32(fRange, zero, 2);

        // = [0, 0, fRange * near, 1]
        result.val[3] = vsetq_lane_f32(fRange * near, unitW, 2);

        return result;
    }

    // TODO: Use proper structs instead of arrays
    inline SimdMatrix4x4F anemone_vectorcall Matrix4x4F_LoadFloat4x4(float const* source)
    {
        return vld1q_f32_x4(source);
    }

    inline void anemone_vectorcall Matrix4x4F_StoreFloat4x4(float* destination, SimdMatrix4x4F source)
    {
        vst1q_f32_x4(destination, source);
    }

    inline SimdMatrix4x4F anemone_vectorcall Matrix4x4F_LoadFloat4x3(float const* source)
    {
        float32x4_t const m11_m12_m13_m21 = vld1q_f32(source + 0);
        float32x4_t const m22_m23_m31_m32 = vld1q_f32(source + 4);
        float32x4_t const m33_m41_m42_m43 = vld1q_f32(source + 8);

        float32x4_t const m21_m22_m23 = vextq_f32(m11_m12_m13_m21, m22_m23_m31_m32, 3);
        float32x4_t const m31_m32_m33 = vcombine_f32(vget_high_f32(m22_m23_m31_m32), vget_low_f32(m33_m41_m42_m43));
        float32x4_t const m41_m42_m43 = vextq_f32(m33_m41_m42_m43, m33_m41_m42_m43, 1);

        uint32x4_t const mask = vld1q_u32(F32x4_SelectMask_XXXn.As<uint32_t>());

        SimdMatrix4x4F result;

        // = [m11, m12, m13, 0]
        result.val[0] = vreinterpretq_f32_u32(vandq_u32(vreinterpretq_u32_f32(m11_m12_m13_m21), mask));
        // = [m21, m22, m23, 0]
        result.val[1] = vreinterpretq_f32_u32(vandq_u32(vreinterpretq_u32_f32(m21_m22_m23), mask));
        // = [m31, m32, m33, 0]
        result.val[2] = vreinterpretq_f32_u32(vandq_u32(vreinterpretq_u32_f32(m31_m32_m33), mask));
        // = [m41, m42, m43, 1]
        result.val[3] = vsetq_lane_f32(1.0f, m41_m42_m43, 3);

        return result;
    }

    inline void anemone_vectorcall Matrix4x4F_StoreFloat4x3(float* destination, SimdMatrix4x4F source)
    {
        uint32x4_t const mask = vld1q_u32(F32x4_SelectMask_XXXn.As<uint32_t>());

        // = [m11, m12, m13, m21]
        float32x4_t const m12_m13_m14_m21 = vextq_f32(source.val[0], source.val[1], 1);
        float32x4_t const m11_m12_m13_m21 = vbslq_f32(mask, source.val[0], m12_m13_m14_m21);
        vst1q_f32(destination + 0, m11_m12_m13_m21);

        // = [m22, m23, m31, m32]
        float32x4_t const m22_m23_m24_m21 = vextq_f32(source.val[1], source.val[1], 1);
        float32x4_t const m22_m23_m31_m32 = vcombine_f32(vget_low_f32(m22_m23_m24_m21), vget_low_f32(source.val[2]));
        vst1q_f32(destination + 4, m22_m23_m31_m32);

        // = [m33, m41, m42, m43]
        float32x4_t const m33_m33_m33_m33 = vdupq_lane_f32(vget_high_f32(source.val[2]), 0);
        float32x4_t const m33_m41_m42_m43 = vextq_f32(m33_m33_m33_m33, source.val[3], 3);
        vst1q_f32(destination + 8, m33_m41_m42_m43);
    }

    inline SimdMatrix4x4F anemone_vectorcall Matrix4x4F_LoadFloat3x4(float const* source)
    {
        uint32x4_t const mask = vld1q_u32(F32x4_SelectMask_XXXn.As<uint32_t>());

        float32x2x4_t r0_r1 = vld4_f32(source + 0);
        float32x4_t m31_m32_m33_m34 = vld1q_f32(source + 8);

        float32x2_t m31_m32 = vget_low_f32(m31_m32_m33_m34);
        float32x4_t m11_m21_m31_m32 = vcombine_f32(r0_r1.val[0], m31_m32);

        float32x2_t m32_m31 = vrev64_f32(m31_m32);
        float32x4_t m12_m22_m32_m31 = vcombine_f32(r0_r1.val[1], m32_m31);

        float32x2_t m33_m34 = vget_high_f32(m31_m32_m33_m34);
        float32x4_t m13_m23_m33_m34 = vcombine_f32(r0_r1.val[2], m33_m34);

        float32x2_t m34_m33 = vrev64_f32(m33_m34);
        float32x4_t m14_m24_m34_m33 = vcombine_f32(r0_r1.val[3], m34_m33);

        SimdMatrix4x4F result;

        // = [m11, m21, m31, 0]
        result.val[0] = vreinterpretq_f32_u32(vandq_u32(vreinterpretq_u32_f32(m11_m21_m31_m32), mask));
        // = [m12, m22, m32, 0
        result.val[1] = vreinterpretq_f32_u32(vandq_u32(vreinterpretq_u32_f32(m12_m22_m32_m31), mask));
        // = [m13, m23, m33, 0]
        result.val[2] = vreinterpretq_f32_u32(vandq_u32(vreinterpretq_u32_f32(m13_m23_m33_m34), mask));
        // = [m14, m24, m34, 1]
        result.val[3] = vsetq_lane_f32(1.f, m14_m24_m34_m33, 3);

        return result;
    }

    inline void anemone_vectorcall Matrix4x4F_StoreFloat3x4(float* destination, SimdMatrix4x4F source)
    {
        float32x4x2_t const p0 = vzipq_f32(source.val[0], source.val[2]);
        float32x4x2_t const p1 = vzipq_f32(source.val[1], source.val[3]);

        float32x4x2_t const t0 = vzipq_f32(p0.val[0], p1.val[0]);
        float32x4x2_t const t1 = vzipq_f32(p0.val[1], p1.val[1]);

        vst1q_f32(destination + 0, t0.val[0]);
        vst1q_f32(destination + 4, t0.val[1]);
        vst1q_f32(destination + 8, t1.val[0]);
    }

    inline SimdMatrix4x4F anemone_vectorcall Matrix4x4F_LoadFloat3x3(float const* source)
    {
        float32x4_t const m11_m12_m13_m21 = vld1q_f32(source + 0);
        float32x4_t const m22_m23_m31_m32 = vld1q_f32(source + 4);
        float32x2_t const m33_cc0 = vcreate_f32(static_cast<uint64_t>(*(reinterpret_cast<uint32_t const*>(source + 8))));
        float32x4_t const m21_m22_m23_m31 = vextq_f32(m11_m12_m13_m21, m22_m23_m31_m32, 3);

        uint32x4_t const mask = vld1q_u32(F32x4_SelectMask_XXXn.As<uint32_t>());

        SimdMatrix4x4F result;

        // = [m11, m12, m13, 0]
        result.val[0] = vreinterpretq_f32_u32(vandq_u32(vreinterpretq_u32_f32(m11_m12_m13_m21), mask));
        // = [m21, m22, m23, 0]
        result.val[1] = vreinterpretq_f32_u32(vandq_u32(vreinterpretq_u32_f32(m21_m22_m23_m31), mask));
        // = [m31, m32, m33, 0]
        result.val[2] = vcombine_f32(vget_high_f32(m22_m23_m31_m32), m33_cc0);
        // = [0, 0, 0, 1]
        result.val[3] = vld1q_f32(F32x4_PositiveUnitW.As<float>());

        return result;
    }

    inline void anemone_vectorcall Matrix4x4F_StoreFloat3x3(float* destination, SimdMatrix4x4F source)
    {
        uint32x4_t const mask = vld1q_u32(F32x4_SelectMask_XXXn.As<uint32_t>());

        // = [m12, m13, m14, m21]
        float32x4_t const m12_m13_m14_m21 = vextq_f32(source.val[0], source.val[1], 1);
        // = [m11, m12, m13, m21]
        float32x4_t const m11_m12_m13_m21 = vbslq_f32(mask, source.val[0], m12_m13_m14_m21);
        vst1q_f32(destination + 0, m11_m12_m13_m21);

        // = [m22, m23, m24, m21]
        float32x4_t const m22_m23_m24_m21 = vextq_f32(source.val[1], source.val[1], 1);
        // = [m22, m23, m31, m32]
        float32x4_t const m22_m23_m31_m32 = vcombine_f32(vget_low_f32(m22_m23_m24_m21), vget_low_f32(source.val[2]));
        vst1q_f32(destination + 4, m22_m23_m31_m32);
        // = [m33]
        vst1q_lane_f32(destination + 8, source.val[2], 2);
    }

    // TODO: Publish
    inline SimdMatrix4x4F anemone_vectorcall Matrix4x4F_LoadTransposeFloat4x4(float const* source)
    {
        return vld4q_f32(source);
    }

    // TODO: Publish
    inline void anemone_vectorcall Matrix4x4F_StoreTransposeFloat4x4(float* destination, SimdMatrix4x4F source)
    {
        vst4q_f32(destination, source);
    }

    template <size_t N>
    inline SimdVector4F anemone_vectorcall Matrix4x4F_Extract(SimdMatrix4x4F m)
        requires(N < 4)
    {
        return m.val[N];
    }

    template <size_t N>
    inline SimdMatrix4x4F anemone_vectorcall Matrix4x4F_Insert(SimdMatrix4x4F m, SimdVector4F v)
        requires(N < 4)
    {
        m.val[N] = v;
        return m;
    }

    inline SimdMatrix4x4F anemone_vectorcall Matrix4x4F_Multiply(SimdMatrix4x4F a, SimdMatrix4x4F b)
    {
        SimdMatrix4x4F result;

        float32x4_t const c00 = vmulq_laneq_f32(b.val[0], a.val[0], 0);
        float32x4_t const c01 = vmulq_laneq_f32(b.val[1], a.val[0], 1);
        float32x4_t const c02 = vmlaq_laneq_f32(c00, b.val[2], a.val[0], 2);
        float32x4_t const c03 = vmlaq_laneq_f32(c01, b.val[3], a.val[0], 3);
        result.val[0] = vaddq_f32(c02, c03);

        float32x4_t const c10 = vmulq_laneq_f32(b.val[0], a.val[1], 0);
        float32x4_t const c11 = vmulq_laneq_f32(b.val[1], a.val[1], 1);
        float32x4_t const c12 = vmlaq_laneq_f32(c10, b.val[2], a.val[1], 2);
        float32x4_t const c13 = vmlaq_laneq_f32(c11, b.val[3], a.val[1], 3);
        result.val[1] = vaddq_f32(c12, c13);

        float32x4_t const c20 = vmulq_laneq_f32(b.val[0], a.val[2], 0);
        float32x4_t const c21 = vmulq_laneq_f32(b.val[1], a.val[2], 1);
        float32x4_t const c22 = vmlaq_laneq_f32(c20, b.val[2], a.val[2], 2);
        float32x4_t const c23 = vmlaq_laneq_f32(c21, b.val[3], a.val[2], 3);
        result.val[2] = vaddq_f32(c22, c23);

        float32x4_t const c30 = vmulq_laneq_f32(b.val[0], a.val[3], 0);
        float32x4_t const c31 = vmulq_laneq_f32(b.val[1], a.val[3], 1);
        float32x4_t const c32 = vmlaq_laneq_f32(c30, b.val[2], a.val[3], 2);
        float32x4_t const c33 = vmlaq_laneq_f32(c31, b.val[3], a.val[3], 3);
        result.val[3] = vaddq_f32(c32, c33);

        return result;
    }

    inline SimdMatrix4x4F anemone_vectorcall Matrix4x4F_MultiplyTranspose(SimdMatrix4x4F a, SimdMatrix4x4F b)
    {
        // Multiply matrices.
        float32x4_t const c00 = vmulq_laneq_f32(b.val[0], a.val[0], 0);
        float32x4_t const c01 = vmulq_laneq_f32(b.val[1], a.val[0], 1);
        float32x4_t const c02 = vmlaq_laneq_f32(c00, b.val[2], a.val[0], 2);
        float32x4_t const c03 = vmlaq_laneq_f32(c01, b.val[3], a.val[0], 3);
        float32x4_t const r0 = vaddq_f32(c02, c03);

        float32x4_t const c10 = vmulq_laneq_f32(b.val[0], a.val[1], 0);
        float32x4_t const c11 = vmulq_laneq_f32(b.val[1], a.val[1], 1);
        float32x4_t const c12 = vmlaq_laneq_f32(c10, b.val[2], a.val[1], 2);
        float32x4_t const c13 = vmlaq_laneq_f32(c11, b.val[3], a.val[1], 3);
        float32x4_t const r1 = vaddq_f32(c12, c13);

        float32x4_t const c20 = vmulq_laneq_f32(b.val[0], a.val[2], 0);
        float32x4_t const c21 = vmulq_laneq_f32(b.val[1], a.val[2], 1);
        float32x4_t const c22 = vmlaq_laneq_f32(c20, b.val[2], a.val[2], 2);
        float32x4_t const c23 = vmlaq_laneq_f32(c21, b.val[3], a.val[2], 3);
        float32x4_t const r2 = vaddq_f32(c22, c23);

        float32x4_t const c30 = vmulq_laneq_f32(b.val[0], a.val[3], 0);
        float32x4_t const c31 = vmulq_laneq_f32(b.val[1], a.val[3], 1);
        float32x4_t const c32 = vmlaq_laneq_f32(c30, b.val[2], a.val[3], 2);
        float32x4_t const c33 = vmlaq_laneq_f32(c31, b.val[3], a.val[3], 3);
        float32x4_t const r3 = vaddq_f32(c32, c33);

        // Transpose result
        float32x4x2_t const s02 = vzipq_f32(r0, r2);
        float32x4x2_t const s13 = vzipq_f32(r1, r3);
        float32x4x2_t const t01 = vzipq_f32(s02.val[0], s13.val[0]);
        float32x4x2_t const t23 = vzipq_f32(s02.val[1], s13.val[1]);

        SimdMatrix4x4F result;

        result.val[0] = t01.val[0];
        result.val[1] = t01.val[1];
        result.val[2] = t23.val[0];
        result.val[3] = t23.val[1];

        return result;
    }

    inline SimdMatrix4x4F anemone_vectorcall Matrix4x4F_Transpose(SimdMatrix4x4F m)
    {
        float32x4x2_t const m11m31m12m32_m13m33m14m34 = vzipq_f32(m.val[0], m.val[2]);
        float32x4x2_t const m21m41m22m42_m23m43m24m44 = vzipq_f32(m.val[1], m.val[3]);
        float32x4x2_t const m11m12m13m14_m21m22m23m24 = vzipq_f32(m11m31m12m32_m13m33m14m34.val[0], m21m41m22m42_m23m43m24m44.val[0]);
        float32x4x2_t const m31m32m33m34_m41m42m43m44 = vzipq_f32(m11m31m12m32_m13m33m14m34.val[1], m21m41m22m42_m23m43m24m44.val[1]);

        SimdMatrix4x4F result;
        result.val[0] = m11m12m13m14_m21m22m23m24.val[0];
        result.val[1] = m11m12m13m14_m21m22m23m24.val[1];
        result.val[2] = m31m32m33m34_m41m42m43m44.val[0];
        result.val[3] = m31m32m33m34_m41m42m43m44.val[1];
        return result;

        /*
        float32x4x2_t const m01_m23 = vtrnq_f32(m.Inner.val[0], m.Inner.val[1]);
        float32x4x2_t const m45_m67 = vtrnq_f32(m.Inner.val[2], m.Inner.val[3]);

        float32x4x2_t const m0123_4567 = vtrnq_f32(m01_m23.val[0], m45_m67.val[0]);
        float32x4x2_t const m1032_5476 = vtrnq_f32(m01_m23.val[1], m45_m67.val[1]);

        return SimdMatrix4x4F{
            m0123_4567.val[0],
            m0123_4567.val[1],
            m1032_5476.val[0],
            m1032_5476.val[1],
        };
        */
    }

    inline SimdVector4F anemone_vectorcall Matrix4x4F_Determinant(SimdMatrix4x4F m)
    {
        // Note: implementation is based on AVX version, however to avoid unnecessary shuffling,
        // we changed the order of columns.

        float32x2_t const m20_m21 = vget_low_f32(m.val[2]);
        float32x2_t const m22_m23 = vget_high_f32(m.val[2]);
        float32x2_t const m30_m31 = vget_low_f32(m.val[3]);
        float32x2_t const m32_m33 = vget_high_f32(m.val[3]);

        float32x2_t const m20_m20 = vdup_lane_f32(m20_m21, 0);
        float32x2_t const m23_m23 = vdup_lane_f32(m22_m23, 1);
        float32x2_t const m22_m22 = vdup_lane_f32(m22_m23, 0);
        float32x2_t const m21_m21 = vdup_lane_f32(m20_m21, 1);

        float32x2_t const m31_m31 = vdup_lane_f32(m30_m31, 1);
        float32x2_t const m30_m30 = vdup_lane_f32(m30_m31, 0);
        float32x2_t const m32_m32 = vdup_lane_f32(m32_m33, 0);
        float32x2_t const m33_m33 = vdup_lane_f32(m32_m33, 1);

        // = [m21, m20, m20, m20]
        float32x4_t const t0 = vcombine_f32(m20_m21, m20_m20);
        // = [m32, m32, m31, m31]
        float32x4_t const t1 = vcombine_f32(m32_m32, m31_m31);
        // = [m33, m33, m33, m32]
        float32x4_t const t2 = vcombine_f32(m33_m33, m32_m33);
        // = [m22, m22, m21, m21]
        float32x4_t const t3 = vcombine_f32(m22_m22, m21_m21);

        // = [m21, m20, m20, m20] * [m32, m32, m31, m31]
        float32x4_t const t01 = vmulq_f32(t0, t1);
        // = [m21, m20, m20, m20] * [m33, m33, m33, m32]
        float32x4_t const t02 = vmulq_f32(t0, t2);
        // = [m33, m33, m33, m32] * [m22, m22, m21, m21]
        float32x4_t const t23 = vmulq_f32(t2, t3);

        // = [m31, m30, m30, m30]
        float32x4_t const t4 = vcombine_f32(m30_m31, m30_m30);
        // = [m23, m23, m23, m22]
        float32x4_t const t5 = vcombine_f32(m23_m23, m22_m23);

        // = [m21, m20, m20, m20] * [m32, m32, m31, m31] - [m22, m22, m21, m21] * [m31, m30, m30, m30]
        float32x4_t const s01 = vmlsq_f32(t01, t3, t4);
        // = [m21, m20, m20, m20] * [m33, m33, m33, m32] - [m23, m23, m23, m22] * [m31, m30, m30, m30]
        float32x4_t const s23 = vmlsq_f32(t02, t5, t4);
        // = [m33, m33, m33, m32] * [m22, m22, m21, m21] - [m23, m23, m23, m22] * [m32, m32, m31, m31]
        float32x4_t const s45 = vmlsq_f32(t23, t5, t1);

        float32x2_t const m10_m11 = vget_low_f32(m.val[1]);
        float32x2_t const m12_m13 = vget_high_f32(m.val[1]);
        float32x2_t const m10_m10 = vdup_lane_f32(m10_m11, 0);
        float32x2_t const m11_m11 = vdup_lane_f32(m10_m11, 1);
        float32x2_t const m12_m12 = vdup_lane_f32(m12_m13, 0);
        float32x2_t const m13_m13 = vdup_lane_f32(m12_m13, 1);

        // = [m13, m13, m13, m12]
        float32x4_t const v0 = vcombine_f32(m13_m13, m12_m13);
        // = [m12, m12, m11, m11]
        float32x4_t const v1 = vcombine_f32(m12_m12, m11_m11);
        // = [m11, m10, m10, m10];
        float32x4_t const v2 = vcombine_f32(m10_m11, m10_m10);

        float32x4_t const negate_yw{-1.0f, 1.0f, -1.0f, 1.0f};

        float32x2_t const m00_m01 = vget_low_f32(m.val[0]);
        float32x2_t const m02_m03 = vget_high_f32(m.val[0]);

        float32x2_t const m01_m00 = vrev64_f32(m00_m01);
        float32x2_t const m03_m02 = vrev64_f32(m02_m03);

        // = [m00, -m01, m02, -m03]
        float32x4_t const r0 = vmulq_f32(vcombine_f32(m01_m00, m03_m02), negate_yw);
        float32x4_t const r1 = vmulq_f32(v0, s01);
        float32x4_t const r2 = vmlsq_f32(r1, v1, s23);
        float32x4_t const r3 = vmlaq_f32(r2, v2, s45);

        return Vector4F_Dot4(r0, r3);
    }

    inline SimdMatrix4x4F anemone_vectorcall Matrix4x4F_Inverse(SimdMatrix4x4F matrix, SimdVector4F& determinant)
    {
        SimdMatrix4x4F const transposed = Matrix4x4F_Transpose(matrix);
        SimdVector4F const r0 = transposed.val[0];
        SimdVector4F const r1 = transposed.val[1];
        SimdVector4F const r2 = transposed.val[2];
        SimdVector4F const r3 = transposed.val[3];

        SimdVector4F const m20_m20_m21_m21 = Vector4F_Permute<0, 0, 1, 1>(r2);
        SimdVector4F const m32_m33_m32_m33 = Vector4F_Permute<2, 3, 2, 3>(r3);
        SimdVector4F const m00_m00_m01_m01 = Vector4F_Permute<0, 0, 1, 1>(r0);
        SimdVector4F const m12_m13_m12_m13 = Vector4F_Permute<2, 3, 2, 3>(r1);
        SimdVector4F const m20_m22_m00_m02 = Vector4F_Permute<0, 2, 4, 6>(r2, r0);
        SimdVector4F const m31_m33_m11_m13 = Vector4F_Permute<1, 3, 5, 7>(r3, r1);

        SimdVector4F d0x_d0y_d0z_d0w = Vector4F_Multiply(m20_m20_m21_m21, m32_m33_m32_m33);
        SimdVector4F d1x_d1y_d1z_d1w = Vector4F_Multiply(m00_m00_m01_m01, m12_m13_m12_m13);
        SimdVector4F d2x_d2y_d2z_d2w = Vector4F_Multiply(m20_m22_m00_m02, m31_m33_m11_m13);

        SimdVector4F const m22_m23_m22_m23 = Vector4F_Permute<2, 3, 2, 3>(r2);
        SimdVector4F const m30_m30_m31_m31 = Vector4F_Permute<0, 0, 1, 1>(r3);
        SimdVector4F const m02_m03_m02_m03 = Vector4F_Permute<2, 3, 2, 3>(r0);
        SimdVector4F const m10_m10_m11_m11 = Vector4F_Permute<0, 0, 1, 1>(r1);
        SimdVector4F const m21_m23_m01_m03 = Vector4F_Permute<1, 3, 5, 7>(r2, r0);
        SimdVector4F const m30_m32_m10_m12 = Vector4F_Permute<0, 2, 4, 6>(r3, r1);

        d0x_d0y_d0z_d0w = Vector4F_NegateMultiplyAdd(m22_m23_m22_m23, m30_m30_m31_m31, d0x_d0y_d0z_d0w);
        d1x_d1y_d1z_d1w = Vector4F_NegateMultiplyAdd(m02_m03_m02_m03, m10_m10_m11_m11, d1x_d1y_d1z_d1w);
        d2x_d2y_d2z_d2w = Vector4F_NegateMultiplyAdd(m21_m23_m01_m03, m30_m32_m10_m12, d2x_d2y_d2z_d2w);

        SimdVector4F const d0y_d0w_d2y_d2y = Vector4F_Permute<1, 3, 5, 5>(d0x_d0y_d0z_d0w, d2x_d2y_d2z_d2w);
        SimdVector4F const m11_m12_m10_m11 = Vector4F_Permute<1, 2, 0, 1>(r1);
        SimdVector4F const d2y_d0y_d0w_d0x = Vector4F_Permute<2, 0, 7, 4>(d0y_d0w_d2y_d2y, d0x_d0y_d0z_d0w);
        SimdVector4F const m02_m00_m01_m00 = Vector4F_Permute<2, 0, 1, 0>(r0);
        SimdVector4F const d0w_d2y_d0y_d0z = Vector4F_Permute<1, 2, 5, 6>(d0y_d0w_d2y_d2y, d0x_d0y_d0z_d0w);

        SimdVector4F const d1y_d1w_d2w_d2w = Vector4F_Permute<1, 3, 7, 7>(d1x_d1y_d1z_d1w, d2x_d2y_d2z_d2w);
        SimdVector4F const m31_m32_m30_m31 = Vector4F_Permute<1, 2, 0, 1>(r3);
        SimdVector4F const d2w_d1y_d1w_d1x = Vector4F_Permute<2, 0, 7, 4>(d1y_d1w_d2w_d2w, d1x_d1y_d1z_d1w);
        SimdVector4F const m22_m20_m21_m20 = Vector4F_Permute<2, 0, 1, 0>(r2);
        SimdVector4F const d1w_d2w_d1y_d1z = Vector4F_Permute<1, 2, 5, 6>(d1y_d1w_d2w_d2w, d1x_d1y_d1z_d1w);

        SimdVector4F c0x_c0y_c0z_c0w = Vector4F_Multiply(m11_m12_m10_m11, d2y_d0y_d0w_d0x);
        SimdVector4F c2x_c2y_c2z_c2w = Vector4F_Multiply(m02_m00_m01_m00, d0w_d2y_d0y_d0z);
        SimdVector4F c4x_c4y_c4z_c4w = Vector4F_Multiply(m31_m32_m30_m31, d2w_d1y_d1w_d1x);
        SimdVector4F c6x_c6y_c6z_c6w = Vector4F_Multiply(m22_m20_m21_m20, d1w_d2w_d1y_d1z);

        SimdVector4F const d0x_d0y_d2x_d2x = Vector4F_Permute<0, 1, 4, 4>(d0x_d0y_d0z_d0w, d2x_d2y_d2z_d2w);
        SimdVector4F const m12_m13_m11_m12 = Vector4F_Permute<2, 3, 1, 2>(r1);
        SimdVector4F const d0w_d0x_d0y_d2x = Vector4F_Permute<3, 0, 5, 6>(d0x_d0y_d0z_d0w, d0x_d0y_d2x_d2x);
        SimdVector4F const m03_m02_m03_m01 = Vector4F_Permute<3, 2, 3, 1>(r0);
        SimdVector4F const d0z_d0y_d2x_d0x = Vector4F_Permute<2, 1, 6, 4>(d0x_d0y_d0z_d0w, d0x_d0y_d2x_d2x);

        SimdVector4F const d1x_d1y_d2z_d2z = Vector4F_Permute<0, 1, 6, 6>(d1x_d1y_d1z_d1w, d2x_d2y_d2z_d2w);
        SimdVector4F const m32_m33_m31_m32 = Vector4F_Permute<2, 3, 1, 2>(r3);
        SimdVector4F const d1w_d1x_d1y_d2z = Vector4F_Permute<3, 0, 5, 6>(d1x_d1y_d1z_d1w, d1x_d1y_d2z_d2z);
        SimdVector4F const m23_m22_m23_m21 = Vector4F_Permute<3, 2, 3, 1>(r2);
        SimdVector4F const d1z_d1y_d2z_d1x = Vector4F_Permute<2, 1, 6, 4>(d1x_d1y_d1z_d1w, d1x_d1y_d2z_d2z);

        c0x_c0y_c0z_c0w = Vector4F_NegateMultiplyAdd(m12_m13_m11_m12, d0w_d0x_d0y_d2x, c0x_c0y_c0z_c0w);
        c2x_c2y_c2z_c2w = Vector4F_NegateMultiplyAdd(m03_m02_m03_m01, d0z_d0y_d2x_d0x, c2x_c2y_c2z_c2w);
        c4x_c4y_c4z_c4w = Vector4F_NegateMultiplyAdd(m32_m33_m31_m32, d1w_d1x_d1y_d2z, c4x_c4y_c4z_c4w);
        c6x_c6y_c6z_c6w = Vector4F_NegateMultiplyAdd(m23_m22_m23_m21, d1z_d1y_d2z_d1x, c6x_c6y_c6z_c6w);

        SimdVector4F const m13_m10_m13_m10 = Vector4F_Permute<3, 0, 3, 0>(r1);
        SimdVector4F const d0z_d0z_d2x_d2y = Vector4F_Permute<2, 2, 4, 5>(d0x_d0y_d0z_d0w, d2x_d2y_d2z_d2w);
        SimdVector4F const d0z_d2y_d2x_d0z = Vector4F_Permute<0, 3, 2, 0>(d0z_d0z_d2x_d2y);
        SimdVector4F const m01_m03_m00_m02 = Vector4F_Permute<1, 3, 0, 2>(r0);
        SimdVector4F const d0x_d0w_d2x_d2y = Vector4F_Permute<0, 3, 4, 5>(d0x_d0y_d0z_d0w, d2x_d2y_d2z_d2w);
        SimdVector4F const d2y_d0x_d0w_d2x = Vector4F_Permute<3, 0, 1, 2>(d0x_d0w_d2x_d2y);
        SimdVector4F const m33_m30_m33_m30 = Vector4F_Permute<3, 0, 3, 0>(r3);
        SimdVector4F const d1z_d1z_d2z_d2w = Vector4F_Permute<2, 2, 6, 7>(d1x_d1y_d1z_d1w, d2x_d2y_d2z_d2w);
        SimdVector4F const d1z_d2w_d2z_d1z = Vector4F_Permute<0, 3, 2, 0>(d1z_d1z_d2z_d2w);
        SimdVector4F const m21_m23_m20_m22 = Vector4F_Permute<1, 3, 0, 2>(r2);
        SimdVector4F const d1x_d1w_d2z_d2w = Vector4F_Permute<0, 3, 6, 7>(d1x_d1y_d1z_d1w, d2x_d2y_d2z_d2w);
        SimdVector4F const d2w_d1x_d1w_d2z = Vector4F_Permute<3, 0, 1, 2>(d1x_d1w_d2z_d2w);

        SimdVector4F const t0 = Vector4F_Multiply(m13_m10_m13_m10, d0z_d2y_d2x_d0z);
        SimdVector4F const t1 = Vector4F_Multiply(m01_m03_m00_m02, d2y_d0x_d0w_d2x);
        SimdVector4F const t2 = Vector4F_Multiply(m33_m30_m33_m30, d1z_d2w_d2z_d1z);
        SimdVector4F const t3 = Vector4F_Multiply(m21_m23_m20_m22, d2w_d1x_d1w_d2z);

        SimdVector4F c1x_c1y_c1z_c1w = Vector4F_Subtract(c0x_c0y_c0z_c0w, t0);
        c0x_c0y_c0z_c0w = Vector4F_Add(c0x_c0y_c0z_c0w, t0);
        SimdVector4F c3x_c3y_c3z_c3w = Vector4F_Add(c2x_c2y_c2z_c2w, t1);
        c2x_c2y_c2z_c2w = Vector4F_Subtract(c2x_c2y_c2z_c2w, t1);
        SimdVector4F c5x_c5y_c5z_c5w = Vector4F_Subtract(c4x_c4y_c4z_c4w, t2);
        c4x_c4y_c4z_c4w = Vector4F_Add(c4x_c4y_c4z_c4w, t2);
        SimdVector4F c7x_c7y_c7z_c7w = Vector4F_Add(c6x_c6y_c6z_c6w, t3);
        c6x_c6y_c6z_c6w = Vector4F_Subtract(c6x_c6y_c6z_c6w, t3);

        SimdVector4F const c0x_c0z_c1y_c1w = Vector4F_Permute<0, 2, 5, 7>(c0x_c0y_c0z_c0w, c1x_c1y_c1z_c1w);
        SimdVector4F const c2x_c2z_c3y_c3w = Vector4F_Permute<0, 2, 5, 7>(c2x_c2y_c2z_c2w, c3x_c3y_c3z_c3w);
        SimdVector4F const c4x_c4z_c5y_c5w = Vector4F_Permute<0, 2, 5, 7>(c4x_c4y_c4z_c4w, c5x_c5y_c5z_c5w);
        SimdVector4F const c6x_c6z_c7y_c7w = Vector4F_Permute<0, 2, 5, 7>(c6x_c6y_c6z_c6w, c7x_c7y_c7z_c7w);
        SimdVector4F const c0x_c1y_c0z_c1w = Vector4F_Permute<0, 2, 1, 3>(c0x_c0z_c1y_c1w);
        SimdVector4F const c2x_c3y_c2z_c3w = Vector4F_Permute<0, 2, 1, 3>(c2x_c2z_c3y_c3w);
        SimdVector4F const c4x_c5y_c4z_c5w = Vector4F_Permute<0, 2, 1, 3>(c4x_c4z_c5y_c5w);
        SimdVector4F const c6x_c7y_c6z_c7w = Vector4F_Permute<0, 2, 1, 3>(c6x_c6z_c7y_c7w);

        SimdVector4F const det = Vector4F_Dot4(c0x_c1y_c0z_c1w, r0);
        determinant = det;

        SimdVector4F const reciprocal = Vector4F_Reciprocal(det);

        SimdMatrix4x4F result;
        result.val[0] = Vector4F_Multiply(c0x_c1y_c0z_c1w, reciprocal);
        result.val[1] = Vector4F_Multiply(c2x_c3y_c2z_c3w, reciprocal);
        result.val[2] = Vector4F_Multiply(c4x_c5y_c4z_c5w, reciprocal);
        result.val[3] = Vector4F_Multiply(c6x_c7y_c6z_c7w, reciprocal);
        return result;
    }

    inline SimdVector4F anemone_vectorcall Matrix4x4F_Diagonal(SimdMatrix4x4F m)
    {
        // = [m11, ..., ..., ...]
        float32x4_t r = m.val[0];
        // = [m11, m22, ..., ...]
        r = vcopyq_laneq_f32(r, 1, m.val[1], 1);
        // = [m11, m22, m33, ...]
        r = vcopyq_laneq_f32(r, 2, m.val[2], 2);
        // = [m11, m22, m33, m44]
        r = vcopyq_laneq_f32(r, 3, m.val[3], 3);
        return r;
    }

    inline SimdVector4F anemone_vectorcall Matrix4x4F_Trace(SimdMatrix4x4F m)
    {
        // = [m11, ...]
        float32x2_t const m11_m12 = vget_low_f32(m.val[0]);
        // = [..., m22]
        float32x2_t const m21_m22 = vget_low_f32(m.val[1]);
        // = [m33, ...]
        float32x2_t const m33_m34 = vget_high_f32(m.val[2]);
        // = [..., m44]
        float32x2_t const m43_m44 = vget_high_f32(m.val[3]);

        // = [m22, m11]
        float32x2_t const m22_m11 = vext_f32(m21_m22, m11_m12, 1);
        // = [m44, m33]
        float32x2_t const m44_m33 = vext_f32(m43_m44, m33_m34, 1);

        // = [m22+m44, m11+m33]
        float32x2_t const t0 = vadd_f32(m22_m11, m44_m33);
        // = [m11+m22+m33+m44]
        float32x2_t const t1 = vpadd_f32(t0, t0);
        return vcombine_f32(t1, t1);

        //// = [m11 + m33, ...]
        // float32x2_t const t0 = vadd_f32(m11_m12, m33_m34);
        //// = [..., m22 + m44]
        // float32x2_t const t1 = vadd_f32(m21_m22, m43_m44);
        //
        //// = [m22 + m44, m11 + m33]
        // float32x2_t const t2 = vext_f32(t1, t0, 1);
        //
        //// = [m11 + m33 + m22 + m44]
        // float32x2_t const t3 = vpadd_f32(t2, t2);
        // return vcombine_f32(t3, t3);
    }

    inline SimdMatrix4x4F anemone_vectorcall Matrix4x4F_TensorProduct(SimdVector4F a, SimdVector4F b)
    {
        return SimdMatrix4x4F{
            vmulq_laneq_f32(a, b, 0),
            vmulq_laneq_f32(a, b, 1),
            vmulq_laneq_f32(a, b, 2),
            vmulq_laneq_f32(a, b, 3),
        };
    }

    inline bool anemone_vectorcall Matrix4x4F_IsIdentity(SimdMatrix4x4F m)
    {
        uint32x4_t const t0 = vceqq_f32(m.val[0], vld1q_f32(F32x4_PositiveUnitX.As<float>()));
        uint32x4_t const t1 = vceqq_f32(m.val[1], vld1q_f32(F32x4_PositiveUnitY.As<float>()));
        uint32x4_t const t2 = vceqq_f32(m.val[2], vld1q_f32(F32x4_PositiveUnitZ.As<float>()));
        uint32x4_t const t3 = vceqq_f32(m.val[3], vld1q_f32(F32x4_PositiveUnitW.As<float>()));

        uint32x4_t const t01 = vandq_u32(t0, t1);
        uint32x4_t const t23 = vandq_u32(t2, t3);
        uint32x4_t const t0123 = vandq_u32(t01, t23);

        return NeonExtractMask4(t0123) == 0b1111;
    }

    inline bool anemone_vectorcall Matrix4x4F_IsNaN(SimdMatrix4x4F m)
    {
        uint32x4_t const t0 = vmvnq_u32(vceqq_f32(m.val[0], m.val[0]));
        uint32x4_t const t1 = vmvnq_u32(vceqq_f32(m.val[1], m.val[1]));
        uint32x4_t const t2 = vmvnq_u32(vceqq_f32(m.val[2], m.val[2]));
        uint32x4_t const t3 = vmvnq_u32(vceqq_f32(m.val[3], m.val[3]));

        uint32x4_t const t01 = vorrq_u32(t0, t1);
        uint32x4_t const t23 = vorrq_u32(t2, t3);
        uint32x4_t const t0123 = vorrq_u32(t01, t23);

        return NeonExtractMask4(t0123) != 0;
    }

    inline bool anemone_vectorcall Matrix4x4F_IsInfinite(SimdMatrix4x4F m)
    {
        float32x4_t const valueInfinity = vld1q_f32(F32x4_PositiveInfinity_XXXX.As<float>());
        float32x4_t const r0 = vabsq_f32(m.val[0]);
        float32x4_t const r1 = vabsq_f32(m.val[1]);
        float32x4_t const r2 = vabsq_f32(m.val[2]);
        float32x4_t const r3 = vabsq_f32(m.val[3]);

        uint32x4_t const t0 = vceqq_f32(r0, valueInfinity);
        uint32x4_t const t1 = vceqq_f32(r1, valueInfinity);
        uint32x4_t const t2 = vceqq_f32(r2, valueInfinity);
        uint32x4_t const t3 = vceqq_f32(r3, valueInfinity);

        uint32x4_t const t01 = vorrq_u32(t0, t1);
        uint32x4_t const t23 = vorrq_u32(t2, t3);
        uint32x4_t const t0123 = vorrq_u32(t01, t23);

        return NeonExtractMask4(t0123) != 0;
    }
}

namespace Anemone::Numerics::Private
{
    inline SimdMask4F anemone_vectorcall RotorF_CompareEqual(SimdVector4F a, SimdVector4F b)
    {
        return Vector4F_CompareEqual(a, b);
    }

    inline bool anemone_vectorcall RotorF_IsEqual(SimdVector4F a, SimdVector4F b)
    {
        return Vector4F_IsEqual4(a, b);
    }

    inline SimdMask4F anemone_vectorcall RotorF_CompareNotEqual(SimdVector4F a, SimdVector4F b)
    {
        return Vector4F_CompareNotEqual(a, b);
    }

    inline bool anemone_vectorcall RotorF_IsNotEqual(SimdVector4F a, SimdVector4F b)
    {
        return Vector4F_IsNotEqual4(a, b);
    }

    inline SimdMask4F anemone_vectorcall RotorF_CompareNaN(SimdVector4F q)
    {
        return Vector4F_CompareNaN(q);
    }

    inline bool anemone_vectorcall RotorF_IsNaN(SimdVector4F q)
    {
        return Vector4F_IsNaN4(q);
    }

    inline SimdMask4F anemone_vectorcall RotorF_CompareInfinite(SimdVector4F q)
    {
        return Vector4F_CompareInfinite(q);
    }

    inline bool anemone_vectorcall RotorF_IsInfinite(SimdVector4F q)
    {
        return Vector4F_IsInfinite4(q);
    }

    inline SimdMask4F anemone_vectorcall RotorF_CompareIdentity(SimdVector4F q)
    {
        return Vector4F_CompareEqual(q, vld1q_f32(F32x4_PositiveUnitW.As<float>()));
    }

    inline bool anemone_vectorcall RotorF_IsIdentity(SimdVector4F q)
    {
        return Vector4F_IsEqual4(q, vld1q_f32(F32x4_PositiveUnitW.As<float>()));
    }

    inline SimdVector4F anemone_vectorcall RotorF_Create(float xy, float xz, float yz, float scalar)
    {
        return Vector4F_Create(xy, xz, yz, scalar);
    }

    inline SimdVector4F anemone_vectorcall RotorF_Create(SimdVector4F bv, float scalar)
    {
        return Vector4F_Insert<3>(bv, scalar);
    }

    inline SimdVector4F anemone_vectorcall RotorF_Identity()
    {
        return vld1q_f32(F32x4_PositiveUnitW.As<float>());
    }

    inline SimdVector4F anemone_vectorcall RotorF_FromAxisAngle(SimdVector4F axis, float angle)
    {
        SimdVector4F const normal = Vector4F_Normalize3(axis);
        return RotorF_FromNormalAngle(normal, angle);
    }

    inline SimdVector4F anemone_vectorcall RotorF_FromNormalAngle(SimdVector4F normal, float angle)
    {
        auto [fSin, fCos] = SinCos<float>(angle * 0.5f);

        // = [-sin, -sin, -sin, -sin]
        float32x4_t r = vdupq_n_f32(-fSin);
        r = vmulq_f32(r, normal);
        r = vsetq_lane_f32(fCos, r, 3);
        return r;
    }

    inline SimdVector4F anemone_vectorcall RotorF_RotationBetween(SimdVector4F from, SimdVector4F to)
    {
        // = 1 + dot3(to, from)
        float32x2_t const t0 = NeonDot3(to, from);
        float32x2_t const t1 = vadd_f32(vdup_n_f32(1.0f), t0);

        float32x4_t const r0 = Vector4F_Wedge3(to, from);
        float32x4_t const r1 = vcopyq_lane_f32(r0, 3, t1, 0);
        return RotorF_Normalize(r1);
    }

    inline SimdVector4F anemone_vectorcall RotorF_Rotate3(SimdVector4F rotor, SimdVector4F v)
    {
        float32x2_t const r_xy = vget_low_f32(rotor);
        float32x2_t const r_zw = vget_high_f32(rotor);

        float32x2_t const r_ww = vdup_lane_f32(r_zw, 1);
        float32x2_t const r_xx = vdup_lane_f32(r_xy, 0);
        float32x2_t const r_yy = vdup_lane_f32(r_xy, 1);
        float32x2_t const r_yz = vext_f32(r_xy, r_zw, 1);
        float32x2_t const r_zx = vcopy_lane_f32(r_xx, 0, r_zw, 0);
        float32x2_t const r_wz = vrev64_f32(r_zw);

        float32x4_t const r_wwwz = vcombine_f32(r_ww, r_wz);
        float32x4_t const r_xxyy = vcombine_f32(r_xx, r_yy);
        float32x4_t const r_yzzx = vcombine_f32(r_yz, r_zx);

        float32x2_t const v_xy = vget_low_f32(v);
        float32x2_t const v_zw = vget_high_f32(v);

        float32x2_t const v_yx = vrev64_f32(v_xy);
        float32x2_t const v_zz = vdup_lane_f32(v_zw, 0);
        float32x2_t const v_yz = vext_f32(v_xy, v_zw, 1);
        float32x2_t const v_zx = vext_f32(v_zz, v_xy, 1);

        float32x4_t const v_xyzx = vcombine_f32(v_xy, v_zx);
        float32x4_t const v_yxxy = vcombine_f32(v_yx, v_xy);
        float32x4_t const v_zzyz = vcombine_f32(v_zz, v_yz);

        // = [1, -1, -1, -1]
        float32x4_t const negate_yzw{1.0f, -1.0f, -1.0f, -1.0f};
        // = [1, 1, -1, 1]
        float32x4_t const negate_z{1.0f, 1.0f, -1.0f, 1.0f};
        // = [1, -1, 1, 1]
        float32x4_t const negate_y{1.0f, -1.0f, 1.0f, 1.0f};
        // = [1, -1, -1, 1]
        float32x4_t const negate_yz{1.0f, -1.0f, -1.0f, 1.0f};

        // qx = ((rw * vx) +  1.0f * (vy * rx)) +  1.0f * (vz * ry);
        // qy = ((rw * vy) + -1.0f * (vx * rx)) +  1.0f * (vz * rz);
        // qz = ((rw * vz) + -1.0f * (vx * ry)) + -1.0f * (vy * rz);
        // qw = ((rz * vx) + -1.0f * (vy * ry)) +  1.0f * (vz * rx);

        //    = ((t1 * negate_yzw) + t0) + ((t3 * negate_yz) + (t2 * negate_z))
        float32x4_t const t0 = vmulq_f32(r_wwwz, v_xyzx);
        float32x4_t const t1 = vmulq_f32(r_xxyy, v_yxxy);
        float32x4_t const r0 = vmlaq_f32(t0, negate_yzw, t1);

        float32x4_t const t2 = vmulq_f32(r_yzzx, v_zzyz);
        float32x4_t const q_xyzw = vmlaq_f32(r0, negate_z, t2);

        // x = ((rw * qx) +  1.0f * (qy * rx)) +  1.0f * (qz * ry) + ( 1.0f * (qw * rz));
        // y = ((rw * qy) + -1.0f * (qx * rx)) + -1.0f * (qw * ry) + ( 1.0f * (qz * rz));
        // z = ((rw * qz) +  1.0f * (qw * rx)) + -1.0f * (qx * ry) + (-1.0f * (qy * rz));
        // w = 0

        float32x2_t const q_xy = vget_low_f32(q_xyzw);
        float32x2_t const q_zw = vget_high_f32(q_xyzw);
        float32x2_t const q_yx = vrev64_f32(q_xy);
        float32x2_t const q_wz = vrev64_f32(q_zw);
        float32x2_t const q_ww = vdup_lane_f32(q_zw, 1);
        float32x2_t const q_xx = vdup_lane_f32(q_xy, 0);
        float32x2_t const q_yy = vdup_lane_f32(q_xy, 1);

        float32x4_t const q_yxww = vcombine_f32(q_yx, q_ww);
        float32x4_t const q_zwxx = vcombine_f32(q_zw, q_xx);
        float32x4_t const q_wzyy = vcombine_f32(q_wz, q_yy);

        float32x4_t const s0 = vmulq_laneq_f32(q_xyzw, rotor, 3);
        float32x4_t const s1 = vmulq_laneq_f32(q_yxww, rotor, 0);
        float32x4_t const w0 = vmlaq_f32(s0, negate_y, s1);
        float32x4_t const s2 = vmulq_laneq_f32(q_zwxx, rotor, 1);
        float32x4_t const s3 = vmulq_laneq_f32(q_wzyy, rotor, 2);
        float32x4_t const s4 = vmulq_f32(negate_z, s3);
        float32x4_t const w1 = vmlaq_f32(s4, negate_yz, s2);

        float32x4_t const result = vaddq_f32(w0, w1);
        return vreinterpretq_f32_u32(
            vandq_u32(
                vreinterpretq_u32_f32(result),
                vld1q_u32(F32x4_SelectMask_XXXn.As<uint32_t>())));
    }

    inline SimdVector4F anemone_vectorcall RotorF_InverseRotate3(SimdVector4F rotor, SimdVector4F v)
    {
        return RotorF_Rotate3(RotorF_Reverse(rotor), v);
    }

    anemone_noinline inline SimdVector4F anemone_vectorcall RotorF_Multiply(SimdVector4F a, SimdVector4F b)
    {
        // rx = (ax * bw) + (negate_w * aw * bx) + (negate_yw * az * by) + (negate_xzw * ay * bz);
        // ry = (ay * bw) + (negate_w * aw * by) + (negate_yw * az * bx) + (negate_xzw * ax * bz);
        // rz = (az * bw) + (negate_w * aw * bz) + (negate_yw * ay * bx) + (negate_xzw * ax * by);
        // rw = (aw * bw) + (negate_w * ax * bx) + (negate_yw * ay * by) + (negate_xzw * az * bz);

        float32x2_t const a_xy = vget_low_f32(a);
        float32x2_t const a_yx = vrev64_f32(a_xy);
        float32x2_t const a_zw = vget_high_f32(a);
        float32x2_t const a_wx = vext_f32(a_zw, a_xy, 1);
        float32x2_t const a_xz = vext_f32(a_yx, a_zw, 1);
        float32x2_t const a_zz = vdup_lane_f32(a_zz, 0);
        float32x2_t const a_ww = vdup_lane_f32(a_zw, 1);
        float32x2_t const a_yy = vdup_lane_f32(a_xy, 1);

        float32x4_t const a_wwwx = vcombine_f32(a_ww, a_wx);
        float32x4_t const a_zzyy = vcombine_f32(a_zz, a_yy);
        float32x4_t const a_yxxz = vcombine_f32(a_yx, a_xz);

        float32x2_t const b_xy = vget_low_f32(b);
        float32x2_t const b_zw = vget_high_f32(b);
        float32x2_t const b_yx = vrev64_f32(b_xy);
        float32x2_t const b_yz = vext_f32(b_xy, b_zw, 1);
        float32x2_t const b_zz = vdup_lane_f32(b_zw, 0);
        float32x2_t const b_zx = vext_f32(b_yz, b_xy, 1);

        float32x4_t const b_xyzx = vcombine_f32(b_xy, b_zx);
        float32x4_t const b_yxxy = vcombine_f32(b_yx, b_xy);
        float32x4_t const b_zzyz = vcombine_f32(b_zz, b_yz);

        float32x4_t const negate_w{1.0f, 1.0f, 1.0f, -1.0f};
        float32x4_t const negate_yw{1.0f, -1.0f, 1.0f, -1.0f};
        float32x4_t const negate_xzw{-1.0f, 1.0f, -1.0f, -1.0f};

        // = ((t1 * negate_w) + t0) + ((t3 * negate_xzw) + (t2 * negate_yw))
        // = a.xyzw * b.wwww
        float32x4_t const t0 = vmulq_laneq_f32(a, b, 3);
        float32x4_t const t1 = vmulq_f32(a_wwwx, b_xyzx);
        float32x4_t const r0 = vmlaq_f32(t0, negate_w, t1);

        float32x4_t const t2 = vmulq_f32(a_zzyy, b_yxxy);
        float32x4_t const r1 = vmulq_f32(negate_yw, t2);
        float32x4_t const t3 = vmulq_f32(a_yxxz, b_zzyz);
        float32x4_t const r2 = vmlaq_f32(r1, negate_xzw, t3);
        float32x4_t const r3 = vaddq_f32(r0, r2);

        return r3;
    }

    inline SimdVector4F anemone_vectorcall RotorF_RotateRotor(SimdVector4F rotor, SimdVector4F rotation)
    {
        SimdVector4F const t = RotorF_Multiply(rotor, rotation);
        return RotorF_Multiply(t, RotorF_Reverse(rotor));
    }

    inline SimdVector4F anemone_vectorcall RotorF_Reverse(SimdVector4F rotor)
    {
        float32x4_t const negate_xyz{-1.0f, -1.0f, -1.0f, 1.0f};
        return vmulq_f32(rotor, negate_xyz);
    }

    inline SimdVector4F anemone_vectorcall RotorF_Length(SimdVector4F rotor)
    {
        return Vector4F_Length4(rotor);
    }

    inline SimdVector4F anemone_vectorcall RotorF_LengthSquared(SimdVector4F rotor)
    {
        return Vector4F_LengthSquared4(rotor);
    }

    inline SimdVector4F anemone_vectorcall RotorF_Normalize(SimdVector4F rotor)
    {
        return Vector4F_Normalize4(rotor);
    }

    anemone_noinline inline SimdMatrix4x4F anemone_vectorcall RotorF_ToMatrix4x4F(SimdVector4F rotor)
    {
        // = [xw, yw, zw, ww]
        float32x4_t const r_xw_yw_zw_ww = vmulq_laneq_f32(rotor, rotor, 3);
        // = [2xw, 2yw, 2zw, 2ww]
        float32x4_t const r_2xw_2yw_2zw_2ww = vaddq_f32(r_xw_yw_zw_ww, r_xw_yw_zw_ww);

        float32x2_t const r_xy = vget_low_f32(rotor);
        float32x2_t const r_zw = vget_high_f32(rotor);

        float32x2_t const r_yx = vrev64_f32(r_xy);
        float32x2_t const r_xx = vdup_lane_f32(r_xy, 0);
        float32x2_t const r_yy = vdup_lane_f32(r_xy, 1);
        float32x2_t const r_zz = vdup_lane_f32(r_zw, 0);

        // = [y, x, x, x]
        float32x4_t const r_yxxx = vcombine_f32(r_yx, r_xx);
        // = [z, z, y, y]
        float32x4_t const r_zzyy = vcombine_f32(r_zz, r_yy);
        // = [yz, xz, xy, xy]
        float32x4_t const r_yz_xz_xy_xy = vmulq_f32(r_yxxx, r_zzyy);
        // = [2yz, 2xz, 2xy, 2xy]
        float32x4_t const r_2yz_2xz_2xy_2xy = vaddq_f32(r_yz_xz_xy_xy, r_yz_xz_xy_xy);

        // = [-1, -1, 1, 1]
        float32x4_t const negate_xy{-1.0f, -1.0f, 1.0f, 1.0f};
        // = [-1, 1, -1, -1]
        float32x4_t const negate_xz{-1.0f, 1.0f, -1.0f, -1.0f};
        // = [1, -1, -1, 1]
        float32x4_t const negate_yz{1.0f, -1.0f, -1.0f, 1.0f};

        // m11 = (rww + -1.0f * rxx) + (-1.0f * ryy + ( 1.0f * rzz));
        // m22 = (rww + -1.0f * rxx) + ( 1.0f * ryy + (-1.0f * rzz));
        // m33 = (rww +  1.0f * rxx) + (-1.0f * ryy + (-1.0f * rzz));

        float32x4_t const r_xx_yy_zz_ww = vmulq_f32(rotor, rotor);
        float32x4_t const r_ww_ww_ww_ww = vdupq_laneq_f32(r_xx_yy_zz_ww, 3);

        float32x4_t t0 = vmulq_laneq_f32(negate_yz, r_xx_yy_zz_ww, 2);
        float32x4_t t1 = vmlaq_laneq_f32(t0, negate_xz, r_xx_yy_zz_ww, 1);
        float32x4_t t2 = vmlaq_laneq_f32(r_ww_ww_ww_ww, negate_xy, r_xx_yy_zz_ww, 0);
        float32x4_t const m11_m22_m33 = vaddq_f32(t1, t2);

        t0 = vmulq_f32(negate_xz, r_2yz_2xz_2xy_2xy);

        // float const m12 = -1.0f * r2yz - r2xw;
        // float const m13 =  1.0f * r2xz - r2yw;
        // float const m23 = -1.0f * r2xy - r2zw;
        float32x4_t const m12_m13_m23 = vsubq_f32(t0, r_2xw_2yw_2zw_2ww);

        // float const m21 = -1.0f * r2yz + r2xw;
        // float const m31 =  1.0f * r2xz + r2yw;
        // float const m32 = -1.0f * r2xy + r2zw;
        float32x4_t const m21_m31_m32 = vaddq_f32(t0, r_2xw_2yw_2zw_2ww);

        float32x2_t const m11_m22 = vget_low_f32(m11_m22_m33);
        float32x2_t const m33_xxx = vget_high_f32(m11_m22_m33);
        float32x2_t const m12_m13 = vget_low_f32(m12_m13_m23);
        float32x2_t const m23_xxx = vget_high_f32(m12_m13_m23);
        float32x2_t const m21_m31 = vget_low_f32(m21_m31_m32);
        float32x2_t const m32_xxx = vget_high_f32(m21_m31_m32);

        float32x2_t const zero = vdup_n_f32(0.0f);
        float32x2_t const m11_m12 = vcopy_lane_f32(m11_m22, 1, m12_m13, 0);
        float32x2_t const m13_zzz = vcopy_lane_f32(zero, 0, m12_m13, 1);
        float32x2_t const m21_m22 = vcopy_lane_f32(m21_m31, 1, m11_m22, 1);
        float32x2_t const m23_zzz = vset_lane_f32(0.0f, m23_xxx, 1);
        float32x2_t const m31_m32 = vext_f32(m21_m31, m32_xxx, 1);
        float32x2_t const m33_zzz = vset_lane_f32(0.0f, m33_xxx, 1);

        float32x4_t const m11_m12_m13 = vcombine_f32(m11_m12, m13_zzz);
        float32x4_t const m21_m22_m23 = vcombine_f32(m21_m22, m23_zzz);
        float32x4_t const m31_m32_m33 = vcombine_f32(m31_m32, m33_zzz);

        SimdMatrix4x4F result;
        result.val[0] = m11_m12_m13;
        result.val[1] = m21_m22_m23;
        result.val[2] = m31_m32_m33;
        result.val[3] = vld1q_f32(F32x4_PositiveUnitW.As<float>());

        return result;
    }

    inline SimdVector4F anemone_vectorcall RotorF_Nlerp(SimdVector4F from, SimdVector4F to, SimdVector4F t)
    {
        // = dot(from, to)
        float32x4_t const dot = Vector4F_Dot4(from, to);
        // = [1, 1, 1, 1]
        float32x4_t const positive_one = vld1q_f32(F32x4_PositiveOne.As<float>());
        // = [-1, -1, -1, -1]
        float32x4_t const negative_one = vld1q_f32(F32x4_NegativeOne.As<float>());
        // = [0, 0, 0, 0]
        float32x4_t const zero = vdupq_n_f32(0.0f);

        // = [dot < 0].xxxx
        uint32x4_t const mask = vcltq_f32(dot, zero);
        // = [dot < 0 ? -1 : 1].xxxx
        float32x4_t const sign = vbslq_f32(mask, negative_one, positive_one);
        to = vmulq_f32(to, sign);

        float32x4_t const r = Vector4F_PreciseLerp(from, to, t);
        return Vector4F_Normalize4(r);
    }

    inline SimdVector4F anemone_vectorcall RotorF_Nlerp(SimdVector4F from, SimdVector4F to, float t)
    {
        return RotorF_Nlerp(from, to, vdupq_n_f32(t));
    }

    inline SimdVector4F anemone_vectorcall RotorF_Slerp(SimdVector4F from, SimdVector4F to, SimdVector4F t)
    {
        // = from * sin((1 - t) * theta) / sin(theta) + to * sin(t * theta) / sin(theta)
        float32x4_t const positive_one = vld1q_f32(F32x4_PositiveOne.As<float>());
        float32x4_t const negative_one = vld1q_f32(F32x4_NegativeOne.As<float>());
        float32x4_t const zero = vdupq_n_f32(0.0f);

        // cos(theta) = dot(from, to)
        float32x4_t cos_theta = Vector4F_Dot4(from, to);
        // = [cos(theta) < 0].xxxx
        uint32x4_t const mask = vcltq_f32(cos_theta, zero);
        // = [cos(theta) < 0 ? -1 : 1]
        float32x4_t const sign = vbslq_f32(mask, negative_one, positive_one);

        // update sign of cos(theta)
        cos_theta = vmulq_f32(cos_theta, sign);
        // update sign of to rotor
        to = vmulq_f32(to, sign);

        // sin(theta) = sqrt(1 - cos(theta)^2)
        float32x4_t sin_theta = vmulq_f32(cos_theta, cos_theta);
        sin_theta = vsubq_f32(positive_one, sin_theta);
        sin_theta = vsqrtq_f32(sin_theta);

        // theta = atan2(sin(theta), cos(theta))
        float32x4_t const theta = Vector4F_Atan2(sin_theta, cos_theta);

        float32x4_t const negate_x = vld1q_f32(F32x4_Negate_Xnnn.As<float>());

        // = [-t, t, t, t]
        float32x4_t coefficient_linear = vmulq_f32(t, negate_x);
        // = [1-t, t, t, t]
        coefficient_linear = vaddq_f32(coefficient_linear, vld1q_f32(F32x4_PositiveUnitX.As<float>()));

        // = [
        //     sin((1 - t) * theta) / sin(theta),
        //     sin(t * theta) / sin(theta),
        // ]
        float32x4_t coefficient_spherical = vmulq_f32(coefficient_linear, theta);
        coefficient_spherical = Vector4F_Sin(coefficient_spherical);
        coefficient_spherical = vdivq_f32(coefficient_spherical, sin_theta);

        // = [1 - eps].xxxx
        float32x4_t const one_minus_epsilon = vdupq_n_f32(1.0f - std::numeric_limits<float>::epsilon());
        // = [cos(theta) < (1 - eps)].xxxx
        uint32x4_t const close_to_one = vcgeq_f32(cos_theta, one_minus_epsilon);

        // = close_to_one ? linear : spherical
        float32x4_t const coefficient = vbslq_f32(close_to_one, coefficient_linear, coefficient_spherical);

        // interpolate
        float32x4_t r = vmulq_laneq_f32(to, coefficient, 1);
        r = vmlaq_laneq_f32(r, from, coefficient, 0);
        return r;
    }

    inline SimdVector4F anemone_vectorcall RotorF_Slerp(SimdVector4F from, SimdVector4F to, float t)
    {
        return RotorF_Slerp(from, to, vdupq_n_f32(t));
    }
}
