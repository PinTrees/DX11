#pragma once

class Camera;

class DisplayManager
{
	SINGLE_HEADER(DisplayManager)

private:
	vector<shared_ptr<Camera>> m_AllCameraComponents;

public:
	void Init();
	void RegisterCameraComponent(const shared_ptr<Camera>& camera); 
	void DeleteCameraComponent(const shared_ptr<Camera>& camera);

	shared_ptr<Camera> GetActiveCamera();
};

