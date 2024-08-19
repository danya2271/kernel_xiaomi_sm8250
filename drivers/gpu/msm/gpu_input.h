// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2024 danya2271 <danya2271@yandex.ru>.
 */
static unsigned int input_boost_level __read_mostly =
CONFIG_INPUT_BOOST_LEVEL;
static unsigned short input_boost_duration __read_mostly =
CONFIG_INPUT_BOOST_DURATION_MS;
static unsigned short wake_boost_duration __read_mostly =
CONFIG_WAKE_BOOST_DURATION_MS;

int boost_adjust_notify(void);

