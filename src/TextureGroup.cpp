#include <assimp/scene.h>

#include "TextureGroup.h"

namespace sr
{
	namespace rendering
	{
		TextureGroup::TextureGroup()
			: mTextureMap()
		{
		}
		
		void TextureGroup::AddMesh(gsl::not_null<gsl::czstring> meshName) noexcept
		{
			Expects(mTextureMap.find(meshName.get()) == mTextureMap.end());

			mTextureMap.emplace(make_pair<std::string, std::vector<Texture>>(meshName.get(), std::vector<Texture>()));
		}

		void TextureGroup::LoadAllTexturesFromMesh(gsl::not_null<gsl::czstring> meshName)
		{
			Expects(mTextureMap.find(meshName.get()) == mTextureMap.end());

			// TODO: Get texture names from mesh and load with Texture class and Scratch Image
		}
	}
}