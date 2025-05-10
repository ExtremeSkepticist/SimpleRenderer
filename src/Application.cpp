#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Application.h"
#include "FrameBuilder.h"
#include "PlatformWindow.h"
#include "RenderingPipeline.h"
#include "Scene.h"
#include "TextureGroup.h"
#include "Model.h"

namespace sr
{
	Application::Application(uint16_t width, uint16_t height, gsl::not_null<gsl::cwzstring> name, gsl::czstring sampleName, int nCmdShow)
		: mSampleName(sampleName)
		, mPlatformWindow(make_unique<rendering::PlatformWindow>(width, height, name, nCmdShow))
		, mPipeline(make_unique<rendering::RenderingPipeline>(*mPlatformWindow, 2))
		, mFrameResource(make_unique<rendering::FrameBuilder>(mPipeline->GetGraphicsDevice()))
		, mTextureGroup(make_unique<rendering::TextureGroup>())
	{
	}

	void Application::Run()
	{
		loadAssets();
	}

	void Application::loadAssets()
	{
		// Import Model
		Assimp::Importer importer;
		const std::string modelPath("resources\\%s", mSampleName);
		constexpr uint32_t importFlag = aiProcess_ConvertToLeftHanded | aiProcess_CalcTangentSpace;
		const aiScene* scene = importer.ReadFile(modelPath, importFlag);
		mScene = make_unique<rendering::Scene>(*scene);

		// Load all textures from each model mesh
		for (auto& model : mScene->GetModels())
		{
			auto& meshes = model.GetMeshes();
			for (const auto& mesh : meshes)
			{
				mTextureGroup->AddMesh(*mesh);
			}
		}
		mTextureGroup->LoadAllTexturesFromMesh(*mPipeline);

		assert(scene != nullptr);
	}
}