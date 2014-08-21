#include "pebble.h"
#include "window-app.h"
#include "window-about.h"
#include "window-instructions.h"
#include "window-set-range.h"
#include "window-set-coin.h"
#include "../common.h"

static void window_load();
static void window_unload();
static void window_appear();
static void click_config_provider(void* context);
static void up_click_handler(ClickRecognizerRef recognizer, void *context);
static void long_up_click_handler(ClickRecognizerRef recognizer, void *context);
static void down_click_handler(ClickRecognizerRef recognizer, void *context);
static void long_down_click_handler(ClickRecognizerRef recognizer, void *context);
static void select_click_handler(ClickRecognizerRef recognizer, void *context);
static void long_select_click_handler(ClickRecognizerRef recognizer, void *context);
static void accel_tap_handler(AccelAxisType axis, int32_t direction);
static void accel_tap_handler(AccelAxisType axis, int32_t direction);
static void set_default_text();
static void flip();
static void generate_random_number();
static void coin_timer_callback();
static void rng_timer_callback();

static Window *window;
static TextLayer *text_layer;
static BitmapLayer *bitmap_layer;
static AppTimer *timer;
static int max;
static int min;
static int num;
static int8_t last_flip = -1;
static int times_called = 0;
static bool coin_view = true;

void window_app_init(){
	window = window_create();
	window_set_window_handlers(window, (WindowHandlers){
		.load = window_load,
		.unload = window_unload,
		.appear = window_appear
	});
	window_set_click_config_provider(window, click_config_provider);
	accel_tap_service_subscribe(accel_tap_handler);
	window_about_init();
	window_set_range_init();
	window_set_coin_init();
	max = get_max();
	min = get_min();
}

void window_app_deinit(){
	window_about_deinit();
	window_set_range_deinit();
	window_set_coin_deinit();
	window_destroy(window);
}

void window_app_show(){
	window_stack_push(window, true);
}

void window_app_update_max_and_min(){
	max = get_max();
	min = get_min();
}

static void window_load(){
	Layer *window_layer = window_get_root_layer(window);
	text_layer = text_layer_create(GRect(0,36,144,144));
	text_layer_set_font(text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
	text_layer_set_text(text_layer, "Shake or Press Select to flip a coin!");
	text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);
	layer_add_child(window_layer, text_layer_get_layer(text_layer));
	bitmap_layer = bitmap_layer_create(GRect(0,0,144,144));
	layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(bitmap_layer));
}

static void window_unload(){
	text_layer_destroy(text_layer);
}

static void window_appear(){
	set_default_text();
}
/**
 * Function to set how the window handles various button clicks.
 * @param context pointer to application specific data, not used in this application.
 */
static void click_config_provider(void* context)
{
	window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
	window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
	window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
	window_long_click_subscribe(BUTTON_ID_UP, 1000, long_up_click_handler, NULL);
	window_long_click_subscribe(BUTTON_ID_DOWN, 1000, long_down_click_handler, NULL);
	window_long_click_subscribe(BUTTON_ID_SELECT, 1000, long_select_click_handler, NULL);
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context){
	if (times_called != 0)
		app_timer_cancel(timer);
	last_flip = -1;
	coin_view = !coin_view;
	set_default_text();
}

static void long_up_click_handler(ClickRecognizerRef recognizer, void *context){
	window_instructions_show();
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context){
	if (times_called != 0)
		app_timer_cancel(timer);
	last_flip = -1;
	coin_view = !coin_view;
	set_default_text();
}

static void long_down_click_handler(ClickRecognizerRef recognizer, void *context)
{
	window_about_show();
}

static void select_click_handler(ClickRecognizerRef recognizer, void *context){
	if (coin_view){
		flip();
	}
	else{
		generate_random_number();
	}
}

static void long_select_click_handler(ClickRecognizerRef recognizer, void *context){
	if (coin_view){
		window_set_coin_show();
	}
	else
		window_set_range_show();
}

static void accel_tap_handler(AccelAxisType axis, int32_t direction)
{
	if (coin_view){
		flip();
	}
	else{
		generate_random_number();
	}
}

static void set_default_text(){
	last_flip = -1;
	layer_set_frame(text_layer_get_layer(text_layer), GRect(0,36,144,144));
	text_layer_set_font(text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
	layer_set_hidden(bitmap_layer_get_layer(bitmap_layer), true);
	layer_set_hidden(text_layer_get_layer(text_layer), false);
	if (coin_view){
		text_layer_set_text(text_layer, "Shake or Press Select to flip a coin!");
	}else{
		text_layer_set_text(text_layer, "Shake or Press Select to generate a random number!");
	}
}

static void flip(){
	if (times_called != 0)
		app_timer_cancel(timer);
	if (last_flip == -1)
		layer_set_frame(text_layer_get_layer(text_layer), (GRect) { .origin = { 0, 60 }, .size = { 144, 40 } });
	layer_set_hidden(text_layer_get_layer(text_layer), false);
	layer_set_hidden(bitmap_layer_get_layer(bitmap_layer), true);
	text_layer_set_text(text_layer, "Flipping   ");
	last_flip = rand() % 2;
	if (last_flip == 0)
		bitmap_layer_set_bitmap(bitmap_layer, get_tails());
	else
		bitmap_layer_set_bitmap(bitmap_layer, get_heads());
	times_called = 0;
	timer = app_timer_register(150, coin_timer_callback, NULL);
}

static void generate_random_number(){
	// if (!window_is_loaded(window) || window_stack_get_top_window() != window)
	// 	return;
	if (times_called != 0)
		app_timer_cancel(timer);
	times_called = 0;
	timer = app_timer_register(10, rng_timer_callback, NULL);
}

static void rng_timer_callback(){
	num = rand() % (max - min + 1) + min;
	char* text = malloc(20);
	layer_set_frame(text_layer_get_layer(text_layer), GRect(0,51,144,144));
	text_layer_set_font(text_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_MEDIUM_NUMBERS));
	snprintf(text, 20, "%d", num);
	text_layer_set_text(text_layer, text);
	free(text);
	if (times_called == 15){
		times_called = 0;
	}
	else{
		times_called++;
		timer = app_timer_register(times_called*10, rng_timer_callback, NULL);
	}
}

static void coin_timer_callback(){
	if (times_called == 0)
	{
		text_layer_set_text(text_layer, "Flipping.  ");
		times_called++;
		timer = app_timer_register(150, coin_timer_callback, NULL);
	}
	else if (times_called == 1)
	{
		text_layer_set_text(text_layer, "Flipping.. ");
		times_called++;
		timer = app_timer_register(150, coin_timer_callback, NULL);
	}
	else if (times_called == 2)
	{
		text_layer_set_text(text_layer, "Flipping...");
		times_called++;
		timer = app_timer_register(150, coin_timer_callback, NULL);
	}
	else
	{
		layer_set_hidden(text_layer_get_layer(text_layer), true);
		layer_set_hidden(bitmap_layer_get_layer(bitmap_layer), false);
		times_called = 0;
	}
}