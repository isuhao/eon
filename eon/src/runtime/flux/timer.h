	/* -*- C++ -*- */
	
	/*
	
	  Heap Layers: An Extensible Memory Allocation Infrastructure
	  
	  Copyright (C) 2000-2003 by Emery Berger
	  http://www.cs.umass.edu/~emery
	  emery@cs.umass.edu
	  
	  This program is free software; you can redistribute it and/or modify
	  it under the terms of the GNU General Public License as published by
	  the Free Software Foundation; either version 2 of the License, or
	  (at your option) any later version.
	  
	  This program is distributed in the hope that it will be useful,
	  but WITHOUT ANY WARRANTY; without even the implied warranty of
	  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	  GNU General Public License for more details.
	  
	  You should have received a copy of the GNU General Public License
	  along with this program; if not, write to the Free Software
	  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
	
	*/
	
	#include <assert.h>
	#include <stdio.h>
	
	
	#ifndef _TIMER_H_
	#define _TIMER_H_
	
	/**
	 * @class Timer
	 * @brief A portable class for high-resolution timing.
	 *
	 * This class simplifies timing measurements across a number of platforms.
	 * 
	 * @code
	 *  Timer t;
	 *  t.start();
	 *  // do some work
	 *  t.stop();
	 *  cout << "That took " << (double) t << " seconds." << endl;
	 * @endcode
	 *
	 */
	
	#if defined(linux) && defined(__GNUG__) && defined(__i386__)
	
	#include <stdio.h>
	#include <limits.h>
	#include <time.h>
	#include <unistd.h>
	#include <fcntl.h>
	#include <string.h>
	
	static void getTime (unsigned long& tlo, unsigned long& thi) {
	  asm volatile ("rdtsc"
			: "=a"(tlo),
			"=d" (thi));
	}
	
	
	static double getFrequency (void) {
	  static double freq = 0.0;
	  unsigned long LTime0, LTime1, HTime0, HTime1;
	  if (freq == 0.0) {
	
	    // Parse out the CPU frequency from the /proc/cpuinfo file.
	
	    enum { MAX_PROCFILE_SIZE = 32768 };
	    const char searchStr[] = "cpu MHz		: ";
	    char line[MAX_PROCFILE_SIZE];
	    int fd = open ("/proc/cpuinfo", O_RDONLY);
	    read (fd, line, MAX_PROCFILE_SIZE);
	    char * pos = strstr (line, searchStr);
	    if (pos == NULL) {
	    
	      // Compute MHz directly.
	      // Wait for approximately one second.
	      
	      getTime (LTime0, HTime0);
	      sleep (1);
	      getTime (LTime1, HTime1);
	
	      freq = (double)(LTime1 - LTime0) + (double)(UINT_MAX)*(double)(HTime1 - HTime0);
	      if(LTime1 < LTime0) { freq -= (double)UINT_MAX; }
	
	    } else {
	      // Move the pointer over to the MHz number itself.
	      pos += strlen(searchStr);
	      float f;
	      sscanf (pos, "%f", &f);
	      freq = (double) f * 1000000.0;
	    }
	 
	  }
	  return freq;
	}
	
	namespace markov {
	
	class Timer {
	public:
	  Timer (void)
	    : timeElapsed (0.0)
	  {}
	  void start (void) {
	    getTime (currentLo, currentHi);
	  }
	  void stop (void) {
	    unsigned long lo, hi;
	    getTime (lo, hi);
	    double d = (double) (lo - currentLo) + (double) (UINT_MAX)*(double)(hi - currentHi);
	    if (lo < currentLo) {
	      d -= (double) UINT_MAX;
	    }
	    timeElapsed = d;
	  }
	  operator double (void) {
	    static double freq = getFrequency ();
	    return timeElapsed / freq;
	  }
	    
	private:
	  double timeElapsed;
	  unsigned long currentLo, currentHi;
	};
	
	};
	
	#else
	
	
	#ifdef __SVR4 // Solaris
	#include <sys/time.h>
	#include <unistd.h>
	#include <fcntl.h>
	#include <sys/procfs.h>
	#include <stdio.h>
	#endif // __SVR4
	
	#include <time.h>
	
	#if defined(unix) || defined(__linux)
	#include <sys/time.h>
	#include <unistd.h>
	#endif
	
	
	#ifdef __sgi
	#include <sys/types.h>
	#include <sys/times.h>
	#include <limits.h>
	#endif
	
	
	#if defined(_WIN32)
	#include <windows.h>
	#endif
	
	
	#if defined(__BEOS__)
	#include <OS.h>
	#endif
	
	
	namespace markov {
	
	class Timer {
	
	public:
	
	  /// Initializes the timer.
	  Timer (void)
	#if !defined(_WIN32)
	    : _starttime (0),
	      _elapsedtime (0)
	#endif
	  {
	  }
	
	  /// Start the timer.
	  void start (void) { _starttime = _time(); }
	
	  /// Stop the timer.
	  void stop (void) { _elapsedtime += _time() - _starttime; }
	
	  /// Reset the timer.
	  void reset (void) { _starttime = _elapsedtime; }
	
	#if 0
	  // Set the timer.
	  void set (double secs) { _starttime = 0; _elapsedtime = _sectotime (secs);}
	#endif
	
	  /// Return the number of seconds elapsed.
	  operator double (void) { return _timetosec (_elapsedtime); }
	
	  static double currentTime (void) { TimeType t; t = _time(); return _timetosec (t); }
	
	
	private:
	
	  // The _timer variable will be different depending on the OS.
	  // We try to use the best timer available.
	
	#ifdef __sgi
	#define TIMER_FOUND
	
	  long _starttime, _elapsedtime;
	
	  long _time (void) {
	    struct tms t;
	    long ticks = times (&t);
	    return ticks;
	  }
	
	  static double _timetosec (long t) {
	    return ((double) (t) / CLK_TCK);
	  }
	
	  static long _sectotime (double sec) {
	    return (long) sec * CLK_TCK;
	  }
	#endif
	
	#ifdef __SVR4 // Solaris
	#define TIMER_FOUND
	  typedef hrtime_t TimeType;
	  TimeType	_starttime, _elapsedtime;
	
	  static TimeType _time (void) {
	    return gethrtime();
	  }
	
	  static TimeType _sectotime (double sec) { return (hrtime_t) (sec * 1.0e9); }
	
	  static double _timetosec (TimeType& t) {
	    return ((double) (t) / 1.0e9);
	  }
	#endif // __SVR4
	
	#if defined(MAC) || defined(macintosh)
	#define TIMER_FOUND
	  double		_starttime, _elapsedtime;
	
	  double _time (void) {
	    return get_Mac_microseconds();
	  }
	
	  double _timetosec (hrtime_t& t) {
	    return t;
	  }
	#endif // MAC
	
	#ifdef _WIN32
	#define TIMER_FOUND
	
	#ifndef __GNUC__
	  class TimeType {
	  public:
	    TimeType (void)
	    {
	      largeInt.QuadPart = 0;
	    }
	    operator double& (void) { return (double&) largeInt.QuadPart; }
	    operator LARGE_INTEGER& (void) { return largeInt; }
	    double timeToSec (void) {
	      return (double) largeInt.QuadPart / getFreq();
	    }
	  private:
	    double getFreq (void) {
	      QueryPerformanceFrequency (&freq);
	      return (double) freq.QuadPart;
	    }
	
	    LARGE_INTEGER largeInt;
	    LARGE_INTEGER freq;
	  };
	
	  TimeType _starttime, _elapsedtime;
	
	  static TimeType _time (void) {
	    TimeType t;
	    int r = QueryPerformanceCounter (&((LARGE_INTEGER&) t));
	    assert (r);
	    return t;
	  }
	
	  static double _timetosec (TimeType& t) {
	    return t.timeToSec();
	  }
	#else
	  typedef DWORD TimeType;
	  DWORD _starttime, _elapsedtime;
	  static DWORD _time (void) {
	    return GetTickCount();
	  }
	
	  static double _timetosec (DWORD& t) {
	    return (double) t / 100000.0;
	  }
	  static unsigned long _sectotime (double sec) {
	    return (unsigned long)(sec);
	  }
	#endif
	#endif // _WIN32
	
	
	#ifdef __BEOS__
	#define TIMER_FOUND
	  bigtime_t _starttime, _elapsedtime;
	  bigtime_t _time(void) {
	    return system_time();
	  }
	  double _timetosec (bigtime_t& t) {
	    return (double) t / 1000000.0;
	  }
	  
	  bigtime_t _sectotime (double sec) {
	    return (bigtime_t)(sec * 1000000.0);
	  }
	#endif // __BEOS__
	
	#ifndef TIMER_FOUND
	
	  typedef long TimeType;
	  TimeType _starttime, _elapsedtime;
	
	  static TimeType _time (void) {
	    struct timeval t;
	    gettimeofday (&t, NULL);
	    return t.tv_sec * 1000000 + t.tv_usec;
	  }
	
	  static double _timetosec (TimeType t) {
	    return ((double) (t) / 1000000.0);
	  }
	
	  static TimeType _sectotime (double sec) {
	    return (TimeType) (sec * 1000000.0);
	  }
	
	#endif // TIMER_FOUND
	
	#undef TIMER_FOUND
	
	};
	
	
	#ifdef __SVR4 // Solaris
	class VirtualTimer : public Timer {
	public:
	  hrtime_t _time (void) {
	    return gethrvtime();
	  }
	};  
	#endif
	
	};
	
	/*
  struct rusage ru;
  int r = getrusage (RUSAGE_SELF, &ru);
  struct timeval& tp = ru.ru_utime;
  long usec = tp.tv_usec;

  long sec = tp.tv_sec;

  printf ("time: %ld.%ld\n", sec, usec);
	 */
	 
	 
	#endif
	
	#endif