#pragma once

#include "rand.h"

#include "extremity.h"
#include "entity.h"
#include "taca.h"
#include "window_draw.h"
#include "window_drawprimitive.h"
#include "assets.h"

static bool withTaca = true;

struct SpawnAnim : SortedDrawable, SelfRegister<SpawnAnim>
{
	int timer = 0;

	vec offset;

	SpawnAnim(vec _pos, vec _offset = vec())
	{
		anim.Ensure(AnimationType::SPAWN);
		pos = _pos;
		pos.y += 150;
		offset = _offset;
		offset.y -= 150;
	}

	void Update(int dt)
	{
		timer += dt;
		if (timer > 650)
		{
			alive = false;
		}
	}

	void Draw()
	{
		Window::Draw(Assets::sprite, pos + offset).withRect(anim.CurrentFrame()).withOrigin(8,8);
	}


};

struct Cadaver : SortedDrawable, Cintable, SelfRegister<Cadaver>
{
	ExtremityColor rightLeg = ExtremityColor::NONE_COLOR;
	ExtremityColor leftLeg = ExtremityColor::NONE_COLOR;
	ExtremityColor rightArm = ExtremityColor::NONE_COLOR;
	ExtremityColor leftArm = ExtremityColor::NONE_COLOR;
	ExtremityColor head = ExtremityColor::NONE_COLOR;
	ExtremityColor body = ExtremityColor::NONE_COLOR;

	bool isCarried = false;
	bool isLet = false;
	float isLetClock;

	float counterBloodTimeLeft = 100.f;

	Cadaver(vec _pos) {
		pos = _pos;

		ExtremityColor color = RandomExtremityColor();

		SetExtremityColor(RandomExtremityType(), color);
		SetExtremityColor(RandomExtremityType(), color);
		SetExtremityColor(RandomExtremityType(), color);
		SetExtremityColor(RandomExtremityType(), color);
		body = color;
	}

	void SetExtremityColor(ExtremityType type, ExtremityColor color) {
		switch (type) {
		case ExtremityType::HEAD:
			head = color;
			break;
		case ExtremityType::LEFT_ARM:
			leftArm = color;
			break;
		case ExtremityType::RIGHT_ARM:
			rightArm = color;
			break;
		case ExtremityType::LEFT_LEG:
			leftLeg = color;
			break;
		case ExtremityType::RIGHT_LEG:
			rightLeg = color;
			break;
		case ExtremityType::BODY:
			body = color;
			break;
		}
	}
	bool HasExtremity(ExtremityType et) {
		switch (et) {
		case ExtremityType::HEAD:
			return head != ExtremityColor::NONE_COLOR;
		case ExtremityType::LEFT_ARM:
			return leftArm != ExtremityColor::NONE_COLOR;
		case ExtremityType::RIGHT_ARM:
			return rightArm != ExtremityColor::NONE_COLOR;
		case ExtremityType::LEFT_LEG:
			return leftLeg != ExtremityColor::NONE_COLOR;
		case ExtremityType::RIGHT_LEG:
			return rightLeg != ExtremityColor::NONE_COLOR;
		case ExtremityType::BODY:
			return body != ExtremityColor::NONE_COLOR;
		}
	}

	void AttachExtremity(Extremity* e) {
		SetExtremityColor(e->type, e->color);
		e->alive = false;

		new SpawnAnim(e->pos);
		new SpawnAnim(this->pos, vec(0, 0));

	}

	Extremity* DeatachExtremity(ExtremityType et, vec pos) {
		ExtremityColor color;
		switch (et) {
		case ExtremityType::HEAD:
			color = head;
			head = ExtremityColor::NONE_COLOR;
			break;
		case ExtremityType::LEFT_ARM:
			color = leftArm;
			leftArm = ExtremityColor::NONE_COLOR;
			break;
		case ExtremityType::RIGHT_ARM:
			color = rightArm;
			rightArm = ExtremityColor::NONE_COLOR;
			break;
		case ExtremityType::LEFT_LEG:
			color = leftLeg;
			leftLeg = ExtremityColor::NONE_COLOR;
			break;
		case ExtremityType::RIGHT_LEG:
			color = rightLeg;
			rightLeg = ExtremityColor::NONE_COLOR;
			break;
		case ExtremityType::BODY:
			color = body;
			body = ExtremityColor::NONE_COLOR;
			break;
		}
		Extremity* e = new Extremity(pos, et, color);
		e->isLet = true;
		e->isCarried = false;

		new SpawnAnim(pos);
		new SpawnAnim(this->pos, vec(0, 0));

		return e;
	}

	vec positionPlz() override { return pos; }
	vec sizePlz() override { return size; }

	void carryCadaver(float x, float y)
	{
		isCarried = true;

		pos.x = x;
		pos.y = y + 2;

		isLet = false;
	}

	void putCadaverOnTable(vec position)
	{
		isCarried = false;
		isLet = true;

		isLetClock = 0;

		pos.x = position.x;
		pos.y = position.y+0.1f;
	}

	void SpawnBlood() {
		Taca *t = new Taca(pos, currCintaDirection);
		counterBloodTimeLeft = 100;
		for (Taca* p : SelfRegister<Taca>::GetAll())
		{
			if (p == t) continue;
			if (p->getFinalPos() == t->getFinalPos())
			{
				t->alive = false;
			}
		}
	}

	void Update(int dt) override
	{
		isLetClock += dt/1000.f;
		counterBloodTimeLeft -= dt * Rand::roll(0, 4);
		if (counterBloodTimeLeft < 0 && withTaca)
		{
			SpawnBlood();
		}
		Move(dt);
	}

	void Move(int dt)
	{
		SetSpeedWithCinta(speed);
		pos += speed * dt;

		speed.x = 0;
		speed.y = 0;

	}

	bool IsOk()
	{
		bool has_all = true;
		for (int i = 0; i < magic_enum::enum_count<ExtremityType>(); ++i)
		{
			has_all &= HasExtremity((ExtremityType)i);
		}
		return has_all;
	}

	void Draw() override
	{

		//BoxBounds(pos, size, true).Draw(window, SDL_Color::Green);

		if (isLet) //DrawEfectito() de la mesa (TODO: mover a la mesa)
		{
			float color = std::max(std::min(1.0f, abs(sin(0.75f*isLetClock))) * 255, 64.0f);
			SDL_Color drawColor = {0, Uint8(((color*3)/4)), Uint8(color), 255};
			vec position = vec(pos.x - 7.5f, pos.y - 8.5f);
			vec sizerino = vec(15, 15);
			Window::DrawPrimitive::Rectangle(position, position+sizerino, -1, drawColor);
		}

		DrawBody();

		if (rightLeg != ExtremityColor::NONE_COLOR)
		{
			DrawRightLeg();
		}

		if (leftLeg != ExtremityColor::NONE_COLOR)
		{
			DrawLeftLeg();
		}

		if (rightArm != ExtremityColor::NONE_COLOR)
		{
			DrawRightArm();
		}

		if (leftArm != ExtremityColor::NONE_COLOR)
		{
			DrawLeftArm();
		}

		if (head != ExtremityColor::NONE_COLOR)
		{
			DrawHead();
		}


	/*	if (isCarriable)
		{
			DrawCarriable();
		}*/

		//BoxBounds(pos.x - 1, pos.y - 1, 2, 2).Draw(window);
	}

	//floor
	const float xt = 3;
	const float yt = -5;

	//carried
	const float xp = 5.5;
	const float yp = -6.5;

	//let
	const float xs = 4;
	const float ys = 3;

	void DrawRightLeg()
	{
		vec drawPos;
		if (isLet) {
			drawPos = vec(pos.x + 1 + xs, pos.y + 3 + ys);
		}
		else if (isCarried) {
			drawPos = vec(pos.x + 3 + xp, pos.y - 1 + yp);
		}
		else {
			drawPos = vec(pos.x + 3 + xt, pos.y - 1 + yt);
		}
		Window::Draw(Assets::sprite, drawPos).withRect(extremitySprPos[rightLeg][ExtremityType::RIGHT_LEG]).withRotationDegs(isLet? 0 : -90).withOrigin(8,8);
	}

	void DrawLeftLeg()
	{
		vec drawPos;
		if (isLet) {
			drawPos = vec(pos.x - 1 + xs, pos.y + 3 + ys);
		}
		else if (isCarried) {
			drawPos = vec(pos.x + 3 + xp, pos.y + 1 + yp);
		}
		else {
			drawPos = vec(pos.x + 3 + xt, pos.y + 1 + yt);
		}
		Window::Draw(Assets::sprite, drawPos).withRect(extremitySprPos[leftLeg][ExtremityType::LEFT_LEG]).withRotationDegs(isLet? 0 : -90).withOrigin(8,8);
	}

	void DrawRightArm()
	{
		vec drawPos;
		if (isLet) {
			drawPos = vec(pos.x+1 + xs, pos.y - 0 + ys);
		}
		else if (isCarried) {
			drawPos = vec(pos.x + xp, pos.y - 1 + yp);
		}
		else {
			drawPos = vec(pos.x + xt, pos.y - 1 + yt);
		}
		Window::Draw(Assets::sprite, drawPos).withRect(extremitySprPos[rightArm][ExtremityType::RIGHT_ARM]).withRotationDegs(isLet? 0 : -90).withOrigin(8,8);
	}

	void DrawLeftArm()
	{
		vec drawPos;
		if (isLet) {
			drawPos = vec(pos.x-1 + xs, pos.y-0 + ys);
		}
		else if (isCarried) {
			drawPos = vec(pos.x + xp, pos.y + 1 + yp);
		}
		else {
			drawPos = vec(pos.x + xt, pos.y+1 + yt);
		}
		Window::Draw(Assets::sprite, drawPos).withRect(extremitySprPos[leftArm][ExtremityType::LEFT_ARM]).withRotationDegs(isLet? 0 : -90).withOrigin(8,8);
	}

	void DrawHead()
	{
		vec drawPos;
		if (isLet) {
			drawPos = vec(pos.x + xs, pos.y-3 + ys);
		}
		else if (isCarried) {
			drawPos = vec(pos.x - 3 + xp, pos.y + yp);
		}
		else {
			drawPos = vec(pos.x - 3 + xt, pos.y + yt);
		}
		Window::Draw(Assets::sprite, drawPos).withRect(extremitySprPos[head][ExtremityType::HEAD]).withRotationDegs(isLet? 0 : -90).withOrigin(8,8);
	}

	void DrawBody()
	{
		vec drawPos;
		if (isLet) {
			drawPos = vec(pos.x + xs, pos.y + ys);
		}
		else if (isCarried) {
			drawPos = vec(pos.x + xp, pos.y + yp);
		}
		else {
			drawPos = vec(pos.x + xt, pos.y + yt);
		}
		Window::Draw(Assets::sprite, drawPos).withRect(extremitySprPos[body][ExtremityType::BODY]).withRotationDegs(isLet? 0 : -90).withOrigin(8,8);
	}

};
