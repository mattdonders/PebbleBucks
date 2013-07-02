
#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"
	
#include "config.h"	
#include "http.h"
#include "util.h"
#include "mini-printf.h"
	
static uint8_t IMAGES[] = {
	RESOURCE_ID_SBUXBARCODE,
	RESOURCE_ID_SBUXSTARS,
	RESOURCE_ID_SBUXREWARDS,
};

// Define Universal Unique Identifier
// 9141B628-BC89-498E-B147--D65A01B823C2
#define MY_HTTP_UUID { 0x91, 0x41, 0xB6, 0x28, 0xBC, 0x89, 0x49, 0x8E, 0xB1, 0x47, 0xD6, 0x5A, 0x01, 0xB8, 0x23, 0xC2 }
PBL_APP_INFO(MY_HTTP_UUID,
             "Pebble Bucks", "Matt Donders",
             1, 0, /* App version */
             DEFAULT_MENU_ICON,
             APP_INFO_WATCH_FACE);

#define HTTP_COOKIE 1949327671

// Posted variables
#define KEY_USERNAME 1
#define KEY_PASSWORD 2

// Received variables
#define KEY_BALANCE_DLR 1
#define KEY_BALANCE_CNTS 2
#define KEY_STARS 3
#define KEY_REWARDS 4
#define KEY_ERRORS 5

	
Window window;
BmpContainer barcodeLayer;
TextLayer balanceLayer;
TextLayer starsLayer;
BmpContainer starsImageLayer;
TextLayer rewardsLayer;
BmpContainer rewardsImageLayer;

char strBalance[10];
char strDollar[5];
char strCents[5];
char strStars[5];
char strRewards[5];
char strError[5];

// HTTPebble Initialization
void request_starbucks();

void failed(int32_t cookie, int http_status, void* context) {
	if(cookie == HTTP_COOKIE) {
		text_layer_set_text(&balanceLayer, "COOKIE 0 | HTTP FAILED");
	}
	else if(cookie == HTTP_COOKIE) {
		text_layer_set_text(&balanceLayer, "COOKIE GOOD | HTTP FAILED");
	}
}

void success(int32_t cookie, int http_status, DictionaryIterator* received, void* context) {
	if(cookie != HTTP_COOKIE) {
		text_layer_set_text(&balanceLayer, "COOKIE BAD | SUCCESS");
		return;
	}
	
	text_layer_set_text(&balanceLayer, "Cookie good - waiting for result...");
	
	Tuple *balance_dollar_tuple = dict_find(received, KEY_BALANCE_DLR);
	Tuple *balance_cents_tuple = dict_find(received, KEY_BALANCE_CNTS);
	Tuple *stars_tuple = dict_find(received, KEY_STARS);
	Tuple *rewards_tuple = dict_find(received, KEY_REWARDS);
	Tuple *errors_tuple = dict_find(received, KEY_ERRORS);
	
	if (errors_tuple) {
		uint16_t intError = errors_tuple->value->int16;
		
		if (intError == 999) {
			text_layer_set_font(&balanceLayer, fonts_get_system_font(FONT_KEY_GOTHIC_18));
			text_layer_set_text(&balanceLayer, "Invalid Login");
		}
		else {
			mini_snprintf(strError, 5, ".%d", intError);
			text_layer_set_text(&balanceLayer, strError);
		}
		
		return;	
	}
	
	if(balance_dollar_tuple && balance_cents_tuple) {
		uint8_t intDollar = balance_dollar_tuple->value->int8;
		uint8_t intCents = balance_cents_tuple->value->int8;
		
		mini_snprintf(strDollar, 5, "$%d", intDollar);
		mini_snprintf(strCents, 5, ".%d", intCents);
		
		strncpy(strBalance, strDollar, sizeof(strBalance));
		strncat(strBalance, strCents, (sizeof(strBalance) - strlen(strBalance)) );
		
		text_layer_set_text(&balanceLayer, strBalance);
	}
	
	if(stars_tuple) {
		uint8_t intStars = stars_tuple->value->int8;
		//mini_snprintf(strStars, 20, "Stars: %d", intStars);
		mini_snprintf(strStars, 5, "%d", intStars);
		//text_layer_set_text(&starsLayer, "S - YES!");
		text_layer_set_text(&starsLayer, strStars);
	}

	if(rewards_tuple) {
		uint8_t intRewards = rewards_tuple->value->int8;
		//mini_snprintf(strRewards, 20, "Rewards: %d", intRewards);
		mini_snprintf(strRewards, 5, "%d", intRewards);
		//text_layer_set_text(&rewardsLayer, "R - YES!");
		text_layer_set_text(&rewardsLayer, strRewards);
	}	
	
}

void reconnect(void* context) {
  request_starbucks();
}


// Standard app initialisation

void handle_init(AppContextRef ctx) {
  (void)ctx;

  window_init(&window, "Pebble Window");
  window_stack_push(&window, true /* Animated */);
	
  resource_init_current_app(&APP_RESOURCES);

/*
  text_layer_init(&barcodeLayer, GRect(0, 0, 144, 66));
  text_layer_set_text(&barcodeLayer, "BARCODE");
  text_layer_set_font(&barcodeLayer, fonts_get_system_font(FONT_KEY_DROID_SERIF_28_BOLD));
  text_layer_set_text_alignment(&barcodeLayer, GTextAlignmentCenter);
  layer_add_child(&window.layer, &barcodeLayer.layer);
*/

  // Barcode Image Layer
  bmp_init_container(IMAGES[0], &barcodeLayer);
  layer_set_frame(&barcodeLayer.layer.layer, GRect(2, 2, 140, 66));
  layer_add_child(&window.layer, &barcodeLayer.layer.layer);
	
  // Balance Text Layer
  text_layer_init(&balanceLayer, GRect(0, 67, 144, 51));
  text_layer_set_text(&balanceLayer, "BALANCE");
  text_layer_set_font(&balanceLayer, fonts_get_system_font(FONT_KEY_GOTHAM_30_BLACK));
  text_layer_set_text_alignment(&balanceLayer, GTextAlignmentCenter);
  layer_add_child(&window.layer, &balanceLayer.layer);
	
	
  // Stars Image & Text Layers
  bmp_init_container(IMAGES[1], &starsImageLayer);
  layer_set_frame(&starsImageLayer.layer.layer, GRect(7, 117, 34, 51));
  layer_add_child(&window.layer, &starsImageLayer.layer.layer);
	
  text_layer_init(&starsLayer, GRect(36, 130, 36, 51));
  text_layer_set_text(&starsLayer, "STARS");
  text_layer_set_font(&starsLayer, fonts_get_system_font(FONT_KEY_GOTHIC_18));
  text_layer_set_text_alignment(&starsLayer, GTextAlignmentCenter);
  layer_add_child(&window.layer, &starsLayer.layer);
	
	
  // Rewards Image & Text Layers
  bmp_init_container(IMAGES[2], &rewardsImageLayer);
  layer_set_frame(&rewardsImageLayer.layer.layer, GRect(77, 117, 34, 51));
  layer_add_child(&window.layer, &rewardsImageLayer.layer.layer);
	
  text_layer_init(&rewardsLayer, GRect(108, 130, 36, 51));
  text_layer_set_text(&rewardsLayer, "REWARDS");
  text_layer_set_font(&rewardsLayer, fonts_get_system_font(FONT_KEY_GOTHIC_18));
  text_layer_set_text_alignment(&rewardsLayer, GTextAlignmentCenter);
  layer_add_child(&window.layer, &rewardsLayer.layer);

  // HTTP Parameter Setup
  http_set_app_id(34345157);
  http_register_callbacks((HTTPCallbacks){
		.failure=failed,
		.success=success,
		.reconnect=reconnect
	}, (void*)ctx);
	
	request_starbucks();
}

void pbl_main(void *params) {
  PebbleAppHandlers handlers = {
    .init_handler = &handle_init,
	.messaging_info = {
			.buffer_sizes = {
				.inbound = 124,
				.outbound = 256,
			}
		},
  };
  app_event_loop(params, &handlers);
}

void request_starbucks() {
	// Build the HTTP request
	DictionaryIterator *body;
	HTTPResult result = http_out_get(SBUXURL, HTTP_COOKIE, &body);
	if(result == HTTP_BUSY) {
		text_layer_set_text(&balanceLayer, "ERROR - HTTP OUT BUSY");
	}
	else if(result != HTTP_OK) {
		//text_layer_set_text(&balanceLayer, (char* )&result);
		text_layer_set_text(&balanceLayer, "ERROR - HTTP OUT GET");
		//return; // Don't care - send it anyway.
	}
	
	dict_write_cstring(body, KEY_USERNAME, SBUXUSER);
	dict_write_cstring(body, KEY_PASSWORD, SBUXPASS);

	// Now that it's built, send it.
	if(http_out_send() != HTTP_OK) {
		text_layer_set_text(&balanceLayer, "ERROR - HTTP OUT SEND");
		text_layer_set_text(&rewardsLayer, "N/A");
		text_layer_set_text(&starsLayer, "N/A");
		//return; // Don't care - want to test it anyway.
	}
	else {
		text_layer_set_text(&balanceLayer, "Wait...");
		text_layer_set_text(&rewardsLayer, "...");
		text_layer_set_text(&starsLayer, "...");
	}
}