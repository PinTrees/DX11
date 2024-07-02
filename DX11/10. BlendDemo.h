#pragma once
#include "App.h"
#include "Waves.h"
#include "LightHelper.h"

class BlendDemo : public App
{
	enum RenderOptions
	{
		Lighting = 0,
		Textures = 1,
		TexturesAndFog = 2
	};
public:
	BlendDemo(HINSTANCE hInstance);
	~BlendDemo();

	bool Init();
	void OnResize();
	void UpdateScene(float dt);
	void DrawScene();

	void OnMouseDown(WPARAM btnState, int x, int y);
	void OnMouseUp(WPARAM btnState, int x, int y);
	void OnMouseMove(WPARAM btnState, int x, int y);

private:
	float GetHillHeight(float x, float z) const;
	XMFLOAT3 GetHillNormal(float x, float z) const;
	void BuildLandGeometryBuffers();
	void BuildWaveGeometryBuffers();
	void BuildCrateGeometryBuffers();

private:
	ComPtr<ID3D11Buffer> _landVB;
	ComPtr<ID3D11Buffer> _landIB;

	ComPtr<ID3D11Buffer> _wavesVB;
	ComPtr<ID3D11Buffer> _wavesIB;

	ComPtr<ID3D11Buffer> _boxVB;
	ComPtr<ID3D11Buffer> _boxIB;

	ComPtr<ID3D11ShaderResourceView> _grassMapSRV;
	ComPtr<ID3D11ShaderResourceView> _wavesMapSRV;
	ComPtr<ID3D11ShaderResourceView> _boxMapSRV;

	Waves _waves;

	DirectionalLight _dirLights[3];
	Material _landMat;
	Material _wavesMat;
	Material _boxMat;

	XMFLOAT4X4 _grassTexTransform;
	XMFLOAT4X4 _waterTexTransform;
	XMFLOAT4X4 _landWorld;
	XMFLOAT4X4 _wavesWorld;
	XMFLOAT4X4 _boxWorld;

	XMFLOAT4X4 _view;
	XMFLOAT4X4 _proj;

	uint32 _landIndexCount;
	XMFLOAT2 _waterTexOffset = {0, 0};

	RenderOptions _renderOptions = RenderOptions::TexturesAndFog;

	XMFLOAT3 _eyePosW = {0, 0, 0};

	float _theta = 1.3f * XM_PI;
	float _phi = 0.4f * XM_PI;
	float _radius = 80.f;

	POINT _lastMousePos;
};
