#include <assimp/scene.h>
#include <codecvt>
#include <locale>

#include "CommandList.h"
#include "CommandQueue.h"
#include "GraphicsDevice.h"
#include "Mesh.h"
#include "TextureGroup.h"
#include "RenderingPipeline.h"

namespace sr
{
	namespace rendering
	{
		TextureGroup::TextureGroup()
			: mTextureMap()
		{
		}
		
		void TextureGroup::AddMesh(Mesh& mesh) noexcept
		{
			Expects(mTextureMap.find(&mesh) == mTextureMap.end());

			mTextureMap.emplace(make_pair(&mesh, std::vector<Texture>()));
		}

		void TextureGroup::LoadAllTexturesFromMesh(RenderingPipeline& pipeline) noexcept
		{
			const gsl::index textureTypeNum = static_cast<gsl::index>(Texture::TextureType::Num);
			for (auto& entry : mTextureMap)
			{
				entry.second.reserve(textureTypeNum);

				for (gsl::index i = 0; i < textureTypeNum; ++i)
				{
					auto& mesh = *entry.first;
					loadTexture(mesh, static_cast<Texture::TextureType>(i), pipeline.GetGraphicsDevice());
					copyTextureIntoGpu(mTextureMap[&mesh][i], pipeline);
				}
			}
		}

		void TextureGroup::loadTexture(Mesh& mesh, Texture::TextureType type, GraphicsDevice& device)
		{
			std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
			std::wstring fileName = converter.from_bytes(mesh.GetTextureName(type));
			const std::wstring path = L"resources\\" + fileName;

			unique_ptr<ScratchImage> scratchImage = make_unique<ScratchImage>();
			// Pre-converted .dds file by texconv.exe
			// TODO: automate the conversion process somehow
			HRESULT hasLoaded = LoadFromDDSFile(path.c_str(), DDS_FLAGS_NONE, nullptr, *scratchImage);
			assert(SUCCEEDED(hasLoaded));

			mTextureMap[&mesh].emplace_back(type, fileName.c_str(), std::move(scratchImage), device);
		}

		void TextureGroup::copyTextureIntoGpu(Texture& texture, RenderingPipeline& pipeline)
		{
			CommandList& graphicsCommandList = pipeline.GetGraphicsCommandList();

			graphicsCommandList.ResetCommandList();
			graphicsCommandList.RecordCommandCopyResource(texture.GetDefaultHeapResource(), texture.GetUploadHeapResource(), texture.GetSubResourceData());
			graphicsCommandList.FinishRecordingCommands();
			pipeline.GetGraphicsCommandQueue().ExecuteCommandList(graphicsCommandList);

			pipeline.WaitForPreviousFrame();
		}
	}
}