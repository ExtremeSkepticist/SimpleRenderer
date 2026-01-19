#include <Windows.h>

#include "Application.h"

#include "AssetLoader.h"

// Definition order
#include "GraphicsDevice.h"
#include "RenderingPipeline.h"

#include "Camera.h"
#include "RenderTargetDescriptorHeap.h"
#include "FrameBuilder.h"
#include "Model.h"
#include "PlatformWindow.h"
#include "Scene.h"
#include "SwapChain.h"
#include "TextureGroup.h"
#include "ConstBuffer.h"

namespace sr
{
	Application::Application(gsl::not_null<gsl::czstring> sampleName, int nCmdShow) noexcept
		: mWidth(1080)
		, mHeight(1080)
		, mName(L"SimpleRenderer")
		, mCmdShow(nCmdShow)
		, mSampleName(sampleName)
		, mPlatformWindow(make_unique<platform::PlatformWindow>(mWidth, mHeight, mName, mCmdShow))
		, mPipeline(make_unique<rendering::RenderingPipeline>(*mPlatformWindow, 2))
		, mTextureGroup(make_unique<rendering::TextureGroup>())
	{
	}

	Application::~Application() = default;

	void Application::Run()
	{
		loadAssets();

		while (true)
		{
			update();
			render();
		}
	}

	void Application::loadAssets()
	{
		// Import Model & Create Scene

		// TODO: Do I have to import .mtl file as well?
		const std::string assetPath = std::string("resources\\") + mSampleName; // TODO: Make this into .json?
		sr::rendering::AssetLoader assetLoader;
		assetLoader.LoadAsset(assetPath.c_str());

		mScene = make_unique<rendering::Scene>(assetLoader);
		mFrameBuilder = make_unique<rendering::FrameBuilder>(*mPipeline, *mScene);

		// Priority: 
		// TODO: complete implementing scene graph
		// - make camera belong to scene
		// - move worldPosition into EntityNode or its base class
		// TODO: render Sponza with a solid color, worry about textures later
		// TODO: implement camera movement
		// - chapter 1 ~ 2 study

		// Load all textures from each model mesh
		//auto& meshes = mScene->GetModel().GetMeshes();
		//for (const auto& mesh : meshes)
		//{
		//	mTextureGroup->AddMesh(*mesh);
		//}
		//mTextureGroup->LoadAllTexturesFromMesh(*mPipeline);
	}

	void Application::update()
	{
		// Flow: 
		//	1.1 Check for keyboard/mouse input
		//	1.2 If true, set a camera-updating flag
		// Control:
		//	Q/E: Ascend & descend
		//	W/A/S/D: Move forward/backward/left/right
		//	Mouse: change orientation
		// TODO:
		// 1. Add the keys in the loop of PlatformWindow::WndProc()
		//	1.1 Implement Ascend & descend
		//	1.2 Implement Move
		//	1.3 Implement Mouse orientation
		MSG message = {};
		const BOOL hasSucceededGetMessage = GetMessage(&message, NULL, 0, 0);
		if (hasSucceededGetMessage > 0)
		{
			TranslateMessage(&message);
			DispatchMessage(&message);
		}

		mScene->Update();
	}

	void Application::render()
	{
		// TODO: render Sponza with solid color first
		// TODO: Make FrameBuilder take scene and render entities
		mScene->Render(*mFrameBuilder, *mPipeline);
	}
}