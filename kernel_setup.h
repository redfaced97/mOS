#ifndef KERNEL_SETUP_H
#define KERNEL_SETUP_H

#define KERNEL_IRQ0_FREQ 100


#include "sys/ports.h"
#include "timers/timer.h"

#include "sys/gdt.h"
#include "sys/idt.h"

#include "func/kernel_funcs.h"

#include "drivers/vga/video.h"
#include "drivers/keyboard/keyboard.h"

#include "drivers/ata/ata.h"
#include "drivers/fs/fat32/fat32.h"

#endif
