# Debug features

## Frame-by-frame

Press F1 to toggle frame-by-frame mode on and off. In this mode, your scene's `Update()` function is not called (but `Draw()` is).

Pressing E while in this mode will cause a single `Update()` to run, so you can advance your game logic one frame at a time. This is also useful to set breakpoints in your `Update()` code, just in the exact frame you want to debug.

Another way to trigger this mode is by manually setting the global `Debug::FrameByFrame` to `true`. This is useful to enter frame-to-frame mode when a certain event happens in your code.

Note that in Release builds `Debug::FrameByFrame` is always `false` and it can't be changed (it's `const`), so don't try to write to it unless the define `_DEBUG` is set.

## Debug draw mode and DebugDraw functions

Press F2 to toggle the global `Debug::Draw` boolean on and off.

Add visuals for bounding boxes, invisible triggers, limits, etc. to be only drawn when `Debug::Draw` is `true`, so that you can show and hide them at runtime.

Note that in Release builds, `Debug::Draw` is always `false` and it can't be changed (it's `const`). This allows the compiler to optimize out any code you guard in an `if (Debug::Draw)` from Release builds.

The classes `vec`, `BoxBounds` and `CircleBounds` all have a `DebugDraw()`. This function draws an outline of the primitive, only in `Debug::Draw` mode and after everything else is drawn (so always on top).

In addition, the `vec` class has `DebugDrawAsArrow()` which takes another `vec` and draws an arrow between the two (useful for directions, forces...).

Note that since `DebugDraw()` calls are queued and drawn at the end of the frame, it doesn't matter where you call them from (eg: you can call `DebugDraw()` from your `Update()` logic). It's also useful to call them from temporary objects, eg: `CircleBounds(pos, triggerRadius).DebugDraw()`.

Both `DebugDraw()` and `DebugDrawAsArrow()` receive an optional color argument to be drawn in.

## Reload assets

Press F4 to call the `LoadAssets()` function before the next frame. The `LoadAssets()` function is meant to read your asset files during the game initialization, but can also be used for hot-reloading asset updates. This allows you to see changes to your art, shaders and other assets without restarting the game.

This does leak memory, but it is only used in debug, so who cares.

## Free camera movement

When both debug draw mode and frame-by-frame mode are enabled, you can move your camera freely with the keyboard arrows, zoom in and out with numpad + and -, and even rotate it with Av Pag and Re Pag.

## Fast forward

Hold F10 to run the game 3x faster than usual (`dt` is multiplied by 3).
