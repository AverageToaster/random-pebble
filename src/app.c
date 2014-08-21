#include "pebble.h"
#include "common.h"
#include "windows/window-error.h"
#include "windows/window-instructions.h"
#include "windows/window-app.h"
#include "windows/window-welcome.h"


static void init(){
	srand(time(NULL));
	common_init();
	bool is_persist_working = persist_working();
	window_instructions_init();
	window_app_init();
	window_app_show();
	if (is_persist_working && is_first_start()){
		window_welcome_init();
		window_instructions_show();
		window_welcome_show();
		persist_write_bool(FIRST_TIME, false);
	}
	if (!is_persist_working){
		window_error_init();
		window_error_set_text(
			"Error. Persist storage is broken.\nPlease factory reset device.\n\nPlease see bit.ly/ urpebstorage\nfor more information.");
		window_error_show();
	}
}

static void deinit(){
	common_deinit();
	window_app_deinit();
	window_error_deinit();
	window_instructions_deinit();
}

int main(){
	init();
	app_event_loop();
	deinit();
}