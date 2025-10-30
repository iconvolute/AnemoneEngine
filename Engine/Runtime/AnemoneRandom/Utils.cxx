#include "AnemoneRandom/Utils.hxx"
#include "AnemoneMath/Functions.hxx"

namespace Anemone
{
    void ComputeHistogram(std::vector<uint64_t>& histogram, std::span<uint8_t const> data)
    {
        histogram.clear();
        histogram.resize(256, 0);

        for (uint8_t const value : data)
        {
            histogram[value] += 1;
        }
    }

    void ComputeHistogram(std::vector<uint64_t>& histogram, std::span<float const> data, size_t buckets, float min, float max)
    {
        if (buckets >= 2)
        {
            histogram.clear();
            histogram.resize(buckets, 0);

            float const range = max - min;
            float const bucketsRange = static_cast<float>(buckets);

            for (float const value : data)
            {
                if ((min <= value) && (value <= max))
                {
                    size_t const index = static_cast<size_t>((value - min) / range * bucketsRange);

                    histogram[index] += 1;
                }
            }
        }
    }

    double ComputeEntropy(std::span<uint64_t const> histogram, uint64_t total)
    {
        double result = 0.0;
        double length = static_cast<double>(total);

        for (uint64_t const value : histogram)
        {
            if (value != 0)
            {
                double const probability = static_cast<double>(value) / length;
                result -= probability * (Log(probability) / Ln2<double>);
            }
        }

        return result;
    }

    double ComputeEntropy(std::span<uint64_t const> histogram)
    {
        uint64_t total = 0;

        for (uint64_t const value : histogram)
        {
            total += value;
        }

        return ComputeEntropy(histogram, total);
    }
}
