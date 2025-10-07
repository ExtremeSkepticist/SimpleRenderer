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
		: mWidth(800)
		, mHeight(600)
		, mName(L"SimpleRenderer")
		, mCmdShow(nCmdShow)
		, mSampleName(sampleName)
		, mPlatformWindow(make_unique<platform::PlatformWindow>(mWidth, mHeight, mName, mCmdShow))
		, mPipeline(make_unique<rendering::RenderingPipeline>(*mPlatformWindow, 2))
		, mTextureGroup(make_unique<rendering::TextureGroup>())
	{
	}

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
		// TODO: Check for keyboard/mouse input
		mScene->Update();
		mFrameBuilder->Update(mScene.get());
	}

	void Application::render()
	{
		// TODO: render Sponza with solid color first
		auto& meshes = mScene->GetModel().GetMeshes();
		for (auto& mesh : meshes)
		{
			mFrameBuilder->SetMesh(mesh.get(), mTextureGroup.get(), mPipeline->GetGraphicsDevice());
			mPipeline->Render(*mFrameBuilder);
		}
	}
}