// (C) 2016 University of Bristol. See LICENSE.txt

#ifndef _timer
#define _timer

#include <sys/wait.h>   /* Wait for Process Termination */
#include <sys/time.h>
#include <time.h>

long long timeval_diff(struct timeval *start_time, struct timeval *end_time);
double timeval_diff_in_seconds(struct timeval *start_time, struct timeval *end_time);
long long timespec_diff(struct timespec *start_time, struct timespec *end_time);

#endif

