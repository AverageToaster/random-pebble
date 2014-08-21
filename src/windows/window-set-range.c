#include "pebble.h"
#include "window-set-range.h"
#include "window-app.h"
#include "../common.h"

static void selected_callback(NumberWindow *window, void *context);

static NumberWindow *window;
static int max;
static int min;
static bool max_set = false;

void window_set_range_init(){
		max = get_max();
		min = get_min();
		window = number_window_create("Select Max Value",
			(NumberWindowCallbacks){.selected = selected_callback},
			NULL);
		number_window_set_value(window, max);
		number_window_set_max(window, 99999);
		number_window_set_min(window, -99999);
}

void window_set_range_deinit(){
	number_window_destroy(window);
}

void window_set_range_show(){
	max = get_max();
	min = get_min();
	max_set = false;
	number_window_set_value(window, max);
	number_window_set_label(window, "Select Max Value");
	window_stack_push(number_window_get_window(window), true);
}

static void selected_callback(NumberWindow *window, void *context){
	if(!max_set){
		max_set = true;
		max = number_window_get_value(window);
		number_window_set_value(window, min);
		number_window_set_label(window, "Select Min Value");
	}
	else{
		min = number_window_get_value(window);
		if (min > max){
			int temp = max;
			max = min;
			min = temp;
		}
		set_max(max);
		set_min(min);
		window_app_update_max_and_min();
		window_stack_pop(true);
	}
}