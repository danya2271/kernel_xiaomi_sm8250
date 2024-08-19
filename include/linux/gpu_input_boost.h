/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (C) 2018-2019 Sultan Alsawaf <sultan@kerneltoast.com>.
 * Copyright (C) 2024 danya2271 <danya2271@yandex.ru>.
 */
#ifndef _GPU_INPUT_BOOST_H_
#define _GPU_INPUT_BOOST_H_

#ifdef CONFIG_GPU_INPUT_BOOST
void gpu_input_boost_kick(void);
void gpu_input_boost_kick_max(unsigned int duration_ms);
#else
static inline void gpu_input_boost_kick(void)
{
}
static inline void gpu_input_boost_kick_max(unsigned int duration_ms)
{
}
#endif

#endif /* _CPU_INPUT_BOOST_H_ */
