#include <pebble.h>

static Window *window;
static TextLayer *text_time_layer;
static BitmapLayer *doge_layer;
static GBitmap *doge;
static TextLayer *tick_tock_layer;
static TextLayer *price_ticker_layer;
static int tick;
char *dogePrice;
char *dogePricePrev;
int dogeComp;


void handle_second_tick(struct tm *ticktime, TimeUnits units_changed){
  static char time_text[]="00:00:00";
  strftime(time_text,sizeof(time_text),"%H:%M:%S", ticktime);
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
  if(units_changed >= MINUTE_UNIT){
    DictionaryIterator *iter;
    app_message_outbox_begin(&iter);
    app_message_outbox_send();
  }
}
 void out_sent_handler(DictionaryIterator *sent, void *context) {
   // outgoing message was delivered
 }


 void out_failed_handler(DictionaryIterator *failed, AppMessageResult reason, void *context) {
   text_layer_set_text(price_ticker_layer, "ERROR OUT!");
 }


 void in_received_handler(DictionaryIterator *received, void *context) {
    Tuple *result = dict_read_first(received);
    dogePricePrev = dogePrice;
    dogePrice = result->value->cstring;
    dogeComp = strcmp(dogePrice, dogePricePrev);
    if(dogeComp > 0){
      strcat(dogePrice,"^");
    } 
    else if(dogeComp < 0){
      strcat(dogePrice,"v");
    }
    else{
      strcat(dogePrice,"-");
    }
    text_layer_set_text(price_ticker_layer, dogePrice);
    APP_LOG(APP_LOG_LEVEL_DEBUG, "%s", dogePrice);
 }


 void in_dropped_handler(AppMessageResult reason, void *context) {
   text_layer_set_text(price_ticker_layer, "ERROR IN!");
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

  doge = gbitmap_create_with_resource(RESOURCE_ID_DOGE);
  doge_layer = bitmap_layer_create(GRect(0,40,144,128));
  bitmap_layer_set_bitmap(doge_layer,doge);
  layer_add_child(window_layer,bitmap_layer_get_layer(doge_layer));

  tick_tock_layer = text_layer_create(GRect(0,90,144,20));
  text_layer_set_text_color(tick_tock_layer, GColorBlack);
  text_layer_set_background_color(tick_tock_layer,GColorClear);
  text_layer_set_font(tick_tock_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
  text_layer_set_text_alignment(tick_tock_layer,GTextAlignmentLeft);
  layer_add_child(window_layer,text_layer_get_layer(tick_tock_layer));
  tick = 1;

  price_ticker_layer = text_layer_create(GRect(0,148,144,20));
  text_layer_set_text_color(price_ticker_layer, GColorBlack);
  text_layer_set_background_color(price_ticker_layer, GColorClear);
  text_layer_set_font(price_ticker_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
  layer_add_child(window_layer,text_layer_get_layer(price_ticker_layer));

  tick_timer_service_subscribe(SECOND_UNIT,handle_second_tick);

  app_message_register_inbox_received(in_received_handler);
  app_message_register_inbox_dropped(in_dropped_handler);
  app_message_register_outbox_sent(out_sent_handler);
  app_message_register_outbox_failed(out_failed_handler);

  const uint32_t inbound_size = app_message_inbox_size_maximum();
  const uint32_t outbound_size = app_message_outbox_size_maximum();
  app_message_open(inbound_size, outbound_size);
  dogePrice = (char *)malloc(sizeof(char)*11);
  dogePrice="0.00000000-";
  dogePricePrev=(char *)malloc(sizeof(char)*11);
  dogePricePrev = dogePrice;
}

static void deinit(void) {
  tick_timer_service_unsubscribe();
  window_destroy(window);
  free(dogePrice);
  free(dogePricePrev);
}



int main(void) {
  init();

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Done initializing, pushed window: %p", window);

  app_event_loop();
  deinit();
}
