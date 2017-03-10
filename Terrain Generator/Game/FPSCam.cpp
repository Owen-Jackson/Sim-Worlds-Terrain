#include "FPSCam.h"
#include <iostream>

FPSCam::FPSCam(float _fieldOfView, float _aspectRatio, float _nearPlaneDistance, float _farPlaneDistance, GameObject* _parent, Vector3 _up)
	: Camera(_fieldOfView, _aspectRatio, _nearPlaneDistance, _farPlaneDistance, _up)
{
	m_parentObject = _parent;
}


FPSCam::~FPSCam()
{
	;
}

void FPSCam::Tick(GameData* _GD)
{
	//Set up position of camera and target position of camera based on new position and orientation of target object
	Matrix rotCam = Matrix::CreateFromYawPitchRoll(m_parentObject->GetYaw(), m_yRot, 0.0f);
	Vector3 cameraOffset = Vector3::Transform(m_offset, rotCam);

	Vector3 offset = m_parentObject->GetPos() + cameraOffset;

	Vector3 transformedReference = Vector3::Transform(m_cameraReference, rotCam);

	m_pos = m_parentObject->GetPos() + m_offset;
	m_target = m_pos - transformedReference;


	if (_GD->m_GS == GS_PLAY_FPS_CAM)
	{
		
		//m_xRot -= _GD->m_mouseState->lX * 0.01;
		m_yRot -= _GD->m_mouseState->lY * 0.01;
		if (m_yRot >= XM_PIDIV2 - 0.01)
		{
			m_yRot = XM_PIDIV2 - 0.01;
		}
		if (m_yRot <= -XM_PIDIV2 + 0.01)
		{
			m_yRot = -XM_PIDIV2 + 0.01;
		}
	}



	//and then set up proj and view matrices
	Camera::Tick(_GD);
}