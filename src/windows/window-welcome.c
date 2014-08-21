#include "pebble.h"
#include "window-welcome.h"
#include "../libs/scroll-text-layer/scroll-text-layer.h"
#include "../common.h"

static void window_load(Window *window);
static void window_unload(Window *window);
static void layer_header_update(Layer *layer, GContext *ctx);
static void click_config_provider(void *context);
static void click_select(ClickRecognizerRef recognizer, void *context);

static Window *window;
static ScrollTextLayer *scroll_layer;
static Layer *layer_header;
static char* text = "Welcome to v2.0! This app has changed, so here's a one time message to get you up to speed.\nThis is now a Random Number Generator as well as a Coin Flip app.\nAdditionally, there is no more settings page, you now set the coin face inside the app.";

/**
 * Initialization method. Creates window and assigns handlers.
 */
void window_welcome_init(void){
	window = window_create();
	window_set_window_handlers(window, (WindowHandlers){
		.load = window_load,
		.unload = window_unload
	});
}

/**
 * Deinitialization method. Destroys the window.
 */
void window_welcome_deinit(void){
	window_destroy(window);
}

/**
 * Shows the window, with animation.
 */
void window_welcome_show(){
	window_stack_push(window, true);
}

/**
 * Window load method. Creates the various layers inside the window.
 * @param window Window being loaded.
 */
static void window_load(Window *window){
	scroll_layer = scroll_text_layer_create(GRect(0,26,144,130));
	scroll_text_layer_set_font(scroll_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
	scroll_text_layer_set_text(scroll_layer, text);
	scroll_text_layer_add_to_window(scroll_layer, window);
	scroll_layer_set_callbacks(scroll_text_layer_get_scroll_layer(scroll_layer), (ScrollLayerCallbacks){
		.click_config_provider = click_config_provider
	});

	layer_header = layer_create(GRect(0,0,144,26));
	layer_set_update_proc(layer_header, layer_header_update);
	layer_add_child(window_get_root_layer(window), layer_header);
}

/**
 * Window unload method. Destroys the various layers inside the window.
 * @param window Window being destroyed.
 */
static void window_unload(Window *window){
	scroll_text_layer_destroy(scroll_layer);
	layer_destroy(layer_header);
}

static void layer_header_update(Layer *layer, GContext *ctx){
	graphics_context_set_fill_color(ctx, GColorBlack);
	graphics_context_set_text_color(ctx, GColorWhite);
	graphics_fill_rect(ctx, layer_get_bounds(layer), 0, GCornerNone);
	graphics_draw_text(ctx, "Welcome to v2.0", fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD), GRect(0,0,144,22), 0, GTextAlignmentCenter, NULL);
}

static void click_config_provider(void *context){
	window_single_click_subscribe(BUTTON_ID_SELECT, click_select);
}

static void click_select(ClickRecognizerRef recognizer, void *context){
	window_stack_pop(true);
}