#ifndef KERNEL_H
#define KERNEL_H

extern volatile unsigned int system_tick;
extern volatile unsigned int system_sec;

void _kpanic(int int_no, int error_code);

#endif
