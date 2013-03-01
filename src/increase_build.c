/*
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <time.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32
#include <windows.h>

struct timespec
{
	long tv_sec;
	long tv_nsec;
};

/* Note: I copy-pasted this from internet (http://stackoverflow.com/questions/5404277/porting-clock-gettime-to-windows/5404467#5404467)
 * I tweaked it to return nanoseconds instead of microseconds
 * It is much more complete than just finding tv_nsec, but I'm keeping it for possible future use. */
LARGE_INTEGER getFILETIMEoffset(void)
{
	SYSTEMTIME s;
	FILETIME f;
	LARGE_INTEGER t;

	s.wYear = 1970;
	s.wMonth = 1;
	s.wDay = 1;
	s.wHour = 0;
	s.wMinute = 0;
	s.wSecond = 0;
	s.wMilliseconds = 0;
	SystemTimeToFileTime(&s, &f);
	t.QuadPart = f.dwHighDateTime;
	t.QuadPart <<= 32;
	t.QuadPart |= f.dwLowDateTime;
	return t;
}

int clock_gettime(int X, struct timespec *tv)
{
	LARGE_INTEGER t;
	FILETIME f;
	double microseconds;
	static LARGE_INTEGER offset;
	static double frequencyToNanoseconds;
	static int initialized = 0;
	static BOOL usePerformanceCounter = 0;

	if (!initialized)
	{
		LARGE_INTEGER performanceFrequency;
		initialized = 1;
		usePerformanceCounter = QueryPerformanceFrequency(&performanceFrequency);
		if (usePerformanceCounter)
		{
			QueryPerformanceCounter(&offset);
			frequencyToNanoseconds = (double)performanceFrequency.QuadPart/1000000000.0;
		}
		else
		{
			offset = getFILETIMEoffset();
			frequencyToNanoseconds = 0.010;
		}
	}
	if (usePerformanceCounter)
		QueryPerformanceCounter(&t);
	else
	{
		GetSystemTimeAsFileTime(&f);
		t.QuadPart = f.dwHighDateTime;
		t.QuadPart <<= 32;
		t.QuadPart |= f.dwLowDateTime;
	}

	t.QuadPart -= offset.QuadPart;
	microseconds = (double)t.QuadPart/frequencyToNanoseconds;
	t.QuadPart = microseconds;
	tv->tv_sec = t.QuadPart/1000000000;
	tv->tv_nsec = t.QuadPart%1000000000;
	return 0;
}

#ifndef CLOCK_MONOTONIC
#define CLOCK_MONOTONIC 0		/* not used anyway */
#endif
#endif

int main(int argc, char **argv)
{
	time_t now_sec;
	struct tm *now;
	FILE *bout;
	struct timespec now_clk;
	if (argc < 2)
	{
		printf("Usage: %s build_file_name\n\n", argv[0]);;
		return EXIT_FAILURE;
	}
	bout = fopen(argv[1], "a");
	if (!bout)
	{
		printf("Could not open file: %s\n\n", argv[1]);
		return EXIT_FAILURE;
	}
	time(&now_sec);
	now = gmtime(&now_sec);
	fprintf(bout, "%02d/%02d/%04d %02d:%02d:%02d", now->tm_mday, now->tm_mon+1, now->tm_year+1900, now->tm_hour, now->tm_min, now->tm_sec);
	clock_gettime(CLOCK_MONOTONIC, &now_clk);
	fprintf(bout, " %ld\n", now_clk.tv_nsec);
	return EXIT_SUCCESS;
}
