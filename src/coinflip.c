#include <pebble.h>

#define NUM_OF_COINS 3

enum
{
	COIN=0,
	US=1,
	UK=2,
	CAN=3
};

static Window *window;
static TextLayer *text_layer;
static GBitmap *heads_bitmap, *tails_bitmap;
static BitmapLayer *bitmap_layer;
static AppTimer *timer;
static int times_called = 0;
static bool persist_storage_working;
static int last_flip = -1;

static void test_persist_storage(){
	int test = persist_write_bool(100, true);
	if (test < 0)
		persist_storage_working = false;
	else{
		persist_storage_working = true;
		persist_delete(100);
	}
}

static void timer_callback(){
	if (times_called == 0){
		text_layer_set_text(text_layer, "Flipping.  ");
		times_called++;
		timer = app_timer_register(150, timer_callback, NULL);
	}
	else if (times_called == 1){
		text_layer_set_text(text_layer, "Flipping.. ");
		times_called++;
		timer = app_timer_register(150, timer_callback, NULL);
	}
	else if (times_called == 2){
		text_layer_set_text(text_layer, "Flipping...");
		times_called++;
		timer = app_timer_register(150, timer_callback, NULL);
	}
	else{
		layer_set_hidden(text_layer_get_layer(text_layer), true);
		layer_set_hidden(bitmap_layer_get_layer(bitmap_layer), false);
		times_called = 0;
	}
}

static void flip(){
	if (!persist_storage_working && last_flip == -1){
			layer_set_frame(text_layer_get_layer(text_layer), (GRect) { .origin = { 0, 60 }, .size = { 144, 40 } });
	}
	if (times_called != 0)
		app_timer_cancel(timer);
	layer_set_hidden(text_layer_get_layer(text_layer), false);
	layer_set_hidden(bitmap_layer_get_layer(bitmap_layer), true);
	text_layer_set_text(text_layer, "Flipping   ");
	last_flip = rand() % 2;
	if (last_flip == 0){
		bitmap_layer_set_bitmap(bitmap_layer, tails_bitmap);
	}
	else{
		bitmap_layer_set_bitmap(bitmap_layer, heads_bitmap);
	}
	times_called = 0;
	timer = app_timer_register(150, timer_callback, NULL);
}

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
	flip();
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
	flip();
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
	flip();
}

static void accel_tap_handler(AccelAxisType axis, int32_t direction)
{
	flip();
}
static void click_config_provider(void *context) {
	window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
	window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
	window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
}

static void process_tuple(Tuple *t){
	int key = t->key;
	char string_value[64];
	strcpy(string_value, t->value->cstring);
	if (key == COIN){
		if (strcmp(string_value, "us") == 0){
			if (persist_storage_working)
				persist_write_int(COIN, US);
			heads_bitmap = gbitmap_create_with_resource(RESOURCE_ID_US_HEADS);
			tails_bitmap = gbitmap_create_with_resource(RESOURCE_ID_US_TAILS);	
		}
		else if (strcmp(string_value, "uk") == 0){
			if (persist_storage_working)
				persist_write_int(COIN, UK);
			heads_bitmap = gbitmap_create_with_resource(RESOURCE_ID_UK_HEADS);
			tails_bitmap = gbitmap_create_with_resource(RESOURCE_ID_UK_TAILS);
		}
		else if (strcmp(string_value, "can") == 0){
			if (persist_storage_working)
				persist_write_int(COIN, CAN);
			heads_bitmap = gbitmap_create_with_resource(RESOURCE_ID_CAN_HEADS);
			tails_bitmap = gbitmap_create_with_resource(RESOURCE_ID_CAN_TAILS);	
		}
	}
	if (last_flip != -1){
		switch(last_flip){
			case 0:
				bitmap_layer_set_bitmap(bitmap_layer, tails_bitmap);
				break;
			default:
				bitmap_layer_set_bitmap(bitmap_layer, heads_bitmap);
		}
	}
}

static void in_received_handler(DictionaryIterator *iter, void *context)
{
	Tuple *t = dict_read_first(iter);
	if (t)
	{
		process_tuple(t);
	}
}

static char *translate_error(AppMessageResult result) 
{
	switch (result)
	{
		case APP_MSG_OK: return "APP_MSG_OK";
		case APP_MSG_SEND_TIMEOUT: return "APP_MSG_SEND_TIMEOUT";
		case APP_MSG_SEND_REJECTED: return "APP_MSG_SEND_REJECTED";
		case APP_MSG_NOT_CONNECTED: return "APP_MSG_NOT_CONNECTED";
		case APP_MSG_APP_NOT_RUNNING: return "APP_MSG_APP_NOT_RUNNING";
		case APP_MSG_INVALID_ARGS: return "APP_MSG_INVALID_ARGS";
		case APP_MSG_BUSY: return "APP_MSG_BUSY";
		case APP_MSG_BUFFER_OVERFLOW: return "APP_MSG_BUFFER_OVERFLOW";
		case APP_MSG_ALREADY_RELEASED: return "APP_MSG_ALREADY_RELEASED";
		case APP_MSG_CALLBACK_ALREADY_REGISTERED: return "APP_MSG_CALLBACK_ALREADY_REGISTERED";
		case APP_MSG_CALLBACK_NOT_REGISTERED: return "APP_MSG_CALLBACK_NOT_REGISTERED";
		case APP_MSG_OUT_OF_MEMORY: return "APP_MSG_OUT_OF_MEMORY";
		case APP_MSG_CLOSED: return "APP_MSG_CLOSED";
		case APP_MSG_INTERNAL_ERROR: return "APP_MSG_INTERNAL_ERROR";
		default: return "UNKNOWN ERROR";
	}
}

static void in_dropped_handler(AppMessageResult reason, void *context) 
{
	APP_LOG(APP_LOG_LEVEL_DEBUG, "App Message Dropped! Reason = %s", translate_error(reason));
}

static void out_failed_handler(DictionaryIterator *failed, AppMessageResult reason, void *context) 
{
	APP_LOG(APP_LOG_LEVEL_DEBUG, "App Message Failed to Send! %d : %s", reason, translate_error(reason));
}

static void app_message_init()
{
	app_message_register_inbox_received(in_received_handler);
	app_message_register_inbox_dropped(in_dropped_handler);
	app_message_register_outbox_failed(out_failed_handler);

	app_message_open(app_message_inbox_size_maximum(),app_message_outbox_size_maximum());
}

static void window_load(Window *window) {
	Layer *window_layer = window_get_root_layer(window);
	GRect bounds = layer_get_bounds(window_layer);
	if (persist_storage_working && persist_exists(COIN)){
		int coin = persist_read_int(COIN);
		switch (coin){
			case US:
				heads_bitmap = gbitmap_create_with_resource(RESOURCE_ID_US_HEADS);
				tails_bitmap = gbitmap_create_with_resource(RESOURCE_ID_US_TAILS);
				break;
			case UK:
				heads_bitmap = gbitmap_create_with_resource(RESOURCE_ID_UK_HEADS);
				tails_bitmap = gbitmap_create_with_resource(RESOURCE_ID_UK_TAILS);
				break;
			case CAN:
				heads_bitmap = gbitmap_create_with_resource(RESOURCE_ID_CAN_HEADS);
				tails_bitmap = gbitmap_create_with_resource(RESOURCE_ID_CAN_TAILS);
		}
	}
	else{
		heads_bitmap = gbitmap_create_with_resource(RESOURCE_ID_US_HEADS);
		tails_bitmap = gbitmap_create_with_resource(RESOURCE_ID_US_TAILS);
	}
	bitmap_layer = bitmap_layer_create(GRect(0,0,144,144));
	layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(bitmap_layer));
	if (persist_storage_working){
		text_layer = text_layer_create((GRect) { .origin = { 0, 60 }, .size = { bounds.size.w, 40 } });
		text_layer_set_text(text_layer, "Shake/Press to Flip");
	}
	else{
		text_layer = text_layer_create((GRect){.origin = {0,0}, .size = {bounds.size.w, 144}});
		text_layer_set_text(text_layer, "Persistant Storage is Broken. Please Factory Reset Watch. Visit bit.ly/ urbpebstorage for more information.");
	}
	text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);
	text_layer_set_font(text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
	

	
	layer_add_child(window_layer, text_layer_get_layer(text_layer));


}

static void window_unload(Window *window) {
	text_layer_destroy(text_layer);
	gbitmap_destroy(heads_bitmap);
	gbitmap_destroy(tails_bitmap);
	bitmap_layer_destroy(bitmap_layer);
}

static void init(void) {
	test_persist_storage();
	window = window_create();
	window_set_window_handlers(window, (WindowHandlers) {
		.load = window_load,
		.unload = window_unload,
	});
	window_set_click_config_provider(window, click_config_provider);
	srand(time(NULL));
	accel_tap_service_subscribe(accel_tap_handler);
	app_message_init();
	const bool animated = true;
	window_stack_push(window, animated);
}

static void deinit(void) {
	window_destroy(window);
}

int main(void) {
	init();
	app_event_loop();
	deinit();
}
