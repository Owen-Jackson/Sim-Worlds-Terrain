#ifndef _FPS_CAMERA_
#define _FPS_CAMERA_
#include "camera.h"
#include "GameData.h"

class FPSCam : public Camera
{
public:
	FPSCam(float _fieldOfView, float _aspectRatio, float _nearPlaneDistance, float _farPlaneDistance, GameObject* _parent, Vector3 _up);
	~FPSCam();

	virtual void Tick(GameData* _GD);
private:
	Vector3 m_offset = Vector3(0.0f, 0.0f, 0.0f);
	Vector3 m_cameraReference = Vector3(0.0f, 0.0f, 10.0f); //the direcion the camera points towards
	GameObject* m_parentObject = nullptr;
	float m_yRot = 0;	//the rotation about the y axis, checked to stop camera rotating too far
	float m_xRot = 0;

};
#endif