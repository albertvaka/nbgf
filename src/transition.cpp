#include "transition.h"

#define _USE_MATH_DEFINES
#include <math.h>
#include <iostream>

//-TRANSITION----------------------------------------------------------------//
Transition::Transition() {
	m_fPos = 0;
	m_fPosToGo = 0;
}

//-TRANSITION-CONSTANT-VELOCITY----------------------------------------------//
void TransitionVelConst::update(float fTime)
{
	float DistLeft = m_fPos - m_fPosToGo;
	if ( DistLeft < 0 )
	{
		float DistToMove = -m_fVel * fTime;
		if ( DistLeft >= DistToMove )
			DistLeft  = 0;
		else	DistLeft -= DistToMove;
	}
	else if ( DistLeft > 0 )
	{
		float DistToMove =  m_fVel * fTime;
		if ( DistLeft <= DistToMove )
			DistLeft  = 0;
		else	DistLeft -= DistToMove;
	}

	m_fPos = DistLeft + m_fPosToGo;
}

//-TRANSITION-LINEAL---------------------------------------------------------//
void TransitionLinear::setPos(float _fPos)
{
	Transition::setPos(_fPos);
	m_fPercent = 1.0f;
}

void TransitionLinear::goPos(float _fPos)
{
	m_fPercent = 0.0f;
	m_fInitPos = m_fPos;
	Transition::goPos(_fPos);
}

void TransitionLinear::update(float _GameTime)
{
	if(m_fPercent < 1.0f)
	{
		m_fPercent += _GameTime/m_fTime;
		if (m_fPercent >= 1.0f)
		{
			m_fPercent = 1.0f;
			m_fPos = m_fPosToGo;
		}
		else m_fPos = 
			m_fInitPos      * (1.0f-m_fPercent) +
			m_fPosToGo * (     m_fPercent);
	}
}

//-TRANSITION-SINUSOIDAL-----------------------------------------------------//
void TransitionSinus::setPos(float _fPos)
{
	Transition::setPos(_fPos);
	m_fSinPos = (float)M_PI;
}

void TransitionSinus::goPos(float _fPos)
{
	m_fSinPos = 0.0f;
	m_fInitPos = m_fPos;
	Transition::goPos(_fPos);
}

void TransitionSinus::update(float _GameTime)
{
	if(m_fSinPos < M_PI)
	{
		float fPercent = (cos(m_fSinPos)+1.0f)/2.0f;
		m_fPos =
			m_fInitPos * (     fPercent) +
			m_fPosToGo * (1.0f-fPercent);
		m_fSinPos += (float)M_PI/m_fVel*_GameTime;

		if (m_fSinPos > M_PI) {
			m_fSinPos = (float)M_PI;
			m_fPos = m_fPosToGo;
		}
	}
}

//-TRANSITION-SINUSOIDAL-FADE-IN---------------------------------------------//
void TransitionSinusFadeIn::setPos(float _fPos)
{
	Transition::setPos(_fPos);
	m_fSinPos = (float)M_PI/2.0f;
}

void TransitionSinusFadeIn::goPos(float _fPos)
{
	m_fSinPos = 0.0f;
	m_fInitPos = m_fPos;
	Transition::goPos(_fPos);
}

void TransitionSinusFadeIn::update(float _GameTime)
{
	if(m_fSinPos < M_PI/2)
	{
		float fPercent = cos(m_fSinPos);
		m_fPos =
			m_fInitPos * (     fPercent) +
			m_fPosToGo * (1.0f-fPercent);
		m_fSinPos += (float)(M_PI/2)/m_fVel*_GameTime;

		if(m_fSinPos > M_PI/2) {
			m_fSinPos = (float)M_PI/2;
			m_fPos = m_fPosToGo;
		}
	}
}

//-TRANSITION-SINUSOIDAL-FADE-OUT--------------------------------------------//
void TransitionSinusFadeOut::setPos(float _fPos)
{
	Transition::setPos(_fPos);
	m_fSinPos = (float)M_PI/2;
}

void TransitionSinusFadeOut::goPos(float _fPos)
{
	m_fSinPos = 0.0f;
	m_fInitPos = m_fPos;

	Transition::goPos(_fPos);
}

void TransitionSinusFadeOut::update(float _GameTime)
{
	if(m_fSinPos < M_PI/2)
	{
		float fPercent = sin(m_fSinPos);
		m_fPos =
			m_fInitPos * (1.0f-fPercent) +
			m_fPosToGo * (     fPercent);
		m_fSinPos += (float)(M_PI/2)/m_fVel*_GameTime;

		if(m_fSinPos > M_PI/2) {
			m_fSinPos = (float)M_PI/2;
			m_fPos = m_fPosToGo;
		}
	}
}

//-TRANSITION-BOUNCE---------------------------------------------------------//
void TransitionBounce::update(float fTime)
{
	float fFacing;

	     if (m_fPos > m_fPosToGo) fFacing = -1;
	else if (m_fPos < m_fPosToGo) fFacing = +1;
	else {
		     if (m_fVel > 0) fFacing = -1;
		else if (m_fVel < 0) fFacing = +1;
		else fFacing = 0;
	}

	//PHYSICS FORMULA
	float fTimeLeft = (-m_fVel + fFacing * sqrt(m_fVel*m_fVel -
			(4 * (fFacing  * m_fAcc / 2) * 
			(m_fPos - m_fPosToGo))))/(fFacing  * m_fAcc);
	float fNextTime = (fTimeLeft - fTime);

	if ( fNextTime < 0 )
	{
			update(fTimeLeft);

			m_fPos = m_fPosToGo;
			m_fVel = m_fVel*m_fVelLostPercent;

			if(fabs(m_fVel) < 0.001) m_fVel = 0;

			update(float(fabs(fNextTime)));
	}
	else {
		m_fPos += (m_fVel*fTime)+(fTime*fTime*fFacing*m_fAcc/ 2);
		m_fVel       =  m_fVel + fFacing*fTime*m_fAcc;
	}
}

//-TRANSITION-INERTIAL-------------------------------------------------------//
void TransitionInertial::update(float fTime)
{
	float fAcc = 0;
	     if (m_fPos < m_fPosToGo) fAcc = +1 * m_fAcc;
	else if (m_fPos > m_fPosToGo) fAcc = -1 * m_fAcc;
	else return;

	register float TimeToStop = (float)fabs(m_fVel)/m_fAcc;
	register float DistToStop =
		(fabs(m_fVel)*TimeToStop) + (-m_fAcc*TimeToStop*TimeToStop/2);
	register float DistLeft   = fabs(m_fPos-m_fPosToGo);

	if((m_fVel*fAcc) > 0 &&  DistLeft <= DistToStop) fAcc *= -1.0;

	register float fVel = m_fVel + (fTime * fAcc);
	if (fVel * m_fVel < 0) fVel = 0;
	if (fVel >= m_fVelMax) fVel = (fVel/fabs(fVel)) * m_fVelMax;
	m_fVel = fVel;

	m_fPos = m_fPos + (m_fVel*fTime) + (fTime*fTime*fAcc*0.5f);
	if(fabs(m_fPos-m_fPosToGo) <= (m_fVel*fTime*0.5f)) {
		m_fPos = m_fPosToGo; m_fVel = 0;
	}
}
