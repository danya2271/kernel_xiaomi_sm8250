/*
 * Cpu suspend driver
 * Copyright (C) 2024 danya2271
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <linux/cpu.h>
#include <linux/printk.h>
#include <linux/cpu_suspend.h>
bool screen_off;
// Leave 2 little cores for AOD
void suspend_cpus_aod(void) {
	unsigned int cpu;
    printk("Killing cpu's");
	for_each_present_cpu(cpu)
		if ((cpu != 0) && (cpu != 1) && cpu_online(cpu))
			cpu_down(cpu);
    printk("Cpu's died");
}

void suspend_cpus(void) {
	unsigned int cpu;
    printk("Killing cpu's");
	for_each_present_cpu(cpu)
		if ((cpu != 0) && (cpu != 1) && (cpu != 2) && (cpu != 3) && (cpu != 4) && (cpu != 5) && cpu_online(cpu))
			cpu_down(cpu);
    printk("Cpu's died");
}

void activate_cpus(void) {
	unsigned int cpu;
    printk("Bringing cpu's");
	for_each_present_cpu(cpu) {
		if (!cpu_online(7 - cpu)) {
            cpu_up(7 - cpu);
        }
	}
	printk("Bringed cpu's");
}
