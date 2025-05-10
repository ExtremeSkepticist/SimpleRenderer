#pragma once

#include "Common.h"

namespace sr
{
	namespace rendering
	{
		class Scene;

		class FrameBuilder;
		class PlatformWindow;
		class RenderingPipeline;
		class TextureGroup;
	}

	class Application
	{
	public:
		Application(uint16_t width, uint16_t height, gsl::not_null<gsl::cwzstring> name, gsl::czstring sampleName, int nCmdShow);
		void Run();

	private:
		void loadAssets();
		void update();
		void render();

		gsl::czstring mSampleName;

		unique_ptr<rendering::Scene> mScene = nullptr;
		unique_ptr<rendering::TextureGroup> mTextureGroup = nullptr;
		unique_ptr<rendering::PlatformWindow> mPlatformWindow = nullptr;
		unique_ptr<rendering::RenderingPipeline> mPipeline = nullptr;
		unique_ptr<rendering::FrameBuilder> mFrameResource = nullptr;
	};
}