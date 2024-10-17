#pragma once

#include <d3dx12.h>

#include "DescriptorRange.h"
#include "Common.h"

namespace sr
{
	namespace rendering
	{
		class RootParameter
		{
		public:
			RootParameter(const uint32_t rootParameterNum) noexcept;

			void AddDescriptorRange(const DescriptorRange::RangeType rangeType, const uint32_t numDescriptors) noexcept;

		private:
			std::vector<DescriptorRange> mDescriptorRanges = {};
			CD3DX12_ROOT_PARAMETER1 mD3dx12RootParameter = {};
		};
	}
}