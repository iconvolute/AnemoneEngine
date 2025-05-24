#pragma once
#include "AnemoneRuntime/Base/Duration.hxx"
#include "AnemoneRuntime/Base/Instant.hxx"
#include "AnemoneRuntime/Base/FunctionRef.hxx"

namespace Anemone
{
    class AdaptiveTimer
    {
    private:
        bool _fixedStep{};
        Duration _elapsedTime{};
        Duration _totalTime{};
        Duration _leftoverTime{};
        Duration _maxDeltaTime = Duration::FromMilliseconds(250);
        Duration _targetDeltaTime = Duration::FromFrequency(60);
        Duration _secondTime{};
        Instant _lastTime{};
        uint64_t _framesPerSecond{};
        uint64_t _framesThisSecond{};
        uint64_t _frameCount{};

    public:
        void Reset()
        {
            this->_lastTime = Instant::Now();
            this->_leftoverTime = {};
            this->_framesPerSecond = 0;
            this->_framesThisSecond = 0;
        }

        uint64_t GetFramesPerSecond() const
        {
            return this->_framesPerSecond;
        }

        uint64_t GetFramesCount() const
        {
            return this->_frameCount;
        }

        void Tick(FunctionRef<void(Duration)> update)
        {
            // Get current time.
            Instant currentTime = Instant::Now();

            // Compute time delta.
            Duration timeDelta = currentTime - this->_lastTime;

            // Update counters.
            this->_lastTime = currentTime;
            this->_secondTime += timeDelta;

            // Clamp delta time max value.
            if (timeDelta > this->_maxDeltaTime)
            {
                timeDelta = this->_maxDeltaTime;
            }

            uint64_t lastFrameCount = this->_frameCount;

            if (this->_fixedStep)
            {
                static constexpr Duration maxTolerance{0, 250'000};

                if (IsNearEqual(timeDelta, this->_targetDeltaTime, maxTolerance))
                {
                    timeDelta = this->_targetDeltaTime;
                }


                // Update the time.
                this->_leftoverTime += timeDelta;

                // Call update function required number of times.
                while (this->_leftoverTime >= this->_targetDeltaTime)
                {
                    this->_elapsedTime = this->_targetDeltaTime;
                    this->_totalTime += this->_targetDeltaTime;
                    this->_leftoverTime -= this->_targetDeltaTime;
                    ++this->_frameCount;

                    update(this->_targetDeltaTime);
                }
            }
            else
            {
                // Just update this measurement.
                this->_elapsedTime = timeDelta;
                this->_totalTime += timeDelta;
                this->_leftoverTime = {};
                ++this->_frameCount;

                update(timeDelta);
            }

            // Update FPS count.
            if (this->_frameCount != lastFrameCount)
            {
                ++this->_framesThisSecond;
            }

            // Update last second frame count.
            if (this->_secondTime.Seconds > 0)
            {
                // Got more than one second measured.
                this->_framesPerSecond = this->_framesThisSecond;
                this->_framesThisSecond = 0;

                // Reset seconds counter value.
                this->_secondTime.Seconds = 0;
            }
        }
    };
}
