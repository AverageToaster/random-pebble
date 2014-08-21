#include "pebble.h"
#include "window-about.h"
#include "libs/scroll-text-layer/scroll-text-layer.h"

static void window_load(Window *window);
static void window_unload(Window *window);
static void layer_header_update(Layer *layer, GContext *ctx);
static void click_config_provider(void *context);
static void click_select(ClickRecognizerRef recognizer, void *context);

static Window *window;
static ScrollTextLayer *scroll_layer;
static Layer *layer_header;
static char* about = "Random is a Pebble application developed by \nTJ Stein.\nIf you want to see what else I'm working on, feel free to check out my github,\ngithub.com/ tstein4,\nor follow my twitter account \n@TheOfficialTJS";

/**
 * Initialization method. Creates window and assigns handlers.
 */
void window_about_init(void){
	window = window_create();
	window_set_window_handlers(window, (WindowHandlers){
		.load = window_load,
		.unload = window_unload
	});
}

/**
 * Deinitialization method. Destroys the window.
 */
void window_about_deinit(void){
	window_destroy(window);
}

/**
 * Shows the window, with animation.
 */
void window_about_show(){
	window_stack_push(window, true);
}

/**
 * Window load method. Creates the various layers inside the window.
 * @param window Window being loaded.
 */
static void window_load(Window *window){
	scroll_layer = scroll_text_layer_create(GRect(0,26,144,130));
	scroll_text_layer_set_font(scroll_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
	scroll_text_layer_set_text(scroll_layer, about);
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

/**
 * Function to draw the header of the window.
 * @param layer Layer being updated
 * @param ctx   Graphics context of the given layer.
 */
static void layer_header_update(Layer *layer, GContext *ctx){
	graphics_context_set_text_color(ctx, GColorBlack);
	graphics_draw_text(ctx, "Random v2.0", fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD), GRect(0,0,144,22), 0, GTextAlignmentCenter, NULL);
}

/**
 * Function to allow the window to recognize when buttons have been pressed.
 * This assigned SELECT to the click handler 'click_select'
 * @param context Application speicific data, not used in this app.
 */
static void click_config_provider(void *context){
	window_single_click_subscribe(BUTTON_ID_SELECT, click_select);
}

/**
 * Function to handle when the SELECT button has been clicked.
 * In this window, that just exits the error screen.
 * @param recognizer Used to recognize that SELECT has been pressed.
 * @param context    Application speicific data, not used in this app.
 */
static void click_select(ClickRecognizerRef recognizer, void *context){
	window_stack_pop(true);
}