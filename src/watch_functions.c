/*
 * watch_functions.c
 * Copyright (C) 2013 Bit Hangar
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */


#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"

#include "watch_functions.h"


 #define MAX(a,b) (((a)>(b))?(a):(b))

 // This function was borrowed from the "Brains" watchface :)
int32_t integer_sqrt(int32_t x) {
	if (x < 0) {
		////    PBL_LOG(LOG_LEVEL_ERROR, "Looking for sqrt of negative number");
		return 0;
	}
	
	int32_t last_res = 0;
	int32_t res = (x + 1)/2;
	while (last_res != res) {
		last_res = res;
		res = (last_res + x / last_res) / 2;
	}
	return res;
}

void update_hand_positions(RotBmpContainer *hour_hand_image_container, RotBmpContainer *minute_hand_image_container) {
	PblTm t;
	
	get_time(&t);
	
	set_hand_angle(hour_hand_image_container, ((t.tm_hour % 12) * 30) + (t.tm_min/2));
	
	set_hand_angle(minute_hand_image_container, t.tm_min * 6);
}

// This function was borrowed from the "Brains" watchface :)
void rot_bitmap_set_src_ic(RotBitmapLayer *image, GPoint ic) {
  image->src_ic = ic;

  // adjust the frame so the whole image will still be visible
  const int32_t horiz = MAX(ic.x, abs(image->bitmap->bounds.size.w - ic.x));
  const int32_t vert = MAX(ic.y, abs(image->bitmap->bounds.size.h - ic.y));

  GRect r = layer_get_frame(&image->layer);
  const int32_t new_dist = (integer_sqrt(horiz*horiz + vert*vert) * 2) + 1; //// Fudge to deal with non-even dimensions--to ensure right-most and bottom-most edges aren't cut off.

  r.size.w = new_dist;
  r.size.h = new_dist;
  layer_set_frame(&image->layer, r);

  r.origin = GPoint(0, 0);
  image->layer.bounds = r;

  image->dest_ic = GPoint(new_dist / 2, new_dist / 2);

  layer_mark_dirty(&(image->layer));
}

// This function was borrowed from the "Brains" watchface :)
void set_hand_angle(RotBmpContainer *hand_image_container, unsigned int hand_angle) {
	signed short x_fudge = 0;
	signed short y_fudge = 0;
	
	hand_image_container->layer.rotation = TRIG_MAX_ANGLE * hand_angle / 360;
	
	//
	// Due to rounding/centre of rotation point/other issues of fitting
	// square pixels into round holes by the time hands get to 6 and 9
	// o'clock there's off-by-one pixel errors.
	//
	// The `x_fudge` and `y_fudge` values enable us to ensure the hands
	// look centred on the minute marks at those points. (This could
	// probably be improved for intermediate marks also but they're not
	// as noticable.)
	//
	// I think ideally we'd only ever calculate the rotation between
	// 0-90 degrees and then rotate again by 90 or 180 degrees to
	// eliminate the error.
	//
	if (hand_angle == 180) {
		x_fudge = -1;
	}
	else if (hand_angle == 270) {
		y_fudge = -1;
	}
	
	// (144 = screen width, 168 = screen height)
	hand_image_container->layer.layer.frame.origin.x = (144/2) - (hand_image_container->layer.layer.frame.size.w/2) + x_fudge;
	hand_image_container->layer.layer.frame.origin.y = (168/2) - (hand_image_container->layer.layer.frame.size.h/2) + y_fudge;
	
	layer_mark_dirty(&hand_image_container->layer.layer);
}