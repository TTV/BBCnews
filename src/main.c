#include <pebble.h>

// sent keys
enum {
  TTVNEWS_METHOD = 0x0
};

// receive keys
enum {
  TTVNEWS_TITLE = 0x0
};

// method types
enum {
  TTVNEWS_METHOD_READ = 0x0
};

static char title[128];
	
Window *my_window;
TextLayer *text_layer;

static void fetch_msg(void) {
  Tuplet symbol_tuple = TupletInteger(TTVNEWS_METHOD, TTVNEWS_METHOD_READ);

  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);

  if (iter == NULL) {
    return;
  }

  dict_write_tuplet(iter, &symbol_tuple);
  dict_write_end(iter);

  app_message_outbox_send();
}

static void in_received_handler(DictionaryIterator *iter, void *context) {
  Tuple *title_tuple = dict_find(iter, TTVNEWS_TITLE);

  if (title_tuple) {
    strncpy(title, title_tuple->value->cstring, 128);
    text_layer_set_text(text_layer, title);
  }
}

static void out_failed_handler(DictionaryIterator *failed, AppMessageResult reason, void *context) {
  snprintf(title, 128, "Fail [%d]", reason);
  text_layer_set_text(text_layer, title);
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
  app_message_open(app_message_inbox_size_maximum(), 64);
  fetch_msg();
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
