#pragma once

#include "../Core/Base.h"
#include "../Core/Matrix.h"
#include "../Core/Name.h"
#include "../Core/SmartPointers.h"
#include "../Core/SmartMemory.h"

#ifdef UGFXCORE_SHARED
#ifdef UGFXCORE_BUILD
#define UGFXCORE_API UMODULE_EXPORT
#else
#define UGFXCORE_API UMODULE_IMPORT
#endif
#else
#define UGFXCORE_API
#endif


namespace UGFX
{
	using namespace UCore;

	//////////////////////////////////////////////////////////////////////////
	class ShaderConstantsCombined;

	//////////////////////////////////////////////////////////////////////////
	enum class EGFXAPI
	{
		Null,
		DirectX9,
		DirectX10,
		DirectX11,
		DirectX12,
		Vulkan,
		OpenGLES2,
		OpenGLES3,
	};
	enum EGFXCommandListType
	{
		Direct = 0,
		Bundle,
		Compute,
		Copy,
	};
	enum class EGFXQueryType
	{
		Occlusion = 0,
		BinaryOcclusion = 1,
		TimesStamp = 2,
		PipelineStatistics = 3,
		ESO_STATISTICS_STREAM0 = 4,
		ESO_STATISTICS_STREAM1 = 5,
		ESO_STATISTICS_STREAM2 = 6,
		ESO_STATISTICS_STREAM3 = 7
	};
	enum class EShader : uint32
	{
		Vertex,
		Hull,
		Domain,
		Geometry,
		Pixel,
		Compute,
	};
	UGFXCORE_API const char* UEnumToStr(EShader in);

	enum class EPrimitiveTopology
	{
		Undefined = 0,

		PointList = 1,

		LineList = 2,
		LineStrip = 3,
		TriangleList = 4,
		TriangleStrip = 5,

		LineListAdj = 10,
		LineStripAdj = 11,
		TriangleListAdj = 12,
		TriangleStripAdj = 13,

		ControlPointPatchList1 = 33,
		ControlPointPatchList2,
		ControlPointPatchList3,
		ControlPointPatchList4,
		ControlPointPatchList5,
		ControlPointPatchList6,
		ControlPointPatchList7,
		ControlPointPatchList8,
		ControlPointPatchList9,
		ControlPointPatchList10,
		ControlPointPatchList11,
		ControlPointPatchList12,
		ControlPointPatchList13,
		ControlPointPatchList14,
		ControlPointPatchList15,
		ControlPointPatchList16,
		ControlPointPatchList17,
		ControlPointPatchList18,
		ControlPointPatchList19,
		ControlPointPatchList20,
		ControlPointPatchList21,
		ControlPointPatchList22,
		ControlPointPatchList23,
		ControlPointPatchList24,
		ControlPointPatchList25,
		ControlPointPatchList26,
		ControlPointPatchList27,
		ControlPointPatchList28,
		ControlPointPatchList29,
		ControlPointPatchList30,
		ControlPointPatchList31,
		ControlPointPatchList32,
	};

	enum class EGFXBlend //D3D12_BLEND
	{
		ZERO = 1,
		ONE = 2,
		SRC_COLOR = 3,
		INV_SRC_COLOR = 4,
		SRC_ALPHA = 5,
		INV_SRC_ALPHA = 6,
		DEST_ALPHA = 7,
		INV_DEST_ALPHA = 8,
		DEST_COLOR = 9,
		INV_DEST_COLOR = 10,
		SRC_ALPHA_SAT = 11,
		BLEND_FACTOR = 14,
		INV_BLEND_FACTOR = 15,
		SRC1_COLOR = 16,
		INV_SRC1_COLOR = 17,
		SRC1_ALPHA = 18,
		INV_SRC1_ALPHA = 19
	};
	enum class EGFXBlendOp //D3D12_BLEND_OP
	{
		ADD = 1,
		SUBTRACT = 2,
		REV_SUBTRACT = 3,
		MIN = 4,
		MAX = 5
	};
	enum class EGFXLogicOp //D3D12_LOGIC_OP
	{
		CLEAR = 0,
		SET = (CLEAR + 1),
		COPY = (SET + 1),
		COPY_INVERTED = (COPY + 1),
		NOOP = (COPY_INVERTED + 1),
		INVERT = (NOOP + 1),
		AND = (INVERT + 1),
		NAND = (AND + 1),
		OR = (NAND + 1),
		NOR = (OR + 1),
		XOR = (NOR + 1),
		EQUIV = (XOR + 1),
		AND_REVERSE = (EQUIV + 1),
		AND_INVERTED = (AND_REVERSE + 1),
		OR_REVERSE = (AND_INVERTED + 1),
		OR_INVERTED = (OR_REVERSE + 1)
	};

	enum EGFXColorWriteEnable //D3D12_COLOR_WRITE_ENABLE
	{
		ECR_RED = 1,
		ECR_GREEN = 2,
		ECR_BLUE = 4,
		ECR_ALPHA = 8,
		ECR_ALL = ECR_RED | ECR_GREEN | ECR_BLUE | ECR_ALPHA
	};
	enum class EGFXComparison //D3D12_COMPARISON_FUNC
	{
		NEVER = 1,
		LESS = 2,
		EQUAL = 3,
		LESS_EQUAL = 4,
		GREATER = 5,
		NOT_EQUAL = 6,
		GREATER_EQUAL = 7,
		ALWAYS = 8
	};
	enum class EGFXFillMode
	{
		Wireframe = 2,
		Solid = 3
	};
	enum class EGFXCullMode //D3D12_CULL_MODE
	{
		None = 1,
		Front = 2,
		Back = 3,
	};
	enum class EGFXStencilOp //D3D12_STENCIL_OP
	{
		KEEP = 1,
		ZERO = 2,
		REPLACE = 3,
		INCR_SAT = 4,
		DECR_SAT = 5,
		INVERT = 6,
		INCR = 7,
		DECR = 8
	};
	enum class EGFXFormat //DXGI_FORMAT
	{
		UNKNOWN = 0,
		R32G32B32A32_TYPELESS = 1,
		R32G32B32A32_FLOAT = 2,
		R32G32B32A32_UINT = 3,
		R32G32B32A32_SINT = 4,
		R32G32B32_TYPELESS = 5,
		R32G32B32_FLOAT = 6,
		R32G32B32_UINT = 7,
		R32G32B32_SINT = 8,
		R16G16B16A16_TYPELESS = 9,
		R16G16B16A16_FLOAT = 10,
		R16G16B16A16_UNORM = 11,
		R16G16B16A16_UINT = 12,
		R16G16B16A16_SNORM = 13,
		R16G16B16A16_SINT = 14,
		R32G32_TYPELESS = 15,
		R32G32_FLOAT = 16,
		R32G32_UINT = 17,
		R32G32_SINT = 18,
		R32G8X24_TYPELESS = 19,
		D32_FLOAT_S8X24_UINT = 20,
		R32_FLOAT_X8X24_TYPELESS = 21,
		X32_TYPELESS_G8X24_UINT = 22,
		R10G10B10A2_TYPELESS = 23,
		R10G10B10A2_UNORM = 24,
		R10G10B10A2_UINT = 25,
		R11G11B10_FLOAT = 26,
		R8G8B8A8_TYPELESS = 27,
		R8G8B8A8_UNORM = 28,
		R8G8B8A8_UNORM_SRGB = 29,
		R8G8B8A8_UINT = 30,
		R8G8B8A8_SNORM = 31,
		R8G8B8A8_SINT = 32,
		R16G16_TYPELESS = 33,
		R16G16_FLOAT = 34,
		R16G16_UNORM = 35,
		R16G16_UINT = 36,
		R16G16_SNORM = 37,
		R16G16_SINT = 38,
		R32_TYPELESS = 39,
		D32_FLOAT = 40,
		R32_FLOAT = 41,
		R32_UINT = 42,
		R32_SINT = 43,
		R24G8_TYPELESS = 44,
		D24_UNORM_S8_UINT = 45,
		R24_UNORM_X8_TYPELESS = 46,
		X24_TYPELESS_G8_UINT = 47,
		R8G8_TYPELESS = 48,
		R8G8_UNORM = 49,
		R8G8_UINT = 50,
		R8G8_SNORM = 51,
		R8G8_SINT = 52,
		R16_TYPELESS = 53,
		R16_FLOAT = 54,
		D16_UNORM = 55,
		R16_UNORM = 56,
		R16_UINT = 57,
		R16_SNORM = 58,
		R16_SINT = 59,
		R8_TYPELESS = 60,
		R8_UNORM = 61,
		R8_UINT = 62,
		R8_SNORM = 63,
		R8_SINT = 64,
		A8_UNORM = 65,
		R1_UNORM = 66,
		R9G9B9E5_SHAREDEXP = 67,
		R8G8_B8G8_UNORM = 68,
		G8R8_G8B8_UNORM = 69,
		BC1_TYPELESS = 70,
		BC1_UNORM = 71,
		BC1_UNORM_SRGB = 72,
		BC2_TYPELESS = 73,
		BC2_UNORM = 74,
		BC2_UNORM_SRGB = 75,
		BC3_TYPELESS = 76,
		BC3_UNORM = 77,
		BC3_UNORM_SRGB = 78,
		BC4_TYPELESS = 79,
		BC4_UNORM = 80,
		BC4_SNORM = 81,
		BC5_TYPELESS = 82,
		BC5_UNORM = 83,
		BC5_SNORM = 84,
		B5G6R5_UNORM = 85,
		B5G5R5A1_UNORM = 86,
		B8G8R8A8_UNORM = 87,
		B8G8R8X8_UNORM = 88,
		R10G10B10_XR_BIAS_A2_UNORM = 89,
		B8G8R8A8_TYPELESS = 90,
		B8G8R8A8_UNORM_SRGB = 91,
		B8G8R8X8_TYPELESS = 92,
		B8G8R8X8_UNORM_SRGB = 93,
		BC6H_TYPELESS = 94,
		BC6H_UF16 = 95,
		BC6H_SF16 = 96,
		BC7_TYPELESS = 97,
		BC7_UNORM = 98,
		BC7_UNORM_SRGB = 99,
		AYUV = 100,
		Y410 = 101,
		Y416 = 102,
		NV12 = 103,
		P010 = 104,
		P016 = 105,
		OPAQUE_420 = 106,
		YUY2 = 107,
		Y210 = 108,
		Y216 = 109,
		NV11 = 110,
		AI44 = 111,
		IA44 = 112,
		P8 = 113,
		A8P8 = 114,
		B4G4R4A4_UNORM = 115,

		P208 = 130,
		V208 = 131,
		V408 = 132,

		Max,
	};
	enum class EPrimitiveTopologyType //D3D12_PRIMITIVE_TOPOLOGY_TYPE
	{
		UNDEFINED = 0,
		POINT = 1,
		LINE = 2,
		TRIANGLE = 3,
		PATCH = 4
	};

	//////////////////////////////////////////////////////////////////////////
	struct GFXRootSignature;
	struct GFXPiplineState;
	struct GFXShader;

	/*
		if (mBlendEnable)
		{
			finalColor.rgb = (mSrcBlend * newColor.rgb) <mBlendOp> (mDestBlend * oldColor.rgb);
			finalColor.a = (mSrcBlendAlpha * newColor.a) <mBlendOpAlpha> (mDestBlendAlpha * oldColor.a);
		}
		else
		{
			finalColor = newColor;
		}

		for alpha blending :
		finalColor.rgb = newAlpha * newColor + (1 - newAlpha) * oldColor;
		finalColor.a = newAlpha.a;
	*/
	struct GFXRenderTargetBlendDesc //D3D12_RENDER_TARGET_BLEND_DESC
	{
		////Specifies whether to enable (or disable) blending. Set to TRUE to enable blending.
		bool					mBlendEnable = false;
		//Specifies whether to enable (or disable) a logical operation.
		bool					mLogicOpEnable = false;

		EGFXBlend				mSrcBlend = EGFXBlend::ONE;
		EGFXBlend				mDestBlend = EGFXBlend::ZERO;
		EGFXBlendOp				mBlendOp = EGFXBlendOp::ADD;
		EGFXBlend				mSrcBlendAlpha = EGFXBlend::ONE;
		EGFXBlend				mDestBlendAlpha = EGFXBlend::ZERO;
		EGFXBlendOp				mBlendOpAlpha = EGFXBlendOp::ADD;
		EGFXLogicOp				mLogicOp = EGFXLogicOp::NOOP;
		EGFXColorWriteEnable	mRenderTargetWriteMask = EGFXColorWriteEnable::ECR_ALL;

		void InitAsAlphaBlend()
		{
			mBlendEnable = true;

			mSrcBlend = EGFXBlend::SRC_ALPHA;
			mBlendOp = EGFXBlendOp::ADD;
			mDestBlend = EGFXBlend::INV_SRC_ALPHA;

			mSrcBlendAlpha = EGFXBlend::ONE;
			mBlendOpAlpha = EGFXBlendOp::ADD;
			mDestBlendAlpha = EGFXBlend::ZERO; // EGFXBlend::INV_SRC_ALPHA
		}

	};

	struct GFXBlendDesc //D3D12_BLEND_DESC
	{
		bool						mAlphaToCoverageEnable = false;
		//Set to true to enable independent blending. If set to false, only the RenderTarget[0] members are used; RenderTarget[1..7] are ignored.
		bool						mIndependentBlendEnable = false;
		GFXRenderTargetBlendDesc	mRenderTarget[8];
	};


	struct GFXRasterizerDesc //D3D12_RASTERIZER_DESC
	{
		//whether wireframe or solid
		bool					mWireframe = false;
		//specifies that triangles facing the specified direction are not drawn. 
		EGFXCullMode			mCullMode = EGFXCullMode::Back;
		//Determines if a triangle is front- or back-facing. If this member is true, a triangle will be considered front-facing if
		//its vertices are counter-clockwise on the render target and considered back-facing if they are clockwise. 
		bool					mFrontCounterClockwise = false;
		//Depth value added to a given pixel. 
		int						mDepthBias = 0;
		//Maximum depth bias of a pixel.
		float					mDepthBiasClamp = 0;
		//Scalar on a given pixel's slope. 
		float					mSlopeScaledDepthBias = 0;
		//Specifies whether to enable clipping based on distance.
		//The hardware always performs x and y clipping of rasterized coordinates.When true, the hardware also clips the z value(that is, the hardware performs the last step of the following algorithm).
		//0 < w
		//-w <= x <= w(or arbitrarily wider range if implementation uses a guard band to reduce clipping burden)
		//- w <= y <= w(or arbitrarily wider range if implementation uses a guard band to reduce clipping burden)
		//0 <= z <= w
		//if false, the hardware skips the z clipping(that is, the last step in the preceding algorithm).However, the hardware still performs the "0 < w" clipping
		//When z clipping is disabled
		bool					mDepthClipEnable = true;
		//Specifies whether to use the quadrilateral or alpha line anti-aliasing algorithm on multisample antialiasing (MSAA) render targets.
		//Set to TRUE to use the quadrilateral line anti-aliasing algorithm and to FALSE to use the alpha line anti-aliasing algorithm.
		bool					mMultisampleEnable = false;
		//Specifies whether to enable line antialiasing; only applies if doing line drawing and MultisampleEnable is FALSE
		bool					mAntialiasedLineEnable = false;
		//The sample count that is forced while UAV rendering or rasterizing. Valid values are 0, 1, 2, 4, 8, and optionally 16. 0 indicates that the sample count is not forced. 
		unsigned				mForcedSampleCount = 0;
		//whether conservative rasterization is on or off. 
		bool					mConservativeRaster = false;
	};


	struct GFXDepthStencilOpDesc //D3D12_DEPTH_STENCILOP_DESC
	{
		EGFXStencilOp	mStencilFailOp = EGFXStencilOp::KEEP;
		EGFXStencilOp	mStencilDepthFailOp = EGFXStencilOp::KEEP;
		EGFXStencilOp	mStencilPassOp = EGFXStencilOp::KEEP;
		EGFXComparison	mStencilFunc = EGFXComparison::ALWAYS;
	};


	struct GFXDepthStencilDesc //D3D12_DEPTH_STENCIL_DESC
	{
		bool					mDepthEnable = true;
		bool					mDepthWriteAll;
		EGFXComparison			mDepthFunc = EGFXComparison::LESS;
		bool					mStencilEnable = false;
		uint8					mStencilReadMask = 0xFF;
		uint8					mStencilWriteMask = 0xFF;
		//describes how to use the results of the depth test and the stencil test for pixels whose surface normal is facing towards the camera.
		GFXDepthStencilOpDesc	mFrontFace;
		//describes how to use the results of the depth test and the stencil test for pixels whose surface normal is facing away from the camera. 
		GFXDepthStencilOpDesc	mBackFace;
	};

	struct GFXInputElementDesc //D3D12_INPUT_ELEMENT_DESC
	{
		const char*			mSemanticName = nullptr;
		unsigned			mSemanticIndex = 0;
		EGFXFormat			mFormat = EGFXFormat::UNKNOWN;
		unsigned			mInputSlot = 0;
		unsigned			mAlignedByteOffset = 0xFFffFFff;
		//whether per vertex or per instance
		bool				mPerVertex = true;
		//The number of instances to draw using the same per-instance data before advancing in the buffer by one element. This value must be 0 for per vertex element
		unsigned			mInstanceDataStepRate = 0;
	};


	struct GFXInputLayout
	{
		static const unsigned MAX_ELEMENT = 32;

		TArray<GFXInputElementDesc>	mElements;
		
		GFXInputElementDesc& Get(unsigned index) { return mElements[index]; }
		unsigned NumInput() const { return mElements.Length(); }
	};

	struct GFXSampleDesc
	{
		unsigned mCount = 1;
		unsigned Quality = 0;
	};


	struct UGFXCORE_API GFXGraphicsPiplineStateDesc //D3D12_GRAPHICS_PIPELINE_STATE_DESC
	{
		
		GFXBlendDesc								mBlendState;
		//The sample mask for the blend state. 
		unsigned									mSampleMask = 0xFFffFFff;
		GFXRasterizerDesc							mRasterizer;
		GFXDepthStencilDesc							mDepthStencil;
		GFXInputLayout*								mInputLayout;
		GFXSampleDesc								mSampleDesc;
		EGFXFormat									mDSVFormat = EGFXFormat::UNKNOWN;
		EPrimitiveTopologyType						mPrimitiveTopologyType = EPrimitiveTopologyType::UNDEFINED;
		unsigned									mNumRenderTargets = 0;
		EGFXFormat									mRTVFormats[8];

		GFXRootSignature*							mRootSignature = nullptr;
		GFXShader*									mVS = nullptr;
		GFXShader*									mPS = nullptr;
		GFXShader*									mDS = nullptr;
		GFXShader*									mHS = nullptr;
		GFXShader*									mGS = nullptr;
		
		unsigned									mNodeMask = 0;

		using HashT = uint32;

		bool HasSuchShader(const GFXShader* pShader) const
		{
			return (pShader == mVS || pShader == mPS || pShader == mDS || pShader == mHS || pShader == mGS);
		}

		HashT GetUniqueHash() const;
	};

	struct GFXComputePiplineStateDesc
	{
		GFXRootSignature* mRootSignature = nullptr;
		GFXShader* mCS = nullptr; 
		unsigned mNodeMask = 0;
	};

	struct GFXViewport
	{
		float mTopLeftX = 0;
		float mTopLeftY = 0;
		float mWidth = 0;
		float mHeight = 0;
		float mMinDepth = 0;
		float mMaxDepth = 1;
	};


	
	

	template< typename T> using TGFXRef = TSPtr<T>;


	struct GFXResource : public ISmartBase
	{
		void* mNativeHandle;
		
		GFXResource(void* nativeHandle = nullptr) : mNativeHandle(nativeHandle) {}
		virtual ~GFXResource() {}
		void* GetNativeHandle() const { return mNativeHandle; }
	};
	struct GFXBuffer : GFXResource
	{
	};
	
	struct GFXPiplineState : GFXResource
	{
	};
	struct GFXComputePiplineState : GFXPiplineState
	{
	};
	struct GFXGraphicPiplineState : GFXPiplineState
	{
	};

	enum ERootSignatureFlags //D3D12_ROOT_SIGNATURE_FLAGS
	{
		ERSF_NONE = 0,

		ERSF_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT = 0x1,

		ERSF_DENY_VERTEX_SHADER_ACCESS = 0x2,
		ERSF_DENY_HULL_SHADER_ACCESS = 0x4,
		ERSF_DENY_DOMAIN_SHADER_ACCESS = 0x8,
		ERSF_DENY_GEOMETRY_SHADER_ACCESS = 0x10,
		ERSF_DENY_PIXEL_SHADER_ACCESS = 0x20,

		ERSF_ALLOW_STREAM_OUTPUT = 0x40,

		ERSF_DEFAULT = ERSF_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT | ERSF_ALLOW_STREAM_OUTPUT
	};

	enum ERootParameterType //D3D12_ROOT_PARAMETER_TYPE
	{
		ERPT_DESCRIPTOR_TABLE = 0,
		ERPT_32BIT_CONSTANTS ,
		ERPT_CBV,	//constant buffer view
		ERPT_SRV,	//shader resource view
		ERPT_UAV	//unordered access view
	};
	enum EDescriptorRange //D3D12_DESCRIPTOR_RANGE_TYPE
	{
		EDR_SRV = 0,
		EDR_UAV,
		EDR_CBV,

		EDR_SAMPLER,
	};

	struct GFXRootParameter
	{
		ERootParameterType mType;
		union 
		{
			struct  
			{
				unsigned mShaderRegister;
				unsigned mNum32BitValues;

			} mConstants;
			struct
			{
				unsigned mShaderRegister;

			} mDescriptor;
			struct
			{
				struct 
				{
					EDescriptorRange mType;
					unsigned mCount; //UINT_MAX for unbounded size
					unsigned mBaseShaderRegister;

				} mRanges[3];

				unsigned mNumRange;
				
			} mDescriptorTable;
		};

		static GFXRootParameter CreateConstans(unsigned num32BitValue, unsigned shaderRegister)
		{
			GFXRootParameter ret;
			ret.mType = ERootParameterType::ERPT_32BIT_CONSTANTS;
			ret.mConstants.mNum32BitValues = num32BitValue;
			ret.mConstants.mShaderRegister = shaderRegister;
			return ret;
		}
		static GFXRootParameter CreateSRV(unsigned shaderRegister)
		{
			GFXRootParameter ret;
			ret.mType = ERootParameterType::ERPT_SRV;
			ret.mDescriptor.mShaderRegister = shaderRegister;
			return ret;
		}
		static GFXRootParameter CreateCBV(unsigned shaderRegister)
		{
			GFXRootParameter ret;
			ret.mType = ERootParameterType::ERPT_CBV;
			ret.mDescriptor.mShaderRegister = shaderRegister;
			return ret;
		}
		static GFXRootParameter CreateUAV(unsigned shaderRegister)
		{
			GFXRootParameter ret;
			ret.mType = ERootParameterType::ERPT_UAV;
			ret.mDescriptor.mShaderRegister = shaderRegister;
			return ret;
		}
		static GFXRootParameter CreateDescriptorTableSampler(unsigned count, unsigned baseShaderRegister = 0)
		{
			return CreateDescriptorTable(EDR_SAMPLER, count, baseShaderRegister);
		}
		//@count	the number of descriptors, use UINT_MAX to specify unbounded size.
		static GFXRootParameter CreateDescriptorTable(EDescriptorRange rangeType, unsigned count, unsigned baseShaderRegister = 0)
		{
			GFXRootParameter ret;
			ret.mType = ERPT_DESCRIPTOR_TABLE;
			ret.mDescriptorTable.mRanges[0].mType = rangeType;
			ret.mDescriptorTable.mRanges[0].mCount = count;
			ret.mDescriptorTable.mRanges[0].mBaseShaderRegister = baseShaderRegister;
			ret.mDescriptorTable.mNumRange = 1;
			return ret;
		}
		//create descriptor table with 3 ranges, a descriptor table can be either CSU or Sampler
		//@Count the number of descriptors, use UINT_MAX to specify unbounded size, Only the last entry can have unbounded size.
		static GFXRootParameter CreateDescriptorTableRanges(
			EDescriptorRange rangeType0, unsigned count0, unsigned baseRegister0,
			EDescriptorRange rangeType1, unsigned count1, unsigned baseRegister1, 
			EDescriptorRange rangeType2, unsigned count2, unsigned baseRegister2)
		{
			GFXRootParameter ret;
			ret.mType = ERPT_DESCRIPTOR_TABLE;
			ret.mDescriptorTable.mRanges[0] = { rangeType0, count0, baseRegister0 };
			ret.mDescriptorTable.mRanges[1] = { rangeType1, count1, baseRegister1 };
			ret.mDescriptorTable.mRanges[2] = { rangeType2, count2, baseRegister2 };
			ret.mDescriptorTable.mNumRange = 3;
			return ret;
		}
		static GFXRootParameter CreateDescriptorTableRanges(
			EDescriptorRange rangeType0, unsigned count0, unsigned baseRegister0,
			EDescriptorRange rangeType1, unsigned count1, unsigned baseRegister1)
		{
			GFXRootParameter ret;
			ret.mType = ERPT_DESCRIPTOR_TABLE;
			ret.mDescriptorTable.mRanges[0] = { rangeType0, count0, baseRegister0 };
			ret.mDescriptorTable.mRanges[1] = { rangeType1, count1, baseRegister1 };
			ret.mDescriptorTable.mNumRange = 2;
			return ret;
		}
	};

	static const unsigned MAX_ROOT_PARAMETER = 64;

	struct GFXRootSignatureDesc
	{
		TArray<GFXRootParameter, TArrayAllocStack<GFXRootParameter, MAX_ROOT_PARAMETER>> mParams;
		ERootSignatureFlags mFlags = ERootSignatureFlags::ERSF_DEFAULT;

		GFXRootSignatureDesc& operator << (const GFXRootParameter& param)
		{
			mParams.Add(param);
			return *this;
		}
	};

	struct GFXRootSignature : GFXResource
	{

	};

	struct GFXTexture : GFXResource
	{

	};
	struct GFXShaderResourceView : GFXResource
	{

	};
	struct GFXDepthStencilView : GFXResource
	{

	};

	static constexpr unsigned SHADER_MAX_MACRO = 256;

	//////////////////////////////////////////////////////////////////////////
	struct ShaderMacro
	{
		Name	mName;
		Name	mValue;

		ShaderMacro(Name name, Name value)
			: mName(name), mValue(value) {}
	};

	using ShaderMacrosHashT = uint32;

	//////////////////////////////////////////////////////////////////////////
	class UGFXCORE_API ShaderMacros
	{
	public:
		size_t Num() const { return mMacros.Length(); }
		const ShaderMacro& Get(size_t index) const { return mMacros[index]; }
		ShaderMacros& Add(ShaderMacro newMacro);
		ShaderMacrosHashT GetHash() const;

	private:
		TArray<ShaderMacro>		mMacros;
	};

	UGFXCORE_API StringStreamOut& operator << (StringStreamOut&, const ShaderMacros&);

	//////////////////////////////////////////////////////////////////////////
	struct ShaderEntry
	{
		Name		mFilename;
		Name		mEntryPoint;
		EShader		mType;

	};

	//////////////////////////////////////////////////////////////////////////
	struct GFXShader : public GFXResource
	{
		ShaderEntry*	mEntry = nullptr;
		ShaderMacros	mMacros;	//macros this shader was compiled with


		EShader GetType() const { return mEntry->mType; }
		Name GetFilename() const { return mEntry->mFilename; }
		Name GetEntryPoint() const { return mEntry->mEntryPoint; }
	};


	//////////////////////////////////////////////////////////////////////////
	struct GFXVertexShader : public GFXShader
	{
		static const EShader EnumType = EShader::Vertex;
	};
	//////////////////////////////////////////////////////////////////////////
	struct GFXHullShader : public GFXShader
	{
		static const EShader EnumType = EShader::Hull;
	};
	//////////////////////////////////////////////////////////////////////////
	struct GFXDomainShader : public GFXShader
	{
		static const EShader EnumType = EShader::Domain;
	};
	//////////////////////////////////////////////////////////////////////////
	struct GFXGeometryShader : public GFXShader
	{
		static const EShader EnumType = EShader::Geometry;
	};
	//////////////////////////////////////////////////////////////////////////
	struct GFXPixelShader : public GFXShader
	{
		static const EShader EnumType = EShader::Pixel;
	};
	//////////////////////////////////////////////////////////////////////////
	struct GFXComputeShader : public GFXShader
	{
		static const EShader EnumType = EShader::Compute;
	};



	//////////////////////////////////////////////////////////////////////////
	template<EShader type> struct TShaderClassFromEnum {  };

	template<> struct TShaderClassFromEnum<EShader::Vertex> : public GFXVertexShader { typedef GFXVertexShader T; };
	template<> struct TShaderClassFromEnum<EShader::Hull> : public GFXHullShader { typedef GFXHullShader T; };
	template<> struct TShaderClassFromEnum<EShader::Domain> : public GFXDomainShader { typedef GFXDomainShader T; };
	template<> struct TShaderClassFromEnum<EShader::Geometry> : public GFXGeometryShader { typedef GFXGeometryShader T; };
	template<> struct TShaderClassFromEnum<EShader::Pixel> : public GFXPixelShader { typedef GFXPixelShader T; };
	template<> struct TShaderClassFromEnum<EShader::Compute> : public GFXComputeShader { typedef GFXComputeShader T; };


	
	
	//////////////////////////////////////////////////////////////////////////
	class IShaderMgr
	{
	public:
		//returns a shader with specified parameters, if not compiled yet compile and return it
		virtual GFXShader* GetShader(Name filename, Name entrypoint, EShader shader, const ShaderMacros& macros = ShaderMacros()) = 0;

		//recompiles the specified shader
		virtual void RecompileShader(GFXShader* shader) = 0;
		//recompile all the shaders with the specified file
		virtual void RecompileShader(Name filename) = 0;

		virtual void ReleaseShder(GFXShader*) = 0;
	};

	//////////////////////////////////////////////////////////////////////////
	class IPipelineStateMgr
	{
	public:
		virtual GFXGraphicPiplineState* GetGPS(const GFXGraphicsPiplineStateDesc& desc) = 0;
		virtual GFXComputePiplineState* GetCPS(const GFXComputePiplineStateDesc& desc) = 0;
	};

	//////////////////////////////////////////////////////////////////////////
	class ITextureMgr
	{
	public:
		virtual GFXTexture* LoadTextureDefault(TSPtr<SmartMemBlock> textureData, bool bSRGB) = 0;
	};
	
	//////////////////////////////////////////////////////////////////////////
	class IDescriptorMgr
	{
	public:
		virtual ~IDescriptorMgr() {}
	};

	//////////////////////////////////////////////////////////////////////////
	class UGFXCORE_API GFXContextBase
	{
		UCLASS(GFXContextBase)

		virtual void Init() {}
		virtual void Release() {}
		virtual EGFXAPI GetAPI() const { return EGFXAPI::Null; }
		virtual const char* GetAPIName() const { return "null"; }
		virtual IShaderMgr* GetShaderMgr() { return nullptr; }
		virtual IPipelineStateMgr* GetPipelineStateMgr() { return nullptr; }
		virtual ITextureMgr* GetTextureMgr() { return nullptr; }
		virtual IDescriptorMgr* GetDescriptorMgr() { return nullptr; }

		virtual GFXRootSignature* CreateRootSignature(const GFXRootSignatureDesc&) { return nullptr; }
		
		virtual GFXBuffer* CreateStaticVB(size_t size, const void* initialData, const char* debugStr = nullptr) { return nullptr; }
		virtual GFXBuffer* CreateStaticIB(size_t size, const void* initialData, const char* debugStr = nullptr) { return nullptr; }
	};

	//global function that return the instance of GFXContext
	UGFXCORE_API GFXContextBase* GetGFXContext();
	UGFXCORE_API void SetGFXContext(GFXContextBase*);


	struct SceneRenderParam
	{

	};

	//////////////////////////////////////////////////////////////////////////
	class UGFXCORE_API GFXSceneRenderer
	{
		UCLASS(GFXSceneRenderer)

		virtual void Render(const SceneRenderParam& param) {}
	};
};