#pragma once

#include <d3d12.h>

#include "Common.h"

namespace sr
{
	namespace rendering
	{
		class InputElementLayout
		{
		public:
			InputElementLayout(const uint32_t inputSlot) noexcept;
			
			~InputElementLayout() = default;

			void AddLayout(gsl::not_null<gsl::czstring> semanticName, uint32_t semanticIndex
				, DXGI_FORMAT dxgiFormat) noexcept;

			inline std::vector<D3D12_INPUT_ELEMENT_DESC>& GetInputElementDescOwner() noexcept
			{
				Expects(mD3d12InputElementDesc.size() > 0);

				return mD3d12InputElementDesc;
			}

		private:
			const uint32_t mInputSlot;

			std::vector<D3D12_INPUT_ELEMENT_DESC> mD3d12InputElementDesc = {};
		};
	}
}