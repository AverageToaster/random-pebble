#pragma once

#include "pebble.h"

#define FIRST_TIME 0
#define MAX 1
#define MIN 2
#define COIN 3

typedef enum{
	US,
	UK,
	CAN,
	LAST
} coin_t;

bool persist_working();
bool is_first_start();
void restore_max_and_min();
int get_max();
int get_min();
void set_max(int new_max);
void set_min(int new_min);
void save_max_and_min();
void set_coin_type(coin_t coin_val);
coin_t get_coin();
GBitmap* get_tails();
GBitmap* get_heads();
void common_deinit();
void common_init();