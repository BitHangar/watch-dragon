
This library is for frame-by-frame animation within a layer. 


What you'll need: Bitmap images of all the frames you want to animate; they can be either regular PNG’s or transparent ones. Your “resource_map.json” file needs to declare all your images IN THE ORDER IN WHICH YOU WANT TO ANIMATE THEM. This is very important! You’ll also need a global FrameAnimation object declared in your main program (more on this below).


Usage: If you want your animation just to run from the first frame to the last, you can use the ‘frame_animation_linear’ function. It can animate then stop, or continue on a loop. Just pass either 'true' or 'false' for the 'continuous' parameter. If you want the animation to cycle through forwards then backwards, use the  ‘frame_animation_alternating' function. That animation can keep looping or stop also. To stop a looping animation, pass your FrameAnimation object, AppContextRef and the AppTimerHandle of the timer you're using for that particular animation to ‘frame_animation_stop’.

In order to initialize the animation, you should use the 'frame_animation_init' function. This takes several parameters. First is a reference to the FrameAnimation object that you will use to hold your animation. This should be a global variable in your main app. Second is a reference to the layer onto which you will put your animation layer (this is usually just &window.layer). Next is a GPoint that will be the x and y coordinates of where you want the animation to appear on the screen. The next part is very important. You’ll need a reference to the FIRST image resource you want in the animation. For a normal PNG, this will be something like ‘RESOURCE_ID_FRAME_1’. For a PNG with transparency, you’ll have to add ‘_WHITE’ to the resource name, i.e. ‘RESOURCE_ID_FRAME_1_WHITE’. Next you just specify the number of frames in your animation. Finally you’ll need to include whether or not the images you’re animating are transparent or regular PNG’s. 
NOTE: Be sure to de-initialize your animation(s)! Simply include ‘frame_animation_deinit(&some_animation_name)’ in your ‘handle_deinit()’ implementation for all FrameAnimation objects you declare.


Both animation functions need some basic parameters to work properly. The FrameAnimation is a reference to your FrameAnimation object to use for this particular animation. The AppContextRef can be obtained from a Pebble app's 'handle_init' function. The AppTimerHandle is usually a global variable declared in your Pebble app. 'num_frames' is the number of frames to animate in your image, and 'fps' is the frames per second to use in the animation. We've already gone through what 'continuous' is for. Finally, the 'cookie' parameter is used if you want to use multiple animations with the same AppTimerHandle variable, just pass in different cookie integers for each animation.


Ok, so how exactly do you implement this stuff? The 'frame_animation_init' function should probably go in your Pebble app's 'handle_init' function. For the actual animation functions, those belong in your app's timer handler function. 
Here's an example:



void handle_timer(AppContextRef ctx, AppTimerHandle handle, uint32_t cookie) 
{

  	(void)ctx;

   	(void)handle;

	...

	...

	if (cookie == 1)  
		frame_animation_linear(&frame_animation, ctx, handle,  1, 10, true);

}



void handle_init(AppContextRef ctx) 
{

	(void)ctx;

	...

	...

	layer_animation_init(&frame_animation,  &window.layer,  GPoint(10, 10), 
		RESOURCE_ID_FRAME_1, 10, false);

	timer_handle = app_timer_send_event(ctx, 10, 1);

}


What the above will do is start the animation 10 milliseconds from when the app loads (so basically instantly). The animation itself has 10 frames at 10 frames per second (that's 100 milliseconds between frames) and will loop continuously until the app is closed or 'app_timer_cancel_event()' or ‘frame_animation_stop()’ is called. 
In the init, we've delcared that the animation will be stored in a FrameAnimation object called ‘frame_animation’, put the animation on the main window at coordinates (10, 10), start at frame 1, there are 10 frames, and the images have no transparency. 


Multiple Animations: You can have more than one animation running at the same time, using the same timer handler function in your Pebble app. You also have the ability to cancel one or all of the animations currently running. So, how to do this? Say you want to have two animations running independently of each other. Simply declare two global AppTimerHandle variables in your app. When you write the animation calls, just pass one timer handle to the first, and the other to the second.
For Example: You have two AppTimerHandle variables in your app

	AppTimerHandle timer_1;
	AppTimerHandle timer_2;

You also have two FrameAnimation objects:

	FrameAnimation animation_1;
	FrameAnimation animation_2;


When you declare your animations:	

	frame_animation_linear(&animation_1, ctx, timer_1,  1, 10, true);
	frame_animation_linear(&animation_2, ctx, timer_2,  1, 10, true);


Then, if you want to cancel just the first animation:

	bool stopped;
	stopped = frame_animation_stop(&animation_1, ctx, timer_1);


The variable 'stopped' will tell you if the animation was successfully stopped or not.
You can also have two completely different animations running at the same time! Just declare and set up another FrameAnimation object to store your second animation. You’ll still need a separate AppTimerHandle variable for the second animation to work properly
**Please Note: Having multiple animations can cause the Pebble to slow down or even crash, please use with caution! 



KNOWN ISSUES:

* Using large transparent png images for the animation can cause a reduction in the framerate of the animation. It can also have an adverse effect on the framerate for property animations.

* Having a 'fps' value of anything over 30 does not seem to actually speed up the animation. Since Pebble has said the max refresh rate of the watch is 30fps, this makes sense. 


