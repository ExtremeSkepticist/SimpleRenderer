#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Application.h"
#include "Camera.h"
#include "RenderTargetDescriptorHeap.h"
#include "FrameBuilder.h"
#include "GraphicsDevice.h"
#include "Model.h"
#include "PlatformWindow.h"
#include "RenderingPipeline.h"
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

	void Application::Init()
	{
		Expects(!mIsInit);
		Expects(!mPlatformWindow && !mPipeline && !mFrameBuilder && !mTextureGroup);

		mPlatformWindow = make_unique<rendering::PlatformWindow>(mWidth, mHeight, mName, mCmdShow);
		mPipeline = make_unique<rendering::RenderingPipeline>(*mPlatformWindow, 2);
		mFrameBuilder = make_unique<rendering::FrameBuilder>(mPipeline->GetGraphicsDevice());
		mTextureGroup = make_unique<rendering::TextureGroup>();

		mIsInit = true;

		Ensures(mPlatformWindow && mPipeline && mFrameBuilder && mTextureGroup);
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
		// Import Model & Create Scene
		Assimp::Importer importer;
		const std::string modelPath("resources\\%s", mSampleName);
		constexpr uint32_t importFlag = aiProcess_ConvertToLeftHanded | aiProcess_CalcTangentSpace;
		const aiScene* scene = importer.ReadFile(modelPath, importFlag);
		assert(scene != nullptr);

		mScene = make_unique<rendering::Scene>(*scene);
		mFrameBuilder->setModel(&mScene->GetModel());

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
		// TODO: move code in main()
		mScene->Update();
		mFrameBuilder->Update();
	}

	void Application::render()
	{
		// TODO: move code in main()
	}
}