#pragma once
#include <atomic>
#include <thread>

static std::atomic<int> counter(0);

int  increment() {
	return counter++;
}
