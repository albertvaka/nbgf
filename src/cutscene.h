#pragma once

#include <vector>
#include <functional>
#include "selfregister.h"
#include "singleinstance.h"

// Use CutSceneBuilder below to create CutScenes
// Or use Tweening to add tweening animations that play immediately

struct AnimationStep
{
	AnimationStep(float total_time, std::function<void(float progress)> fn)
		: fn(fn)
		, progress(0.0f)
		, total_time(total_time)
		, blocking(false)
	{}

	std::function<void(float progress)> fn;
	float progress;
	float total_time;
	bool blocking;

	bool Run(float dt)
	{
		progress += dt;
		if (progress >= total_time)
		{
			fn(1.0f);
			return true;
		}
		else
		{
			fn(progress / total_time);
			return false;
		}
	}

};

struct Tweening : SingleInstance<Tweening>
{
	std::vector<AnimationStep> playing;

	void Update(float dt)
	{
		playing.erase(std::remove_if(playing.begin(), playing.end(), [dt](AnimationStep& a) { return a.Run(dt); }), playing.end());
	}

	static void Play(float total_time, std::function<void(float progress)> fn)
	{
		instance()->playing.emplace_back(AnimationStep(total_time, fn));
	}

	void Clear() {
		playing.clear();
	}

};

struct CutScene : SelfRegister<CutScene>
{
	void PlayFromQueue()
	{
		auto after_first_blocking = queue.begin();
		while (after_first_blocking != queue.end() && !after_first_blocking->blocking)
		{
			after_first_blocking++;
		}
		if (after_first_blocking != queue.end())
		{
			after_first_blocking++;
		}

		playing.insert(playing.end(), std::make_move_iterator(queue.begin()), std::make_move_iterator(after_first_blocking));
		queue.erase(queue.begin(), after_first_blocking);
	}

	// Returns true if the game should be paused while animations run
	void Update(float dt)
	{
		playing.erase(std::remove_if(playing.begin(), playing.end(), [dt](AnimationStep& a) { return a.Run(dt); }), playing.end());
		if (playing.empty())
		{
			if (!queue.empty())
			{
				PlayFromQueue();
			}
			else
			{
				alive = false;
			}
		}
	}

	std::function<void()> callback = nullptr;
	bool pauseGame = false;
	std::vector<AnimationStep> queue;
	std::vector<AnimationStep> playing;
	bool alive = true;
};

struct CutSceneBuilder
{

	CutScene* cutScene;

	CutSceneBuilder(bool pauseGame = false)
	{
		cutScene = new CutScene();
		cutScene->pauseGame = pauseGame;
	}

	~CutSceneBuilder()
	{
		// If this isn't called, the cutscene still plays but it is delayed one frame
		cutScene->PlayFromQueue();
	}

	CutSceneBuilder& Play(float total_time, std::function<void(float progress)> fn)
	{
		cutScene->queue.emplace_back(AnimationStep(total_time, fn));
		return *this;
	}

	CutSceneBuilder& PlayOneFrame(std::function<void()> fn)
	{
		cutScene->queue.emplace_back(AnimationStep(0.f, [fn](float) { fn(); }));
		return *this;
	}

	CutSceneBuilder& DoNothingFor(float t)
	{
		cutScene->queue.emplace_back(AnimationStep(t, [](float) { }));
		cutScene->queue.back().blocking = true;
		return *this;
	}

	CutSceneBuilder& WaitAndThen()
	{
		cutScene->queue.back().blocking = true;
		return *this;
	}
};
