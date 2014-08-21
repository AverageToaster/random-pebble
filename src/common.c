#include "pebble.h"
#include "common.h"

int max;
int min;

coin_t coin;

GBitmap *heads;
GBitmap *tails;
/**
 * Function to test if persistent storage is working.
 * @return If persist is working.
 */
bool persist_working(){
	int test = persist_write_bool(100, true);
	persist_delete(100);
	if (test < 0)
		return false;
	return true;
}

bool is_first_start(){
	return !persist_exists(FIRST_TIME);
}

void restore_max_and_min(){
	if (persist_exists(MAX))
		max = persist_read_int(MAX);
	else
		max = 10;
	if (persist_exists(MIN))
		min = persist_read_int(MIN);
	else
		min = 0;
}

int get_max(){
	return max;
}

int get_min(){
	return min;
}

void set_max(int new_max){
	max = new_max;
}
void set_min(int new_min){
	min = new_min;
}

void save_max_and_min(){
	persist_write_int(MAX, max);
	persist_write_int(MIN, min);
}

void restore_coin(){
	if (persist_working() && persist_exists(COIN)){
		coin = persist_read_int(COIN);
		switch(coin){
			case US:
				heads = gbitmap_create_with_resource(RESOURCE_ID_US_HEADS);
				tails = gbitmap_create_with_resource(RESOURCE_ID_US_TAILS);
				break;
			case UK:
				heads = gbitmap_create_with_resource(RESOURCE_ID_UK_HEADS);
				tails = gbitmap_create_with_resource(RESOURCE_ID_US_TAILS);
				break;
			case CAN:
				heads = gbitmap_create_with_resource(RESOURCE_ID_CAN_HEADS);
				tails = gbitmap_create_with_resource(RESOURCE_ID_CAN_TAILS);
				break;
			default:
				heads = gbitmap_create_with_resource(RESOURCE_ID_US_HEADS);
				tails = gbitmap_create_with_resource(RESOURCE_ID_US_TAILS);
		}
	}
	else{
		coin = US;
		heads = gbitmap_create_with_resource(RESOURCE_ID_US_HEADS);
		tails = gbitmap_create_with_resource(RESOURCE_ID_US_TAILS);
	}
}

coin_t get_coin(){
	return coin;
}

GBitmap* get_heads(){
	return heads;
}

GBitmap* get_tails(){
	return tails;
}

void set_coin_type(coin_t coin_val){
	coin = coin_val;
	switch(coin){
		case US:
			if (persist_working())
				persist_write_int(COIN, US);
			gbitmap_destroy(heads);
			gbitmap_destroy(tails);
			heads = gbitmap_create_with_resource(RESOURCE_ID_US_HEADS);
			tails = gbitmap_create_with_resource(RESOURCE_ID_US_TAILS);
			break;
		case UK:
			if (persist_working())
				persist_write_int(COIN, UK);
			gbitmap_destroy(heads);
			gbitmap_destroy(tails);
			heads = gbitmap_create_with_resource(RESOURCE_ID_UK_HEADS);
			tails = gbitmap_create_with_resource(RESOURCE_ID_UK_TAILS);
			break;
		case CAN:
			if (persist_working())
				persist_write_int(COIN, CAN);
			gbitmap_destroy(heads);
			gbitmap_destroy(tails);
			heads = gbitmap_create_with_resource(RESOURCE_ID_CAN_HEADS);
			tails = gbitmap_create_with_resource(RESOURCE_ID_CAN_TAILS);
			break;
		default:
			if (persist_working())
				persist_write_int(COIN, US);
			gbitmap_destroy(heads);
			gbitmap_destroy(tails);
			heads = gbitmap_create_with_resource(RESOURCE_ID_US_HEADS);
			tails = gbitmap_create_with_resource(RESOURCE_ID_US_TAILS);
	}
}

void common_init(){
	restore_max_and_min();
	restore_coin();
}

void common_deinit(){
	gbitmap_destroy(heads);
	gbitmap_destroy(tails);
	save_max_and_min();
}