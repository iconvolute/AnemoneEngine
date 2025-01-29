#include "AnemoneRuntime/Math/Rect.hxx"

#include <catch_amalgamated.hpp>

TEST_CASE("RectF")
{
    using namespace Anemone::Math;

    SECTION("Create")
    {
        SECTION("From bounds")
        {
            RectF const r = RectF::FromBounds(1.0f, 2.0f, 4.0f, 7.0f);

            CHECK(r.Left() == 1.0f);
            CHECK(r.Top() == 2.0f);
            CHECK(r.Right() == 4.0f);
            CHECK(r.Bottom() == 7.0f);
            CHECK(r.Width == 3.0f);
            CHECK(r.Height == 5.0f);
        }
    }

    SECTION("Boolean Operations")
    {
        SECTION("Overlapping")
        {
            RectF const a{1.0f, 2.0f, 3.0f, 4.0f};
            RectF const b{2.0f, 3.0f, 4.0f, 5.0f};

            SECTION("Intersect")
            {
                RectF const r = Intersect(a, b);

                CHECK(r.Left() == b.Left());
                CHECK(r.Top() == b.Top());
                CHECK(r.Right() == a.Right());
                CHECK(r.Bottom() == a.Bottom());
                CHECK(r.Width == a.Right() - b.Left());
                CHECK(r.Height == a.Bottom() - b.Top());
            }

            SECTION("Union")
            {
                RectF const r = Union(a, b);

                CHECK(r.Left() == a.Left());
                CHECK(r.Top() == a.Top());
                CHECK(r.Right() == b.Right());
                CHECK(r.Bottom() == b.Bottom());
                CHECK(r.Width == b.Right() - a.Left());
                CHECK(r.Height == b.Bottom() - a.Top());
            }
        }

        SECTION("Non-Overlapping")
        {
            RectF const a{1.0f, 2.0f, 3.0f, 4.0f};
            RectF const b{5.0f, 7.0f, 7.0f, 8.0f};

            SECTION("Intersect")
            {
                RectF const r = Intersect(a, b);

                CHECK(r.Left() == 0.0f);
                CHECK(r.Top() == 0.0f);
                CHECK(r.Right() == 0.0f);
                CHECK(r.Bottom() == 0.0f);
                CHECK(r.Width == 0.0f);
                CHECK(r.Height == 0.0f);
            }

            SECTION("Union")
            {
                RectF const r = Union(a, b);

                CHECK(r.Left() == a.Left());
                CHECK(r.Top() == a.Top());
                CHECK(r.Right() == b.Right());
                CHECK(r.Bottom() == b.Bottom());
                CHECK(r.Width == b.Right() - a.Left());
                CHECK(r.Height == b.Bottom() - a.Top());
            }
        }
    }

    SECTION("Clip & Contains")
    {
        RectF const a{1.0f, 2.0f, 3.0f, 4.0f};

        // Test against 3x3 grid of points on each side, with one in the middle of the rect
        SECTION("Top")
        {
            float const y = a.Y - 1.0f;

            SECTION("Left")
            {
                float const x = a.X - 1.0f;

                PointF const p{x, y};
                PointF const c = Clip(a, p);

                REQUIRE(c.X == a.X);
                REQUIRE(c.Y == a.Y);

                REQUIRE(Contains(a, p) == false);
            }

            SECTION("Center")
            {
                float const x = a.X + a.Width * 0.5f;

                PointF const p{x, y};
                PointF const c = Clip(a, p);

                REQUIRE(c.X == x);
                REQUIRE(c.Y == a.Y);

                REQUIRE(Contains(a, p) == false);
            }

            SECTION("Right")
            {
                float const x = a.X + a.Width + 1.0f;

                PointF const p{x, y};
                PointF const c = Clip(a, p);

                REQUIRE(c.X == a.X + a.Width);
                REQUIRE(c.Y == a.Y);

                REQUIRE(Contains(a, p) == false);
            }
        }

        SECTION("Center")
        {
            float const y = a.Y + a.Height * 0.5f;

            SECTION("Left")
            {
                float const x = a.X - 1.0f;

                PointF const p{x, y};
                PointF const c = Clip(a, p);

                REQUIRE(c.X == a.X);
                REQUIRE(c.Y == y);

                REQUIRE(Contains(a, p) == false);
            }

            SECTION("Center")
            {
                float const x = a.X + a.Width * 0.5f;

                PointF const p{x, y};
                PointF const c = Clip(a, p);

                REQUIRE(c.X == x);
                REQUIRE(c.Y == y);

                REQUIRE(Contains(a, p) == true);
            }

            SECTION("Right")
            {
                float const x = a.X + a.Width + 1.0f;

                PointF const p{x, y};
                PointF const c = Clip(a, p);

                REQUIRE(c.X == a.X + a.Width);
                REQUIRE(c.Y == y);

                REQUIRE(Contains(a, p) == false);
            }
        }

        SECTION("Bottom")
        {
            float const y = a.Y + a.Height + 1.0f;

            SECTION("Left")
            {
                float const x = a.X - 1.0f;

                PointF const p{x, y};
                PointF const c = Clip(a, p);

                REQUIRE(c.X == a.X);
                REQUIRE(c.Y == a.Y + a.Height);

                REQUIRE(Contains(a, p) == false);
            }

            SECTION("Center")
            {
                float const x = a.X + a.Width * 0.5f;

                PointF const p{x, y};
                PointF const c = Clip(a, p);

                REQUIRE(c.X == x);
                REQUIRE(c.Y == a.Y + a.Height);

                REQUIRE(Contains(a, p) == false);
            }

            SECTION("Right")
            {
                float const x = a.X + a.Width + 1.0f;

                PointF const p{x, y};
                PointF const c = Clip(a, p);

                REQUIRE(c.X == a.X + a.Width);
                REQUIRE(c.Y == a.Y + a.Height);

                REQUIRE(Contains(a, p) == false);
            }
        }
    }
}
