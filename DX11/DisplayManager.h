#pragma once

class Camera;

class DisplayManager
{
	SINGLE_HEADER(DisplayManager)

private:
	vector<weak_ptr<Camera>> m_AllCameraComponents; 

public:
	void Init();
	void RegisterCameraComponent(const weak_ptr<Camera>& camera); 
	void DeleteCameraComponent(const weak_ptr<Camera>& camera); 

	shared_ptr<Camera> GetActiveCamera();
};

