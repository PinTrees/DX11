#include "pch.h"
#include "29. MeshViewDemo.h"
#include "MathHelper.h"
#include "GeometryGenerator.h"
#include "Effects.h"
#include "Vertex.h"
#include "RenderStates.h"
#include "Sky.h"
#include "ShadowMap.h"
#include "Ssao.h"


MeshViewDemo::MeshViewDemo(HINSTANCE hInstance)
	: App(hInstance)
{
	_mainWindowCaption = L"MeshView Demo";

	_lastMousePos.x = 0;
	_lastMousePos.y = 0;

	_camera.SetPosition(0.0f, 2.0f, -15.0f);

	_dirLights[0].Ambient = XMFLOAT4(0.6f, 0.6f, 0.6f, 1.0f);
	_dirLights[0].Diffuse = XMFLOAT4(0.8f, 0.7f, 0.7f, 1.0f);
	_dirLights[0].Specular = XMFLOAT4(0.6f, 0.6f, 0.7f, 1.0f);
	_dirLights[0].Direction = XMFLOAT3(-0.57735f, -0.57735f, 0.57735f);

	_dirLights[1].Ambient = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	_dirLights[1].Diffuse = XMFLOAT4(0.4f, 0.4f, 0.4f, 1.0f);
	_dirLights[1].Specular = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	_dirLights[1].Direction = XMFLOAT3(0.707f, -0.707f, 0.0f);

	_dirLights[2].Ambient = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	_dirLights[2].Diffuse = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	_dirLights[2].Specular = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	_dirLights[2].Direction = XMFLOAT3(0.0f, 0.0, -1.0f);

	_originalLightDir[0] = _dirLights[0].Direction;
	_originalLightDir[1] = _dirLights[1].Direction;
	_originalLightDir[2] = _dirLights[2].Direction;
}

MeshViewDemo::~MeshViewDemo()
{
	Effects::DestroyAll();
	InputLayouts::DestroyAll();
	RenderStates::DestroyAll();
}

bool MeshViewDemo::Init()
{
	if (!App::Init())
		return false;

	// Must init Effects first since InputLayouts depend on shader signatures.
	Effects::InitAll(_device, L"../Shaders/28. Basic.fx");
	Shaders::InitAll(_device);
	InputLayouts::InitAll(_device);
	RenderStates::InitAll(_device);

	_texMgr.Init(_device);

	_sky = make_shared<Sky>(_device, L"../Resources/Textures/desertcube1024.dds", 5000.0f);
	_smap = make_shared<ShadowMap>(_device, SMapSize, SMapSize);

	_camera.SetLens(0.25f * MathHelper::Pi, AspectRatio(), 1.0f, 1000.0f);
	_ssao = make_shared<class Ssao>(_device, _deviceContext, _clientWidth, _clientHeight, _camera.GetFovY(), _camera.GetFarZ());

	BuildScreenQuadGeometryBuffers();

	_treeModel = make_shared<Mesh>(_device, _texMgr, "../Resources/Models/tree.m3d", L"../Resources/Textures/");
	_baseModel = make_shared<Mesh>(_device, _texMgr, "../Resources/Models/base.m3d", L"../Resources/Textures/");
	_stairsModel = make_shared<Mesh>(_device, _texMgr, "../Resources/Models/stairs.m3d", L"../Resources/Textures/");
	_pillar1Model = make_shared<Mesh>(_device, _texMgr, "../Resources/Models/pillar1.m3d", L"../Resources/Textures/");
	_pillar2Model = make_shared<Mesh>(_device, _texMgr, "../Resources/Models/pillar2.m3d", L"../Resources/Textures/");
	_pillar3Model = make_shared<Mesh>(_device, _texMgr, "../Resources/Models/pillar5.m3d", L"../Resources/Textures/");
	_pillar4Model = make_shared<Mesh>(_device, _texMgr, "../Resources/Models/pillar6.m3d", L"../Resources/Textures/");
	_rockModel = make_shared<Mesh>(_device, _texMgr, "../Resources/Models/rock.m3d", L"../Resources/Textures/");

	_Fbx = make_shared<Mesh>(_device, _texMgr, "../Resources/Meshs/Banana.fbx", L"../Resources/Textures/");

	GameObject* go = new GameObject("TestObject1");
	GameObject* g1 = new GameObject("TestObject2");
	GameObject* g2 = new GameObject("TestObject3");

	go->AddComponent<MeshRenderer>()->SetMesh(_rockModel);
	go->GetComponent<MeshRenderer>()->SetShader(Shaders::GetShaderByName(L"../Shaders/28. Basic.fx"));
	go->AddComponent<BoxCollider>();

	SceneManager::GetI()->GetCurrentScene()->AddRootGameObject(go);
	SceneManager::GetI()->GetCurrentScene()->AddRootGameObject(g1);
	SceneManager::GetI()->GetCurrentScene()->AddRootGameObject(g2);

	MeshInstance treeInstance;
	MeshInstance baseInstance;
	MeshInstance stairsInstance;
	MeshInstance pillar1Instance;
	MeshInstance pillar2Instance;
	MeshInstance pillar3Instance;
	MeshInstance pillar4Instance;
	MeshInstance rockInstance1;
	MeshInstance rockInstance2;
	MeshInstance rockInstance3;
	MeshInstance fbxInstance;

	treeInstance.Model = _treeModel;
	baseInstance.Model = _baseModel;
	stairsInstance.Model = _stairsModel;
	pillar1Instance.Model = _pillar1Model;
	pillar2Instance.Model = _pillar2Model;
	pillar3Instance.Model = _pillar3Model;
	pillar4Instance.Model = _pillar4Model;
	rockInstance1.Model = _rockModel;
	rockInstance2.Model = _rockModel;
	rockInstance3.Model = _rockModel;
	fbxInstance.Model = _Fbx;

	XMMATRIX modelScale = ::XMMatrixScaling(1.0f, 1.0f, 1.0f);
	XMMATRIX modelRot = ::XMMatrixRotationY(0.0f);
	XMMATRIX modelOffset = ::XMMatrixTranslation(0.0f, 0.0f, 0.0f);
	::XMStoreFloat4x4(&treeInstance.World, modelScale * modelRot * modelOffset);
	::XMStoreFloat4x4(&baseInstance.World, modelScale * modelRot * modelOffset);

	modelRot = ::XMMatrixRotationY(0.5f * XM_PI);
	modelOffset = ::XMMatrixTranslation(0.0f, -2.5f, -12.0f);
	::XMStoreFloat4x4(&stairsInstance.World, modelScale * modelRot * modelOffset);

	modelScale = ::XMMatrixScaling(0.8f, 0.8f, 0.8f);
	modelOffset = ::XMMatrixTranslation(-5.0f, 1.5f, 5.0f);
	::XMStoreFloat4x4(&pillar1Instance.World, modelScale * modelRot * modelOffset);

	modelScale = ::XMMatrixScaling(0.8f, 0.8f, 0.8f);
	modelOffset = ::XMMatrixTranslation(5.0f, 1.5f, 5.0f);
	::XMStoreFloat4x4(&pillar2Instance.World, modelScale * modelRot * modelOffset);

	modelScale = ::XMMatrixScaling(0.8f, 0.8f, 0.8f);
	modelOffset = ::XMMatrixTranslation(5.0f, 1.5f, -5.0f);
	::XMStoreFloat4x4(&pillar3Instance.World, modelScale * modelRot * modelOffset);

	modelScale = ::XMMatrixScaling(1.0f, 1.0f, 1.0f);
	modelOffset = ::XMMatrixTranslation(-5.0f, 1.0f, -5.0f);
	::XMStoreFloat4x4(&pillar4Instance.World, modelScale * modelRot * modelOffset);

	modelScale = ::XMMatrixScaling(0.8f, 0.8f, 0.8f);
	modelOffset = ::XMMatrixTranslation(-1.0f, 1.4f, -7.0f);
	::XMStoreFloat4x4(&rockInstance1.World, modelScale * modelRot * modelOffset);

	modelScale = ::XMMatrixScaling(0.8f, 0.8f, 0.8f);
	modelOffset = ::XMMatrixTranslation(5.0f, 1.2f, -2.0f);
	::XMStoreFloat4x4(&rockInstance2.World, modelScale * modelRot * modelOffset);

	modelScale = ::XMMatrixScaling(0.8f, 0.8f, 0.8f);
	modelOffset = ::XMMatrixTranslation(-4.0f, 1.3f, 3.0f);
	::XMStoreFloat4x4(&rockInstance3.World, modelScale * modelRot * modelOffset);

	modelScale = ::XMMatrixScaling(5.0f, 5.0f, 5.0f);
	modelOffset = ::XMMatrixTranslation(-4.0f, 3.0f, 3.0f);
	::XMStoreFloat4x4(&fbxInstance.World, modelScale * modelRot * modelOffset);

	_alphaClippedModelInstances.push_back(treeInstance);

	_modelInstances.push_back(baseInstance);
	_modelInstances.push_back(stairsInstance);
	_modelInstances.push_back(pillar1Instance);
	_modelInstances.push_back(pillar2Instance);
	_modelInstances.push_back(pillar3Instance);
	_modelInstances.push_back(pillar4Instance);
	_modelInstances.push_back(rockInstance1);
	_modelInstances.push_back(rockInstance2);
	_modelInstances.push_back(rockInstance3);
	_modelInstances.push_back(fbxInstance);

	//
	// Compute scene bounding box.
	//

	XMFLOAT3 minPt(+MathHelper::Infinity, +MathHelper::Infinity, +MathHelper::Infinity);
	XMFLOAT3 maxPt(-MathHelper::Infinity, -MathHelper::Infinity, -MathHelper::Infinity);
	for (uint32 i = 0; i < _modelInstances.size(); ++i)
	{
		for (uint32 j = 0; j < _modelInstances[i].Model->Vertices.size(); ++j)
		{
			XMFLOAT3 P = _modelInstances[i].Model->Vertices[j].pos;

			minPt.x = MathHelper::Min(minPt.x, P.x);
			minPt.y = MathHelper::Min(minPt.x, P.x);
			minPt.z = MathHelper::Min(minPt.x, P.x);

			maxPt.x = MathHelper::Max(maxPt.x, P.x);
			maxPt.y = MathHelper::Max(maxPt.x, P.x);
			maxPt.z = MathHelper::Max(maxPt.x, P.x);
		}
	}

	//
	// Derive scene bounding sphere from bounding box.
	//
	_sceneBounds.Center = XMFLOAT3(
		0.5f * (minPt.x + maxPt.x),
		0.5f * (minPt.y + maxPt.y),
		0.5f * (minPt.z + maxPt.z));

	XMFLOAT3 extent(
		0.5f * (maxPt.x - minPt.x),
		0.5f * (maxPt.y - minPt.y),
		0.5f * (maxPt.z - minPt.z));

	_sceneBounds.Radius = sqrtf(extent.x * extent.x + extent.y * extent.y + extent.z * extent.z);

	return true;
}

void MeshViewDemo::OnResize()
{
	App::OnResize();

	_camera.SetLens(0.25f * MathHelper::Pi, AspectRatio(), 1.0f, 1000.0f);

	if (_ssao)
	{
		_ssao->OnSize(_clientWidth, _clientHeight, _camera.GetFovY(), _camera.GetFarZ());
	}
}

void MeshViewDemo::UpdateScene(float dt)
{
	//
	// Control the camera.
	//
	if (GetAsyncKeyState('W') & 0x8000)
		_camera.Walk(10.0f * dt);

	if (GetAsyncKeyState('S') & 0x8000)
		_camera.Walk(-10.0f * dt);

	if (GetAsyncKeyState('A') & 0x8000)
		_camera.Strafe(-10.0f * dt);

	if (GetAsyncKeyState('D') & 0x8000)
		_camera.Strafe(10.0f * dt);

	//
	// Animate the lights (and hence shadows).
	//

	BuildShadowTransform();

	_camera.UpdateViewMatrix();
}

void MeshViewDemo::RenderApplication()
{
	//
	// Render the scene to the shadow map.
	//
	_smap->BindDsvAndSetNullRenderTarget(_deviceContext);

	DrawSceneToShadowMap();

	_deviceContext->RSSetState(0);

	//
	// Render the view space normals and depths.  This render target has the
	// same dimensions as the back buffer, so we can use the screen viewport.
	// This render pass is needed to compute the ambient occlusion.
	// Notice that we use the main depth/stencil buffer in this pass.  
	//

	_deviceContext->ClearDepthStencilView(_depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	_deviceContext->RSSetViewports(1, &_viewport);
	_ssao->SetNormalDepthRenderTarget(_depthStencilView.Get());

	DrawSceneToSsaoNormalDepthMap();

	//
	// Now compute the ambient occlusion.
	//
	_ssao->ComputeSsao(_camera);
	_ssao->BlurAmbientMap(2);

	//
	// Restore the back and depth buffer and viewport to the OM stage.
	//
	ID3D11RenderTargetView* renderTargets[1] = { _renderTargetView.Get() };
	_deviceContext->OMSetRenderTargets(1, renderTargets, _depthStencilView.Get() );
	_deviceContext->RSSetViewports(1, &_viewport);

	_deviceContext->ClearRenderTargetView(_renderTargetView.Get(), reinterpret_cast<const float*>(&Colors::Silver));

	// We already laid down scene depth to the depth buffer in the Normal/Depth map pass,
	// so we can set the depth comparison test to “EQUALS.? This prevents any overdraw
	// in this rendering pass, as only the nearest visible pixels will pass this depth
	// comparison test.

	_deviceContext->OMSetDepthStencilState(RenderStates::EqualsDSS.Get(), 0);

	XMMATRIX view = _camera.View();
	XMMATRIX proj = _camera.Proj();
	XMMATRIX viewProj = _camera.ViewProj();

	float blendFactor[] = { 0.0f, 0.0f, 0.0f, 0.0f };

	// Set per frame constants.
	Effects::NormalMapFX->SetDirLights(_dirLights);
	Effects::NormalMapFX->SetEyePosW(_camera.GetPosition());
	Effects::NormalMapFX->SetCubeMap(_sky->CubeMapSRV().Get());
	Effects::NormalMapFX->SetShadowMap(_smap->DepthMapSRV().Get());
	Effects::NormalMapFX->SetSsaoMap(_ssao->AmbientSRV().Get());

	ComPtr<ID3DX11EffectTechnique> tech = Effects::NormalMapFX->Light3TexTech;
	ComPtr<ID3DX11EffectTechnique> alphaClippedTech = Effects::NormalMapFX->Light3TexAlphaClipTech;

	_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	XMMATRIX world;
	XMMATRIX worldInvTranspose;
	XMMATRIX worldViewProj;

	// Transform NDC space [-1,+1]^2 to texture space [0,1]^2
	XMMATRIX toTexSpace(
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.0f, 1.0f);

	XMMATRIX shadowTransform = XMLoadFloat4x4(&_shadowTransform);

	if (GetAsyncKeyState('1') & 0x8000)
		_deviceContext->RSSetState(RenderStates::WireframeRS.Get());

	RenderManager::GetI()->shadowTransform = XMLoadFloat4x4(&_shadowTransform);
	RenderManager::GetI()->cameraViewProjectionMatrix = view * proj;

	uint32 stride = sizeof(Vertex::PosNormalTexTan);
	uint32 offset = 0;

	_deviceContext->IASetInputLayout(InputLayouts::PosNormalTexTan.Get());

	SceneManager::GetI()->GetCurrentScene()->RenderScene();

	//
	// Draw opaque objects.
	//
	D3DX11_TECHNIQUE_DESC techDesc;
	tech->GetDesc(&techDesc);


	for (uint32 p = 0; p < techDesc.Passes; ++p)
	{
		for (uint32 modelIndex = 0; modelIndex < _modelInstances.size(); ++modelIndex)
		{
			world = XMLoadFloat4x4(&_modelInstances[modelIndex].World);
			worldInvTranspose = MathHelper::InverseTranspose(world);
			worldViewProj = world * view * proj;

			Effects::NormalMapFX->SetWorld(world);
			Effects::NormalMapFX->SetWorldInvTranspose(worldInvTranspose);
			Effects::NormalMapFX->SetWorldViewProj(worldViewProj);
			Effects::NormalMapFX->SetWorldViewProjTex(worldViewProj * toTexSpace);
			Effects::NormalMapFX->SetShadowTransform(world * shadowTransform);
			Effects::NormalMapFX->SetTexTransform(XMMatrixScaling(1.0f, 1.0f, 1.0f));

			for (uint32 subset = 0; subset < _modelInstances[modelIndex].Model->SubsetCount; ++subset)
			{
				Effects::NormalMapFX->SetMaterial(_modelInstances[modelIndex].Model->Mat[subset]);
				Effects::NormalMapFX->SetDiffuseMap(_modelInstances[modelIndex].Model->DiffuseMapSRV[subset].Get());
				Effects::NormalMapFX->SetNormalMap(_modelInstances[modelIndex].Model->NormalMapSRV[subset].Get());

				tech->GetPassByIndex(p)->Apply(0, _deviceContext.Get());
				_modelInstances[modelIndex].Model->ModelMesh.Draw(_deviceContext, subset);
			}
		}
	}

	// The alpha tested triangles are leaves, so render them double sided.
	_deviceContext->RSSetState(RenderStates::NoCullRS.Get());
	alphaClippedTech->GetDesc(&techDesc);
	for (uint32 p = 0; p < techDesc.Passes; ++p)
	{
		for (uint32 modelIndex = 0; modelIndex < _alphaClippedModelInstances.size(); ++modelIndex)
		{
			world = XMLoadFloat4x4(&_alphaClippedModelInstances[modelIndex].World);
			worldInvTranspose = MathHelper::InverseTranspose(world);
			worldViewProj = world * view * proj;

			Effects::NormalMapFX->SetWorld(world);
			Effects::NormalMapFX->SetWorldInvTranspose(worldInvTranspose);
			Effects::NormalMapFX->SetWorldViewProj(worldViewProj);
			Effects::NormalMapFX->SetWorldViewProjTex(worldViewProj * toTexSpace);
			Effects::NormalMapFX->SetShadowTransform(world * shadowTransform);
			Effects::NormalMapFX->SetTexTransform(XMMatrixScaling(1.0f, 1.0f, 1.0f));

			for (uint32 subset = 0; subset < _alphaClippedModelInstances[modelIndex].Model->SubsetCount; ++subset)
			{
				Effects::NormalMapFX->SetMaterial(_alphaClippedModelInstances[modelIndex].Model->Mat[subset]);
				Effects::NormalMapFX->SetDiffuseMap(_alphaClippedModelInstances[modelIndex].Model->DiffuseMapSRV[subset].Get());
				Effects::NormalMapFX->SetNormalMap(_alphaClippedModelInstances[modelIndex].Model->NormalMapSRV[subset].Get());

				alphaClippedTech->GetPassByIndex(p)->Apply(0, _deviceContext.Get());
				_alphaClippedModelInstances[modelIndex].Model->ModelMesh.Draw(_deviceContext, subset);
			}
		}
	}

	// Turn off wireframe.
	_deviceContext->RSSetState(0);

	// Restore from RenderStates::EqualsDSS
	_deviceContext->OMSetDepthStencilState(0, 0);

	// Debug view SSAO map.
	//DrawScreenQuad(mSsao->AmbientSRV());

	_sky->Draw(_deviceContext, _camera);

	// restore default states, as the SkyFX changes them in the effect file.
	_deviceContext->RSSetState(0);
	_deviceContext->OMSetDepthStencilState(0, 0);

	// Unbind shadow map and AmbientMap as a shader input because we are going to render
	// to it next frame.  These textures can be at any slot, so clear all slots.
	ID3D11ShaderResourceView* nullSRV[16] = { 0 };
	_deviceContext->PSSetShaderResources(0, 16, nullSRV);
}

void MeshViewDemo::OnRender(ID3D11RenderTargetView* renderTargetView)
{
	_smap->BindDsvAndSetNullRenderTarget(_deviceContext);

	DrawSceneToShadowMap();

	_deviceContext->RSSetState(0);

	_deviceContext->ClearDepthStencilView(_depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	_deviceContext->RSSetViewports(1, &_viewport);
	_ssao->SetNormalDepthRenderTarget(_depthStencilView.Get());

	DrawSceneToSsaoNormalDepthMap();

	_ssao->ComputeSsao(_camera);
	_ssao->BlurAmbientMap(10);

	ID3D11RenderTargetView* renderTargets[1] = { renderTargetView };
	_deviceContext->OMSetRenderTargets(1, renderTargets, _depthStencilView.Get());
	_deviceContext->RSSetViewports(1, &_viewport);
	_deviceContext->ClearRenderTargetView(renderTargetView, reinterpret_cast<const float*>(&Colors::Silver));

	_deviceContext->OMSetDepthStencilState(RenderStates::EqualsDSS.Get(), 0);

	XMMATRIX view = _camera.View();
	XMMATRIX proj = _camera.Proj();
	XMMATRIX viewProj = _camera.ViewProj();

	float blendFactor[] = { 0.0f, 0.0f, 0.0f, 0.0f };

	Effects::NormalMapFX->SetDirLights(_dirLights);
	Effects::NormalMapFX->SetEyePosW(_camera.GetPosition());
	Effects::NormalMapFX->SetCubeMap(_sky->CubeMapSRV().Get());
	Effects::NormalMapFX->SetShadowMap(_smap->DepthMapSRV().Get());
	Effects::NormalMapFX->SetSsaoMap(_ssao->AmbientSRV().Get());

	ComPtr<ID3DX11EffectTechnique> tech = Effects::NormalMapFX->Light3TexTech;
	ComPtr<ID3DX11EffectTechnique> alphaClippedTech = Effects::NormalMapFX->Light3TexAlphaClipTech;

	_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	XMMATRIX world;
	XMMATRIX worldInvTranspose;
	XMMATRIX worldViewProj;
	XMMATRIX toTexSpace(
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.0f, 1.0f);

	XMMATRIX shadowTransform = XMLoadFloat4x4(&_shadowTransform);

	RenderManager::GetI()->shadowTransform = XMLoadFloat4x4(&_shadowTransform);
	RenderManager::GetI()->cameraViewProjectionMatrix = view * proj;

	uint32 stride = sizeof(Vertex::PosNormalTexTan);
	uint32 offset = 0;

	_deviceContext->IASetInputLayout(InputLayouts::PosNormalTexTan.Get());

	SceneManager::GetI()->GetCurrentScene()->RenderScene();

	D3DX11_TECHNIQUE_DESC techDesc;
	tech->GetDesc(&techDesc);

	for (uint32 p = 0; p < techDesc.Passes; ++p)
	{
		for (uint32 modelIndex = 0; modelIndex < _modelInstances.size(); ++modelIndex)
		{
			world = XMLoadFloat4x4(&_modelInstances[modelIndex].World);
			worldInvTranspose = MathHelper::InverseTranspose(world);
			worldViewProj = world * view * proj;

			Effects::NormalMapFX->SetWorld(world);
			Effects::NormalMapFX->SetWorldInvTranspose(worldInvTranspose);
			Effects::NormalMapFX->SetWorldViewProj(worldViewProj);
			Effects::NormalMapFX->SetWorldViewProjTex(worldViewProj * toTexSpace);
			Effects::NormalMapFX->SetShadowTransform(world * shadowTransform);
			Effects::NormalMapFX->SetTexTransform(XMMatrixScaling(1.0f, 1.0f, 1.0f));

			for (uint32 subset = 0; subset < _modelInstances[modelIndex].Model->SubsetCount; ++subset)
			{
				Effects::NormalMapFX->SetMaterial(_modelInstances[modelIndex].Model->Mat[subset]);
				Effects::NormalMapFX->SetDiffuseMap(_modelInstances[modelIndex].Model->DiffuseMapSRV[subset].Get());
				Effects::NormalMapFX->SetNormalMap(_modelInstances[modelIndex].Model->NormalMapSRV[subset].Get());

				tech->GetPassByIndex(p)->Apply(0, _deviceContext.Get());
				_modelInstances[modelIndex].Model->ModelMesh.Draw(_deviceContext, subset);
			}
		}
	}

	// The alpha tested triangles are leaves, so render them double sided.
	_deviceContext->RSSetState(RenderStates::NoCullRS.Get());
	alphaClippedTech->GetDesc(&techDesc);

	for (uint32 p = 0; p < techDesc.Passes; ++p)
	{
		for (uint32 modelIndex = 0; modelIndex < _alphaClippedModelInstances.size(); ++modelIndex)
		{
			world = XMLoadFloat4x4(&_alphaClippedModelInstances[modelIndex].World);
			worldInvTranspose = MathHelper::InverseTranspose(world);
			worldViewProj = world * view * proj;

			Effects::NormalMapFX->SetWorld(world);
			Effects::NormalMapFX->SetWorldInvTranspose(worldInvTranspose);
			Effects::NormalMapFX->SetWorldViewProj(worldViewProj);
			Effects::NormalMapFX->SetWorldViewProjTex(worldViewProj * toTexSpace);
			Effects::NormalMapFX->SetShadowTransform(world * shadowTransform);
			Effects::NormalMapFX->SetTexTransform(XMMatrixScaling(1.0f, 1.0f, 1.0f));

			for (uint32 subset = 0; subset < _alphaClippedModelInstances[modelIndex].Model->SubsetCount; ++subset)
			{
				Effects::NormalMapFX->SetMaterial(_alphaClippedModelInstances[modelIndex].Model->Mat[subset]);
				Effects::NormalMapFX->SetDiffuseMap(_alphaClippedModelInstances[modelIndex].Model->DiffuseMapSRV[subset].Get());
				Effects::NormalMapFX->SetNormalMap(_alphaClippedModelInstances[modelIndex].Model->NormalMapSRV[subset].Get());

				alphaClippedTech->GetPassByIndex(p)->Apply(0, _deviceContext.Get());
				_alphaClippedModelInstances[modelIndex].Model->ModelMesh.Draw(_deviceContext, subset);
			}
		}
	}

	_deviceContext->RSSetState(0);
	_deviceContext->OMSetDepthStencilState(0, 0);

	_sky->Draw(_deviceContext, _camera);

	_deviceContext->RSSetState(0);
	_deviceContext->OMSetDepthStencilState(0, 0);

	ID3D11ShaderResourceView* nullSRV[16] = { 0 };
	_deviceContext->PSSetShaderResources(0, 16, nullSRV);
}

void MeshViewDemo::OnMouseDown(WPARAM btnState, int32 x, int32 y)
{
	_lastMousePos.x = x;
	_lastMousePos.y = y;

	SetCapture(_hMainWnd);
}

void MeshViewDemo::OnMouseUp(WPARAM btnState, int32 x, int32 y)
{
	ReleaseCapture();
}

void MeshViewDemo::OnMouseMove(WPARAM btnState, int32 x, int32 y)
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

void MeshViewDemo::DrawSceneToSsaoNormalDepthMap()
{
	XMMATRIX view = _camera.View();
	XMMATRIX proj = _camera.Proj();
	XMMATRIX viewProj = XMMatrixMultiply(view, proj);

	RenderManager::GetI()->cameraViewMatrix = _camera.View();
	RenderManager::GetI()->cameraProjectionMatrix = _camera.Proj();
	RenderManager::GetI()->cameraViewProjectionMatrix = XMMatrixMultiply(view, proj);

	ComPtr<ID3DX11EffectTechnique> tech = Effects::SsaoNormalDepthFX->NormalDepthTech;
	ComPtr<ID3DX11EffectTechnique> alphaClippedTech = Effects::SsaoNormalDepthFX->NormalDepthAlphaClipTech;

	XMMATRIX world;
	XMMATRIX worldInvTranspose;
	XMMATRIX worldView;
	XMMATRIX worldInvTransposeView;
	XMMATRIX worldViewProj;

	_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	_deviceContext->IASetInputLayout(InputLayouts::PosNormalTexTan.Get());

	// Draw Scene Objects
	SceneManager::GetI()->GetCurrentScene()->RenderSceneShadowNormal();

	if (GetAsyncKeyState('1') & 0x8000)
		_deviceContext->RSSetState(RenderStates::WireframeRS.Get());

	D3DX11_TECHNIQUE_DESC techDesc;
	tech->GetDesc(&techDesc);
	for (uint32 p = 0; p < techDesc.Passes; ++p)
	{
		for (uint32 modelIndex = 0; modelIndex < _modelInstances.size(); ++modelIndex)
		{
			world = XMLoadFloat4x4(&_modelInstances[modelIndex].World);
			worldInvTranspose = MathHelper::InverseTranspose(world);
			worldView = world * view;
			worldInvTransposeView = worldInvTranspose * view;
			worldViewProj = world * view * proj;

			Effects::SsaoNormalDepthFX->SetWorldView(worldView);
			Effects::SsaoNormalDepthFX->SetWorldInvTransposeView(worldInvTransposeView);
			Effects::SsaoNormalDepthFX->SetWorldViewProj(worldViewProj);
			Effects::SsaoNormalDepthFX->SetTexTransform(XMMatrixScaling(1.0f, 1.0f, 1.0f));

			tech->GetPassByIndex(p)->Apply(0, _deviceContext.Get());
			for (uint32 subset = 0; subset < _modelInstances[modelIndex].Model->SubsetCount; ++subset)
			{
				_modelInstances[modelIndex].Model->ModelMesh.Draw(_deviceContext, subset);
			}
		}
	}

	// The alpha tested triangles are leaves, so render them double sided.
	_deviceContext->RSSetState(RenderStates::NoCullRS.Get());
	alphaClippedTech->GetDesc(&techDesc);

	for (uint32 p = 0; p < techDesc.Passes; ++p)
	{
		for (uint32 modelIndex = 0; modelIndex < _alphaClippedModelInstances.size(); ++modelIndex)
		{
			world = XMLoadFloat4x4(&_alphaClippedModelInstances[modelIndex].World);
			worldInvTranspose = MathHelper::InverseTranspose(world);
			worldView = world * view;
			worldInvTransposeView = worldInvTranspose * view;
			worldViewProj = world * view * proj;

			Effects::SsaoNormalDepthFX->SetWorldView(worldView);
			Effects::SsaoNormalDepthFX->SetWorldInvTransposeView(worldInvTransposeView);
			Effects::SsaoNormalDepthFX->SetWorldViewProj(worldViewProj);
			Effects::SsaoNormalDepthFX->SetTexTransform(XMMatrixScaling(1.0f, 1.0f, 1.0f));

			for (uint32 subset = 0; subset < _alphaClippedModelInstances[modelIndex].Model->SubsetCount; ++subset)
			{
				Effects::SsaoNormalDepthFX->SetDiffuseMap(_alphaClippedModelInstances[modelIndex].Model->DiffuseMapSRV[subset].Get());
				alphaClippedTech->GetPassByIndex(p)->Apply(0, _deviceContext.Get());
				_alphaClippedModelInstances[modelIndex].Model->ModelMesh.Draw(_deviceContext, subset);
			}
		}
	}

	_deviceContext->RSSetState(0);
}

void MeshViewDemo::DrawSceneToShadowMap()
{
	XMMATRIX view = XMLoadFloat4x4(&_lightView);
	XMMATRIX proj = XMLoadFloat4x4(&_lightProj);
	XMMATRIX viewProj = XMMatrixMultiply(view, proj);

	RenderManager::GetI()->directinalLightViewProjection = XMMatrixMultiply(view, proj);

	Effects::BuildShadowMapFX->SetEyePosW(_camera.GetPosition());
	Effects::BuildShadowMapFX->SetViewProj(viewProj);

	// Draw Scene Objects
	SceneManager::GetI()->GetCurrentScene()->RenderSceneShadow();

	ComPtr<ID3DX11EffectTechnique> tech = Effects::BuildShadowMapFX->BuildShadowMapTech;
	ComPtr<ID3DX11EffectTechnique> alphaClippedTech = Effects::BuildShadowMapFX->BuildShadowMapAlphaClipTech;

	_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	XMMATRIX world;
	XMMATRIX worldInvTranspose;
	XMMATRIX worldViewProj;

	_deviceContext->IASetInputLayout(InputLayouts::PosNormalTexTan.Get());

	if (GetAsyncKeyState('1') & 0x8000)
		_deviceContext->RSSetState(RenderStates::WireframeRS.Get());

	D3DX11_TECHNIQUE_DESC techDesc;
	tech->GetDesc(&techDesc);

	for (uint32 p = 0; p < techDesc.Passes; ++p)
	{
		for (uint32 modelIndex = 0; modelIndex < _modelInstances.size(); ++modelIndex)
		{
			world = XMLoadFloat4x4(&_modelInstances[modelIndex].World);
			worldInvTranspose = MathHelper::InverseTranspose(world);
			worldViewProj = world * view * proj;

			Effects::BuildShadowMapFX->SetWorld(world);
			Effects::BuildShadowMapFX->SetWorldInvTranspose(worldInvTranspose);
			Effects::BuildShadowMapFX->SetWorldViewProj(worldViewProj);
			Effects::BuildShadowMapFX->SetTexTransform(::XMMatrixScaling(1.0f, 1.0f, 1.0f));

			tech->GetPassByIndex(p)->Apply(0, _deviceContext.Get());

			for (uint32 subset = 0; subset < _modelInstances[modelIndex].Model->SubsetCount; ++subset)
			{
				_modelInstances[modelIndex].Model->ModelMesh.Draw(_deviceContext, subset);
			}
		}
	}

	alphaClippedTech->GetDesc(&techDesc);
	for (uint32 p = 0; p < techDesc.Passes; ++p)
	{
		for (uint32 modelIndex = 0; modelIndex < _alphaClippedModelInstances.size(); ++modelIndex)
		{
			world = XMLoadFloat4x4(&_alphaClippedModelInstances[modelIndex].World);
			worldInvTranspose = MathHelper::InverseTranspose(world);
			worldViewProj = world * view * proj;

			Effects::BuildShadowMapFX->SetWorld(world);
			Effects::BuildShadowMapFX->SetWorldInvTranspose(worldInvTranspose);
			Effects::BuildShadowMapFX->SetWorldViewProj(worldViewProj);
			Effects::BuildShadowMapFX->SetTexTransform(XMMatrixScaling(1.0f, 1.0f, 1.0f));

			for (uint32 subset = 0; subset < _alphaClippedModelInstances[modelIndex].Model->SubsetCount; ++subset)
			{
				Effects::BuildShadowMapFX->SetDiffuseMap(_alphaClippedModelInstances[modelIndex].Model->DiffuseMapSRV[subset].Get());
				alphaClippedTech->GetPassByIndex(p)->Apply(0, _deviceContext.Get());
				_alphaClippedModelInstances[modelIndex].Model->ModelMesh.Draw(_deviceContext, subset);
			}
		}
	}

	_deviceContext->RSSetState(0);
}

void MeshViewDemo::DrawScreenQuad(ComPtr<ID3D11ShaderResourceView> srv)
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
		Effects::DebugTexFX->SetTexture(srv.Get());

		tech->GetPassByIndex(p)->Apply(0, _deviceContext.Get());
		_deviceContext->DrawIndexed(6, 0, 0);
	}
}

void MeshViewDemo::BuildShadowTransform()
{
	// Only the first "main" light casts a shadow.
	XMVECTOR lightDir = ::XMLoadFloat3(&_dirLights[0].Direction);
	XMVECTOR lightPos = -2.0f * _sceneBounds.Radius * lightDir;
	XMVECTOR targetPos = ::XMLoadFloat3(&_sceneBounds.Center);
	XMVECTOR up = ::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	XMMATRIX V = ::XMMatrixLookAtLH(lightPos, targetPos, up);

	// Transform bounding sphere to light space.
	XMFLOAT3 sphereCenterLS;
	::XMStoreFloat3(&sphereCenterLS, XMVector3TransformCoord(targetPos, V));

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

void MeshViewDemo::BuildScreenQuadGeometryBuffers()
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
