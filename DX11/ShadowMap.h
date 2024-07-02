#pragma once

class ShadowMap
{
public:
	ShadowMap(ComPtr<ID3D11Device> device, uint32 width, uint32 height);
	~ShadowMap();

	ComPtr<ID3D11ShaderResourceView> DepthMapSRV();

	void BindDsvAndSetNullRenderTarget(ComPtr<ID3D11DeviceContext> dc);

private:
	uint32 _width;
	uint32 _height;

	ComPtr<ID3D11ShaderResourceView> _depthMapSRV;
	ComPtr<ID3D11DepthStencilView> _depthMapDSV;

	D3D11_VIEWPORT _viewport;
};
