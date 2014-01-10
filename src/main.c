#include <pebble.h>

// sent keys
enum {
  TTVNEWS_METHOD = 0x0
};

// receive keys
enum {
  TTVNEWS_TITLE = 0x1
};

static char title[128];
	
Window *my_window;
TextLayer *text_layer;

bool refetch_story = false;

static void fetch_story(void) {
  refetch_story = false;

  Tuplet method_tuple = TupletCString(TTVNEWS_METHOD, "story");
	
  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);

  if (iter == NULL) {
    return;
  }

  dict_write_tuplet(iter, &method_tuple);
  dict_write_end(iter);

  app_message_outbox_send();
}

static void in_received_handler(DictionaryIterator *iter, void *context) {
  Tuple *method_tuple = dict_find(iter, TTVNEWS_METHOD);

  if (!method_tuple) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Missing method");
    return;
  }

  // APP_LOG(APP_LOG_LEVEL_DEBUG, "Method : %s", method_tuple->value->cstring);	
	
  if (strcmp(method_tuple->value->cstring, "ready") == 0)
	  refetch_story = true; // first msg fails so re-fetch on the fail handler
  else if (strcmp(method_tuple->value->cstring, "story") == 0) {
	  Tuple *title_tuple = dict_find(iter, TTVNEWS_TITLE);
		
	  if (title_tuple) {
		strncpy(title, title_tuple->value->cstring, 128);
		text_layer_set_text(text_layer, title);
	  } else {
		snprintf(title, 128, "Missing title");
		text_layer_set_text(text_layer, title);	  
	  }		
  }
}

static void out_failed_handler(DictionaryIterator *failed, AppMessageResult reason, void *context) {
  if (refetch_story)
    fetch_story(); // first call which triggers ready always fails
  else
  	APP_LOG(APP_LOG_LEVEL_DEBUG, "out_failed_handler %d", reason);
}

void handle_init(void) {
  my_window = window_create();
  Layer *window_layer = window_get_root_layer(my_window);
  GRect bounds = layer_get_bounds(window_layer);
  text_layer = text_layer_create(bounds);
  text_layer_set_text(text_layer, "Loading...");
  layer_add_child(window_layer, text_layer_get_layer(text_layer));
  window_stack_push(my_window, false);

  app_message_register_inbox_received(in_received_handler);
  app_message_register_outbox_failed(out_failed_handler);

  app_message_open(64, 64);

  fetch_story();
}

void handle_deinit(void) {
  text_layer_destroy(text_layer);
  window_destroy(my_window);
}

int main(void) {
  handle_init();
  app_event_loop();
  handle_deinit();
}
