#pragma once

#include <vector>
#include <functional>
#include "selfregister.h"

// Use CutSceneBuilder below to create CutScenes
struct CutScene : SelfRegister<CutScene>
{
	struct Animation
	{
		Animation(float total_time, std::function<void(float progress)> fn)
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
		playing.erase(std::remove_if(playing.begin(), playing.end(), [dt](Animation& a) { return a.Run(dt); }), playing.end());
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
	std::vector<Animation> queue;
	std::vector<Animation> playing;
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
		if (cutScene->playing.empty())
		{
			Debug::out << "Warning: Destroying CutScene builder but the built CutScene isn't playing anything";
		}
	}

	void Start() {
		cutScene->PlayFromQueue();
	}

	CutSceneBuilder& Play(float total_time, std::function<void(float progress)> fn)
	{
		cutScene->queue.emplace_back(CutScene::Animation(total_time, fn));
		return *this;
	}

	CutSceneBuilder& PlayOneFrame(std::function<void()> fn)
	{
		cutScene->queue.emplace_back(CutScene::Animation(0.f, [fn](float) { fn(); }));
		return *this;
	}

	CutSceneBuilder& WaitAndThen()
	{
		cutScene->queue.back().blocking = true;
		return *this;
	}
};
