#include "Player.h"
#include <dinput.h>
#include "GameData.h"

Player::Player(string _fileName, ID3D11Device* _pd3dDevice, IEffectFactory* _EF) : CMOGO(_fileName, _pd3dDevice, _EF)
{
	//any special set up for Player goes here
	m_fudge = Matrix::CreateRotationY(XM_PI);

	m_pos.y = 10.0f;

	SetDrag(0.7);
	SetPhysicsOn(true);
	m_isVisible = false;
}

Player::~Player()
{
	//tidy up anything I've created
	delete m_FPSCam;
}


void Player::Tick(GameData* _GD)
{
	if (_GD->m_GS == GS_PLAY_MAIN_CAM)
	{
		//MOUSE CONTROL SCHEME HERE
		float speed = 10.0f;
		m_acc.x += speed * _GD->m_mouseState->lX;
		m_acc.z += speed * _GD->m_mouseState->lY;
	}
	else if (_GD->m_GS == GS_PLAY_TPS_CAM || _GD->m_GS == GS_PLAY_FPS_CAM)
	{
		//TURN AND FORWARD CONTROL HERE
		Vector3 forwardMove = 50.0f * Vector3::Forward;
		Matrix rotMove = Matrix::CreateRotationY(m_yaw);
		forwardMove = Vector3::Transform(forwardMove, rotMove);
		if (_GD->m_keyboardState[DIK_W] & 0x80)
		{
			m_acc += forwardMove;
		}
		if (_GD->m_keyboardState[DIK_S] & 0x80)
		{
			m_acc -= forwardMove;
		}
	}

	//change orientation of player
	if (_GD->m_GS != GS_PLAY_FPS_CAM)
	{
		float rotSpeed = 2.0f * _GD->m_dt;
		if (_GD->m_keyboardState[DIK_A] & 0x80)
		{
			m_yaw += rotSpeed;
		}
		if (_GD->m_keyboardState[DIK_D] & 0x80)
		{
			m_yaw -= rotSpeed;
		}
	}

	if (_GD->m_GS == GS_PLAY_FPS_CAM)
	{
		//allow left/right movement with keyboard input
		Vector3 sideMove = 40.0f * Vector3::Right;
		Matrix rotMove = Matrix::CreateRotationY(m_yaw);
		sideMove = Vector3::Transform(sideMove, rotMove);
		if (_GD->m_keyboardState[DIK_D] & 0x80)
		{
			m_acc += sideMove;
		}
		if (_GD->m_keyboardState[DIK_A] & 0x80)
		{
			m_acc -= sideMove;
		}
		m_yaw -= _GD->m_mouseState->lX * 0.01;	//change player orientation with mouse input
	}

	//move player up and down
	if (_GD->m_keyboardState[DIK_R] & 0x80)
	{
		m_acc.y += 40.0f;
	}

	if (_GD->m_keyboardState[DIK_F] & 0x80)
	{
		m_acc.y -= 40.0f;
	}

	//limit motion of the player
	//float length = m_pos.Length();
	//float maxLength = 500.0f;
	//if (length > maxLength)
	//{
	//	m_pos.Normalize();
	//	m_pos *= maxLength;
	//	m_vel *= -0.9; //VERY simple bounce back
	//}

	//apply my base behaviour
	CMOGO::Tick(_GD);
}