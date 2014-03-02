#include <pebble.h>

static Window *window;
static TextLayer *text_time_layer;
static BitmapLayer *doge_layer;
static GBitmap *doge;
static TextLayer *tick_tock_layer;
static int tick;



void handle_second_tick(struct tm *ticktime, TimeUnits units_changed){
  static char time_text[]="<00,00,00>";
  strftime(time_text,sizeof(time_text),"<%H,%M,%S>", ticktime);
  text_layer_set_text(text_time_layer,time_text);
  if(tick){
    text_layer_set_text(tick_tock_layer,"such tick");
    text_layer_set_text_alignment(tick_tock_layer,GTextAlignmentLeft);

    tick = 0;
  } else{
    text_layer_set_text(tick_tock_layer,"very tock");
    text_layer_set_text_alignment(tick_tock_layer,GTextAlignmentRight);
    tick = 1;
  }
}

static void init(void) {
  window = window_create();
  window_stack_push(window, true);
  Layer *window_layer = window_get_root_layer(window);
  window_set_background_color(window,GColorBlack);

  text_time_layer = text_layer_create(GRect(0,0,144,40));
  text_layer_set_text_color(text_time_layer,GColorWhite);
  text_layer_set_background_color(text_time_layer,GColorClear);
  text_layer_set_font(text_time_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28));
  text_layer_set_text_alignment(text_time_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(text_time_layer));
  tick_timer_service_subscribe(SECOND_UNIT,handle_second_tick);

  doge = gbitmap_create_with_resource(RESOURCE_ID_DOGE);
  doge_layer = bitmap_layer_create(GRect(0,40,144,168-40));
  bitmap_layer_set_bitmap(doge_layer,doge);
  layer_add_child(window_layer,bitmap_layer_get_layer(doge_layer));

  tick_tock_layer = text_layer_create(GRect(0,90,144,20));
  text_layer_set_text_color(tick_tock_layer, GColorBlack);
  text_layer_set_background_color(tick_tock_layer,GColorClear);
  text_layer_set_font(tick_tock_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
  text_layer_set_text_alignment(tick_tock_layer,GTextAlignmentLeft);
  layer_add_child(window_layer,text_layer_get_layer(tick_tock_layer));
  tick = 1;


}

static void deinit(void) {
  tick_timer_service_unsubscribe();
  window_destroy(window);
}

int main(void) {
  init();

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Done initializing, pushed window: %p", window);

  app_event_loop();
  deinit();
}
