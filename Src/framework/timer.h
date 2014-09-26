///////////////////////////////////////////////////////////////////
// File: timer.h
// Desc: Header for Timer object
///////////////////////////////////////////////////////////////////
#ifndef __TIMER_H__
#define __TIMER_H__

//Includes:
#include <map>

//Capping based on a limit
#define CAP( a, from, to )		(min( max( a, from ), to ))

//-----------------------------------------------------------------
// Class:	SBaseTimer
// Desc:	Represents a timer in the timer
struct SBaseTimer
{
	SBaseTimer()
	{
		bPaused = false;
		time = 1.0f;
		cur_time = 0.0f;
		time_flag = NULL;
		tick_flag = NULL;
		time_flag_in = 0.0f;
		tick_flag_in = false;
	}
	
	void Reset()
	{
		cur_time = 0.0f;
		time_flag_in = 0.0f;
		tick_flag_in = false;
		if( time_flag )
			*time_flag = 0.0f;
		if( tick_flag )
			*tick_flag = false;
	}
	
	void Clear()
	{
		bPaused = false;
		time = 1.0f;
		cur_time = 0.0f;
		time_flag = NULL;
		tick_flag = NULL;
		time_flag_in = 0.0f;
		tick_flag_in = false;
	}

	void Update( float timeDelta )
	{
		if( !bPaused )
		{
			cur_time += timeDelta;
			time_flag_in = cur_time / time;	//A value from 0.0 to 1.0+ to represent the current state
			if( time_flag )
				*time_flag = time_flag_in;	
			if( cur_time >= time )
			{
				//Tick!
				tick_flag_in = true;
				if( tick_flag )
					*tick_flag = true;
				//Reset current time
				cur_time = 0.0f;
			}
			else
			{
				//No Tick!
				tick_flag_in = false;
				if( tick_flag )
					*tick_flag = false;
			}
		}
	}
	
	void Pause()
	{
		bPaused = true;
	}

	void Resume()
	{
		bPaused = false;
	}

	void Stop()
	{
		Pause();
		Reset();
	}

	void SetTimeFlag( float current_time )
	{
		CAP( current_time, 0.0f, 1.1f );
		cur_time = current_time * time;
		time_flag_in = current_time;
		if( time_flag )
			*time_flag = current_time;
		if( time_flag_in >= 1.0f )
			tick_flag_in = true;
		else
			tick_flag_in = false;
		if( tick_flag )
			*tick_flag = tick_flag_in;
	}

	float		time;
	float		cur_time;
	float		*time_flag;
	bool		*tick_flag;
	float		time_flag_in;
	bool		tick_flag_in;
	bool		bPaused;
};

//-----------------------------------------------------------------
// Class:	CTimer
// Desc:	The main timer system object.
class CTimer
{
public:
	//Constructor/Desctructor
	CTimer()
	{
	}
	~CTimer()
	{
	}

public:
	//Public methods
	void Update( float timeDelta )
	{
		if( mTimers.empty() )
			return;
		for( mTimers_i = mTimers.begin(); mTimers_i != mTimers.end();
			mTimers_i++ )
			mTimers_i->second.Update( timeDelta ); 
	}

	void CleanUp()
	{
		mTimers.clear();
	}
	
	void AddTimer( unsigned int id, float Time, float *TimeFlag = NULL, bool *TickFlag = NULL )
	{
		if( mTimers.find( id ) == mTimers.end() )
		{
			SBaseTimer t;
			t.Clear();
			t.time = Time;
			t.time_flag = TimeFlag;
			t.tick_flag = TickFlag;
			mTimers.insert( timer_pair( id, t ) );
		}
		else
		{
			//It exists, then modify the timer instead of creating a new one
			mTimers[id].Clear();
			mTimers[id].time = Time;
			mTimers[id].time_flag = TimeFlag;
			mTimers[id].tick_flag = TickFlag;
		}
	}

	void ChangeTime( unsigned int id, float Time )
	{
		mTimers[id].time = Time;
		//Update the timer
		mTimers[id].Update( 0.0f );
	}

	void RemoveTimer( unsigned int id )
	{
		mTimers.erase( id );
	}

	bool IsTicked( unsigned int id )
	{
		return mTimers[id].tick_flag_in;
	}

	float GetFloatValue( unsigned int id )
	{
		return mTimers[id].time_flag_in;
	}
	
	void ResetTimer( unsigned int id )
	{
		mTimers[id].Reset();
	}

	void PauseTimer( unsigned int id )
	{
		mTimers[id].Pause();
	}

	void ResumeTimer( unsigned int id )
	{
		mTimers[id].Resume();
	}

	void ResetAllTimers()
	{
		for( mTimers_i = mTimers.begin(); mTimers_i != mTimers.end(); mTimers_i++ )
			mTimers_i->second.Reset();
	}

	void PauseAllTimers()
	{
		for( mTimers_i = mTimers.begin(); mTimers_i != mTimers.end(); mTimers_i++ )
			mTimers_i->second.Pause();
	}
	
	void ResumeAllTimers()
	{
		for( mTimers_i = mTimers.begin(); mTimers_i != mTimers.end(); mTimers_i++ )
			mTimers_i->second.Resume();
	}

	void StopAllTimers()
	{
		for( mTimers_i = mTimers.begin(); mTimers_i != mTimers.end(); mTimers_i++ )
			mTimers_i->second.Resume();
	}

	void StopTimer( unsigned int id )
	{
		mTimers[id].Stop();
	}
	
	void SetTimeFlag( unsigned int id, float current_time )
	{
		mTimers[id].SetTimeFlag( current_time );
	}

	//Access methods
	bool isRunning( unsigned int id )
	{
		return !mTimers[id].bPaused;
	}

private:
	//Parameteres
	std::map < unsigned int, SBaseTimer >				mTimers;
	std::map < unsigned int, SBaseTimer >::iterator		mTimers_i;
	typedef std::pair< unsigned int, SBaseTimer >		timer_pair;

};

#endif