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
			enum class RangeType
			{
				None
				, ConstBuffer
				, ShaderResourceBuffer
				, Num
			};

			RootParameter(const gsl::span<DescriptorRange> descriptorRanges) noexcept;

			inline CD3DX12_ROOT_PARAMETER1& GetD3dx12RootParameter() noexcept
			{
				return mD3dx12RootParameter;
			}

		private:
			std::vector<CD3DX12_DESCRIPTOR_RANGE1> mD3dx12DescriptorRanges = {};
			CD3DX12_ROOT_PARAMETER1 mD3dx12RootParameter = {};
		};
	}
}