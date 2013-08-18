#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"
#include "util.h"

#define MY_UUID { 0x19, 0xA1, 0x31, 0x59, 0x2A, 0x49, 0x4E, 0xE6, 0x9F, 0x3D, 0xC3, 0xC3, 0xA5, 0x42, 0x06, 0x8F }
PBL_APP_INFO(MY_UUID,
             "Variable Hands", "Smognus",
             1, 0, /* App version */
             DEFAULT_MENU_ICON,
             APP_INFO_WATCH_FACE);

#define PI 3.141592653589793

Window window;
Layer clockface_layer;
Layer hands_layer;
BmpContainer clockface;

GPathInfo hour_hand_path_points = {
  .num_points = 6,
  .points = (GPoint []) {{6,0},{0,8},{-6,0},{-4,-60},{0,-65},{4,-60}}
};
GPathInfo minute_hand_path_points = {
  .num_points = 6,
  .points = (GPoint []) {{4,0},{0,8},{-4,0},{-3,-60},{0,-65},{3,-60}}
};
GPathInfo second_hand_path_points = {
  .num_points = 6,
  .points = (GPoint []) {{4,0},{0,8},{-4,0},{-3,-60},{0,-65},{3,-60}}
};

void handle_tick(AppContextRef ctx, PebbleTickEvent* tick) {
  (void) tick;
  layer_mark_dirty(&window.layer);
}

static void clockface_layer_update_proc (Layer* layer, GContext* ctx) {
  graphics_draw_bitmap_in_rect(ctx, &clockface.bmp, layer_get_frame(layer));
}

static void hands_layer_update_proc (Layer* layer, GContext* ctx) {
  GRect rect = layer_get_frame(layer);
  GPoint center = grect_center_point(&rect);
  GPath second_hand_path;
  GPath minute_hand_path;
  GPath hour_hand_path;
  PblTm time;
  /* char length_text[] = "     "; */
  /* char seconds_text[] = "  "; */
  /* char second_angle_text[] = "   "; */
  char day_text[] = "  ";
  int32_t second_angle;
  int32_t minute_angle;
  int32_t hour_angle;
  
  int32_t trig_ninety = TRIG_MAX_ANGLE / 4;
  int32_t trig_one_eighty = TRIG_MAX_ANGLE / 2;

  int32_t trig_second_angle;
  float second_length;

  int32_t trig_minute_angle;
  float minute_length = 60;

  int32_t trig_hour_angle;
  float hour_length = 60;

  get_time(&time);

  if (time.tm_hour > 12) {
    time.tm_hour-=12;
}

  second_angle = time.tm_sec * 6;
  minute_angle = time.tm_min * 6;
  hour_angle = (time.tm_hour * 30) + (time.tm_min / 2);

  trig_second_angle = (TRIG_MAX_ANGLE / 60) * time.tm_sec;
  trig_minute_angle = (TRIG_MAX_ANGLE / 60) * time.tm_min;
  trig_hour_angle = (TRIG_MAX_ANGLE / 360) * hour_angle;

  if (((time.tm_sec >= 7 && time.tm_sec <= 23) ||
       (time.tm_sec >= 37 && time.tm_sec <= 53)) &&
      time.tm_sec != 0) {
    second_length = (72 * TRIG_MAX_ANGLE) / (cos_lookup(trig_second_angle - trig_ninety));
    second_hand_path_points.points[4].y = (second_length > 0) ? -(second_length) : second_length;
    second_hand_path_points.points[3].y = (second_length > 0) ? -(second_length-5) : second_length+5;
    second_hand_path_points.points[5].y = (second_length > 0) ? -(second_length-5) : second_length+5;
  }
  else {
    second_length = (84 * TRIG_MAX_ANGLE) / (cos_lookup(trig_second_angle - trig_one_eighty));
    second_hand_path_points.points[4].y = (second_length > 0) ? -(second_length) : second_length;
    second_hand_path_points.points[3].y = (second_length > 0) ? -(second_length-5) : second_length+5;
    second_hand_path_points.points[5].y = (second_length > 0) ? -(second_length-5) : second_length+5;
  }

  if (((time.tm_min >= 7 && time.tm_min <= 23) ||
       (time.tm_min >= 37 && time.tm_min <= 53)) &&
      time.tm_min != 0) {
    minute_length = (72 * TRIG_MAX_ANGLE) / (cos_lookup(trig_minute_angle - trig_ninety));
    minute_hand_path_points.points[4].y = (minute_length > 0) ? -(minute_length) : minute_length;
    minute_hand_path_points.points[3].y = (minute_length > 0) ? -(minute_length-5) : minute_length+5;
    minute_hand_path_points.points[5].y = (minute_length > 0) ? -(minute_length-5) : minute_length+5;
  }
  else {
    minute_length = (84 * TRIG_MAX_ANGLE) / (cos_lookup(trig_minute_angle - trig_one_eighty));
    minute_hand_path_points.points[4].y = (minute_length > 0) ? -(minute_length) : minute_length;
    minute_hand_path_points.points[3].y = (minute_length > 0) ? -(minute_length-5) : minute_length+5;
    minute_hand_path_points.points[5].y = (minute_length > 0) ? -(minute_length-5) : minute_length+5;
  }

  // the number of pixels the hour hand will remain from the window edge
  int hour_length_offset = 25;

  if (((hour_angle >= 49 && hour_angle <= 139) ||
       (hour_angle >= 229 && hour_angle <= 311)) &&
      hour_angle != 0) {
    hour_length = (72 * TRIG_MAX_ANGLE) / (cos_lookup(trig_hour_angle - trig_ninety));
    hour_hand_path_points.points[4].y = (hour_length > 0) ? -(hour_length-hour_length_offset) : hour_length+hour_length_offset;
    hour_hand_path_points.points[3].y = (hour_length > 0) ? -(hour_length-(hour_length_offset+5)) : hour_length+(hour_length_offset+5);
    hour_hand_path_points.points[5].y = (hour_length > 0) ? -(hour_length-(hour_length_offset+5)) : hour_length+(hour_length_offset+5);
  }
  else {
    hour_length = (84 * TRIG_MAX_ANGLE) / (cos_lookup(trig_hour_angle - trig_one_eighty));
    hour_hand_path_points.points[4].y = (hour_length > 0) ? -(hour_length-hour_length_offset) : hour_length+hour_length_offset;
    hour_hand_path_points.points[3].y = (hour_length > 0) ? -(hour_length-(hour_length_offset + 5)) : hour_length+(hour_length_offset + 5);
    hour_hand_path_points.points[5].y = (hour_length > 0) ? -(hour_length-(hour_length_offset + 5)) : hour_length+(hour_length_offset + 5);
  }

  /* strcpy(day_text, itoa(time.tm_mday)); */
  /* graphics_text_draw(ctx, */
  /* 		     day_text, */
  /* 		     fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD), */
  /* 		     GRect(0,90,144,168), */
  /* 		     GTextOverflowModeWordWrap, */
  /* 		     GTextAlignmentCenter, */
  /* 		     NULL); */

  gpath_init(&minute_hand_path, &minute_hand_path_points);
  gpath_move_to(&minute_hand_path, center);
  gpath_rotate_to(&minute_hand_path, TRIG_MAX_ANGLE / 360 * minute_angle);
  graphics_context_set_fill_color(ctx, GColorWhite);
  graphics_context_set_stroke_color(ctx, GColorBlack);
  gpath_draw_filled(ctx, &minute_hand_path);
  gpath_draw_outline(ctx, &minute_hand_path);

  gpath_init(&hour_hand_path, &hour_hand_path_points);
  gpath_move_to(&hour_hand_path, center);
  gpath_rotate_to(&hour_hand_path, TRIG_MAX_ANGLE / 360 * hour_angle);
  graphics_context_set_fill_color(ctx, GColorWhite);
  graphics_context_set_stroke_color(ctx, GColorBlack);
  gpath_draw_filled(ctx, &hour_hand_path);
  gpath_draw_outline(ctx, &hour_hand_path);

  gpath_init(&second_hand_path, &second_hand_path_points);
  gpath_move_to(&second_hand_path, center);
  gpath_rotate_to(&second_hand_path, TRIG_MAX_ANGLE / 360 * second_angle);
  graphics_context_set_fill_color(ctx, GColorBlack);
  graphics_context_set_stroke_color(ctx, GColorWhite);
  gpath_draw_filled(ctx, &second_hand_path);
  gpath_draw_outline(ctx, &second_hand_path);

  graphics_context_set_text_color(ctx, GColorWhite);

  /* strcpy(second_angle_text, itoa(cos_lookup(trig_second_angle - trig_ninety))); */
  /* graphics_text_draw(ctx, */
  /* 		     second_angle_text, */
  /* 		     fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD), */
  /* 		     GRect(0,70,144,168), */
  /* 		     GTextOverflowModeWordWrap, */
  /* 		     GTextAlignmentCenter, */
  /* 		     NULL); */

  /* strcpy(length_text, itoa((int)hour_length)); */
  /* graphics_text_draw(ctx, */
  /* 		     length_text, */
  /* 		     fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD), */
  /* 		     GRect(0,90,144,168), */
  /* 		     GTextOverflowModeWordWrap, */
  /* 		     GTextAlignmentCenter, */
  /* 		     NULL); */

  /* strcpy(seconds_text, itoa(time.tm_hour)); */
  /* graphics_text_draw(ctx, */
  /* 		     seconds_text, */
  /* 		     fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD), */
  /* 		     GRect(0,110,144,168), */
  /* 		     GTextOverflowModeWordWrap, */
  /* 		     GTextAlignmentCenter, */
  /* 		     NULL); */
}

void handle_init(AppContextRef ctx) {

  window_init(&window, "Window Name");
  window_stack_push(&window, true /* Animated */);

  resource_init_current_app(&APP_RESOURCES);
  bmp_init_container(RESOURCE_ID_CLOCKFACE, &clockface);

  layer_init(&clockface_layer, window.layer.frame);
  layer_init(&hands_layer, window.layer.frame);

  clockface_layer.update_proc = &clockface_layer_update_proc;
  hands_layer.update_proc = &hands_layer_update_proc;

  layer_add_child(&window.layer, &clockface_layer);
  layer_add_child(&window.layer, &hands_layer);
}

void handle_deinit(AppContextRef ctx) {
  bmp_deinit_container(&clockface);
}

void pbl_main(void *params) {
  PebbleAppHandlers handlers = {
    .init_handler = &handle_init,
    .deinit_handler = &handle_deinit,
    .tick_info = {
      .tick_handler = &handle_tick,
      .tick_units = SECOND_UNIT
    }
  };
  app_event_loop(params, &handlers);
}
