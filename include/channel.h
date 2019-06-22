#pragma once

#include "mgos.h"

int channel_get_total(void);
bool channel_is_valid(int channel);
bool channel_get(int channel, bool *state);
bool channel_set(int channel, bool state);
bool channel_toggle(int channel);
bool channel_init(void);
