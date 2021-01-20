# NBGF: No-Bullshit Game Framework

<img src="https://img.shields.io/static/v1.svg?label=no%20tests&message=always%20green&color=green" />

An opinionated C++ framework to put images on screen and move them (aka: make games).

## Loading assets

Place your assets in `bin/data/`. Your game is small enough so all your assets fit in RAM. For this reason, we are going to load all the assets at once and keep them loaded until your game exits.

Your assets should be defined in code in `src/assets.h` as global objects inside the `Assets` namespace, and should be loaded in `src/assets.cpp` by the `Assets::LoadAll()` function.

The following are all the supported assets types and how to load and store them:

### Images: `GPU_Texture*`

All your images must be in PNG format. Use `LoadImage("<path>")` to load them into a `GPU_Texture*` that you will use to draw later.

### Sound effects: `Sound`

All your sounds must be either OGG or WAV. Define your sound effects as `static inline` instances of the `Sound` class and use its `Load("<path>")` to load them.

### Music: `Mix_Music*`

All your music files must be in OGG format. Use `LoadMusic("<path>")` to load them into a `Mix_Music*`.

### Fonts: `TTF_Font*`

Fonts are used to render strings of text. Use `LoadFont("<path>", <size>)` and `LoadFontOutline("data/PressStart2P.ttf", <size>, <outline_size>)` to load them into a `TTF_Font*`.

Caveats:
 - You will need to store one `TTF_Font*` per font size you want to render text at.
 - You will need two `TTF_Font*` to draw a font with outline. `LoadFontOutline()` loads the outline _only_ so you still need to use `LoadFont()` on the same font path and size and store both `TTF_Font*`.

### Shaders: `Shader`

Define your shaders as `static inline` instances of the `Shader` class and use its `Load("<vertex_path>", "<geometry_path>", "<fragment_path>")` method to load, compile and link together up to a vertex, geometry and frament shaders.

Don't include `#version` or `precision` directives in your shaders, depending on where your game runs we will pick these for you (and hope it compiles).

If any path is `nullptr`, a default shader will be loaded in its place.

## Scene and SceneManager

Games organize in scene. When the game starts, it will load your `EntryPointScene` defined in `src/scene_entrypoint.h`. `EntryPointScene` is meant to be a `typedef` to your actual scene class, unless you want to name your scene `EntryPointScene`.

All scenes should inherit from the `Scene` class and implement a few methods. We use `virtual` functions here, but I promise this is the only place. 

 - `Update(float dt)` will be called once per frame, so you can move your things around. `dt` is the time in seconds since the last frame (capped if too high) so you can make movement time-dependent and not framerate-dependent.
 - `Draw() const` will be called after `Update()` and is where you should put your calls to `Window::Draw` (see "Drawing Images" below).
 - `EnterScene()` and `ExitScene()` will be called before/after the `Update` and `Draw` functions first start/stop being called, respectively. This is different that the constructor given that you could have more than one `Scene` instantiated, but only one running.

## Drawing images

To draw an asset you have loaded in the game Window, call `Window::Draw(Assets::<my_asset>, <position>)` and optionally chain any transformations you want. Eg:
```
	Window::Draw(Assets::mySprite, vec(0,0))
		.withRotationDegs(45)
		.withScale(4)
		.with;
```

Some 
 

## Playing sounds and music

To play a sound just call `Assets::mySound.Play()`. Sounds also have a `SetVolume(<0-100>)` method you can use. See `engine/sound.h`.

To play a music track, use `MusicPlayer::Play(Assets::myMusic)`. Note only one music track can play at a time. The current track can be controlled with `MusicPlayer::IsPlaying()`, `MusicPlayer::Pause()`, `MusicPlayer::Resume()`, `MusicPlayer::Stop()` and `MusicPlayer::SetVolume(<0-100>)`. See `engine/musicplayer.h`.

## Rendering text

You can pass your `TTF_Font*` to the `Text` class to be able to render and draw text.




## Entities and SelfRegister

You can organize your game code however you like but, if I were you, I would define classes for my different game entities (enemies, powerups, bullets, player...) and make them extend `Entity` and `SelfRegister`.

Inheriting `MyClass` from `SelfRegister<MyClass>`, will give you a `MyClass::GetAll()` method that will return all the instances of `MyClass` you have created (and not destroyed yet). Use it as follows:

## points, vectors: the vec struct

You will be using lots of (x,y) pairs when making your game. If you are lazy like me, you will appreciate this struct is only 3 letters long. You should pass `vec` arguments by value.

The `vec` struct has all the goodies you can image: `+`, `-`, `/` and `*` operators, methods to rotate, normalize, get the lenght... See the definitions in the `engine/vec.h` header.

## Collisions

## Input

## Debug features

### Frame-by-frame

Press F1 to toggle frame-by-frame mode on and off. In this mode, your scene's `Update()` function is not called (but `Draw()` is).

Pressing E while in this mode will cause a single `Update()` to run, so you can advance your game logic one frame at a time. This is also useful to set breakpoints in your `Update()` code, just in the exact frame you want to debug.

Another way to trigger this mode is by manually setting the global `Debug::FrameByFrame` to `true`. This is useful to enter frame-to-frame mode when a certain event happens in your code.

Note that in Release builds `Debug::FrameByFrame` is always `false` and it can't be changed (it's `const`), so don't try to write to it unless the define `_DEBUG` is set.

### Debug draw mode and DebugDraw functions

Press F2 to toggle the global `Debug::Draw` boolean on and off.

Add visuals for bounding boxes, invisible triggers, limits, etc. to be only drawn when `Debug::Draw` is `true`, so that you can show and hide them at runtime.

Note that in Release builds, `Debug::Draw` is always `false` and it can't be changed (it's `const`). This allows the compiler to optimize out any code you guard in an `if (Debug::Draw)` from Release builds.

The classes `vec`, `BoxBounds` and `CircleBounds` all have a `DebugDraw()`. This function draws an outline of the primitive, only in `Debug::Draw` mode and after everything else is drawn (so always on top).

In addition, the `vec` class has `DebugDrawAsArrow()` which takes another `vec` and draws an arrow between the two (useful for directions, forces...).

Note that since `DebugDraw()` calls are queued and drawn at the end of the frame, it doesn't matter where you call them from (eg: you can call `DebugDraw()` from your `Update()` logic). It's also useful to call them from temporary objects, eg: `CircleBounds(pos, triggerRadius).DebugDraw()`.

Both `DebugDraw()` and `DebugDrawAsArrow()` receive an optional color argument to be drawn in.

### Reload assets

Press F4 to call the `LoadAssets()` function before the next frame. The `LoadAssets()` function is meant to read your asset files during the game initialization, but can also be used for hot-reloading asset updates. This allows you to see changes to your art, shaders and other assets without restarting the game.

This does leak memory, but it is only used in debug, so who cares.

### Free camera movement

When both debug draw mode and frame-by-frame mode are enabled, you can move your camera freely with the keyboard arrows, zoom in and out with numpad + and -, and even rotate it with Av Pag and Re Pag.

### Fast forward

Hold F10 to run the game 3x faster than usual (`dt` is multiplied by 3).

### Printing to console

Use the `Debug::out` stream to print to stdout like you would with `std::cout` but without having to end with `std::endl` (a newline is added automatically).

The classes `vec`, `BoxBounds`, `CircleBounds` and `GPU_Rect` all can be streamed to `Debug::out` and have a text representation.

On Windows, the Debug build of the game will also open a terminal window so you can see the stdout output.

## Drawing primitives

You can draw simple geometric shapes with `Window::DrawPrimitive::Pixel`, `Window::DrawPrimitive::Rectangle`, `Window::DrawPrimitive::Line`, `Window::DrawPrimitive::Circle`, and `Window::DrawPrimitive::Arc`.

The arguments of those are quite self-explanatory, you can find the definitions in `engine/window.h`.

## Drawing raw quads

If you like it hard, you can pass in series of vertex coordinates for them to be drawn in batch.

To do so, call `Window::DrawRaw::BatchTexturedQuad(GPU_Image* image, float x, float y, float w, float h, GPU_Rect& texture_coords)` for all your quads followed by a call to `Window::DrawRaw::FlushTexturedQuads(GPU_Image* image)`.

You can do the same for untextured quads with `Window::DrawRaw::BatchRGBQuad(...)` and `Window::DrawRaw::FlushRGBQuads()` or for textured and colored quads with `Window::DrawRaw::BatchColoredTexturedQuad(...)` and `Window::DrawRaw::FlushColoredTexturedQuads(...)`.

