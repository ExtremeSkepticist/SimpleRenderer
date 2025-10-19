#pragma once

#include "Common.h"

namespace sr
{
	namespace platform
	{
		class PlatformWindow;
	}
	namespace rendering
	{
		class Scene;
		class FrameBuilder;
		class RenderingPipeline;
		class TextureGroup;
	}

	class Application
	{
	public:
		Application(gsl::not_null<gsl::czstring> sampleName, int nCmdShow) noexcept;
		~Application();

		void Run();

	private:
		void loadAssets();
		void update();
		void render();

		uint16_t mWidth = 0;
		uint16_t mHeight = 0;
		gsl::cwzstring mName = L"SimpleRenderer";
		gsl::czstring mSampleName;
		int mCmdShow = 0;

		unique_ptr<rendering::Scene> mScene = nullptr;
		unique_ptr<rendering::TextureGroup> mTextureGroup = nullptr;
		unique_ptr<platform::PlatformWindow> mPlatformWindow = nullptr;
		unique_ptr<rendering::RenderingPipeline> mPipeline = nullptr;
		unique_ptr<rendering::FrameBuilder> mFrameBuilder = nullptr;
	};
}