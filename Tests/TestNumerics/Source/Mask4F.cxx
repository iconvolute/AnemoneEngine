#include "AnemoneMath/Detail/SimdFloat.hxx"

ANEMONE_EXTERNAL_HEADERS_BEGIN

#include <catch_amalgamated.hpp>

ANEMONE_EXTERNAL_HEADERS_END

TEST_CASE("Mask4F_Select")
{
    using namespace Anemone::Internal;
    using namespace Anemone;
    using namespace Catch::Matchers;

    SimdVector4F const a = Vector4F_Create(1.0f, 2.0f, 3.0f, 4.0f);
    SimdVector4F const b = Vector4F_Create(5.0f, 6.0f, 7.0f, 8.0f);

    for (bool x : {false, true})
    {
        for (bool y : {false, true})
        {
            for (bool z : {false, true})
            {
                for (bool w : {false, true})
                {
                    SimdMask4F const mask = Mask4F_Create(x, y, z, w);
                    SimdVector4F const result = Vector4F_Select(mask, a, b);

                    if (x)
                    {
                        REQUIRE(Vector4F_Extract<0>(result) == 5.0f);
                    }
                    else
                    {
                        REQUIRE(Vector4F_Extract<0>(result) == 1.0f);
                    }

                    if (y)
                    {
                        REQUIRE(Vector4F_Extract<1>(result) == 6.0f);
                    }
                    else
                    {
                        REQUIRE(Vector4F_Extract<1>(result) == 2.0f);
                    }

                    if (z)
                    {
                        REQUIRE(Vector4F_Extract<2>(result) == 7.0f);
                    }
                    else
                    {
                        REQUIRE(Vector4F_Extract<2>(result) == 3.0f);
                    }

                    if (w)
                    {
                        REQUIRE(Vector4F_Extract<3>(result) == 8.0f);
                    }
                    else
                    {
                        REQUIRE(Vector4F_Extract<3>(result) == 4.0f);
                    }
                }
            }
        }
    }
}
