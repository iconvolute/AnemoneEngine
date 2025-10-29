#include "AnemoneMath/Functions.hxx"

#include <cmath>
#include <catch_amalgamated.hpp>

// Test case for dy/dx = y, analytical solution: y = y0 * exp(x)
TEST_CASE("RungeKutta4: Exponential Growth")
{
    using namespace Catch;
    using namespace Anemone::Math;

    auto const dYdX = [](double x, double y)
    {
        (void)x;
        return y;
    };

    double const x0 = 0.0;
    double const y0 = 1.0;
    double const dt = 0.01;
    size_t const steps = 100;

    double const numerical = RungeKutta4(dYdX, x0, y0, dt, steps);
    double const analytical = y0 * std::exp(x0 + dt * static_cast<double>(steps));

    REQUIRE(numerical == Approx(analytical).epsilon(1e-6));
}

// Test case for dy/dx = -2 * y, analytical solution: y = y0 * exp(-2 * x)
TEST_CASE("RungeKutta4: Exponential Decay")
{
    using namespace Catch;
    using namespace Anemone::Math;

    auto const dYdX = [](double x, double y)
    {
        (void)x;
        return -2 * y;
    };

    double const x0 = 0.0;
    double const y0 = 1.0;
    double const dt = 0.01;
    size_t const steps = 100;

    double const numerical = RungeKutta4(dYdX, x0, y0, dt, steps);
    double const analytical = y0 * std::exp(-2 * (x0 + dt * static_cast<double>(steps)));

    REQUIRE(numerical == Approx(analytical).epsilon(1e-6));
}

// Test case for dy/dx = x, analytical solution: y = y0 + 0.5 * x^2
TEST_CASE("RungeKutta4: Quadratic Growth")
{
    using namespace Catch;
    using namespace Anemone::Math;

    auto const dYdX = [](double x, double y)
    {
        (void)y;
        return x;
    };

    double const x0 = 0.0;
    double const y0 = 0.0;
    double const dt = 0.01;
    size_t const steps = 100;

    double const numerical = RungeKutta4(dYdX, x0, y0, dt, steps);
    double const analytical = y0 + 0.5 * std::pow(x0 + dt * static_cast<double>(steps), 2);

    REQUIRE(numerical == Approx(analytical).epsilon(1e-6));
}
