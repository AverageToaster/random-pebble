#include "pebble.h"
#include "common.h"

static void window_load();
static void window_unload();
static void click_config_provider(void* context);
static void up_click_handler(ClickRecognizerRef recognizer, void *context);
static void select_click_handler(ClickRecognizerRef recognizer, void *context);
static void down_click_handler(ClickRecognizerRef recognizer, void* context);
static void update_set_coin_text();

static Window *window;
static TextLayer *text_layer;
static TextLayer *coin_layer;
static ActionBarLayer *action_bar;
static GBitmap *up_bitmap;
static GBitmap *confirm_bitmap;
static GBitmap *down_bitmap;
static coin_t coin;


void window_set_coin_init()
{
	window = window_create();
	window_set_window_handlers(window, (WindowHandlers){
		.load = window_load,
		.unload = window_unload
	});
	coin = get_coin();
}

void window_set_coin_deinit()
{
	window_destroy(window);
}

void window_set_coin_show()
{
	window_stack_push(window, true);
}

static void window_load(){
	Layer *window_layer = window_get_root_layer(window);
	text_layer = text_layer_create(GRect(0, 0, 124, 108));
	text_layer_set_font(text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
	text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);
	text_layer_set_text(text_layer, "Select Coin");
	layer_add_child(window_layer, text_layer_get_layer(text_layer));

	coin_layer = text_layer_create(GRect(0, 60, 124, 108));
	text_layer_set_font(coin_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
	text_layer_set_text_alignment(coin_layer, GTextAlignmentCenter);
	layer_add_child(window_layer, text_layer_get_layer(coin_layer));

	up_bitmap = gbitmap_create_with_resource(RESOURCE_ID_UP);
	down_bitmap = gbitmap_create_with_resource(RESOURCE_ID_DOWN);
	confirm_bitmap = gbitmap_create_with_resource(RESOURCE_ID_CONFIRM);

	action_bar = action_bar_layer_create();
	action_bar_layer_add_to_window(action_bar, window);
	action_bar_layer_set_click_config_provider(action_bar, click_config_provider);
	action_bar_layer_set_icon(action_bar, BUTTON_ID_UP, up_bitmap);
	action_bar_layer_set_icon(action_bar, BUTTON_ID_SELECT, confirm_bitmap);
	action_bar_layer_set_icon(action_bar, BUTTON_ID_DOWN, down_bitmap);

	update_set_coin_text();
}

static void window_unload(){
	text_layer_destroy(text_layer);
	action_bar_layer_destroy(action_bar);
	gbitmap_destroy(confirm_bitmap);
	gbitmap_destroy(up_bitmap);
	gbitmap_destroy(down_bitmap);
}

static void click_config_provider(void* context)
{
	window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
	window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
	window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context){
	coin++;
	if (coin == LAST)
		coin = US;
	update_set_coin_text();
}

static void select_click_handler(ClickRecognizerRef recognizer, void *context){
	set_coin_type(coin);
	window_stack_pop(true);
}

static void down_click_handler(ClickRecognizerRef recognizer, void* context){
	if (coin == US)
		coin = CAN;
	else
		coin--;
	update_set_coin_text();
}

static void update_set_coin_text(){
	switch(coin){
		case US:
			text_layer_set_text(coin_layer, "US Quarter");
			break;
		case UK:
			text_layer_set_text(coin_layer, "UK Pound");
			break;
		case CAN:
			text_layer_set_text(coin_layer, "Canadian Dollar");
			break;
		default:
			text_layer_set_text(coin_layer, "US Quarter");
	}
}