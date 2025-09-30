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
	Application::Application(uint16_t width, uint16_t height, gsl::not_null<gsl::cwzstring> name, gsl::czstring sampleName, int nCmdShow)
		: mWidth(width)
		, mHeight(height)
		, mName(name)
		, mCmdShow(nCmdShow)
		, mSampleName(sampleName)
		, mIsInit(false)
	{
	}
	Application::~Application() = default;

	void Application::Init()
	{
		Expects(!mIsInit);
		Expects(!mPlatformWindow && !mPipeline && !mTextureGroup);

		mPlatformWindow = make_unique<platform::PlatformWindow>(mWidth, mHeight, mName, mCmdShow);
		mPipeline = make_unique<rendering::RenderingPipeline>(*mPlatformWindow, 2);
		mTextureGroup = make_unique<rendering::TextureGroup>();

		mIsInit = true;

		Ensures(mPlatformWindow && mPipeline && mTextureGroup);
	}

	void Application::Run()
	{
		Expects(mIsInit);

		loadAssets();

		while (true)
		{
			update();
			render();
		}
	}

	void Application::loadAssets()
	{
		// TODO: Move this into AssetLoader class
		// - contains scene graph hierachy information (mesh, material) using assimp, which is abstracted under the hood
		// - Depercate model class
		// - Entity class contains Node. Node contains Transform, ChildNodes, Mesh and Material class
		// - DataLoader takes a path to the model and initializes assimp scene
		// - Entity class takes a valid DataLoader object and load assimp info into entity nodes
		// Import Model & Create Scene

		const std::string assetPath = std::string("resources\\") + mSampleName; // TODO: Make this into .json?
		sr::rendering::AssetLoader assetLoader;
		assetLoader.LoadAsset(assetPath.c_str());

		mScene = make_unique<rendering::Scene>(assetLoader);
		mFrameBuilder = make_unique<rendering::FrameBuilder>(*mPipeline, *mScene);

		// Load all textures from each model mesh
		auto& meshes = mScene->GetModel().GetMeshes();
		for (const auto& mesh : meshes)
		{
			mTextureGroup->AddMesh(*mesh);
		}
		mTextureGroup->LoadAllTexturesFromMesh(*mPipeline);
	}

	void Application::update()
	{
		mScene->Update();
		mFrameBuilder->Update(mScene.get());
	}

	void Application::render()
	{
		auto& meshes = mScene->GetModel().GetMeshes();
		for (auto& mesh : meshes)
		{
			mFrameBuilder->SetMesh(mesh.get(), mTextureGroup.get(), mPipeline->GetGraphicsDevice());
			mPipeline->Render(*mFrameBuilder);
		}
	}
}