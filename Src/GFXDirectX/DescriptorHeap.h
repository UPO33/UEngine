#pragma once

#include "Base.h"


#include <mutex>
#include <vector>
#include <queue>
#include <string>


namespace UGFX
{
	//////////////////////////////////////////////////////////////////////////
	class GFXContextDX12;

	UGFXDIRECTX_API unsigned UGetDescHandleIncSize(D3D12_DESCRIPTOR_HEAP_TYPE);

	//////////////////////////////////////////////////////////////////////////
	struct DescHandleBase
	{
		DescHandleBase()
		{
			m_CpuHandle.ptr = D3D12_GPU_VIRTUAL_ADDRESS_UNKNOWN;
			m_GpuHandle.ptr = D3D12_GPU_VIRTUAL_ADDRESS_UNKNOWN;
		}

		DescHandleBase(D3D12_CPU_DESCRIPTOR_HANDLE CpuHandle)
			: m_CpuHandle(CpuHandle)
		{
			m_GpuHandle.ptr = D3D12_GPU_VIRTUAL_ADDRESS_UNKNOWN;
		}

		DescHandleBase(D3D12_CPU_DESCRIPTOR_HANDLE CpuHandle, D3D12_GPU_DESCRIPTOR_HANDLE GpuHandle)
			: m_CpuHandle(CpuHandle), m_GpuHandle(GpuHandle)
		{
		}
		//increase the handle address in bytes
		void Offset(size_t offsetScaled)
		{
			if (m_CpuHandle.ptr != D3D12_GPU_VIRTUAL_ADDRESS_UNKNOWN)
				m_CpuHandle.ptr += offsetScaled;
			if (m_GpuHandle.ptr != D3D12_GPU_VIRTUAL_ADDRESS_UNKNOWN)
				m_GpuHandle.ptr += offsetScaled;
		}
		DescHandleBase GetAt(size_t offsetScaled) const
		{
			DescHandleBase ret = *this;
			ret.Offset(offsetScaled);
			return ret;
		}
		D3D12_CPU_DESCRIPTOR_HANDLE GetCpuHandle() const { return m_CpuHandle; }

		D3D12_GPU_DESCRIPTOR_HANDLE GetGpuHandle() const { return m_GpuHandle; }

		bool IsNull() const { return m_CpuHandle.ptr == D3D12_GPU_VIRTUAL_ADDRESS_UNKNOWN; }
		bool IsShaderVisible() const { return m_GpuHandle.ptr != D3D12_GPU_VIRTUAL_ADDRESS_UNKNOWN; }

		operator D3D12_CPU_DESCRIPTOR_HANDLE () const { return GetCpuHandle(); }
		operator D3D12_GPU_DESCRIPTOR_HANDLE () const { return GetGpuHandle(); }

		D3D12_CPU_DESCRIPTOR_HANDLE m_CpuHandle;
		D3D12_GPU_DESCRIPTOR_HANDLE m_GpuHandle;
	};

	//////////////////////////////////////////////////////////////////////////
	template<D3D12_DESCRIPTOR_HEAP_TYPE HandleType> struct TDescHandle : DescHandleBase
	{
		TDescHandle() : DescHandleBase() {}
		TDescHandle(const DescHandleBase& copy) : DescHandleBase(copy)
		{}
		TDescHandle(D3D12_CPU_DESCRIPTOR_HANDLE CpuHandle)
			: DescHandleBase(CpuHandle)
		{
		}

		TDescHandle(D3D12_CPU_DESCRIPTOR_HANDLE CpuHandle, D3D12_GPU_DESCRIPTOR_HANDLE GpuHandle)
			: DescHandleBase(CpuHandle, GpuHandle)
		{
		}

		TDescHandle operator+ (size_t offset) const
		{
			TDescHandle ret = *this;
			ret += offset;
			return ret;
		}

		void operator += (size_t offset)
		{
			if (m_CpuHandle.ptr != D3D12_GPU_VIRTUAL_ADDRESS_UNKNOWN)
				m_CpuHandle.ptr += offset * UGetDescHandleIncSize(HandleType);
			if (m_GpuHandle.ptr != D3D12_GPU_VIRTUAL_ADDRESS_UNKNOWN)
				m_GpuHandle.ptr += offset * UGetDescHandleIncSize(HandleType);
		}
	};

	using DescHandleRTV = TDescHandle<D3D12_DESCRIPTOR_HEAP_TYPE_RTV>;
	using DescHandleDSV = TDescHandle<D3D12_DESCRIPTOR_HEAP_TYPE_DSV>;
	using DescHandleCSU = TDescHandle<D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV>;
	using DescHandleSampler = TDescHandle<D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER>;


	
	//////////////////////////////////////////////////////////////////////////
	struct DescHandleRange
	{
		D3D12_CPU_DESCRIPTOR_HANDLE m_BaseCpuHandle;
		D3D12_GPU_DESCRIPTOR_HANDLE m_BaseGpuHandle;
		size_t mNumHandle;
		ID3D12DescriptorHeap* mHeap;	//the heap that own this range
		D3D12_DESCRIPTOR_HEAP_TYPE mType;

		auto GetBaseGPUHandle() const { return m_BaseGpuHandle; }
		auto GetBaseCPUHandle() const { return m_BaseCpuHandle; }

		auto GetCPUHandleAt(unsigned index) const
		{
			UASSERT(index < mNumHandle);
			return D3D12_CPU_DESCRIPTOR_HANDLE{ m_BaseCpuHandle.ptr + index * GetHandleIncSize() };
		}
		auto GetGPUHandleAt(unsigned index)
		{
			UASSERT(index < mNumHandle);
			return D3D12_GPU_DESCRIPTOR_HANDLE{ m_BaseGpuHandle.ptr + index * GetHandleIncSize() };
		}
		size_t GetHandleIncSize() const
		{
			return 0;
		}
	};


	struct DynamicDiscRange
	{
		ID3D12Device* mDevice;
		ID3D12DescriptorHeap* mHeap;
		D3D12_DESCRIPTOR_HEAP_TYPE mType;
		D3D12_CPU_DESCRIPTOR_HANDLE mBaseCpuHandle;
		D3D12_GPU_DESCRIPTOR_HANDLE mBaseGpuHandle;
		size_t mHandleIncSize;
		size_t mLength;

		DynamicDiscRange(ID3D12DescriptorHeap* pHeap /*the heap that we write to*/, unsigned length, unsigned offset)
		{
			mHeap = pHeap;
			mLength = length;
			mType = mHeap->GetDesc().Type;
			mHeap->GetDevice(IID_PPV_ARGS(&mDevice));
			mHandleIncSize = mDevice->GetDescriptorHandleIncrementSize(mType);
			mBaseGpuHandle = mHeap->GetGPUDescriptorHandleForHeapStart() + (offset * mHandleIncSize);
			mBaseCpuHandle = mHeap->GetCPUDescriptorHandleForHeapStart() + (offset * mHandleIncSize);
		}
		void Set(unsigned index, D3D12_CPU_DESCRIPTOR_HANDLE handle)
		{
			UASSERT(index < mLength);
			mDevice->CopyDescriptorsSimple(1, mBaseCpuHandle + (index * mHandleIncSize), handle, mType);
		}
	};

	//////////////////////////////////////////////////////////////////////////
	class UGFXDIRECTX_API DescHeapPool
	{
	public:
		void Init(ID3D12Device* pDevice, D3D12_DESCRIPTOR_HEAP_TYPE handleType, unsigned capcity, bool shaderVisible);

		bool HasAvailableSpace(uint32_t Count) const { return Count <= mNumFreeDescriptors; }
		DescHandleBase Alloc(uint32_t Count = 1);

		DescHandleBase GetHandleAtOffset(uint32_t Offset) const;

		bool ValidateHandle(const DescHandleBase& DHandle) const;

		ID3D12DescriptorHeap* GetHeapPointer() const { return mHeap; }

		ID3D12DescriptorHeap* mHeap;
		D3D12_DESCRIPTOR_HEAP_DESC mHeapDesc;
		uint32_t mHandleIncSize;
		uint32_t mNumFreeDescriptors;
		DescHandleBase mFirstHandle;
		DescHandleBase mNextFreeHandle;
	};




	enum EDescriptorHeapType //D3D12_DESCRIPTOR_HEAP_TYPE
	{
		EDH_CBV_SRV_UAV = 0,
		EDH_SAMPLER,
		EDH_RTV,
		EDH_DSV,
		EDH_NUM_TYPES
	};
	inline D3D12_DESCRIPTOR_HEAP_TYPE UToDX12(EDescriptorHeapType type)
	{
		return (D3D12_DESCRIPTOR_HEAP_TYPE)type;
	}

	//////////////////////////////////////////////////////////////////////////
	class UGFXDIRECTX_API DescriptorMgrDX12 : public IDescriptorMgr
	{
	public:
		GFXContextDX12* mCtx;

		DescriptorMgrDX12(GFXContextDX12* ctx);
		
		DescHeapPool	mCSUHeap;
		DescHeapPool  mSamplerHeap;

		DescHeapPool  mNonVisibleCSUHeap;
		DescHeapPool  mNonVisibleSmaplerHeap;
		DescHeapPool	mNonVisibleRTVHeap;
		DescHeapPool  mNonVisibleDSVHeap;
		
		DescHandleBase AllocRTV(unsigned count = 1);
		DescHandleBase AllocDSV(unsigned count = 1);
		//Shader visible descriptor are descriptor  that can be referenced by shaders through descriptor tables.
		//constant buffer view, shader resource view, unordered access view
		DescHandleBase AllocCSU(bool shaderVisible, unsigned count = 1);
		DescHandleBase AllocSampler(bool shaderVisible, unsigned count = 1);
	};

	
};