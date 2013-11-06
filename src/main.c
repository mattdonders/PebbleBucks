
#include <pebble_os.h>
#include <pebble_app.h>
#include <pebble_fonts.h>
	
#include "config.h"
#include "util.h"
#include "mini-printf.h"


enum StarbucksKey {
  KEY_USERNAME,   // TUPLE_CSTRING
  KEY_PASSWORD,   // TUPLE_CSTRING
  KEY_BALANCE,    // TUPLE_CSTRING
  KEY_STARS,      // TUPLE_INT
  KEY_REWARDS,    // TUPLE_INT
  KEY_ERROR       // TUPLE_INT
} StarbucksKey;


#define kAppSyncInBuffSize  124
#define kAppSyncOutBuffSize 256
static AppSync sync;
static uint8_t sync_buffer[kAppSyncOutBuffSize];

#define kSyncValueEmpty     ((const char *)"")
#define kSyncValueLoading   ((const char *)"...")

char strStars[5];
char strRewards[5];
char strError[5];

Window *window;
BitmapLayer *barcodeLayer;
GBitmap *barcodeBitmap;
TextLayer *balanceLayer;
TextLayer *starsLayer;
BitmapLayer *starsImageLayer;
GBitmap *starsBitmap;
TextLayer *rewardsLayer;
BitmapLayer *rewardsImageLayer;
GBitmap *rewardsBitmap;

// HTTPebble Initialization
void request_starbucks();

static void sync_error_callback(DictionaryResult dict_error, AppMessageResult app_message_error, void *context)
{
  APP_LOG(APP_LOG_LEVEL_DEBUG, "App Message Sync Error: %d", app_message_error);
}

static void sync_tuple_changed_callback(const uint32_t key, const Tuple* new_tuple, const Tuple* old_tuple, void* context)
{
  switch (new_tuple->key) {
    case KEY_BALANCE:
      APP_LOG(APP_LOG_LEVEL_DEBUG, "KEY_BALANCE: %s", new_tuple->value->cstring);
      text_layer_set_text(balanceLayer, new_tuple->value->cstring);
      break;
    case KEY_STARS:
      {
        uint8_t intStars = new_tuple->value->int8;
        APP_LOG(APP_LOG_LEVEL_DEBUG, "KEY_STARS: %d", intStars);
        
        mini_snprintf(strStars, 5, "%d", intStars);
        
        text_layer_set_text(starsLayer, strStars);
      }
      break;
    case KEY_REWARDS:
      {
        uint8_t intRewards = new_tuple->value->int8;
        APP_LOG(APP_LOG_LEVEL_DEBUG, "KEY_REWARDS: %d", intRewards);
        
        mini_snprintf(strRewards, 5, "%d", intRewards);
        
        text_layer_set_text(rewardsLayer, strRewards);
      }
      break;
    case KEY_ERROR:
      {
        uint8_t intError = new_tuple->value->int8;
        APP_LOG(APP_LOG_LEVEL_DEBUG, "KEY_ERROR: %d", intError);
        
        mini_snprintf(strError, 5, "%d", intError);
        
        text_layer_set_text(balanceLayer, strError);
      }
      break;
    default:
      break;
  }
}

void request_starbucks()
{
  DictionaryIterator *iter;
  Tuplet user = TupletCString(KEY_USERNAME, SBUXUSER);
  Tuplet pass = TupletCString(KEY_PASSWORD, SBUXPASS);
  app_message_out_get(&iter);

  if (iter == NULL) {
    return;
  }

  dict_write_tuplet(iter, &user);
  dict_write_tuplet(iter, &pass);
  dict_write_end(iter);

  app_message_out_send();
  app_message_out_release();
}

void select_single_click_handler(ClickRecognizerRef recognizer, void *context)
{
  request_starbucks();
}

// Click Handler Config
// Only setup select single click for now
void click_config_provider(ClickConfig **config, Window *window)
{
  (void)window;

  config[BUTTON_ID_SELECT]->click.handler = (ClickHandler) select_single_click_handler;
	
/*
  config[BUTTON_ID_SELECT]->long_click.handler = (ClickHandler) select_long_click_handler;

  config[BUTTON_ID_UP]->click.handler = (ClickHandler) up_single_click_handler;
  config[BUTTON_ID_UP]->click.repeat_interval_ms = 100;

  config[BUTTON_ID_DOWN]->click.handler = (ClickHandler) down_single_click_handler;
  config[BUTTON_ID_DOWN]->click.repeat_interval_ms = 100;
*/
}


// Standard app initialisation

void window_load(Window *window)
{
  Layer *window_layer = window_get_root_layer(window);
	
  // Attach our desired button functionality
  window_set_click_config_provider(window, (ClickConfigProvider) click_config_provider);

  // Barcode Image Layer
  barcodeLayer = bitmap_layer_create(GRect(2, 2, 140, 66));//(GRect) { .origin = { 2, 2 }, .size = { 140, 66 }});
  barcodeBitmap = gbitmap_create_with_resource(RESOURCE_ID_SBUXBARCODE);
  bitmap_layer_set_bitmap(barcodeLayer, barcodeBitmap);
  layer_add_child(window_layer, bitmap_layer_get_layer(barcodeLayer));
	
  // Balance Text Layer
  balanceLayer = text_layer_create(GRect(0, 67, 144, 51));
  text_layer_set_text(balanceLayer, "BALANCE");
  text_layer_set_font(balanceLayer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
  text_layer_set_text_alignment(balanceLayer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(balanceLayer));
	
	
  // Stars Image & Text Layers
  starsImageLayer = bitmap_layer_create(GRect(7, 117, 34, 51));
  starsBitmap = gbitmap_create_with_resource(RESOURCE_ID_SBUXSTARS);
  bitmap_layer_set_bitmap(starsImageLayer, starsBitmap);
  layer_add_child(window_layer, bitmap_layer_get_layer(starsImageLayer));
	
  starsLayer = text_layer_create(GRect(36, 130, 36, 51));
  text_layer_set_text(starsLayer, "STARS");
  text_layer_set_font(starsLayer, fonts_get_system_font(FONT_KEY_GOTHIC_18));
  text_layer_set_text_alignment(starsLayer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(starsLayer));
	
	
  // Rewards Image & Text Layers
  
  rewardsImageLayer = bitmap_layer_create(GRect(77, 117, 34, 51));
  rewardsBitmap = gbitmap_create_with_resource(RESOURCE_ID_SBUXREWARDS);
  bitmap_layer_set_bitmap(rewardsImageLayer, rewardsBitmap);
  layer_add_child(window_layer, bitmap_layer_get_layer(rewardsImageLayer));
	
  rewardsLayer = text_layer_create(GRect(108, 130, 36, 51));
  text_layer_set_text(rewardsLayer, "REWARDS");
  text_layer_set_font(rewardsLayer, fonts_get_system_font(FONT_KEY_GOTHIC_18));
  text_layer_set_text_alignment(rewardsLayer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(rewardsLayer));
}

void window_unload(Window *window)
{
  bitmap_layer_destroy(barcodeLayer);
  bitmap_layer_destroy(starsImageLayer);
  bitmap_layer_destroy(rewardsImageLayer);
  
  gbitmap_destroy(barcodeBitmap);
  gbitmap_destroy(starsBitmap);
  gbitmap_destroy(rewardsBitmap);

  text_layer_destroy(balanceLayer);
  text_layer_destroy(starsLayer);
  text_layer_destroy(rewardsLayer);
}

void init()
{
  window = window_create();
  window_set_fullscreen(window, true);
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  const bool animated = true;

  Tuplet initial_values[] = {
    TupletCString(KEY_USERNAME, kSyncValueEmpty),
    TupletCString(KEY_PASSWORD, kSyncValueEmpty),
    TupletCString(KEY_BALANCE,  kSyncValueLoading),
    TupletInteger(KEY_STARS,    0),
    TupletInteger(KEY_REWARDS,  0),
    TupletInteger(KEY_ERROR,    0)
  };
    
  app_message_open(kAppSyncInBuffSize, kAppSyncOutBuffSize);
  request_starbucks();
    
  app_sync_init(&sync, sync_buffer, sizeof(sync_buffer), initial_values, ARRAY_LENGTH(initial_values),
                sync_tuple_changed_callback, sync_error_callback, NULL);
    
  window_stack_push(window, animated);
}

void deinit()
{
  app_sync_deinit(&sync);
    
  window_destroy(window);
}

int main(void)
{
  init();
  
  app_event_loop();
  
  deinit();
}

