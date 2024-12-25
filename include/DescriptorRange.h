#pragma once

#include <d3dx12.h>

#include "Common.h"

namespace sr
{
	namespace rendering
	{
		class DescriptorRange
		{
		public:
			enum class RangeType
			{
				None
				, ConstBuffer
				, ShaderResourceBuffer
				, Num
			};

			DescriptorRange(const RangeType rangeType, const uint32_t numDescriptors) noexcept;
			
			DescriptorRange() = delete;

			inline CD3DX12_DESCRIPTOR_RANGE1& GetD3d12Range() noexcept
			{
				return mD3dx12Range;
			}

		private:
			RangeType mRangeType = RangeType::None;
			uint32_t mNumDescriptors = 0;
			CD3DX12_DESCRIPTOR_RANGE1 mD3dx12Range = {};
		};
	}
}