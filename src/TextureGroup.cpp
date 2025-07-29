#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING

#include <Windows.h> // To fix "byte is ambiguous"

#include <assimp/scene.h>
#include <codecvt>
#include <locale>
#include <filesystem>

#include "TextureGroup.h"

#include "CommandList.h"
#include "CommandQueue.h"
#include "GraphicsDevice.h"
#include "Mesh.h"
#include "RenderingPipeline.h"


namespace sr
{
	namespace rendering
	{
		TextureGroup::TextureGroup() noexcept
			: mTextureMap()
		{
		}
		
		void TextureGroup::AddMesh(Mesh& mesh) noexcept
		{
			Expects(mTextureMap.find(&mesh) == mTextureMap.end());

			mTextureMap.insert(std::make_pair(&mesh, std::vector<Texture>()));
		}

		void TextureGroup::LoadAllTexturesFromMesh(RenderingPipeline& pipeline) noexcept
		{
			// TODO: add emissive map
			// TODO: add AO map
			const gsl::index textureTypeNum = static_cast<gsl::index>(Texture::TextureType::Num) - 2;
			for (auto& entry : mTextureMap)
			{
				entry.second.reserve(textureTypeNum);

				for (gsl::index i = 0; i < textureTypeNum; ++i)
				{
					const Mesh& mesh = *entry.first;
					loadTexture(mesh, static_cast<Texture::TextureType>(i), pipeline.GetGraphicsDevice());
				}
				for (gsl::index i = 0; i < textureTypeNum; ++i)
				{
					auto& mesh = *entry.first;
					copyTextureIntoGpu(mTextureMap[&mesh][i], pipeline);
				}
				
			}
		}

		Texture& TextureGroup::GetTexture(const Mesh* mesh, Texture::TextureType type) noexcept
		{
			return mTextureMap[mesh][static_cast<gsl::index>(type)];
		}

		void TextureGroup::loadTexture(const Mesh& mesh, Texture::TextureType type, GraphicsDevice& device)
		{
			std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
			std::wstring fileName = converter.from_bytes(mesh.GetTextureName(type));
			std::filesystem::path newPath(std::wstring(L"resources\\" + fileName));
			newPath.replace_extension("dds");
			// TODO: add emissive map
			// TODO: add AO map
			if (type == Texture::TextureType::Emissive || type == Texture::TextureType::AmbientOcclusion)
			{
				return;
			}
			unique_ptr<ScratchImage> scratchImage = make_unique<ScratchImage>();
			// Pre-converted .dds file by texconv.exe
			// TODO: automate the conversion process somehow
			HRESULT hasLoaded = LoadFromDDSFile(newPath.generic_wstring().c_str(), DDS_FLAGS_NONE, nullptr, *scratchImage);
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