# NBGF: No-Bullshit Game Framework

<img src="https://img.shields.io/static/v1.svg?label=no%20tests&message=always%20green&color=green" />

An opinionated C++ framework to make games in minutes.

Compiles to Windows, Mac, Linux and HTML for the web browser (emscripten).

# Requisites

### Windows
- VisualStudio 2019
- All the dependencies are bundled, the project does just build :)

### Linux and Mac
- SDL 2.0.12+
- SDL_ttf 2.0.15+
- SDL_mixer 2.0.4+
- SDL_image 2.0.5+
- Build with `make`

### HTML
- You will need a working emscripten setup
- Build with `emmake make`

# Developer's reference

- [Loading assets](#loading-assets)
	- [Images](#images-gpu_texture)
	- [Sound effects](#sound-effects-sound)
	- [Music](#music-mix_music)
	- [Fonts](#fonts-ttf_font)
	- [Shaders](#shaders-shader)
- [Scenes](#scenes)
	- [The Scene class](#the-scene-class)
	- [EntryPointScene](#entrypointscene)
	- [Changing scenes](#changing-scenes-scenemanager)
- [Drawing on screen 1 (the basics)](#drawing-on-screen-1-the-basics)
	- [Drawing images](#drawing-images)
	- [Animated sprites](#animated-sprites)
	- [Rendering text](#rendering-text)
	- [Clearing the window](#clearing-the-window)
	- [The camera](#the-camera)
		- [Camera for GUIs](#camera-for-guisa)
- [Playing sounds and music](#playing-sounds-and-music)
- [Entities and SelfRegister](#entities-and-selfregister)
- [Points and vectors: the vec struct](#points-and-vectors-the-vec-struct)
- [Bounding boxes and collisions](#bounding-boxes-and-collisions)
	- [BoxEntity and CircleEntity](#boxentity-and-circleentity)
	- [Collisions](#collisions)
		- [Collisions within entities of the same class](#collisions-within-entities-of-the-same-class)
- [Input](#input)
	- [Actions-based input](#actions-based-input)
	- [Keyboard input](#keyboard-input)
	- [Mouse input](#mouse-input)
	- [Gamepad input](#gamepad-input)
- [Drawing on screen 2 (the advanced stuff)](#drawing-on-screen-2-the-advanced-stuff)
	- [Particle systems](#particle-systems)
	- [Using shaders](#using-shaders)
	- [Screen effects](#screen-effects)
		- [Screenshake](#screenshake)
		- [Screen transitions](#screen-transitions)
		- [Freeze the image](#freeze-the-image)
		- [Fullscreen shaders](#fullscreen-shaders)
	- [Drawing primitives](#drawing-primitives)
	- [Drawing raw quads](#drawing-raw-quads)
- [Save games](#save-games)
- [Window properties](#window-properties)
	- [Game window size](#game-window-size)
	- [Fullscreen mode](#fullscreen-mode)
	- [Window title](#window-title)
	- [Hide the mouse cursor](#hide-the-mouse-cursor)
- [Debug features](#debug-features) (if you are not convinced to use NBGF yet, read this part first :)
	- [Frame-by-frame](#frame-by-frame)
	- [Debug draw mode and DebugDraw functions](#debug-draw-mode-and-debugdraw-functions)
	- [Reload assets](#reload-assets)
	- [Free camera movement](#free-camera-movement)
	- [Fast forward](#fast-forward)
	- [Imgui](#imgui)
	- [Printing text](#printing-text)

## Loading assets

Place your assets in `bin/data/`. Your game is small enough so all your assets fit in RAM. For this reason, we are going to load all the assets at once and keep them loaded until your game exits.

Your assets should be defined in code in [`src/assets.h`](src/assets.h) as global objects inside the `Assets` namespace, and should be loaded in [`src/assets.cpp`](src/assets.cpp) inside the `Assets::LoadAll()` function.

The following are all the supported assets types and how to load and store them:

### Images: `GPU_Texture*`

All your images must be in PNG format. Use [`LoadImage("<path>")`](engine/asset_load.h) to load them into a `GPU_Texture*` that you will use to draw later.

### Sound effects: [`Sound`](engine/sound.h)

All your sounds must be either OGG or WAV. Define your sound effects as `static inline` instances of the [`Sound`](engine/sound.h) class and use its `sound.Load("<path>")` method to load them.

### Music: `Mix_Music*`

All your music files must be in OGG format. Use [`LoadMusic("<path>")`](engine/asset_load.h) to load them into a `Mix_Music*`.

### Fonts: `TTF_Font*`

Fonts are used to render strings of text. Use [`LoadFont("<path>", <size>)`](engine/asset_load.h) and [`LoadFontOutline("data/PressStart2P.ttf", <size>, <outline_size>)`](engine/asset_load.h) to load them into a `TTF_Font*`.

Caveats:
 - You will need to store one `TTF_Font*` per font size you want to render text at.
 - You will need two `TTF_Font*` to draw a font with outline. `LoadFontOutline()` loads the outline _only_ so you still need to use `LoadFont()` on the same font path and size and store both `TTF_Font*`.

### Shaders: [`Shader`](engine/shader.h)

Define your shaders as `static inline` instances of the [`Shader`](engine/shader.h) class and use its `shader.Load("<vertex_path>", "<geometry_path>", "<fragment_path>")` method to load, compile and link together up to a vertex, geometry and frament shaders.

Don't include `#version` or `precision` directives in your shaders, depending on where your game runs we will pick these for you (and hope it compiles).

If any path is `nullptr`, a default shader will be loaded in its place.

## Scenes

### The Scene class

Games are split in scenes. All scenes should inherit from the [`Scene`](engine/scene.h) class and implement these methods:

 - `Update(float dt)` will be called once per frame, so you can move your things around. `dt` is the time in seconds since the last frame (capped if too high) so you can make movement time-dependent and not framerate-dependent.
 - `Draw() const` will be called after `Update()` and is where you should put your calls to `Window::Draw` (see "Drawing Images" below).
 - `EnterScene()` and `ExitScene()` will be called before/after the `Update` and `Draw` functions first start/stop being called, respectively. This is different that the constructor given that you could have more than one `Scene` instantiated, but only one running.

### EntryPointScene

When the game starts, it will load your `EntryPointScene` defined in [`src/scene_entrypoint.h`](src/scene_entrypoint.h). `EntryPointScene` is meant to be a `typedef` to your actual scene class, unless you want to name your scene `EntryPointScene`.

### Changing scenes: [`SceneManager`](engine/scene_manager.h)

Use `SceneManager::ChangeScene(new MyAwesomeScene())` to change to a new scene and `SceneManager::RestartScene()` to re-enter the current one.

Note the constructor of the new scene runs right away, while the current scene is still active. Don't put your scene initialization logic in the constructor (use `EnterScene` for that), and specially don't instantiate any self-registering entities there (since they would show up from your current scene).

## Drawing on screen 1 (the basics)

### Drawing images

To draw an asset you have loaded somewhere in the world, call [`Window::Draw(Assets::<my_asset>, <position>)`](engine/window_draw.h) and optionally chain any transformations you want. Eg:
```
vec position = vec(0,0);
Window::Draw(Assets::mySprite, position)
	.withRotationDegs(45)
	.withScale(4);
```

Find all the supported transformations in the definition of `Window::Draw` in [`engine/window_draw.h`](engine/window_draw.h) file.

### Animated sprites

TODO

### Rendering text

Use the `TTF_Font*` from `Assets` to instantiate the [`Text`](engine/text.h) class, set the string you want to draw and then draw it the same way as an image:

```
Text myText(Assets::myAwesomeFont);
myText.SetFillColor(255,128,128);
myText.SetString("my cool string");
Window::Draw(myText, <position>);
```

Keep your `Text` instances in scope instead of recreating them each frame, since rendering a string is expensive.

If you want to render a string with an outline, you will have to pass two `TTF_Font*` to the `Text` constructor.

You can use `SetFillColor(...)` and `SetOutlineColor(...)` to change the color of your text.

You can also draw strings containing newlines. Use `SetMultilineAlignment(...)`, `SetMultilineSpacing(...)` and `SetEmptyLinesMultilineSpacing(...)` to tweak how they are rendered.

### Clearing the window

Unless you are going to draw to every single pixel in you scene's `Draw` method, you probably want to clear the previous image with a solid background color before drawing the new one. Use `Window::Clear(r,g,b)` as defined in [`engine/window.h`](engine/window.h).

### The camera

The default camera has the top left corner at `(0,0)`.

To move it arround, use the `Camera::SetCenter()` and  `Camera::SetTopLeft()` functions, defined in [`engine/camera.h`](engine/camera.h).

The [`Camera`](engine/camera.h) class also provides functions to zoom in and out, rotate the camera, as well as getting the current position, zoom and rotation.

#### Camera for GUIs

It's common to have GUI elements on screen that shouldn't be affected by the camera movement, zoom or rotation. You can draw without applying any camera transformations by placing your `Window::Draw` calls between `Camera::InScreenCoords::Begin()` and `Camera::InScreenCoords::End()`.

## Playing sounds and music

To play a sound just call `Assets::mySound.Play()`. Sounds also have a `SetVolume(<0-100>)` method you can use. See `engine/sound.h`.

To play a music track, use `MusicPlayer::Play(Assets::myMusic)`. Note only one music track can play at a time. The current track can be controlled with `MusicPlayer::IsPlaying()`, `MusicPlayer::Pause()`, `MusicPlayer::Resume()`, `MusicPlayer::Stop()` and `MusicPlayer::SetVolume(<0-100>)`. See `engine/musicplayer.h`.

## Entities and SelfRegister

You can organize your game code however you like but, if I were you, I would define classes for my different game entities (enemies, powerups, bullets, player...) and make them extend `Entity` and `SelfRegister`.

Inheriting `MyClass` from `SelfRegister<MyClass>`, will give you a `MyClass::GetAll()` method that will return all the instances of `MyClass` you have created (and not destroyed yet). Use it as follows:

TODO

## Points and vectors: the [`vec`](engine/vec.h) struct

You will be using lots of (x,y) pairs when making your game. If you are lazy like me, you will appreciate this struct is only 3 letters long. You should pass `vec` arguments by value.

The `vec` struct has all the goodies you can image: `+`, `-`, `/` and `*` operators, methods to rotate, normalize, get the lenght... See the definitions in the [`engine/vec.h`](engine/vec.h) header.

## Bounding boxes and collisions

The collision primitives are rectangles (`BoxBounds`) and circles (`CircleBounds`), both defined in [`engine/bounds.h`](engine/bounds.h).

- `BoxBounds` are defined by a position (from their top-left) and a size. Use `BoxBounds::FromCenter(center, size)` to specify the center point instead of the top-left when creating it.
- `CircleBounds` are defined by a center position and a radius.

### BoxEntity and CircleEntity

`BoxEntity` and `CircleEntity` (defined in [`engine/entity.h`](engine/entity.h)) are subclasses of the `Entity` class which provide a `Bounds()` method that returns a `BoxBounds` or `CircleBounds` respectively, centered on the position of the `Entity`. This is useful so you don't have to keep in sync the position of an `Entity` and its bounds.

### Collisions

Use the `Collide(a,b)` function defined in [`engine/collide.h`](engine/collide.h) to check if two bounds or two `BoxEntity`/`CircleEntity` collide. See this example, which makes use of `SelfRegister` class to check if any `Enemy` collides with any `Bullet`:

```
for (Bullet* bullet, Bullet::GetAll())
{
	for (Enemy* enemy, Enemy::GetAll())
	{
		if (Collide(bullet, enemy))
		{
			enemy->HitByBullet(bullet);
		}
	}
}
```

If you are into the brevity thing, a more compact way to achieve the same result is using the `CollideWithCallback(...)` method which takes both sets of objects as returned by `GetAll()` and a lambda:

```
CollideWithCallback(Bullet::GetAll(), Enemy::GetAll(), [](Bullet* bullet, Enemy* enemy)
{
	enemy->HitByBullet(bullet);
});
```

#### Collisions within entities of the same class

When checking collisions between objects of the same class, use `CollideSelf(...)` like this:

```
CollideSelf(Enemy::GetAll(), [](Enemy* enemy_a, Enemy* enemy_b)
{
	enemy_a->HitOtherEnemy(enemy_b);
	enemy_b->HitOtherEnemy(enemy_a);
});
```

This skips checking both `Collide(a,b)` and `Collide(b,a)` for the same pair of objects, which is faster.

## Input

TODO

### Actions-based input

TODO

### Keyboard input

TODO

### Mouse input

TODO

### Gamepad input

TODO

## Drawing on screen 2 (the advanced stuff)

### Particle systems

[`PartSys`](engine/partsys.h)

TODO

### Using shaders

TODO

### Screen effects

#### Screenshake

TODO

#### Screen transitions

TODO

#### Freeze the image

TODO

#### Fullscreen shaders

Fullscreen shaders do a render-to-texture, then render the result applying the shader.

TODO

### Drawing primitives

The `Window::DrawPrimitive` namespace in [`engine/window_drawprimitive.h`](engine/window_drawprimitive.h) contains functions to draw simple geometric shapes:

- `Window::DrawPrimitive::Pixel(...)`
- `Window::DrawPrimitive::Rectangle(...)`
- `Window::DrawPrimitive::Line(...)`
- `Window::DrawPrimitive::Circle(...)`
- `Window::DrawPrimitive::Arc(...)`

### Drawing raw quads

If you like it hard, you can submit vertex and texture coordinates directly for them to be drawn in batch.

Do so with the following pairs of batch and flush functions, defined in `Window::DrawRaw` in [`engine/window_drawraw.h`](engine/window_drawraw.h):

- `Window::DrawRaw::BatchTexturedQuad(GPU_Image* image, float x, float y, float w, float h, GPU_Rect& texture_coords)` and `Window::DrawRaw::FlushTexturedQuads(GPU_Image* image)` for textured quads.
- `Window::DrawRaw::BatchColoredTexturedQuad(...)` and `Window::DrawRaw::FlushColoredTexturedQuads(...)`. for quads textured & colored .
- `Window::DrawRaw::BatchRGBQuad(...)` and `Window::DrawRaw::FlushRGBQuads()` for untextured quads.

## Save games

TODO

## Window properties

### Game window size

The size in virtual pixels of the window is defined in [`src/window_conf.h`](src/window_conf.h). Those are virtual, in-game pixels, but the actual window size will be upscaled (preserving the aspect ratio) to the biggest multiple that fits the screen.

### Fullscreen mode

Pressing Alt+Enter will toggle between windowed and fullscreen. You can also do this in code using `Window::SetFullScreen(bool b)` defined in [`engine/window.h`](engine/window.h).

### Window title

The title can be set in [`src/window_conf.h`](src/window_conf.h).

### Hide the mouse cursor

Call `Window::SetShowCursor(bool b)` defined in [`engine/window.h`](engine/window.h) to show and hide the mouse pointer.

## Debug features

### Frame-by-frame

Press F1 to toggle frame-by-frame mode on and off. In this mode, your scene's `Update()` function is not called (but `Draw()` is).

Pressing E while in this mode will cause a single `Update()` to run, so you can advance your game logic one frame at a time. This is also useful to set breakpoints in your `Update()` code, just in the exact frame you want to debug.

Another way to trigger this mode is by manually setting the global [`Debug::FrameByFrame`](engine/debug.h) to `true`. This is useful to enter frame-to-frame mode when a certain event happens in your code.

Note that in Release builds `Debug::FrameByFrame` is always `false` and it can't be changed (it's `const`), so don't try to write to it unless the define `_DEBUG` is set.

### Debug draw mode and DebugDraw functions

Press F2 to toggle the global [`Debug::Draw`](engine/debug.h) boolean on and off.

Add visuals for bounding boxes, invisible triggers, limits, etc. to be only drawn when `Debug::Draw` is `true`, so that you can show and hide them at runtime.

Note that in Release builds, `Debug::Draw` is always `false` and it can't be changed (it's `const`). This allows the compiler to optimize out any code you guard in an `if (Debug::Draw)` from Release builds.

The classes [`vec`](engine/vec.h), `BoxBounds` and `CircleBounds` (defined in [`engine/bounds.h`](engine/bounds.h)) all have a `DebugDraw()` method. Calling `DebugDraw()` will draw an outline of the bounds/vec, only when in `Debug::Draw` mode and after everything else is drawn (so always on top).

In addition, the [`vec`](engine/vec.h) class has `DebugDrawAsArrow()` which takes another `vec` and draws an arrow between the two (useful for directions, forces...).

Note that since `DebugDraw()` calls are queued and drawn at the end of the frame, it doesn't matter where you call them from (eg: you can call `DebugDraw()` from your `Update()` logic). It's also useful to call them from temporary objects, eg: `CircleBounds(pos, triggerRadius).DebugDraw()`.

Both `DebugDraw()` and `DebugDrawAsArrow()` receive an optional color argument to be drawn in.

### Reload assets

Press F4 to call the `LoadAssets()` function before the next frame. The `LoadAssets()` function is meant to read your asset files during the game initialization, but can also be used for hot-reloading asset updates. This allows you to see changes to your art, shaders and other assets without restarting the game.

This does leak memory, but it is only used in debug, so who cares.

### Free camera movement

When both debug draw mode and frame-by-frame mode are enabled, you can move your camera freely with the keyboard arrows, zoom in and out with numpad + and -, and even rotate it with Av Pag and Re Pag.

### Fast forward

Hold F10 to run the game 3x faster than usual (`dt` is multiplied by 3).

### Imgui

[Dear Imgui](https://github.com/ocornut/imgui) is included. The best library ever made to create tooling and debug UIs. Use it.

The `ParticleSys` class has a `DrawImGUI()` function that shows an ImGUI window with all the parameters of the particle system, so you can tweak them and see how it looks in-game, before writing them in your code. 

### Printing text

Use the `Debug::out` stream to print to stdout like you would with `std::cout` but without having to end with `std::endl` (a newline is added automatically).

The classes `vec`, `BoxBounds`, `CircleBounds` and `GPU_Rect` all can be streamed to `Debug::out` and have a text representation.

On Windows, the Debug build of the game will also open a terminal window so you can see the stdout output.
