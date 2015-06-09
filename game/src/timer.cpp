#include "SDL.h"
#include "timer.h"

Timer::Timer()
{
	startTicks = 0;
	pausedTicks = 0;
	paused = false;
	started = false;
};

Timer::~Timer() { };

void Timer::Start()
{
	started = true;
	paused = false;
	startTicks = SDL_GetTicks();
};

void Timer::Stop()
{
	started = false;
	paused = false;
};

void Timer::Pause()
{
	if ((started == true) && (paused == false)) {		
		paused = true;
		pausedTicks = SDL_GetTicks() - startTicks;
	}
};

void Timer::Unpause()
{
	if (paused == true) {	
		paused = false;
		startTicks = SDL_GetTicks() - pausedTicks;
		pausedTicks = 0;
	}
};

int Timer::GetTicks()
{
	if (started == true) {
		// if timer is paused, return the ticks from paused time
		if (paused == true) {
			return pausedTicks;
		}
		else {
			return SDL_GetTicks() - startTicks;
		}
	}

	// timer isn't running
	return 0;
};

bool Timer::is_started()
{
	return started;
};

bool Timer::is_paused()
{
	return paused;
};

// execute a function every n milliseconds
void Timer::TickCall(int N, void (*cb)())
{
	int t = this->GetTicks() % N;
	if (t == 0) {	
		cb();		
	}
};

bool Timer::Tick(int N)
{
	bool tick = false;

	int t = this->GetTicks() % N;
	if (t == 0) {
		tick = true;
	}
	return tick;
}