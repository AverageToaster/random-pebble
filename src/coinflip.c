#include <pebble.h>
#define NUM_OF_COINS 3

// For AppMessage
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


/*
 * Make sure the persistant storage is working. 
 * If not, have to prevent any persist calls from happening.
 */
static void test_persist_storage()
{
	int test = persist_write_bool(100, true);
	if (test < 0)
		persist_storage_working = false;
	else
	{
		persist_storage_working = true;
		persist_delete(100);
	}
}

/*
 * Recursive callback just to make a nice little animation so the user knows the coin has changed.
 */
static void timer_callback()
{
	if (times_called == 0)
	{
		text_layer_set_text(text_layer, "Flipping.  ");
		times_called++;
		timer = app_timer_register(150, timer_callback, NULL);
	}
	else if (times_called == 1)
	{
		text_layer_set_text(text_layer, "Flipping.. ");
		times_called++;
		timer = app_timer_register(150, timer_callback, NULL);
	}
	else if (times_called == 2)
	{
		text_layer_set_text(text_layer, "Flipping...");
		times_called++;
		timer = app_timer_register(150, timer_callback, NULL);
	}
	else
	{
		layer_set_hidden(text_layer_get_layer(text_layer), true);
		layer_set_hidden(bitmap_layer_get_layer(bitmap_layer), false);
		times_called = 0;
	}
}

/*
 * Method for flipping the coin.
 * Initiate the animation for the timer callback, then get a random number from 0 to 1.
 * 0 = tails, 1 = heads
 */
static void flip()
{
	if (!persist_storage_working && last_flip == -1)
			layer_set_frame(text_layer_get_layer(text_layer), (GRect) { .origin = { 0, 60 }, .size = { 144, 40 } });
	if (times_called != 0)
		app_timer_cancel(timer);
	layer_set_hidden(text_layer_get_layer(text_layer), false);
	layer_set_hidden(bitmap_layer_get_layer(bitmap_layer), true);
	text_layer_set_text(text_layer, "Flipping   ");
	last_flip = rand() % 2;
	if (last_flip == 0)
		bitmap_layer_set_bitmap(bitmap_layer, tails_bitmap);
	else
		bitmap_layer_set_bitmap(bitmap_layer, heads_bitmap);
	times_called = 0;
	timer = app_timer_register(150, timer_callback, NULL);
}

/*
 * Handler for select button.
 */
static void select_click_handler(ClickRecognizerRef recognizer, void *context) 
{
	flip();
}
/*
 * Handler for up button.
 */
static void up_click_handler(ClickRecognizerRef recognizer, void *context) 
{
	flip();
}
/*
 * Handler for select button.
 */
static void down_click_handler(ClickRecognizerRef recognizer, void *context) 
{
	flip();
}
/*
 * Handler for accelerator.
 */
static void accel_tap_handler(AccelAxisType axis, int32_t direction)
{
	flip();
}
/*
 * Method to set up all the click handlers.
 */
static void click_config_provider(void *context) 
{
	window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
	window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
	window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
}
/*
 * Method to process the AppMessage.
 * Gets whatever coin was passed in from AppMessage,
 * writes that value into persist storage so it remembers it across sessions,
 * and sets the current bitmaps to the selected coin face.
 */
static void process_tuple(Tuple *t)
{
	int key = t->key;
	char string_value[64];
	strcpy(string_value, t->value->cstring);
	if (key == COIN)
	{
		if (strcmp(string_value, "us") == 0)
		{
			if (persist_storage_working)
				persist_write_int(COIN, US);
			gbitmap_destroy(heads_bitmap);
			gbitmap_destroy(tails_bitmap);
			heads_bitmap = gbitmap_create_with_resource(RESOURCE_ID_US_HEADS);
			tails_bitmap = gbitmap_create_with_resource(RESOURCE_ID_US_TAILS);
		}
		else if (strcmp(string_value, "uk") == 0)
		{
			if (persist_storage_working)
				persist_write_int(COIN, UK);
			gbitmap_destroy(heads_bitmap);
			gbitmap_destroy(tails_bitmap);
			heads_bitmap = gbitmap_create_with_resource(RESOURCE_ID_UK_HEADS);
			tails_bitmap = gbitmap_create_with_resource(RESOURCE_ID_UK_TAILS);
		}
		else if (strcmp(string_value, "can") == 0)
		{
			if (persist_storage_working)
				persist_write_int(COIN, CAN);
			gbitmap_destroy(heads_bitmap);
			gbitmap_destroy(tails_bitmap);
			heads_bitmap = gbitmap_create_with_resource(RESOURCE_ID_CAN_HEADS);
			tails_bitmap = gbitmap_create_with_resource(RESOURCE_ID_CAN_TAILS);	
		}
	}
}
/*
 * Handler for recieved AppMessage.
 */
static void in_received_handler(DictionaryIterator *iter, void *context)
{
	Tuple *t = dict_read_first(iter);
	if (t)
		process_tuple(t);
}
/*
 * Helper function for translating AppMessage errors.
 */
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
/*
 * Handler for dropped messages
 */
static void in_dropped_handler(AppMessageResult reason, void *context) 
{
	APP_LOG(APP_LOG_LEVEL_DEBUG, "App Message Dropped! Reason = %s", translate_error(reason));
}
/*
 * Handler for failed messages.
 */
static void out_failed_handler(DictionaryIterator *failed, AppMessageResult reason, void *context) 
{
	APP_LOG(APP_LOG_LEVEL_DEBUG, "App Message Failed to Send! %d : %s", reason, translate_error(reason));
}

/*
 * Initalizer for app message. Sets handlers.
 */
static void app_message_init()
{
	app_message_register_inbox_received(in_received_handler);
	app_message_register_inbox_dropped(in_dropped_handler);
	app_message_register_outbox_failed(out_failed_handler);

	app_message_open(app_message_inbox_size_maximum(),app_message_outbox_size_maximum());
}
/*
 * Function to create the window.
 */
static void window_load(Window *window) 
{
	Layer *window_layer = window_get_root_layer(window);
	GRect bounds = layer_get_bounds(window_layer);
	if (persist_storage_working && persist_exists(COIN))
	{
		int coin = persist_read_int(COIN);
		switch (coin)
		{
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
	else
	{
		heads_bitmap = gbitmap_create_with_resource(RESOURCE_ID_US_HEADS);
		tails_bitmap = gbitmap_create_with_resource(RESOURCE_ID_US_TAILS);
	}
	bitmap_layer = bitmap_layer_create(GRect(0,0,144,144));
	layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(bitmap_layer));
	if (persist_storage_working)
	{
		text_layer = text_layer_create((GRect) { .origin = { 0, 60 }, .size = { bounds.size.w, 40 } });
		text_layer_set_text(text_layer, "Shake/Press to Flip");
	}
	else
	/*
	 * If persistant storage is broken, we want to inform the user that this is so.
	 * However, since this app can still function without it, we still allow them to carry on
	 * after displaying the error.
	 */
	{
		
		text_layer = text_layer_create((GRect){.origin = {0,0}, .size = {bounds.size.w, 144}});
		text_layer_set_text(text_layer, "Persistent Storage is Broken. Please Factory Reset Watch. Visit bit.ly/ urbpebstorage for more information.");
	}
	text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);
	text_layer_set_font(text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
	layer_add_child(window_layer, text_layer_get_layer(text_layer));
}
/*
 * Function for cleaning up when the window (and app) close.
 */
static void window_unload(Window *window) 
{
	text_layer_destroy(text_layer);
	gbitmap_destroy(heads_bitmap);
	gbitmap_destroy(tails_bitmap);
	bitmap_layer_destroy(bitmap_layer);
}

/*
 * Init. Start everything here.
 */
static void init(void) 
{
	// Very first thing we want to do is test that persist storage works.
	test_persist_storage();
	window = window_create();
	window_set_window_handlers(window, (WindowHandlers) 
	{
		.load = window_load,
		.unload = window_unload,
	});
	window_set_click_config_provider(window, click_config_provider);
	// We want a different random order every time, so we seed the random function with the current time.
	srand(time(NULL));
	accel_tap_service_subscribe(accel_tap_handler);
	app_message_init();
	window_stack_push(window, true);
}

/*
 * Deinit. After window_unload is called, we want to also destroy the window. No memory leaks!
 */
static void deinit(void) 
{
	window_destroy(window);
}

/*
 * Main!
 */
int main(void) 
{
	init();
	app_event_loop();
	deinit();
}
