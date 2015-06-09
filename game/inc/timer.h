#ifndef _H_TIMER
#define _H_TIMER

class Timer
{
public:
	Timer();
	~Timer();

	void Start();
	void Stop();
	void Pause();
	void Unpause();

	int GetTicks();

	bool is_started();
	bool is_paused();

	void TickCall(int N, void(*cb)());
	bool Tick(int N);

private:
	int startTicks;
	int pausedTicks;

	bool started;
	bool paused;	
};
#endif