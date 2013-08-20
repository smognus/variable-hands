#include "pebble_os.h"
#include "outlined_text.h"

void draw_outlined_text(GContext* ctx, char* text, GFont font, GRect rect, GTextOverflowMode mode, GTextAlignment alignment, int outline_pixels, bool inverted) {
  (inverted) ? graphics_context_set_text_color(ctx, GColorBlack) :
    graphics_context_set_text_color(ctx, GColorWhite);

  GRect volatile_rect = rect;
  for (int i=0; i<outline_pixels; i++) {
    volatile_rect.origin.x+=1;
    graphics_text_draw(ctx,
		       text,
		       font,
		       volatile_rect,
		       mode,
		       alignment,
		       NULL);
  }
  volatile_rect = rect;
  for (int i=0; i<outline_pixels; i++) {
    volatile_rect.origin.y+=1;
    graphics_text_draw(ctx,
		       text,
		       font,
		       volatile_rect,
		       mode,
		       alignment,
		       NULL);
  }
  volatile_rect = rect;
  for (int i=0; i<outline_pixels; i++) {
    volatile_rect.origin.x-=1;
    graphics_text_draw(ctx,
		       text,
		       font,
		       volatile_rect,
		       mode,
		       alignment,
		       NULL);
  }
  volatile_rect = rect;
  for (int i=0; i<outline_pixels; i++) {
    volatile_rect.origin.y-=1;
    graphics_text_draw(ctx,
		       text,
		       font,
		       volatile_rect,
		       mode,
		       alignment,
		       NULL);
  }
  (inverted) ? graphics_context_set_text_color(ctx, GColorWhite) :
    graphics_context_set_text_color(ctx, GColorBlack);
  graphics_text_draw(ctx,
		     text,
		     font,
		     rect,
		     mode,
		     alignment,
		     NULL);
}
