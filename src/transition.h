#ifndef TRANSITION_H_
#define TRANSITION_H_

//-TRANSITION-(ABSTRACT-CLASS)-----------------------------------------------//
class Transition {
public:
	 Transition();
	~Transition() {}

	virtual void  setPos(float _fPos) {m_fPos = m_fPosToGo = _fPos;}
	virtual void   goPos(float _fPos) {m_fPosToGo = _fPos;}

	virtual void update(float _GameTime) = 0;

	virtual float getPos() {return m_fPos;}
	virtual bool reached() {return m_fPosToGo == m_fPos;}

protected:

	float m_fPos;
	float m_fPosToGo;
};

//-TRANSITION-CONSTANT-VELOCITY----------------------------------------------//
class TransitionVelConst : public Transition {
public:
	 TransitionVelConst() : m_fVel(1.0f) {}
	~TransitionVelConst() {}

	void update(float _GameTime);

	void setVel(float _fSecsTotal) {m_fVel = _fSecsTotal;}

protected:

	float m_fVel;
};

//-TRANSITION-LINEAR---------------------------------------------------------//
class TransitionLinear : public Transition {
public:
	 TransitionLinear() : m_fTime(1.0f), m_fPercent(1.0f) {}
	~TransitionLinear() {}

	void setPos(float _fPos);
	void  goPos(float _fPos);
	void update(float _GameTime);

	void setTime(float _fTimeToReach) { m_fTime = _fTimeToReach;}

protected:

	float m_fTime;
	float m_fPercent;
	float m_fInitPos;
};

//-TRANSITION-SINUSOIDAL-----------------------------------------------------//
class TransitionSinus : public Transition {
public:
	 TransitionSinus() : m_fSinPos(0.0f), m_fVel(0.5f) {}
	~TransitionSinus() {}

	void setPos(float _fPos);
	void  goPos(float _fPos);
	void update(float _GameTime);

	void setTime(float _fSecsTotal) { m_fVel = _fSecsTotal;}

protected:

	float m_fSinPos;
	float m_fVel;
	float m_fInitPos;
};

//-TRANSITION-SINUSOIDAL-FADE-IN---------------------------------------------//

class TransitionSinusFadeIn : public Transition {
public:
	 TransitionSinusFadeIn() : m_fSinPos(0.0f), m_fVel(0.5f) {}
	~TransitionSinusFadeIn() {}

	void setPos(float _fPos);
	void  goPos(float _fPos);
	void update(float _GameTime);

	void setTime(float _fSecsTotal) { m_fVel = _fSecsTotal;}

protected:

	float m_fSinPos;
	float m_fVel;
	float m_fInitPos;
};

//-TRANSITION-SINUSOIDAL-FADE-OUT--------------------------------------------//
class TransitionSinusFadeOut : public Transition {
public:
	 TransitionSinusFadeOut() : m_fSinPos(0.0f), m_fVel(0.5f) {}
	~TransitionSinusFadeOut() {}

	void setPos(float _fPos);
	void  goPos(float _fPos);
	void update(float _GameTime);

	void setTime(float _fSecsTotal) { m_fVel = _fSecsTotal;}

protected:

	float m_fSinPos;
	float m_fVel;
	float m_fInitPos;
};

//-TRANSITION-BOUNCE---------------------------------------------------------//
class TransitionBounce : public Transition {
public:
	 TransitionBounce() : m_fVel(0), m_fAcc(5), m_fVelLostPercent(0.4f) {}
	~TransitionBounce();

	void update(float _GameTime);
	bool reached() {return (m_fPosToGo == m_fPos) && m_fVel == 0;}

	void setAcc(float _fUnitsPerSec) {m_fAcc = _fUnitsPerSec;}
	void setVelLost(float _fPercentVel) {m_fVelLostPercent = _fPercentVel;}

protected:

	float   m_fVel;
	float   m_fAcc;
	float   m_fVelLostPercent;
};

//-TRANSITION-INERTIAL-------------------------------------------------------//
class TransitionInertial : public Transition {
public:
	 TransitionInertial() : m_fVel(0), m_fAcc(1.f), m_fVelMax(3.0f) {}
	~TransitionInertial();

	void update(float _GameTime);
	void update(int   _GameTime) {update(float(_GameTime)/1000.f);}
	bool reached() {return (m_fPosToGo == m_fPos) && m_fVel == 0;}

	void setAcc(float _fUnitsPerSec) {m_fAcc = _fUnitsPerSec;}
	void setVelMax(float _fVelMax) {m_fVelMax = _fVelMax;}

protected:

	float m_fVel;
	float m_fAcc;
	float m_fVelMax;
};
#endif /* TRANSITION_H_ */
