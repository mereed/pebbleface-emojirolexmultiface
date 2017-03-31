/*

Copyright (C) 2017 Mark Reed

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), 
to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, 
and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

-------------------------------------------------------------------

*/

#include <pebble.h>
#include "main.h"
#include "hands.h"

#define ANTIALIASING true

//WINDOW
Window* window;
//LAYER
Layer* layer;

//BACKGROUND
static GBitmap* background;
static BitmapLayer *background_layer;   

//SETTINGS
bool seconds = true;

TextLayer *text_daynum_layer;
TextLayer *layer_time_text;
TextLayer *battery_text_layer;

int charge_percent = 0;

int cur_day = -1;

static GBitmap *bluetooth_image;
static BitmapLayer *bluetooth_layer;   

// A struct for our specific settings (see main.h)
ClaySettings settings;

int stringToInt(char *str);

static GPath *s_minute_arrow, *s_hour_arrow, *s_second_arrow;

static GFont font1;

// Initialize the default settings
static void prv_default_settings() {	
  settings.background = 0;
  settings.date = true;
  settings.secs = true;
  settings.batt = true;
  settings.hrcol = GColorWhite;
  settings.mincol = GColorWhite;
  settings.seccol = GColorRed;
//  settings.center = GColorWhite;
}

static void tick(struct tm *tick_time, TimeUnits units_changed);

void update_layer(Layer *me, GContext* ctx) {
	
//watchface drawing
	
	//get tick_time
	time_t temp = time(NULL); 
  	struct tm *tick_time = localtime(&temp);
	
	//draw hands
#ifdef PBL_PLATFORM_CHALK
	GPoint center = GPoint(90,90);
#else
	GPoint center = GPoint(72,84);
#endif
    graphics_context_set_antialiased(ctx, ANTIALIASING);

// second hand

	if((settings.secs) && (seconds == 1)) {
	
		tick_timer_service_subscribe(SECOND_UNIT, tick);
		graphics_context_set_antialiased(ctx, ANTIALIASING);
		graphics_context_set_stroke_width(ctx, 1);
		graphics_context_set_stroke_color(ctx, settings.seccol);
		gpath_draw_outline(ctx, s_second_arrow);
		graphics_context_set_fill_color(ctx, settings.seccol);
		gpath_draw_filled(ctx, s_second_arrow);
		gpath_rotate_to(s_second_arrow, TRIG_MAX_ANGLE * tick_time->tm_sec / 60);
				
	} else {
		tick_timer_service_subscribe(MINUTE_UNIT, tick);
	}
	
// minute hand
	
	//black outline
	graphics_context_set_stroke_width(ctx, 5);
	graphics_context_set_stroke_color(ctx, GColorBlack);
	gpath_draw_outline(ctx, s_minute_arrow);
	
    // white hand
	graphics_context_set_stroke_width(ctx, 3);
	graphics_context_set_stroke_color(ctx, settings.mincol);
	gpath_draw_outline(ctx, s_minute_arrow);

	graphics_context_set_fill_color(ctx, settings.mincol);
	gpath_draw_filled(ctx, s_minute_arrow);
	gpath_rotate_to(s_minute_arrow, TRIG_MAX_ANGLE * tick_time->tm_min / 60);

// hour hand
    graphics_context_set_stroke_width(ctx, 4);
	graphics_context_set_stroke_color(ctx, GColorBlack);
	gpath_draw_outline(ctx, s_hour_arrow);	
	
    graphics_context_set_stroke_width(ctx, 2);
	graphics_context_set_stroke_color(ctx, settings.hrcol);
	gpath_draw_outline(ctx, s_hour_arrow);
	
	graphics_context_set_fill_color(ctx, settings.hrcol);
	gpath_draw_filled(ctx, s_hour_arrow);
	gpath_rotate_to(s_hour_arrow, (TRIG_MAX_ANGLE * (((tick_time->tm_hour % 12) * 6) + (tick_time->tm_min / 10))) / (12 * 6)); // from Pebble SDK example

	
// center circles
	graphics_context_set_fill_color(ctx, GColorBlack);
	graphics_fill_circle(ctx, center, 6);
	graphics_context_set_fill_color(ctx, GColorWhite);
	graphics_fill_circle(ctx, center, 5);
	graphics_context_set_fill_color(ctx, GColorBlack);
	graphics_fill_circle(ctx, center, 3);	
	graphics_context_set_fill_color(ctx, GColorWhite);
	graphics_fill_circle(ctx, center, 2);	
	
}

// Read settings from persistent storage
static void prv_load_settings() {
  // Load the default settings
  prv_default_settings();
  // Read settings from persistent storage, if they exist
  persist_read_data(SETTINGS_KEY, &settings, sizeof(settings));
}

// Save the settings to persistent storage
static void prv_save_settings() {
  persist_write_data(SETTINGS_KEY, &settings, sizeof(settings));
  // Update the display based on new settings
  prv_update_display();
}

// Update the display elements
static void prv_update_display() {
	
	if (settings.batt){
		  layer_set_hidden(text_layer_get_layer(battery_text_layer), false);
	} else {
		  layer_set_hidden(text_layer_get_layer(battery_text_layer), true);		
	}
	
	if (settings.date){
		  layer_set_hidden(text_layer_get_layer(text_daynum_layer), false);
	} else {
		  layer_set_hidden(text_layer_get_layer(text_daynum_layer), true);		
	}
	
	switch(settings.background) {
		
	case 0:  // rolex explorer shaded

	    if (background) {
		gbitmap_destroy(background);
		background = NULL;
	    }
		background = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BACKGROUND);
		
 	    if (background != NULL) {
		bitmap_layer_set_bitmap(background_layer, background);
		layer_set_hidden(bitmap_layer_get_layer(background_layer), false);
		layer_mark_dirty(bitmap_layer_get_layer(background_layer));
        }
#ifdef PBL_PLATFORM_CHALK
		layer_set_frame(bitmap_layer_get_layer(bluetooth_layer), GRect(82, 35, 16,18));
#else
		layer_set_frame(bitmap_layer_get_layer(bluetooth_layer), GRect(64, 30, 16,18));
#endif
		break;		
		
	case 1:  // rolex explorer plain black
		
		if (background) {
		gbitmap_destroy(background);
		background = NULL;
	    }
		background = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BACKGROUND1);
		
 	    if (background != NULL) {
		bitmap_layer_set_bitmap(background_layer, background);
		layer_set_hidden(bitmap_layer_get_layer(background_layer), false);
		layer_mark_dirty(bitmap_layer_get_layer(background_layer));
		}
#ifdef PBL_PLATFORM_CHALK
		layer_set_frame(bitmap_layer_get_layer(bluetooth_layer), GRect(82, 35, 16,18));
#else
		layer_set_frame(bitmap_layer_get_layer(bluetooth_layer), GRect(64, 30, 16,18));
#endif
		break;		
	
    case 2:  // rolex explorer II
		
		if (background) {
		gbitmap_destroy(background);
		background = NULL;
	    }
		background = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BACKGROUND5);
		
 	    if (background != NULL) {
		bitmap_layer_set_bitmap(background_layer, background);
		layer_set_hidden(bitmap_layer_get_layer(background_layer), false);
		layer_mark_dirty(bitmap_layer_get_layer(background_layer));
		}
#ifdef PBL_PLATFORM_CHALK
		layer_set_frame(bitmap_layer_get_layer(bluetooth_layer), GRect(82, 42, 16,18));
#else
		layer_set_frame(bitmap_layer_get_layer(bluetooth_layer), GRect(64, 30, 16,18));
#endif
		break;		

    case 3:  // rolex submariner
		
		if (background) {
		gbitmap_destroy(background);
		background = NULL;
	    }
		background = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BACKGROUND3);
		
 	    if (background != NULL) {
		bitmap_layer_set_bitmap(background_layer, background);
		layer_set_hidden(bitmap_layer_get_layer(background_layer), false);
		layer_mark_dirty(bitmap_layer_get_layer(background_layer));
		}
#ifdef PBL_PLATFORM_CHALK
		layer_set_frame(bitmap_layer_get_layer(bluetooth_layer), GRect(82, 36, 16,18));
#else
		layer_set_frame(bitmap_layer_get_layer(bluetooth_layer), GRect(64, 30, 16,18));
#endif
		break;		

	case 4:  // rolex commando
		
		if (background) {
		gbitmap_destroy(background);
		background = NULL;
	    }
		background = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BACKGROUND4);
		
 	    if (background != NULL) {
		bitmap_layer_set_bitmap(background_layer, background);
		layer_set_hidden(bitmap_layer_get_layer(background_layer), false);
		layer_mark_dirty(bitmap_layer_get_layer(background_layer));
		}
#ifdef PBL_PLATFORM_CHALK
		layer_set_frame(bitmap_layer_get_layer(bluetooth_layer), GRect(82, 13, 16,18));
#else
		layer_set_frame(bitmap_layer_get_layer(bluetooth_layer), GRect(64, 13, 16,18));
#endif
		break;		


		
	case 5:  // rolex date
		
		if (background) {
		gbitmap_destroy(background);
		background = NULL;
	    }
		background = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BACKGROUND6);
		
 	    if (background != NULL) {
		bitmap_layer_set_bitmap(background_layer, background);
		layer_set_hidden(bitmap_layer_get_layer(background_layer), false);
		layer_mark_dirty(bitmap_layer_get_layer(background_layer));
		}
#ifdef PBL_PLATFORM_CHALK
		layer_set_frame(bitmap_layer_get_layer(bluetooth_layer), GRect(82, 16, 16,18));
#else
		layer_set_frame(bitmap_layer_get_layer(bluetooth_layer), GRect(64, 14, 16,18));
#endif
		break;		

    case 6:  // rolex sea dweller
		
		if (background) {
		gbitmap_destroy(background);
		background = NULL;
	    }
		background = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BACKGROUND7);
		
 	    if (background != NULL) {
		bitmap_layer_set_bitmap(background_layer, background);
		layer_set_hidden(bitmap_layer_get_layer(background_layer), false);
		layer_mark_dirty(bitmap_layer_get_layer(background_layer));
		}
#ifdef PBL_PLATFORM_CHALK
		layer_set_frame(bitmap_layer_get_layer(bluetooth_layer), GRect(82, 45, 16,18));
#else
		layer_set_frame(bitmap_layer_get_layer(bluetooth_layer), GRect(64, 30, 16,18));
#endif
		break;		

	}
}

int stringToInt(char *str) {
    int i=0,sum=0;
    while(str[i]!='\0'){
         if(str[i]< 48 || str[i] > 57){
            // if (DEBUG) APP_LOG(APP_LOG_LEVEL_ERROR, "Unable to convert it into integer.");
          //   return 0;
         }
         else{
             sum = sum*10 + (str[i] - 48);
             i++;
         }
    }
    return sum;
}

// Handle the response from AppMessage
static void prv_inbox_received_handler(DictionaryIterator *iter, void *context) {
	
  // background
  Tuple *bg_t = dict_find(iter, MESSAGE_KEY_background);
  if (bg_t) {
    settings.background = stringToInt((char*) bg_t->value->data);
  }

 // day
  Tuple *hands_t = dict_find(iter, MESSAGE_KEY_hands);
  if (hands_t) {
    settings.hands = hands_t->value->int32 == 1;
  }
	
 // date
  Tuple *date_t = dict_find(iter, MESSAGE_KEY_date);
  if (date_t) {
    settings.date = date_t->value->int32 == 1;
  }
	
// secs
  Tuple *secs_t = dict_find(iter, MESSAGE_KEY_secs);
  if (secs_t) {
    settings.secs = secs_t->value->int32 == 1;
  }

 // batt
  Tuple *batt_t = dict_find(iter, MESSAGE_KEY_batt);
  if (batt_t) {
    settings.batt = batt_t->value->int32 == 1;
  }
	
 // Hour hand Color
  Tuple *hr_color_t = dict_find(iter, MESSAGE_KEY_hrcol);
  if (hr_color_t) {
    settings.hrcol = GColorFromHEX(hr_color_t->value->int32);
  }
	
 // Minute hand Color
  Tuple *min_color_t = dict_find(iter, MESSAGE_KEY_mincol);
  if (min_color_t) {
    settings.mincol = GColorFromHEX(min_color_t->value->int32);
  }
	
 // Second hand Color
  Tuple *sec_color_t = dict_find(iter, MESSAGE_KEY_seccol);
  if (sec_color_t) {
    settings.seccol = GColorFromHEX(sec_color_t->value->int32);
  }	  
	  // center dot Color
//  Tuple *cx_color_t = dict_find(iter, MESSAGE_KEY_center);
//  if (cx_color_t) {
//    settings.center = GColorFromHEX(cx_color_t->value->int32);
//  }

// Save the new settings to persistent storage
  prv_save_settings();
}

void update_battery_state(BatteryChargeState charge_state) {
    static char battery_text[] = "x100";

    if (charge_state.is_charging) {

        snprintf(battery_text, sizeof(battery_text), "+%d", charge_state.charge_percent);
    } else {
        snprintf(battery_text, sizeof(battery_text), "%d", charge_state.charge_percent);
        
    } 
    charge_percent = charge_state.charge_percent;
    text_layer_set_text(battery_text_layer, battery_text);
} 

static void toggle_bluetooth_icon(bool connected) {

  layer_set_hidden(bitmap_layer_get_layer(bluetooth_layer), !connected);
	  
}

void bluetooth_connection_callback(bool connected) {
  toggle_bluetooth_icon(connected);
}

void tick(struct tm *tick_time, TimeUnits units_changed) {
	
//redraw every tick
  layer_mark_dirty(layer);		

  static char date_buffer[] = "25";
  strftime(date_buffer, sizeof("25"), "%e", tick_time);
  text_layer_set_text(text_daynum_layer, date_buffer);
		
}

void init() {
	
  prv_load_settings();
	
  // Listen for AppMessages
  app_message_register_inbox_received(prv_inbox_received_handler);
  app_message_open(256, 256);

	//create window
	window = window_create();
	window_set_background_color(window,GColorBlack);
	window_stack_push(window, true);
	Layer* window_layer = window_get_root_layer(window);	
	GRect bounds = layer_get_bounds(window_layer);

  //background
  background = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BACKGROUND);
  background_layer = bitmap_layer_create(bounds);
  bitmap_layer_set_bitmap(background_layer, background);
  layer_add_child(window_layer, bitmap_layer_get_layer(background_layer));
	
  font1 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_CUSTOM_12));
	
  bluetooth_image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BLUETOOTH);
  GRect bitmap_bounds_bt_on = gbitmap_get_bounds(bluetooth_image);
#ifdef PBL_PLATFORM_CHALK
  GRect frame_bt = GRect(82, 35, bitmap_bounds_bt_on.size.w, bitmap_bounds_bt_on.size.h);
#else
  GRect frame_bt = GRect(44, 24, bitmap_bounds_bt_on.size.w, bitmap_bounds_bt_on.size.h);	
#endif
  bluetooth_layer = bitmap_layer_create(frame_bt);
  bitmap_layer_set_bitmap(bluetooth_layer, bluetooth_image);
//  bitmap_layer_set_compositing_mode(bluetooth_layer, GCompOpSet);
  layer_add_child(window_layer, bitmap_layer_get_layer(bluetooth_layer));									 
								 
#ifdef PBL_PLATFORM_CHALK
  text_daynum_layer = text_layer_create(GRect( 137, 83,  40,  20 ));
#else
  text_daynum_layer = text_layer_create(GRect( 114, 77,  40,  20 ));
#endif
  text_layer_set_text_alignment(text_daynum_layer, GTextAlignmentCenter);			
  text_layer_set_background_color(text_daynum_layer, GColorClear);
  text_layer_set_text_color(text_daynum_layer, GColorBlack );
  text_layer_set_font(text_daynum_layer, font1);
  layer_add_child( window_layer, text_layer_get_layer( text_daynum_layer ) );
										 
   
#ifdef PBL_PLATFORM_CHALK
   battery_text_layer = text_layer_create(GRect(38, 83, 40, 20));
#else
   battery_text_layer = text_layer_create(GRect(24, 77, 40, 20));
#endif
	text_layer_set_background_color(battery_text_layer, GColorClear);
    text_layer_set_text_color(battery_text_layer, GColorWhite);
    text_layer_set_text_alignment(battery_text_layer, GTextAlignmentCenter);
   text_layer_set_font(battery_text_layer, font1);
   layer_add_child(window_layer, text_layer_get_layer(battery_text_layer));
	
//create hands layer
#if PBL_PLATFORM_CHALK	
  layer = layer_create(GRect(0,0,180,180));
#else
  layer = layer_create(GRect(0,0,144,168));
#endif
  layer_set_update_proc(layer, update_layer);
  layer_add_child(window_layer, layer);	
	
  prv_update_display();

  toggle_bluetooth_icon(bluetooth_connection_service_peek());
  update_battery_state(battery_state_service_peek());

//subscribe events
  tick_timer_service_subscribe(MINUTE_UNIT, tick);
  bluetooth_connection_service_subscribe(bluetooth_connection_callback);
  battery_state_service_subscribe(&update_battery_state);
	
// init hand paths
  s_second_arrow = gpath_create(&SECOND_HAND_POINTS);
  s_minute_arrow = gpath_create(&MINUTE_HAND_POINTS);
  s_hour_arrow = gpath_create(&HOUR_HAND_POINTS);
	
  GPoint center = grect_center_point(&bounds);
	
  gpath_move_to(s_second_arrow, center);
  gpath_move_to(s_minute_arrow, center);
  gpath_move_to(s_hour_arrow, center);
	
	
// Avoids a blank screen on watch start.
  time_t now = time(NULL);
  struct tm *tick_time = localtime(&now);  
  tick(tick_time, DAY_UNIT + HOUR_UNIT + MINUTE_UNIT);
	
 update_battery_state(battery_state_service_peek());
 toggle_bluetooth_icon(bluetooth_connection_service_peek());
	
}

static void deinit(void) {

  layer_destroy(layer);
  text_layer_destroy(battery_text_layer);
  text_layer_destroy(text_daynum_layer);
	
  layer_remove_from_parent(bitmap_layer_get_layer(background_layer));
  bitmap_layer_destroy(background_layer);

	if (background != NULL) {
		gbitmap_destroy(background);
    }
	
  tick_timer_service_unsubscribe();
  bluetooth_connection_service_unsubscribe();
  battery_state_service_unsubscribe();
	
  layer_remove_from_parent(bitmap_layer_get_layer(bluetooth_layer));
  bitmap_layer_destroy(bluetooth_layer);
  gbitmap_destroy(bluetooth_image);
  bluetooth_image = NULL;

  fonts_unload_custom_font(font1);
	
  window_destroy(window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}