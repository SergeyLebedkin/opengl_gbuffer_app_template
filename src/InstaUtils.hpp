#include <chrono>
#include <ostream>

// created SL-200419
class TimeStamp {
private:
	std::chrono::time_point<std::chrono::steady_clock> mPrevTimePoint;
	std::chrono::time_point<std::chrono::steady_clock> mNextTimePoint;
	float mAccumTime = 0.0f;
	float mPrintTime = 0.0f;
	uint64_t mTicks = 0;
public:
	float mDeltaTime = 0.0f;
public:
	void Tick();
	void Reset();
	void Print(std::ostream& os, float period);
};
