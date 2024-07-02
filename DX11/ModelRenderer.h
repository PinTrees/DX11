#pragma once
#include "Component.h"

class Model;
class Shader;
class Material;

class ModelRenderer : Component
{
	using Super = Component;

private:
	shared_ptr<Shader>	_shader;
	uint8				_pass = 0;
	shared_ptr<Model>	_model;

public:
	ModelRenderer(Shader* shader);
	virtual ~ModelRenderer();

	void SetModel(shared_ptr<Model> model);
	void SetPass(uint8 pass) { _pass = pass; }
};

