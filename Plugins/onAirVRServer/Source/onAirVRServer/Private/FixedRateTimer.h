/***********************************************************

  Copyright (c) 2017-present Clicked, Inc.

  Licensed under the MIT license found in the LICENSE file 
  in the Docs folder of the distributed package.

 ***********************************************************/

#pragma once

#include <chrono>

class FFixedRateTimer 
{
public:
    FFixedRateTimer() : Interval(0.0), ElapsedTimeFromLastExpire(0.0), PrevTime(std::chrono::steady_clock::now()), bExpired(false) {}

private:
    std::chrono::duration<double> Interval;
    std::chrono::duration<double> ElapsedTimeFromLastExpire;
    std::chrono::time_point<std::chrono::steady_clock> PrevTime;
    bool bExpired;

public:
    bool Expired() const    { return bExpired; }
    
    void Set(double RatePerSec) 
    {
        if (RatePerSec > 0.0) {
            Interval = std::chrono::duration<double>(1.0 / RatePerSec);
            ElapsedTimeFromLastExpire = Interval;
            PrevTime = std::chrono::steady_clock::now();
        }
    }

    void Reset()
    {
        Interval = std::chrono::duration<double>(0.0);
        bExpired = false;
    }

    void UpdatePerFrame()
    {
        if (Interval > std::chrono::duration<double>(0.0)) {
            std::chrono::steady_clock::time_point Current = std::chrono::steady_clock::now();
            ElapsedTimeFromLastExpire += Current - PrevTime;
            if (ElapsedTimeFromLastExpire >= Interval) {
                bExpired = true;
                ElapsedTimeFromLastExpire = ElapsedTimeFromLastExpire - Interval;

                if (ElapsedTimeFromLastExpire >= Interval) {      // prevent from accumulating elapsed time when frame time is longer than Interval
                    ElapsedTimeFromLastExpire = Interval;
                }
            }
            else {
                bExpired = false;
            }

            PrevTime = Current;
        }
    }
};
