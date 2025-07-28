#include <Windows.h> // byte is ambigous error resolution
 
#include "Application.h"
#include "Common.h"

// TODO: deprecate d3dx12 and implement its own initialization
int CALLBACK wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE, _In_ LPWSTR, _In_ int nCmdShow)
{
	constexpr uint32_t width = 800;
	constexpr uint32_t height = 600;
	gsl::cwzstring name = L"SimpleRenderer"; // TODO: Concatenate sample name e.g. SimpleRenderer - DamagedHelmet
	gsl::czstring sampleName = "DamagedHelmet.gltf";

	unique_ptr<sr::Application> app = make_unique<sr::Application>(width, height, name, sampleName, nCmdShow);
	app->Run();

	return 0;
}