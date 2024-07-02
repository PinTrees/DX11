#include "pch.h"
#include "26. ShadowsDemo.h"
#include "MathHelper.h"
#include "GeometryGenerator.h"
#include "Effects.h"
#include "Vertex.h"
#include "RenderStates.h"
#include "Sky.h"
#include "ShadowMap.h"

ShadowsDemo::ShadowsDemo(HINSTANCE hInstance)
	: App(hInstance)
{
	_mainWindowCaption = L"Shadows Demo";

	_lastMousePos.x = 0;
	_lastMousePos.y = 0;

	_camera.SetPosition(0.0f, 2.0f, -15.0f);

	// Estimate the scene bounding sphere manually since we know how the scene was constructed.
	// The grid is the "widest object" with a width of 20 and depth of 30.0f, and centered at
	// the world space origin.  In general, you need to loop over every world space vertex
	// position and compute the bounding sphere.
	_sceneBounds.Center = XMFLOAT3(0.0f, 0.0f, 0.0f);
	_sceneBounds.Radius = sqrtf(10.0f * 10.0f + 15.0f * 15.0f);

	XMMATRIX I = ::XMMatrixIdentity();
	::XMStoreFloat4x4(&_gridWorld, I);

	XMMATRIX boxScale = ::XMMatrixScaling(3.0f, 1.0f, 3.0f);
	XMMATRIX boxOffset = ::XMMatrixTranslation(0.0f, 0.5f, 0.0f);
	::XMStoreFloat4x4(&_boxWorld, ::XMMatrixMultiply(boxScale, boxOffset));

	XMMATRIX skullScale = ::XMMatrixScaling(0.5f, 0.5f, 0.5f);
	XMMATRIX skullOffset = ::XMMatrixTranslation(0.0f, 1.0f, 0.0f);
	::XMStoreFloat4x4(&_skullWorld, ::XMMatrixMultiply(skullScale, skullOffset));

	for (int32 i = 0; i < 5; ++i)
	{
		::XMStoreFloat4x4(&_cylWorld[i * 2 + 0], ::XMMatrixTranslation(-5.0f, 1.5f, -10.0f + i * 5.0f));
		::XMStoreFloat4x4(&_cylWorld[i * 2 + 1], ::XMMatrixTranslation(+5.0f, 1.5f, -10.0f + i * 5.0f));

		::XMStoreFloat4x4(&_sphereWorld[i * 2 + 0], ::XMMatrixTranslation(-5.0f, 3.5f, -10.0f + i * 5.0f));
		::XMStoreFloat4x4(&_sphereWorld[i * 2 + 1], ::XMMatrixTranslation(+5.0f, 3.5f, -10.0f + i * 5.0f));
	}

	_dirLights[0].Ambient = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	_dirLights[0].Diffuse = XMFLOAT4(0.7f, 0.7f, 0.6f, 1.0f);
	_dirLights[0].Specular = XMFLOAT4(0.8f, 0.8f, 0.7f, 1.0f);
	_dirLights[0].Direction = XMFLOAT3(-0.57735f, -0.57735f, 0.57735f);

	// Shadow acne gets worse as we increase the slope of the polygon (from the
	// perspective of the light).
	//mDirLights[0].Direction = XMFLOAT3(5.0f/sqrtf(50.0f), -5.0f/sqrtf(50.0f), 0.0f);
	//mDirLights[0].Direction = XMFLOAT3(10.0f/sqrtf(125.0f), -5.0f/sqrtf(125.0f), 0.0f);
	//mDirLights[0].Direction = XMFLOAT3(10.0f/sqrtf(116.0f), -4.0f/sqrtf(116.0f), 0.0f);
	//mDirLights[0].Direction = XMFLOAT3(10.0f/sqrtf(109.0f), -3.0f/sqrtf(109.0f), 0.0f);

	_dirLights[1].Ambient = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	_dirLights[1].Diffuse = XMFLOAT4(0.40f, 0.40f, 0.40f, 1.0f);
	_dirLights[1].Specular = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	_dirLights[1].Direction = XMFLOAT3(0.707f, -0.707f, 0.0f);

	_dirLights[2].Ambient = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	_dirLights[2].Diffuse = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	_dirLights[2].Specular = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	_dirLights[2].Direction = XMFLOAT3(0.0f, 0.0, -1.0f);

	_originalLightDir[0] = _dirLights[0].Direction;
	_originalLightDir[1] = _dirLights[1].Direction;
	_originalLightDir[2] = _dirLights[2].Direction;

	_gridMat.Ambient = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	_gridMat.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	_gridMat.Specular = XMFLOAT4(0.4f, 0.4f, 0.4f, 16.0f);
	_gridMat.Reflect = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);

	_cylinderMat.Ambient = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	_cylinderMat.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	_cylinderMat.Specular = XMFLOAT4(1.0f, 1.0f, 1.0f, 32.0f);
	_cylinderMat.Reflect = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);

	_sphereMat.Ambient = XMFLOAT4(0.2f, 0.3f, 0.4f, 1.0f);
	_sphereMat.Diffuse = XMFLOAT4(0.2f, 0.3f, 0.4f, 1.0f);
	_sphereMat.Specular = XMFLOAT4(0.9f, 0.9f, 0.9f, 16.0f);
	_sphereMat.Reflect = XMFLOAT4(0.4f, 0.4f, 0.4f, 1.0f);

	_boxMat.Ambient = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	_boxMat.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	_boxMat.Specular = XMFLOAT4(0.8f, 0.8f, 0.8f, 16.0f);
	_boxMat.Reflect = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);

	_skullMat.Ambient = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	_skullMat.Diffuse = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	_skullMat.Specular = XMFLOAT4(0.8f, 0.8f, 0.8f, 16.0f);
	_skullMat.Reflect = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
}

ShadowsDemo::~ShadowsDemo()
{
	Effects::DestroyAll();
	InputLayouts::DestroyAll();
	RenderStates::DestroyAll();
}

bool ShadowsDemo::Init()
{
	if (!App::Init())
		return false;

	// Must init Effects first since InputLayouts depend on shader signatures.
	Effects::InitAll(_device, L"../Shaders/26. Basic.fx");
	InputLayouts::InitAll(_device);
	RenderStates::InitAll(_device);

	_sky = make_shared<Sky>(_device, L"../Resources/Textures/desertcube1024.dds", 5000.0f);

	_smap = make_shared<ShadowMap>(_device, SMapSize, SMapSize);

	_stoneTexSRV = Utils::LoadTexture(_device, L"../Resources/Textures/floor.dds");
	_brickTexSRV = Utils::LoadTexture(_device, L"../Resources/Textures/bricks.dds");
	_stoneNormalTexSRV = Utils::LoadTexture(_device, L"../Resources/Textures/floor_nmap.dds");
	_brickNormalTexSRV = Utils::LoadTexture(_device, L"../Resources/Textures/bricks_nmap.dds");

	BuildShapeGeometryBuffers();
	BuildSkullGeometryBuffers();
	BuildScreenQuadGeometryBuffers();

	return true;
}

void ShadowsDemo::OnResize()
{
	App::OnResize();

	_camera.SetLens(0.25f * MathHelper::Pi, AspectRatio(), 1.0f, 1000.0f);
}

void ShadowsDemo::UpdateScene(float dt)
{
	//
	// Control the camera.
	//
	if (::GetAsyncKeyState('W') & 0x8000)
		_camera.Walk(10.0f * dt);

	if (::GetAsyncKeyState('S') & 0x8000)
		_camera.Walk(-10.0f * dt);

	if (::GetAsyncKeyState('A') & 0x8000)
		_camera.Strafe(-10.0f * dt);

	if (::GetAsyncKeyState('D') & 0x8000)
		_camera.Strafe(10.0f * dt);

	//
	// Switch the rendering effect based on key presses.
	//
	if (::GetAsyncKeyState('2') & 0x8000)
		_renderOptions = RenderOptionsBasic;

	if (::GetAsyncKeyState('3') & 0x8000)
		_renderOptions = RenderOptionsNormalMap;

	if (::GetAsyncKeyState('4') & 0x8000)
		_renderOptions = RenderOptionsDisplacementMap;

	//
	// Animate the lights (and hence shadows).
	//

	_lightRotationAngle += 0.1f * dt;

	XMMATRIX R = ::XMMatrixRotationY(_lightRotationAngle);
	for (int i = 0; i < 3; ++i)
	{
		XMVECTOR lightDir = ::XMLoadFloat3(&_originalLightDir[i]);
		lightDir = ::XMVector3TransformNormal(lightDir, R);
		::XMStoreFloat3(&_dirLights[i].Direction, lightDir);
	}

	BuildShadowTransform();

	_camera.UpdateViewMatrix();
}

void ShadowsDemo::DrawScene()
{
	_smap->BindDsvAndSetNullRenderTarget(_deviceContext);

	DrawSceneToShadowMap();

	_deviceContext->RSSetState(0);

	//
	// Restore the back and depth buffer to the OM stage.
	//
	ID3D11RenderTargetView* renderTargets[1] = { _renderTargetView.Get() };
	_deviceContext->OMSetRenderTargets(1, renderTargets, _depthStencilView.Get() );
	_deviceContext->RSSetViewports(1, &_viewport);


	_deviceContext->ClearRenderTargetView(_renderTargetView.Get(), reinterpret_cast<const float*>(&Colors::Silver));
	_deviceContext->ClearDepthStencilView(_depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	XMMATRIX view = _camera.View();
	XMMATRIX proj = _camera.Proj();
	XMMATRIX viewProj = _camera.ViewProj();

	float blendFactor[] = { 0.0f, 0.0f, 0.0f, 0.0f };

	// Set per frame constants.
	Effects::BasicFX->SetDirLights(_dirLights);
	Effects::BasicFX->SetEyePosW(_camera.GetPosition());
	Effects::BasicFX->SetCubeMap(_sky->CubeMapSRV().Get());
	Effects::BasicFX->SetShadowMap(_smap->DepthMapSRV().Get());

	Effects::NormalMapFX->SetDirLights(_dirLights);
	Effects::NormalMapFX->SetEyePosW(_camera.GetPosition());
	Effects::NormalMapFX->SetCubeMap(_sky->CubeMapSRV().Get());
	Effects::NormalMapFX->SetShadowMap(_smap->DepthMapSRV().Get());

	Effects::DisplacementMapFX->SetDirLights(_dirLights);
	Effects::DisplacementMapFX->SetEyePosW(_camera.GetPosition());
	Effects::DisplacementMapFX->SetCubeMap(_sky->CubeMapSRV().Get());
	Effects::DisplacementMapFX->SetShadowMap(_smap->DepthMapSRV().Get());

	// These properties could be set per object if needed.
	Effects::DisplacementMapFX->SetHeightScale(0.07f);
	Effects::DisplacementMapFX->SetMaxTessDistance(1.0f);
	Effects::DisplacementMapFX->SetMinTessDistance(25.0f);
	Effects::DisplacementMapFX->SetMinTessFactor(1.0f);
	Effects::DisplacementMapFX->SetMaxTessFactor(5.0f);

	// Figure out which technique to use for different geometry.

	ComPtr<ID3DX11EffectTechnique> activeTech = Effects::DisplacementMapFX->Light3TexTech;
	ComPtr<ID3DX11EffectTechnique> activeSphereTech = Effects::BasicFX->Light3ReflectTech;
	ComPtr<ID3DX11EffectTechnique> activeSkullTech = Effects::BasicFX->Light3ReflectTech;

	switch (_renderOptions)
	{
	case RenderOptionsBasic:
		activeTech = Effects::BasicFX->Light3TexTech;
		_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		break;
	case RenderOptionsNormalMap:
		activeTech = Effects::NormalMapFX->Light3TexTech;
		_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		break;
	case RenderOptionsDisplacementMap:
		activeTech = Effects::DisplacementMapFX->Light3TexTech;
		_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST);
		break;
	}

	XMMATRIX world;
	XMMATRIX worldInvTranspose;
	XMMATRIX worldViewProj;

	XMMATRIX shadowTransform = XMLoadFloat4x4(&_shadowTransform);

	//
	// Draw the grid, cylinders, and box without any cubemap reflection.
	// 

	uint32 stride = sizeof(Vertex::PosNormalTexTan);
	uint32 offset = 0;

	_deviceContext->IASetInputLayout(InputLayouts::PosNormalTexTan.Get());
	_deviceContext->IASetVertexBuffers(0, 1, _shapesVB.GetAddressOf(), &stride, &offset);
	_deviceContext->IASetIndexBuffer(_shapesIB.Get(), DXGI_FORMAT_R32_UINT, 0);

	if (GetAsyncKeyState('1') & 0x8000)
		_deviceContext->RSSetState(RenderStates::WireframeRS.Get());

	D3DX11_TECHNIQUE_DESC techDesc;
	activeTech->GetDesc(&techDesc);

	for (uint32 p = 0; p < techDesc.Passes; ++p)
	{
		// Draw the grid.
		world = XMLoadFloat4x4(&_gridWorld);
		worldInvTranspose = MathHelper::InverseTranspose(world);
		worldViewProj = world * view * proj;

		switch (_renderOptions)
		{
		case RenderOptionsBasic:
			Effects::BasicFX->SetWorld(world);
			Effects::BasicFX->SetWorldInvTranspose(worldInvTranspose);
			Effects::BasicFX->SetWorldViewProj(worldViewProj);
			Effects::BasicFX->SetShadowTransform(world * shadowTransform);
			Effects::BasicFX->SetTexTransform(XMMatrixScaling(8.0f, 10.0f, 1.0f));
			Effects::BasicFX->SetMaterial(_gridMat);
			Effects::BasicFX->SetDiffuseMap(_stoneTexSRV.Get());
			break;
		case RenderOptionsNormalMap:
			Effects::NormalMapFX->SetWorld(world);
			Effects::NormalMapFX->SetWorldInvTranspose(worldInvTranspose);
			Effects::NormalMapFX->SetWorldViewProj(worldViewProj);
			Effects::NormalMapFX->SetShadowTransform(world * shadowTransform);
			Effects::NormalMapFX->SetTexTransform(XMMatrixScaling(8.0f, 10.0f, 1.0f));
			Effects::NormalMapFX->SetMaterial(_gridMat);
			Effects::NormalMapFX->SetDiffuseMap(_stoneTexSRV.Get());
			Effects::NormalMapFX->SetNormalMap(_stoneNormalTexSRV.Get());
			break;
		case RenderOptionsDisplacementMap:
			Effects::DisplacementMapFX->SetWorld(world);
			Effects::DisplacementMapFX->SetWorldInvTranspose(worldInvTranspose);
			Effects::DisplacementMapFX->SetViewProj(viewProj);
			Effects::DisplacementMapFX->SetWorldViewProj(worldViewProj);

			// Note: No world pre-multiply for displacement mapping since the DS computes the world
			// space position, we just need the light view/proj.
			Effects::DisplacementMapFX->SetShadowTransform(shadowTransform);
			Effects::DisplacementMapFX->SetTexTransform(XMMatrixScaling(8.0f, 10.0f, 1.0f));
			Effects::DisplacementMapFX->SetMaterial(_gridMat);
			Effects::DisplacementMapFX->SetDiffuseMap(_stoneTexSRV.Get());
			Effects::DisplacementMapFX->SetNormalMap(_stoneNormalTexSRV.Get());
			break;
		}

		activeTech->GetPassByIndex(p)->Apply(0, _deviceContext.Get());
		_deviceContext->DrawIndexed(_gridIndexCount, _gridIndexOffset, _gridVertexOffset);

		// Draw the box.
		world = XMLoadFloat4x4(&_boxWorld);
		worldInvTranspose = MathHelper::InverseTranspose(world);
		worldViewProj = world * view * proj;

		switch (_renderOptions)
		{
		case RenderOptionsBasic:
			Effects::BasicFX->SetWorld(world);
			Effects::BasicFX->SetWorldInvTranspose(worldInvTranspose);
			Effects::BasicFX->SetWorldViewProj(worldViewProj);
			Effects::BasicFX->SetShadowTransform(world * shadowTransform);
			Effects::BasicFX->SetTexTransform(XMMatrixScaling(2.0f, 1.0f, 1.0f));
			Effects::BasicFX->SetMaterial(_boxMat);
			Effects::BasicFX->SetDiffuseMap(_brickTexSRV.Get());
			break;
		case RenderOptionsNormalMap:
			Effects::NormalMapFX->SetWorld(world);
			Effects::NormalMapFX->SetWorldInvTranspose(worldInvTranspose);
			Effects::NormalMapFX->SetWorldViewProj(worldViewProj);
			Effects::NormalMapFX->SetShadowTransform(world * shadowTransform);
			Effects::NormalMapFX->SetTexTransform(XMMatrixScaling(2.0f, 1.0f, 1.0f));
			Effects::NormalMapFX->SetMaterial(_boxMat);
			Effects::NormalMapFX->SetDiffuseMap(_brickTexSRV.Get());
			Effects::NormalMapFX->SetNormalMap(_brickNormalTexSRV.Get());
			break;
		case RenderOptionsDisplacementMap:
			Effects::DisplacementMapFX->SetWorld(world);
			Effects::DisplacementMapFX->SetWorldInvTranspose(worldInvTranspose);
			Effects::DisplacementMapFX->SetViewProj(viewProj);
			Effects::DisplacementMapFX->SetWorldViewProj(worldViewProj);

			// Note: No world pre-multiply for displacement mapping since the DS computes the world
			// space position, we just need the light view/proj.
			Effects::DisplacementMapFX->SetShadowTransform(shadowTransform);
			Effects::DisplacementMapFX->SetTexTransform(XMMatrixScaling(2.0f, 1.0f, 1.0f));
			Effects::DisplacementMapFX->SetMaterial(_boxMat);
			Effects::DisplacementMapFX->SetDiffuseMap(_brickTexSRV.Get());
			Effects::DisplacementMapFX->SetNormalMap(_brickNormalTexSRV.Get());
			break;
		}

		activeTech->GetPassByIndex(p)->Apply(0, _deviceContext.Get());
		_deviceContext->DrawIndexed(_boxIndexCount, _boxIndexOffset, _boxVertexOffset);

		// Draw the cylinders.
		for (int i = 0; i < 10; ++i)
		{
			world = XMLoadFloat4x4(&_cylWorld[i]);
			worldInvTranspose = MathHelper::InverseTranspose(world);
			worldViewProj = world * view * proj;

			switch (_renderOptions)
			{
			case RenderOptionsBasic:
				Effects::BasicFX->SetWorld(world);
				Effects::BasicFX->SetWorldInvTranspose(worldInvTranspose);
				Effects::BasicFX->SetWorldViewProj(worldViewProj);
				Effects::BasicFX->SetShadowTransform(world * shadowTransform);
				Effects::BasicFX->SetTexTransform(XMMatrixScaling(1.0f, 2.0f, 1.0f));
				Effects::BasicFX->SetMaterial(_cylinderMat);
				Effects::BasicFX->SetDiffuseMap(_brickTexSRV.Get());
				break;
			case RenderOptionsNormalMap:
				Effects::NormalMapFX->SetWorld(world);
				Effects::NormalMapFX->SetWorldInvTranspose(worldInvTranspose);
				Effects::NormalMapFX->SetWorldViewProj(worldViewProj);
				Effects::NormalMapFX->SetShadowTransform(world * shadowTransform);
				Effects::NormalMapFX->SetTexTransform(XMMatrixScaling(1.0f, 2.0f, 1.0f));
				Effects::NormalMapFX->SetMaterial(_cylinderMat);
				Effects::NormalMapFX->SetDiffuseMap(_brickTexSRV.Get());
				Effects::NormalMapFX->SetNormalMap(_brickNormalTexSRV.Get());
				break;
			case RenderOptionsDisplacementMap:
				Effects::DisplacementMapFX->SetWorld(world);
				Effects::DisplacementMapFX->SetWorldInvTranspose(worldInvTranspose);
				Effects::DisplacementMapFX->SetViewProj(viewProj);
				Effects::DisplacementMapFX->SetWorldViewProj(worldViewProj);

				// Note: No world pre-multiply for displacement mapping since the DS computes the world
				// space position, we just need the light view/proj.
				Effects::DisplacementMapFX->SetShadowTransform(shadowTransform);
				Effects::DisplacementMapFX->SetTexTransform(XMMatrixScaling(1.0f, 2.0f, 1.0f));
				Effects::DisplacementMapFX->SetMaterial(_cylinderMat);
				Effects::DisplacementMapFX->SetDiffuseMap(_brickTexSRV.Get());
				Effects::DisplacementMapFX->SetNormalMap(_brickNormalTexSRV.Get());
				break;
			}


			activeTech->GetPassByIndex(p)->Apply(0, _deviceContext.Get());
			_deviceContext->DrawIndexed(_cylinderIndexCount, _cylinderIndexOffset, _cylinderVertexOffset);
		}
	}

	// FX sets tessellation stages, but it does not disable them.  So do that here
	// to turn off tessellation.
	_deviceContext->HSSetShader(0, 0, 0);
	_deviceContext->DSSetShader(0, 0, 0);

	_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//
	// Draw the spheres with cubemap reflection.
	//

	activeSphereTech->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		// Draw the spheres.
		for (int i = 0; i < 10; ++i)
		{
			world = XMLoadFloat4x4(&_sphereWorld[i]);
			worldInvTranspose = MathHelper::InverseTranspose(world);
			worldViewProj = world * view * proj;

			Effects::BasicFX->SetWorld(world);
			Effects::BasicFX->SetWorldInvTranspose(worldInvTranspose);
			Effects::BasicFX->SetWorldViewProj(worldViewProj);
			Effects::BasicFX->SetShadowTransform(world * shadowTransform);
			Effects::BasicFX->SetTexTransform(XMMatrixIdentity());
			Effects::BasicFX->SetMaterial(_sphereMat);

			activeSphereTech->GetPassByIndex(p)->Apply(0, _deviceContext.Get());
			_deviceContext->DrawIndexed(_sphereIndexCount, _sphereIndexOffset, _sphereVertexOffset);
		}
	}

	stride = sizeof(Vertex::Basic32);
	offset = 0;

	_deviceContext->RSSetState(0);

	_deviceContext->IASetInputLayout(InputLayouts::Basic32.Get());
	_deviceContext->IASetVertexBuffers(0, 1, _skullVB.GetAddressOf(), &stride, &offset);
	_deviceContext->IASetIndexBuffer(_skullIB.Get(), DXGI_FORMAT_R32_UINT, 0);

	activeSkullTech->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		// Draw the skull.
		world = XMLoadFloat4x4(&_skullWorld);
		worldInvTranspose = MathHelper::InverseTranspose(world);
		worldViewProj = world * view * proj;

		Effects::BasicFX->SetWorld(world);
		Effects::BasicFX->SetWorldInvTranspose(worldInvTranspose);
		Effects::BasicFX->SetWorldViewProj(worldViewProj);
		Effects::BasicFX->SetMaterial(_skullMat);
		Effects::BasicFX->SetShadowTransform(world * shadowTransform);

		activeSkullTech->GetPassByIndex(p)->Apply(0, _deviceContext.Get());
		_deviceContext->DrawIndexed(_skullIndexCount, 0, 0);
	}

	// Debug view depth buffer.
//	if( GetAsyncKeyState('Z') & 0x8000 )
	{
		DrawScreenQuad();
	}

	_sky->Draw(_deviceContext, _camera);

	// restore default states, as the SkyFX changes them in the effect file.
	_deviceContext->RSSetState(0);
	_deviceContext->OMSetDepthStencilState(0, 0);

	// Unbind shadow map as a shader input because we are going to render to it next frame.
	// The shadow might might be at any slot, so clear all slots.
	ID3D11ShaderResourceView* nullSRV[16] = { 0 };
	_deviceContext->PSSetShaderResources(0, 16, nullSRV);

	HR(_swapChain->Present(0, 0));
}

void ShadowsDemo::OnMouseDown(WPARAM btnState, int32 x, int32 y)
{
	_lastMousePos.x = x;
	_lastMousePos.y = y;

	SetCapture(_hMainWnd);
}

void ShadowsDemo::OnMouseUp(WPARAM btnState, int32 x, int32 y)
{
	ReleaseCapture();
}

void ShadowsDemo::OnMouseMove(WPARAM btnState, int32 x, int32 y)
{
	if ((btnState & MK_LBUTTON) != 0)
	{
		// Make each pixel correspond to a quarter of a degree.
		float dx = XMConvertToRadians(0.25f * static_cast<float>(x - _lastMousePos.x));
		float dy = XMConvertToRadians(0.25f * static_cast<float>(y - _lastMousePos.y));

		_camera.Pitch(dy);
		_camera.RotateY(dx);
	}

	_lastMousePos.x = x;
	_lastMousePos.y = y;
}

void ShadowsDemo::DrawSceneToShadowMap()
{
	XMMATRIX view = XMLoadFloat4x4(&_lightView);
	XMMATRIX proj = XMLoadFloat4x4(&_lightProj);
	XMMATRIX viewProj = XMMatrixMultiply(view, proj);

	Effects::BuildShadowMapFX->SetEyePosW(_camera.GetPosition());
	Effects::BuildShadowMapFX->SetViewProj(viewProj);

	// These properties could be set per object if needed.
	Effects::BuildShadowMapFX->SetHeightScale(0.07f);
	Effects::BuildShadowMapFX->SetMaxTessDistance(1.0f);
	Effects::BuildShadowMapFX->SetMinTessDistance(25.0f);
	Effects::BuildShadowMapFX->SetMinTessFactor(1.0f);
	Effects::BuildShadowMapFX->SetMaxTessFactor(5.0f);

	ComPtr<ID3DX11EffectTechnique> tessSmapTech = Effects::BuildShadowMapFX->BuildShadowMapTech;
	ComPtr<ID3DX11EffectTechnique> smapTech = Effects::BuildShadowMapFX->BuildShadowMapTech;

	switch (_renderOptions)
	{
	case RenderOptionsBasic:
		_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		smapTech = Effects::BuildShadowMapFX->BuildShadowMapTech;
		tessSmapTech = Effects::BuildShadowMapFX->BuildShadowMapTech;
		break;
	case RenderOptionsNormalMap:
		_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		smapTech = Effects::BuildShadowMapFX->BuildShadowMapTech;
		tessSmapTech = Effects::BuildShadowMapFX->BuildShadowMapTech;
		break;
	case RenderOptionsDisplacementMap:
		_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST);
		smapTech = Effects::BuildShadowMapFX->BuildShadowMapTech;
		tessSmapTech = Effects::BuildShadowMapFX->TessBuildShadowMapTech;
		break;
	}

	XMMATRIX world;
	XMMATRIX worldInvTranspose;
	XMMATRIX worldViewProj;

	//
	// Draw the grid, cylinders, and box without any cubemap reflection.
	// 

	uint32 stride = sizeof(Vertex::PosNormalTexTan);
	uint32 offset = 0;

	_deviceContext->IASetInputLayout(InputLayouts::PosNormalTexTan.Get());
	_deviceContext->IASetVertexBuffers(0, 1, _shapesVB.GetAddressOf(), &stride, &offset);
	_deviceContext->IASetIndexBuffer(_shapesIB.Get(), DXGI_FORMAT_R32_UINT, 0);

	if (GetAsyncKeyState('1') & 0x8000)
		_deviceContext->RSSetState(RenderStates::WireframeRS.Get());

	D3DX11_TECHNIQUE_DESC techDesc;
	tessSmapTech->GetDesc(&techDesc);
	for (uint32 p = 0; p < techDesc.Passes; ++p)
	{
		// Draw the grid.
		world = ::XMLoadFloat4x4(&_gridWorld);
		worldInvTranspose = MathHelper::InverseTranspose(world);
		worldViewProj = world * view * proj;

		Effects::BuildShadowMapFX->SetWorld(world);
		Effects::BuildShadowMapFX->SetWorldInvTranspose(worldInvTranspose);
		Effects::BuildShadowMapFX->SetWorldViewProj(worldViewProj);
		Effects::BuildShadowMapFX->SetTexTransform(XMMatrixScaling(8.0f, 10.0f, 1.0f));

		tessSmapTech->GetPassByIndex(p)->Apply(0, _deviceContext.Get());
		_deviceContext->DrawIndexed(_gridIndexCount, _gridIndexOffset, _gridVertexOffset);

		// Draw the box.
		world = ::XMLoadFloat4x4(&_boxWorld);
		worldInvTranspose = MathHelper::InverseTranspose(world);
		worldViewProj = world * view * proj;

		Effects::BuildShadowMapFX->SetWorld(world);
		Effects::BuildShadowMapFX->SetWorldInvTranspose(worldInvTranspose);
		Effects::BuildShadowMapFX->SetWorldViewProj(worldViewProj);
		Effects::BuildShadowMapFX->SetTexTransform(XMMatrixScaling(2.0f, 1.0f, 1.0f));

		tessSmapTech->GetPassByIndex(p)->Apply(0, _deviceContext.Get());
		_deviceContext->DrawIndexed(_boxIndexCount, _boxIndexOffset, _boxVertexOffset);

		// Draw the cylinders.
		for (int32 i = 0; i < 10; ++i)
		{
			world = ::XMLoadFloat4x4(&_cylWorld[i]);
			worldInvTranspose = MathHelper::InverseTranspose(world);
			worldViewProj = world * view * proj;

			Effects::BuildShadowMapFX->SetWorld(world);
			Effects::BuildShadowMapFX->SetWorldInvTranspose(worldInvTranspose);
			Effects::BuildShadowMapFX->SetWorldViewProj(worldViewProj);
			Effects::BuildShadowMapFX->SetTexTransform(XMMatrixScaling(1.0f, 2.0f, 1.0f));

			tessSmapTech->GetPassByIndex(p)->Apply(0, _deviceContext.Get());
			_deviceContext->DrawIndexed(_cylinderIndexCount, _cylinderIndexOffset, _cylinderVertexOffset);
		}
	}

	// FX sets tessellation stages, but it does not disable them.  So do that here
	// to turn off tessellation.
	_deviceContext->HSSetShader(0, 0, 0);
	_deviceContext->DSSetShader(0, 0, 0);

	_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//
	// Draw the spheres with cubemap reflection.
	//

	smapTech->GetDesc(&techDesc);
	for (uint32 p = 0; p < techDesc.Passes; ++p)
	{
		// Draw the spheres.
		for (int32 i = 0; i < 10; ++i)
		{
			world = XMLoadFloat4x4(&_sphereWorld[i]);
			worldInvTranspose = MathHelper::InverseTranspose(world);
			worldViewProj = world * view * proj;

			Effects::BuildShadowMapFX->SetWorld(world);
			Effects::BuildShadowMapFX->SetWorldInvTranspose(worldInvTranspose);
			Effects::BuildShadowMapFX->SetWorldViewProj(worldViewProj);
			Effects::BuildShadowMapFX->SetTexTransform(XMMatrixIdentity());

			smapTech->GetPassByIndex(p)->Apply(0, _deviceContext.Get());
			_deviceContext->DrawIndexed(_sphereIndexCount, _sphereIndexOffset, _sphereVertexOffset);
		}
	}

	stride = sizeof(Vertex::Basic32);
	offset = 0;

	_deviceContext->RSSetState(0);

	_deviceContext->IASetInputLayout(InputLayouts::Basic32.Get());
	_deviceContext->IASetVertexBuffers(0, 1, _skullVB.GetAddressOf(), &stride, &offset);
	_deviceContext->IASetIndexBuffer(_skullIB.Get(), DXGI_FORMAT_R32_UINT, 0);

	smapTech = Effects::BuildShadowMapFX->BuildShadowMapTech;
	smapTech->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		// Draw the skull.
		world = XMLoadFloat4x4(&_skullWorld);
		worldInvTranspose = MathHelper::InverseTranspose(world);
		worldViewProj = world * view * proj;

		Effects::BuildShadowMapFX->SetWorld(world);
		Effects::BuildShadowMapFX->SetWorldInvTranspose(worldInvTranspose);
		Effects::BuildShadowMapFX->SetWorldViewProj(worldViewProj);
		Effects::BuildShadowMapFX->SetTexTransform(XMMatrixIdentity());

		smapTech->GetPassByIndex(p)->Apply(0, _deviceContext.Get());
		_deviceContext->DrawIndexed(_skullIndexCount, 0, 0);
	}
}

void ShadowsDemo::DrawScreenQuad()
{
	uint32 stride = sizeof(Vertex::Basic32);
	uint32 offset = 0;

	_deviceContext->IASetInputLayout(InputLayouts::Basic32.Get());
	_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	_deviceContext->IASetVertexBuffers(0, 1, _screenQuadVB.GetAddressOf(), &stride, &offset);
	_deviceContext->IASetIndexBuffer(_screenQuadIB.Get(), DXGI_FORMAT_R32_UINT, 0);

	// Scale and shift quad to lower-right corner.
	XMMATRIX world(
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.5f, -0.5f, 0.0f, 1.0f);

	ComPtr<ID3DX11EffectTechnique> tech = Effects::DebugTexFX->ViewRedTech;
	D3DX11_TECHNIQUE_DESC techDesc;

	tech->GetDesc(&techDesc);

	for (uint32 p = 0; p < techDesc.Passes; ++p)
	{
		Effects::DebugTexFX->SetWorldViewProj(world);
		Effects::DebugTexFX->SetTexture(_smap->DepthMapSRV().Get());

		tech->GetPassByIndex(p)->Apply(0, _deviceContext.Get());
		_deviceContext->DrawIndexed(6, 0, 0);
	}
}

void ShadowsDemo::BuildShadowTransform()
{
	// Only the first "main" light casts a shadow.
	XMVECTOR lightDir = ::XMLoadFloat3(&_dirLights[0].Direction);
	XMVECTOR lightPos = -2.0f * _sceneBounds.Radius * lightDir;
	XMVECTOR targetPos = ::XMLoadFloat3(&_sceneBounds.Center);
	XMVECTOR up = ::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	XMMATRIX V = ::XMMatrixLookAtLH(lightPos, targetPos, up);

	// Transform bounding sphere to light space.
	XMFLOAT3 sphereCenterLS;
	::XMStoreFloat3(&sphereCenterLS, ::XMVector3TransformCoord(targetPos, V));

	// Ortho frustum in light space encloses scene.
	float l = sphereCenterLS.x - _sceneBounds.Radius;
	float b = sphereCenterLS.y - _sceneBounds.Radius;
	float n = sphereCenterLS.z - _sceneBounds.Radius;
	float r = sphereCenterLS.x + _sceneBounds.Radius;
	float t = sphereCenterLS.y + _sceneBounds.Radius;
	float f = sphereCenterLS.z + _sceneBounds.Radius;
	XMMATRIX P = ::XMMatrixOrthographicOffCenterLH(l, r, b, t, n, f);

	// Transform NDC space [-1,+1]^2 to texture space [0,1]^2
	XMMATRIX T(
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.0f, 1.0f);

	XMMATRIX S = V * P * T;

	::XMStoreFloat4x4(&_lightView, V);
	::XMStoreFloat4x4(&_lightProj, P);
	::XMStoreFloat4x4(&_shadowTransform, S);
}

void ShadowsDemo::BuildShapeGeometryBuffers()
{
	GeometryGenerator::MeshData box;
	GeometryGenerator::MeshData grid;
	GeometryGenerator::MeshData sphere;
	GeometryGenerator::MeshData cylinder;

	GeometryGenerator geoGen;
	geoGen.CreateBox(1.0f, 1.0f, 1.0f, box);
	geoGen.CreateGrid(20.0f, 30.0f, 50, 40, grid);
	geoGen.CreateSphere(0.5f, 20, 20, sphere);
	geoGen.CreateCylinder(0.5f, 0.5f, 3.0f, 15, 15, cylinder);

	// Cache the vertex offsets to each object in the concatenated vertex buffer.
	_boxVertexOffset = 0;
	_gridVertexOffset = box.vertices.size();
	_sphereVertexOffset = _gridVertexOffset + grid.vertices.size();
	_cylinderVertexOffset = _sphereVertexOffset + sphere.vertices.size();

	// Cache the index count of each object.
	_boxIndexCount = box.indices.size();
	_gridIndexCount = grid.indices.size();
	_sphereIndexCount = sphere.indices.size();
	_cylinderIndexCount = cylinder.indices.size();

	// Cache the starting index for each object in the concatenated index buffer.
	_boxIndexOffset = 0;
	_gridIndexOffset = _boxIndexCount;
	_sphereIndexOffset = _gridIndexOffset + _gridIndexCount;
	_cylinderIndexOffset = _sphereIndexOffset + _sphereIndexCount;

	uint32 totalVertexCount =
		box.vertices.size() +
		grid.vertices.size() +
		sphere.vertices.size() +
		cylinder.vertices.size();

	uint32 totalIndexCount =
		_boxIndexCount +
		_gridIndexCount +
		_sphereIndexCount +
		_cylinderIndexCount;

	//
	// Extract the vertex elements we are interested in and pack the
	// vertices of all the meshes into one vertex buffer.
	//

	std::vector<Vertex::PosNormalTexTan> vertices(totalVertexCount);

	uint32 k = 0;
	for (size_t i = 0; i < box.vertices.size(); ++i, ++k)
	{
		vertices[k].pos = box.vertices[i].position;
		vertices[k].normal = box.vertices[i].normal;
		vertices[k].tex = box.vertices[i].texC;
		vertices[k].tangentU = box.vertices[i].tangentU;
	}

	for (size_t i = 0; i < grid.vertices.size(); ++i, ++k)
	{
		vertices[k].pos = grid.vertices[i].position;
		vertices[k].normal = grid.vertices[i].normal;
		vertices[k].tex = grid.vertices[i].texC;
		vertices[k].tangentU = grid.vertices[i].tangentU;
	}

	for (size_t i = 0; i < sphere.vertices.size(); ++i, ++k)
	{
		vertices[k].pos = sphere.vertices[i].position;
		vertices[k].normal = sphere.vertices[i].normal;
		vertices[k].tex = sphere.vertices[i].texC;
		vertices[k].tangentU = sphere.vertices[i].tangentU;
	}

	for (size_t i = 0; i < cylinder.vertices.size(); ++i, ++k)
	{
		vertices[k].pos = cylinder.vertices[i].position;
		vertices[k].normal = cylinder.vertices[i].normal;
		vertices[k].tex = cylinder.vertices[i].texC;
		vertices[k].tangentU = cylinder.vertices[i].tangentU;
	}

	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Vertex::PosNormalTexTan) * totalVertexCount;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = &vertices[0];
	HR(_device->CreateBuffer(&vbd, &vinitData, _shapesVB.GetAddressOf()));

	//
	// Pack the indices of all the meshes into one index buffer.
	//

	std::vector<uint32> indices;
	indices.insert(indices.end(), box.indices.begin(), box.indices.end());
	indices.insert(indices.end(), grid.indices.begin(), grid.indices.end());
	indices.insert(indices.end(), sphere.indices.begin(), sphere.indices.end());
	indices.insert(indices.end(), cylinder.indices.begin(), cylinder.indices.end());

	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(uint32) * totalIndexCount;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = &indices[0];
	HR(_device->CreateBuffer(&ibd, &iinitData, _shapesIB.GetAddressOf()));
}

void ShadowsDemo::BuildSkullGeometryBuffers()
{
	std::ifstream fin("../Resources/Models/skull.txt");

	if (!fin)
	{
		MessageBox(0, L"../Resources/Models/skull.txt not found.", 0, 0);
		return;
	}

	uint32 vcount = 0;
	uint32 tcount = 0;
	std::string ignore;

	fin >> ignore >> vcount;
	fin >> ignore >> tcount;
	fin >> ignore >> ignore >> ignore >> ignore;

	std::vector<Vertex::Basic32> vertices(vcount);
	for (uint32 i = 0; i < vcount; ++i)
	{
		fin >> vertices[i].pos.x >> vertices[i].pos.y >> vertices[i].pos.z;
		fin >> vertices[i].normal.x >> vertices[i].normal.y >> vertices[i].normal.z;
	}

	fin >> ignore;
	fin >> ignore;
	fin >> ignore;

	_skullIndexCount = 3 * tcount;
	std::vector<uint32> indices(_skullIndexCount);
	for (uint32 i = 0; i < tcount; ++i)
	{
		fin >> indices[i * 3 + 0] >> indices[i * 3 + 1] >> indices[i * 3 + 2];
	}

	fin.close();

	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Vertex::Basic32) * vcount;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = &vertices[0];
	HR(_device->CreateBuffer(&vbd, &vinitData, _skullVB.GetAddressOf()));

	//
	// Pack the indices of all the meshes into one index buffer.
	//

	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(uint32) * _skullIndexCount;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = &indices[0];
	HR(_device->CreateBuffer(&ibd, &iinitData, _skullIB.GetAddressOf()));
}

void ShadowsDemo::BuildScreenQuadGeometryBuffers()
{
	GeometryGenerator::MeshData quad;

	GeometryGenerator geoGen;
	geoGen.CreateFullscreenQuad(quad);

	//
	// Extract the vertex elements we are interested in and pack the
	// vertices of all the meshes into one vertex buffer.
	//

	std::vector<Vertex::Basic32> vertices(quad.vertices.size());

	for (uint32 i = 0; i < quad.vertices.size(); ++i)
	{
		vertices[i].pos = quad.vertices[i].position;
		vertices[i].normal = quad.vertices[i].normal;
		vertices[i].tex = quad.vertices[i].texC;
	}

	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Vertex::Basic32) * quad.vertices.size();
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = &vertices[0];
	HR(_device->CreateBuffer(&vbd, &vinitData, _screenQuadVB.GetAddressOf()));

	//
	// Pack the indices of all the meshes into one index buffer.
	//

	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(uint32) * quad.indices.size();
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = &quad.indices[0];
	HR(_device->CreateBuffer(&ibd, &iinitData, _screenQuadIB.GetAddressOf()));
}
