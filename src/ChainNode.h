#pragma once
#include <utility>
#include "entity.h"
#include "ChainUtils.h"
#include "vec.h"
#include "animation.h"
#include "window_draw.h"

struct ChainNode : CircleEntity
{
	ChainNode(vec aPosition, bool aIsMaster = false);

	void UpdateUnchained(float dt, ChainUtils::tNodesContainer aNodes);
	void UpdateRight(float dt);
	void UpdateLeft(float dt);
	void UpdateVelAndPos(float dt, bool isMaster);
	void UpdateUnchainedVelAndPos(float dt);
	void UpdatePuppet(float aDt, bool isMaster);
	void Draw(SDL_Color aNodeColor) const;

	std::pair<Window::PartialDraw, Window::PartialDraw> PartialDraws(SDL_Color aNodeColor) const;

	void ActivateChainCooldown();
	bool CanBeChained() const;
	bool MustBeUnchained(float& anOutDistance) const;

	bool IsNodeRightReachable(ChainNode* aNodeToReach) const;
	bool IsNodeLeftReachable(ChainNode* aNodeToReach) const;

	void SetRightNeighbor(ChainNode* aRightNeighbor);
	ChainNode* GetRightNeighbor() const;

	void SetLeftNeighbor(ChainNode* aLeftNeighbor);
	ChainNode* GetLeftNeighbor() const;

	const uint16_t myId;
private:
	void UpdatePuppet(float aDt, vec aMasterPos, bool isMaster);
	void RunAwayFrom(vec aPos);
	void Idle();
	bool CheckUnchainDistance(const ChainNode* const aNeighbor, float& anOutDistance) const;

	static uint16_t theLastId;

	ChainNode* myRightNeighbor;
	ChainNode* myLeftNeighbor;
	vec acc;
	Animation anim;										
	float myCooldownToBeChained;
	bool mIsMaster;
};
