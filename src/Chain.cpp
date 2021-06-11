
#include "Chain.h"
#include "ChainNode.h"

#include "input.h"
#include "assets.h"
#include "anim_lib.h"
#include "camera.h"
#include "window.h"
#include "vec.h"

constexpr uint8_t MaxNodes = 50;

Chain::Chain()
{
	mNodes[0] = new ChainNode(vec(Window::GAME_WIDTH*0.1, Window::GAME_HEIGHT*0.5));
}

