#pragma once
#include "EditorDailog.h"  
#include <filesystem> 

namespace fs = std::filesystem;

class MeshSelectEditorDialog : public EditorDialog
{
private:
	vector<shared_ptr<MeshFile>> m_MeshFiles; 

	// Runtime Value
	Mesh*						m_SelectMesh;
	SkinnedMesh*				m_SelectSkinnedMesh;
	int&						m_SelectSubsetIndex; 

public:
	MeshSelectEditorDialog(Mesh* selectMesh, SkinnedMesh* selectSkinnedMesh, int& selectSubsetIndex);
	~MeshSelectEditorDialog();

	static void Open(Mesh* selectMesh, SkinnedMesh* selectSkinnedMesh, int& selectSubsetIndex);

	virtual void Show() override;
	virtual void Close() override;
	virtual void OnRender() override;
};

