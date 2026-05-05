#pragma once

#include "types/config.h"
#include "types/state.h"

void display_hw_init();
void display_hw_render(const AppState* s);
void display_hw_clear();
void display_hw_send();
