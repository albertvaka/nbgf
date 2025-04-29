#pragma once

const GPU_Rect tr_grass(0 * 16, 0 * 16, 16, 16);
const GPU_Rect tr_water(1 * 16, 0 * 16, 16, 16);
const GPU_Rect tr_dock(2 * 16, 2 * 16, 16, 16);
const GPU_Rect tr_coin(0 * 16, 2 * 16, 16, 16);
const GPU_Rect tr_equits(8 * 16, 1 * 16, 16, 16);

const GPU_Rect tr_selection(0, 16, 16, 16);
const GPU_Rect tr_cursor(8 * 16, 0 * 16, 16, 16);
const GPU_Rect tr_cursor_grab(9 * 16, 0 * 16, 16, 16);

const GPU_Rect tr_goldplace(3 * 16, 0 * 16, 16, 16);
const GPU_Rect tr_goldmine(4 * 16, 0 * 16, 16, 16);
const GPU_Rect tr_pick(5 * 16, 0 * 16, 16, 16);

const GPU_Rect tr_food(3 * 16, 7 * 16, 16, 16);
const GPU_Rect tr_lofmxn(48*3, 10 * 16, 48, 48);

const GPU_Rect tr_key(5 * 16, 8 * 16, 16, 16);
const GPU_Rect tr_candado(6 * 16, 8 * 16, 16, 16);

const GPU_Rect spr_idle_apple(7, 5, 16, 16);
const GPU_Rect spr_idle_boy(3, 5, 16, 16);
const GPU_Rect spr_idle_capdetrunyo(5, 5, 16, 16);
const GPU_Rect spr_idle_circle(5, 5, 16, 16);
const GPU_Rect spr_idle_eggman(9, 6, 16, 16);
const GPU_Rect spr_idle_fada(5, 6, 16, 16);
const GPU_Rect spr_idle_megajam(7, 6, 16, 16);
const GPU_Rect spr_idle_sperm(3, 6, 16, 16);


enum struct AnimationType
{
	EGG_SHELL,
	EGG_MOTAS,
	IDLE,

	IDLE_APPLE,
	IDLE_BOY,
	IDLE_CAPDETRUNYO,
	IDLE_CIRCLE,
	IDLE_EGGMAN,
	IDLE_FADA,
	IDLE_MEGAJAM,
	IDLE_SPERM,
	LOVE_MAKING,
	RIP,
	PICK_EFFECT,
	LOVE_MACHINE,
	COINZOR
};

GPU_Rect Animation__GetSprite(AnimationType type, int time, int anim_speed = 0)
{
	GPU_Rect result;

	switch (type)
	{
		case AnimationType::EGG_SHELL:
		{
			time = time % 1000;
			if (time < 500)
			{
				result = GPU_Rect(0, 4, 16, 16);
			}
			else
			{
				result = GPU_Rect(1, 4, 16, 16);
			}
		} break;
		case AnimationType::EGG_MOTAS:
		{
			time = time % 1000;
			if (time < 500)
			{
				result = GPU_Rect(2, 4, 16, 16);
			}
			else
			{
				result = GPU_Rect(3, 4, 16, 16);
			}
		} break;
		case AnimationType::RIP:
		{
			result = GPU_Rect(15, 3, 16, 16);
		} break;
		case AnimationType::IDLE:
		{
			int animod = 800;
			if (anim_speed != 0)
			{
				animod = anim_speed;
			}
			
			time = time % animod;
			if (time < animod / 2)
			{
				//result = GPU_Rect(0, 3, 16, 16);
			}
			else
			{
				result.x += 1;
				//result = GPU_Rect(1, 3, 16, 16);
			}
		} break;

		case AnimationType::IDLE_APPLE:
		{
			result = spr_idle_apple;
			result.x += (((time%anim_speed) < (anim_speed / 2)) ? 1 : 0);
		} break;

		case AnimationType::IDLE_BOY:
		{
			result = spr_idle_boy;
			result.x += (((time%anim_speed) < (anim_speed / 2)) ? 1 : 0);
		} break;

		case AnimationType::IDLE_CAPDETRUNYO:
		{
			result = spr_idle_capdetrunyo;
			result.x += (((time%anim_speed) < (anim_speed / 2)) ? 1 : 0);
		} break;

		case AnimationType::IDLE_CIRCLE:
		{
			result = spr_idle_circle;
			result.x += (((time%anim_speed) < (anim_speed / 2)) ? 1 : 0);
		} break;

		case AnimationType::IDLE_EGGMAN:
		{
			result = spr_idle_eggman;
			result.x += (((time%anim_speed) < (anim_speed / 2)) ? 1 : 0);
		} break;

		case AnimationType::IDLE_FADA:
		{
			result = spr_idle_fada;
			result.x += (((time%anim_speed) < (anim_speed / 2)) ? 1 : 0);
		} break;

		case AnimationType::IDLE_MEGAJAM:
		{
			result = spr_idle_megajam;
			result.x += (((time%anim_speed) < (anim_speed / 2)) ? 1 : 0);
		} break;

		case AnimationType::IDLE_SPERM:
		{
			result = spr_idle_sperm;
			result.x += (((time%anim_speed) < (anim_speed / 2)) ? 1 : 0);
		} break;
				
			

			

		case AnimationType::PICK_EFFECT:
		{
			time = time % 100;
			if (time < 25)
			{
				result = GPU_Rect(6, 1, 16, 16);
			}
			else if (time < 50)
			{
				result = GPU_Rect(7, 1, 16, 16);
			}
			else if (time < 75)
			{
				result = GPU_Rect(8, 1, 16, 16);
			}
			else
			{
				result = GPU_Rect(9, 1, 16, 16);
			}
		} break;

		case AnimationType::LOVE_MACHINE:
		{
			time = time % 1500;
			if (time < 500)
			{
				return GPU_Rect(48 * 2, 10 * 16, 48, 48);
			}
			else if (time < 1000)
			{
				return GPU_Rect(48, 10 * 16, 48, 48);
			}
			else
			{
				return GPU_Rect(0, 10 * 16, 48, 48);
			}
		} break;


		case AnimationType::COINZOR:
		{
			time = time % 80;

			if (time < 10)
			{
				result = GPU_Rect(0, 9, 16, 16);
			}
			else if (time < 20)
			{
				result = GPU_Rect(1, 9, 16, 16);
			}
			else if (time < 30)
			{
				result = GPU_Rect(2, 9, 16, 16);
			}
			else if (time < 40)
			{
				result = GPU_Rect(3, 9, 16, 16);
			}
			else if (time < 50)
			{
				result = GPU_Rect(4, 9, 16, 16);
			}
			else if (time < 60)
			{
				result = GPU_Rect(3, 9, 16, 16);
			}
			else if (time < 70)
			{
				result = GPU_Rect(2, 9, 16, 16);
			}
			else if (time < 80)
			{
				result = GPU_Rect(1, 9, 16, 16);
			}
		} break;

		default: 
		{
			result = GPU_Rect(1, 2, 16, 16);
		} break;
		
	}

	result.x *= 16;
	result.y *= 16;

	return result;
}
