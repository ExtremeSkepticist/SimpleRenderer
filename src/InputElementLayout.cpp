#include "InputElementLayout.h"

namespace sr
{
	namespace rendering
	{
		InputElementLayout::InputElementLayout(const uint32_t inputSlot) noexcept
			: mInputSlot(inputSlot)
			, mD3d12InputElementDesc({})
		{
			constexpr uint32_t defaultElementsCapacity = 16;
			mD3d12InputElementDesc.reserve(defaultElementsCapacity);
		}

		void InputElementLayout::AddLayout(gsl::not_null<gsl::czstring> semanticName, const uint32_t semanticIndex
			, const DXGI_FORMAT dxgiFormat) noexcept
		{
			const D3D12_INPUT_ELEMENT_DESC inputElementDesc = { 
				semanticName, semanticIndex, dxgiFormat
				, mInputSlot, D3D12_APPEND_ALIGNED_ELEMENT
				, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 
			};

			mD3d12InputElementDesc.emplace_back(inputElementDesc);
		}
	}
}