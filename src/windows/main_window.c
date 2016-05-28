#include "main_window.h"

#define MAX_METRICS HealthMetricActiveKCalories

static Window *s_window;
static TextLayer *s_value_layer, *s_label_layer;

static HealthMetric s_metric;
static GBitmapSequence *s_sequence;
static GBitmap *s_bitmap;
static BitmapLayer *s_bitmap_layer;

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
  s_metric -= (s_metric > 0) ? 1 : -(MAX_METRICS);
  main_window_update_time();
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
  s_metric += (s_metric < MAX_METRICS) ? 1 : -(MAX_METRICS);
  main_window_update_time();
}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
}

static TextLayer* make_text_layer(int y_inset, char *font_key) {
  Layer *window_layer = window_get_root_layer(s_window);
  GRect bounds = layer_get_bounds(window_layer);

  TextLayer *this = text_layer_create(grect_inset(bounds,
                                                GEdgeInsets(y_inset, 0, 0, 0)));
  text_layer_set_text_alignment(this, GTextAlignmentCenter);
  text_layer_set_text_color(this, GColorWhite);
  text_layer_set_background_color(this, GColorClear);
  text_layer_set_font(this, fonts_get_system_font(font_key));

#if defined(PBL_ROUND)
  text_layer_enable_screen_text_flow_and_paging(this, 5);
#endif

  return this;
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
  // Create sequence
//   s_sequence = gbitmap_sequence_create_with_resource(RESOURCE_ID_SLOTH_ANIMATE);
  // Create blank GBitmap using APNG frame size
//   GSize frame_size = gbitmap_sequence_get_bitmap_size(s_sequence);
//   s_bitmap = gbitmap_create_blank(frame_size, GBitmapFormat8Bit);
  s_bitmap = gbitmap_create_with_resource(RESOURCE_ID_SLOTH_PNG);
  s_bitmap_layer = bitmap_layer_create(bounds);
  bitmap_layer_set_compositing_mode(s_bitmap_layer, GCompOpSet);
  bitmap_layer_set_bitmap(s_bitmap_layer, s_bitmap);

  s_value_layer = make_text_layer(100, FONT_KEY_ROBOTO_BOLD_SUBSET_49 );
  s_label_layer = make_text_layer(90, FONT_KEY_GOTHIC_24_BOLD);
  layer_add_child(window_layer, bitmap_layer_get_layer(s_bitmap_layer));
  layer_add_child(window_layer, text_layer_get_layer(s_value_layer));
  layer_add_child(window_layer, text_layer_get_layer(s_label_layer));
}

static void window_unload(Window *window) {
  text_layer_destroy(s_value_layer);
  text_layer_destroy(s_label_layer);
  gbitmap_destroy(s_bitmap);
  bitmap_layer_destroy(s_bitmap_layer);
  window_destroy(s_window);
  s_window = NULL;
}

void main_window_push() {
  if(!s_window) {
    s_window = window_create();
    window_set_click_config_provider(s_window, click_config_provider);
    window_set_window_handlers(s_window, (WindowHandlers) {
      .load = window_load,
      .unload = window_unload,
    });
  }
  window_stack_push(s_window, true);

  main_window_update_time();
}

static void set_ui_values(char *label_text, GColor bg_color) {
  text_layer_set_text(s_label_layer, label_text);
// window_set_background_color(s_window, bg_color);
}

void update_gif() {
  uint32_t next_delay;
  // Advance to the next APNG frame, and get the delay for this frame
  if(gbitmap_sequence_update_bitmap_next_frame(s_sequence, s_bitmap, &next_delay)) {
    // Set the new frame into the BitmapLayer
    bitmap_layer_set_bitmap(s_bitmap_layer, s_bitmap);
    layer_mark_dirty(bitmap_layer_get_layer(s_bitmap_layer));
  }
}

void main_window_update_time() {
  time_t now = time(NULL);
  struct tm *time_now = localtime(&now);
  static char s_buffer[32];
  strftime(s_buffer, sizeof(s_buffer), "%H:%M", time_now);
  text_layer_set_text(s_value_layer, s_buffer);
  text_layer_set_text_color(s_value_layer, GColorBlack);
}
