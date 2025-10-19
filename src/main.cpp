#include <Windows.h> // byte is ambigous error resolution
 
#include "Application.h"
#include "Common.h"

// TODO: deprecate d3dx12 and implement its own initialization
int CALLBACK wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE, _In_ LPWSTR, _In_ int nCmdShow)
{
	std::unique_ptr<sr::Application> app(make_unique<sr::Application>("Sponza\\IBL\\sponza2.gltf", nCmdShow));

	app->Run();

	return 0;
}