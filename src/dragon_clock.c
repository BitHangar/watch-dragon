/*
 * Dragon Watchface for Pebble
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

#include "frame_animations.h" // See README for info on what this is!
#include "watch_functions.h" 

#define MY_UUID {0xE9, 0x94, 0x3A, 0x69, 0xEC, 0xE9, 0x45, 0xF6, 0x9E, 0x3A, 0x19, 0xB3, 0xD2, 0x2B, 0x8E, 0x4C}

PBL_APP_INFO(MY_UUID, "Dragon Clock", "Bit Hangar", 1,3 /* App version */, RESOURCE_ID_IMAGE_MENU_ICON, APP_INFO_WATCH_FACE);

Window window;

FrameAnimation blink_animation;

Layer background_layer;

BmpContainer background_image_container_white;
BmpContainer background_image_container_black;

RotBmpContainer hour_hand_image_container;
RotBmpContainer minute_hand_image_container;

AppTimerHandle timer_handle;


void handle_timer(AppContextRef ctx, AppTimerHandle handle, uint32_t cookie) {
    (void)ctx;
    (void)handle;
    
    if (cookie == 1) {
    	// Animate blink with parameters: 8 frames per second, don't repeat, cookie=1
    	frame_animation_alternating(&blink_animation, ctx, handle, 1, 8, false);
    	// If you have no idea what this function is, have a look at the README in the src folder!
	}
}

void handle_minute_tick(AppContextRef ctx, PebbleTickEvent *t) {
	(void)ctx;
	(void)t;
	
	update_hand_positions(&hour_hand_image_container, &minute_hand_image_container);

    timer_handle = app_timer_send_event(ctx, 100, 1); // Send a timer to start the blink animation
}


void handle_init(AppContextRef ctx) {
	(void)ctx;
    	
	window_init(&window, "Dragon Clock");
	window_stack_push(&window, true /* Animated */);
	
	resource_init_current_app(&APP_RESOURCES);
    
    // Set up layer for the hour hand
	rotbmp_init_container(RESOURCE_ID_DRAGON_HOUR_HAND, &hour_hand_image_container);
	rot_bitmap_set_src_ic(&hour_hand_image_container.layer, GPoint(2, 28));
	layer_add_child(&window.layer, &hour_hand_image_container.layer.layer);
	
	// Set up layer for the minute hand
	rotbmp_init_container(RESOURCE_ID_DRAGON_MINUTE_HAND, &minute_hand_image_container);
	rot_bitmap_set_src_ic(&minute_hand_image_container.layer, GPoint(0, 45));
	layer_add_child(&window.layer, &minute_hand_image_container.layer.layer);
    
    // Set up layer for dragon background image
	layer_init(&background_layer, window.layer.frame);

	// Using a "hack" to get a transparent png to render without slowing the watch down to a crawl.
	// Usually a transparent png needs a RotBmpPairContainer, but in this case we are taking both
	// the white part and the black part and rendering them in separate BmpContainers, then setting
	// the compositing of each, and finally putting images on the same layer, thus creating a 
	// "transparent" bitmap. Thanks to Philip from Pebble for this "completely un-endorsed, 
	// un-official, don't-actually-do-this-in-production-code" suggestion :)
    bmp_init_container(RESOURCE_ID_DRAGON_BACKGROUND_WHITE, &background_image_container_white);
    background_image_container_white.layer.compositing_mode = GCompOpOr;
    layer_add_child(&background_layer, &background_image_container_white.layer.layer);
    
    bmp_init_container(RESOURCE_ID_DRAGON_BACKGROUND_BLACK, &background_image_container_black);
    background_image_container_black.layer.compositing_mode = GCompOpClear;
    layer_add_child(&background_layer, &background_image_container_black.layer.layer);
    
    layer_add_child(&window.layer, &background_layer);

    // Set up layer that will hold the blink animation
   	frame_animation_init(&blink_animation, &window.layer, GPoint(11, 111), RESOURCE_ID_DRAGON_BLINK_1_WHITE, 4, true, true);

	update_hand_positions(&hour_hand_image_container, &minute_hand_image_container);	
}

void handle_deinit(AppContextRef ctx) {
	(void)ctx;
	
    bmp_deinit_container(&background_image_container_white);
    bmp_deinit_container(&background_image_container_black);
	rotbmp_deinit_container(&hour_hand_image_container);
	rotbmp_deinit_container(&minute_hand_image_container);
	frame_animation_deinit(&blink_animation);
}


void pbl_main(void *params) {
	PebbleAppHandlers handlers = {
		.init_handler = &handle_init,
		.deinit_handler = &handle_deinit,
        .timer_handler = &handle_timer,
        
        .tick_info = {
            .tick_handler = &handle_minute_tick,
            .tick_units = MINUTE_UNIT
        }
        
	};
	app_event_loop(params, &handlers);
}
