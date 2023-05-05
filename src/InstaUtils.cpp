#include "InstaUtils.hpp"

// created SL-200419
void TimeStamp::Tick() {
	mPrevTimePoint = mNextTimePoint;
	mNextTimePoint = std::chrono::high_resolution_clock::now();
	mDeltaTime = std::chrono::duration_cast<std::chrono::duration<float>>(mNextTimePoint - mPrevTimePoint).count();
	mPrintTime = mPrintTime + mDeltaTime;
	mAccumTime = mAccumTime + mDeltaTime;
	mTicks++;
};

// created SL-200419
void TimeStamp::Reset() {
	mPrevTimePoint = std::chrono::high_resolution_clock::now();
	mNextTimePoint = std::chrono::high_resolution_clock::now();
	mDeltaTime = 0.0f;
	mPrintTime = 0.0f;
	mAccumTime = 0.0f;
	mTicks = 0;
};

// created SL-200419
void TimeStamp::Print(std::ostream& os, float period) {
	if (mPrintTime >= period) {
		os << mTicks << " ticks per " << mPrintTime << " sec" << std::endl;
		mPrintTime = 0.0f;
		mTicks = 0;
	}
};
