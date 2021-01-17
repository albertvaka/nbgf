# Debug features

## Frame-by-frame

Press F1 to toggle frame-by-frame mode. In this mode, your scene's Update() function is not called (but Draw is).
Pressing E while in this mode will cause a single Update() to run, so you can watch things in "slow motion".
Another way to trigger this mode is by manually setting the global `Debug::FrameByFrame` to `true`. This is useful when you want to pause the game when certain event triggers in your code.
Note that in Release builds, `Debug::FrameByFrame` is always false and it can't be changed (it's `const`), so take this into account if you use it from your code.

## Debug draw mode and DebugDraw functions

Press F2 to toggle the `Debug::Draw` global boolean on and off.

Use it to add visuals for bounding boxes, invisible triggers, limits, etc. that are only drawn when `Debug::Draw` is true, so that you can show and hide them at runtime.

Note that in Release builds, `Debug::Draw` is always `false` and it can't be changed (it's `const`). This allows the compiler to optimize out any code you guard in an `if (Debug::Draw)` from Release builds.

The classes `vec`, `BoxBounds` and `CircleBounds` all have a `DebugDraw` function that will automatically draw an outline of them in Debug::Draw mode after everything else is drawn (so they are always on top). Note that since DebugDraw() calls are queued and drawn at the end of the frame, it doesn't matter where you call the function from (eg: you can call if from your `Update()` logic).

In addition, the `vec` class has `DebugDrawAsArrow` which takes another vec and draws an arrow between the two (useful for directions, forces...).

Both `DebugDraw` and `DebugDrawAsArrow` receive an optional RGB color to be drawn with.

## Reload assets

Press F4 to call the init function `LoadAssets` again at any point in time. The code using the assets should pickup the updated versions, so you can try changes to your art, shaders and other assets without reloading the game.

This does leak memory, but it is only used in debug, so who cares.

## Free camera movement

When both bebug draw mode and Frame-by-frame mode are enabled, you can move your camera freely with the keyboard arrows, zoom in and out with keypad + and -, and even rotate it with Av Pag and Re Pag.

## Fast forward

Hold F10 to run the game 3x faster than usual (`dt` is multiplied by 3).
