// Host-side test: g++ -std=c++11 -I../Countdown_Timer countdown_timer_test.cpp && ./a.out
#include <cstdio>
#include <cstdlib>

#include "CountdownTimer.h"

static int failures = 0;
#define CHECK(c) do { if (!(c)) { std::printf("FAIL line %d: %s\n", __LINE__, #c); failures++; } } while (0)

static const uint32_t D = 600000UL;

int main() {
  // Idle until triggered.
  { CountdownTimer t(D); CHECK(!t.update(0)); CHECK(!t.update(123456)); }

  // Runs for exactly D ms, then stops.
  { CountdownTimer t(D); t.trigger(1000);
    CHECK(t.update(1000)); CHECK(t.update(1000 + D - 1)); CHECK(!t.update(1000 + D)); CHECK(!t.update(1000 + D + 1)); }

  // Re-trigger extends the countdown.
  { CountdownTimer t(D); t.trigger(0); t.trigger(500000);
    CHECK(t.update(D + 1)); CHECK(t.update(500000 + D - 1)); CHECK(!t.update(500000 + D)); }

  // Triggered just before the 32-bit wrap: still runs through it, expires on time.
  { CountdownTimer t(D); uint32_t start = 0xFFFFFFFFUL - 1000;  // wraps ~1 s later
    t.trigger(start);
    CHECK(t.update(start)); CHECK(t.update(0xFFFFFFFFUL)); CHECK(t.update(5000));
    CHECK(t.update(start + D - 1)); CHECK(!t.update(start + D)); }

  // Once expired it must stay off for a full counter cycle (no spurious restart).
  { CountdownTimer t(D); t.trigger(100); CHECK(!t.update(100 + D));
    for (uint64_t ms = 100 + D; ms < 100 + D + 4294967296ULL; ms += 1000000ULL)
      if (t.update(static_cast<uint32_t>(ms))) { CHECK(false); break; } }

  // Polling every 20 ms across a wrap with a re-trigger after it.
  { CountdownTimer t(D); uint32_t now = 0xFFFFFFFFUL - 5000; t.trigger(now);
    bool on = true;
    for (int i = 0; i < 1000; i++, now += 20) on &= t.update(now);   // crosses wrap
    CHECK(on); t.trigger(now); CHECK(t.update(now + D - 1)); CHECK(!t.update(now + D)); }

  std::printf(failures ? "%d FAILED\n" : "all tests passed\n", failures);
  return failures != 0;
}
