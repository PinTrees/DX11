#include "pch.h"
#include "App.h"

#include "01. InitDemo.h"
#include "02. BoxDemo.h"
#include "03. HillsDemo.h"
#include "04. ShapesDemo.h"
#include "05. SkullDemo.h"
#include "06. WavesDemo.h"
#include "07. LightingDemo.h"
#include "08. CrateDemo.h"
#include "09. TexturedHillsAndWavesDemo.h"
#include "10. BlendDemo.h"
#include "11. MirrorDemo.h"
#include "12. TreeBillboardDemo.h"
#include "13. VecAddDemo.h"
#include "14. BlurDemo.h"
#include "15. BasicTessellation.h"
#include "16. BasicTriangleTessellation.h"
#include "17. BezierTessellation.h"
#include "18. CameraDemo.h"
#include "19. InstancingAndCullingDemo.h"
#include "20. PickingDemo.h"
#include "21. CubeMapDemo.h"
#include "22. DynamicCubeMapDemo.h"
#include "23. NormalDisplacementMapDemo.h"
#include "24. TerrainDemo.h"
#include "25. ParticlesDemo.h"
#include "26. ShadowsDemo.h"
#include "27. AmbientOcclusionDemo.h"
#include "28. SsaoDemo.h"
#include "29. MeshViewDemo.h"
#include "30. QuatDemo.h"
#include "31. SkinnedMeshDemo.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd)
{
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	//InitDemo theApp(hInstance);
	//BoxDemo theApp(hInstance);
	//HillsDemo theApp(hInstance);
	//ShapesDemo theApp(hInstance);
	//SkullDemo theApp(hInstance);
	//WavesDemo theApp(hInstance);	
	//LightingDemo theApp(hInstance);
	
	//CrateDemo theApp(hInstance);
	//TexturedHillsAndWavesDemo theApp(hInstance);
	//BlendDemo theApp(hInstance);
	//MirrorDemo theApp(hInstance);
	//TreeBillboardDemo theApp(hInstance);

	//VecAddDemo theApp(hInstance);
	//BlurDemo theApp(hInstance);

	//BasicTessellation theApp(hInstance);
	//BasicTriangleTessellation theApp(hInstance);
	//BezierTessellation theApp(hInstance);
		
	//CameraDemo theApp(hInstance);
	//InstancingAndCullingDemo theApp(hInstance);
	
	//PickingDemo theApp(hInstance);
	//CubeMapDemo theApp(hInstance);
	//DynamicCubeMapDemo theApp(hInstance);
	//NormalDisplacementMapDemo theApp(hInstance);
	//TerrainDemo theApp(hInstance);
	
	//ParticlesDemo theApp(hInstance);
	//ShadowsDemo theApp(hInstance);
	//AmbientOcclusionDemo theApp(hInstance);
	//SsaoDemo theApp(hInstance);

	MeshViewDemo theApp(hInstance);
	//QuatDemo theApp(hInstance);
	//SkinnedMeshDemo theApp(hInstance);
	
	if (!theApp.Init())
		return 0;

	return theApp.Run();
}
