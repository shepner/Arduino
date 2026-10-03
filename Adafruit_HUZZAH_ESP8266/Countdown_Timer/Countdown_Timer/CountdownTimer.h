// Rollover-safe countdown timer driven by a free-running millisecond counter.
// Header-only and free of Arduino dependencies so it can be tested on a host.
#pragma once

#include <stdint.h>

class CountdownTimer {
 public:
  explicit CountdownTimer(uint32_t duration_ms) : duration_ms_(duration_ms) {}

  // (Re)start the countdown from `now_ms`.
  void trigger(uint32_t now_ms) {
    start_ms_ = now_ms;
    active_ = true;
  }

  // Returns true while the countdown is running. Call often (well under
  // duration_ms) so an expired timer is cleared before the counter can wrap
  // all the way around.
  bool update(uint32_t now_ms) {
    // Unsigned subtraction yields the correct elapsed time even when the
    // counter has wrapped between start_ms_ and now_ms. Never compare against
    // `start + duration`; that breaks at the wrap.
    if (active_ && static_cast<uint32_t>(now_ms - start_ms_) >= duration_ms_) {
      active_ = false;
    }
    return active_;
  }

 private:
  uint32_t duration_ms_;
  uint32_t start_ms_ = 0;
  bool active_ = false;
};
