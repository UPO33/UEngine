#include "Config.h"
#include "Meta.h"

namespace UCore
{
	
	struct TestConfigClass
	{
		UCLASS(TestConfigClass)


		bool			mFullScreen = false;
		unsigned		mWindowSizeX = 800;
		unsigned		mWindowSizeY = 600;
		Name			mRendererClass = "RendererDefferdDX12";
		Name			mStartupWorld = "Scenes\Startup";

		void PrintDBG()
		{
#define DBG(M) #M, M
			ULOG_WARN("% %", DBG(mFullScreen));
			ULOG_WARN("% %  % %", DBG(mWindowSizeX), DBG(mWindowSizeY));
			ULOG_WARN("% %", DBG(mRendererClass));
#undef DBG
		}
	};


	UCLASS_BEGIN_IMPL(TestConfigClass)
		UPROPERTY(mFullScreen)
		UPROPERTY(mWindowSizeX)
		UPROPERTY(mWindowSizeY)
		UPROPERTY(mRendererClass)
	UCLASS_END_IMPL(TestConfigClass)
};

namespace UCore
{
#if 0
	void UConfigWriteProperty(const PropertyInfo* pProperty, size_t arrayElementIndex, void* mappedValue, ConfNode& node)
	{
		EFieldType type = pProperty->GetSelf().GetType();
		const char* propertyName = pProperty->GetName().CStr();
		const FieldInfo* typePtr = pProperty->GetSelf().GetPtr();

		if (arrayElementIndex != INVALID_INDEX)
		{
			type = pProperty->GetArg0().GetType();
			typePtr = pProperty->GetArg0().GetPtr();
		}


		switch (type)
		{
		case EFieldType::EPT_bool:
			node.Add(propertyName, *((bool*)mappedValue));
			break;
		case EFieldType::EPT_int8:
			node.Add(propertyName, *((int8*)mappedValue));
			break;
		case EFieldType::EPT_uint8:
			node.Add(propertyName, *((uint8*)mappedValue));
			break;
		case EFieldType::EPT_int16:
			node.Add(propertyName, *((int16*)mappedValue));
			break;
		case EFieldType::EPT_uint16:
			node.Add(propertyName, *((uint16*)mappedValue));
			break;
		case EFieldType::EPT_int32:
			node.Add(propertyName, *((int32*)mappedValue));
			break;
		case EFieldType::EPT_uint32:
			node.Add(propertyName, *((uint32*)mappedValue));
			break;
		case EFieldType::EPT_int64:
			node.Add(propertyName, *((int64*)mappedValue));
			break;
		case EFieldType::EPT_uint64:
			node.Add(propertyName, *((uint64*)mappedValue));
			break;
		case EFieldType::EPT_float:
			node.Add(propertyName, *((float*)mappedValue));
			break;
		case EFieldType::EPT_double:
			node.Add(propertyName, *((double*)mappedValue));
			break;
		case EFieldType::EPT_enum:
			//#TODO should we write element's name or value?
			break;

		case EFieldType::EPT_TStackArray:
		case EFieldType::EPT_TArray:
		{
			// 			auto elementType = pProperty->GetArg0().GetType();
			// 			if(elementType == EFieldType::EPT_ObjectPoniter || elementType == EFieldType::EPT_TObjectPtr)
			// 			{ 
			// 				ULOG_ERROR("ignoring [%.%] Object* or TObjectPtr is not allowed", pProperty->GetOwner()->GetName(), pProperty->GetName());
			// 			}
			// 			else
			// 			{
			// 				node[propertyName] = JSONDoc::object();
			// 				MetaArray metaArray = MetaArray(mappedValue, type ==  EFieldType::EPT_TArray, pProperty->GetArg0());
			// 				for (size_t iElement = 0; iElement < metaArray.Length(); iElement++)
			// 				{
			// 					JSONWriteProperty(pProperty, iElement, metaArray.GetElementAt(iElement), node[propertyName]);
			// 				}
			// 			}
		}
		break;
		case EFieldType::EPT_ObjectPoniter:
		case EFieldType::EPT_TObjectPtr:
			ULOG_ERROR("ignoring [%.%] Object* or TObjectPtr is not allowed", pProperty->GetOwner()->GetName(), pProperty->GetName());
			break;
		case EFieldType::EPT_TSubClass:
		{
			// 			if (const ClassInfo* theClass = ((TSubClass<Object>&)mappedValue).Get())
			// 			{
			// 				auto str = theClass->GetName().CStr();
			// 				outJSON[propertyName] = str ? std::string(str) : std::string();
			// 			}
			// 			else
			// 				outJSON[propertyName] = "";
			break;
		}

		case EFieldType::EPT_Class:
			// 			if (const ClassInfo* metaClass = Cast<ClassInfo>(typePtr))
			// 			{
			// 				//we first check if class is special such as Ver2, Vec3 , ..
			// 
			// 				if (metaClass == Vec2::GetClassStatic)	//Vec2
			// 				{
			// 					const Vec2& v = Ptr2Ref<const Vec2>(mappedValue); //*((Vec2*)mappedValue);
			// 					node[propertyName] = json::array({ v.x, v.y });
			// 				}
			// 				else if (metaClass == Vec3::GetClassStatic)	//Vec3
			// 				{
			// 					const Vec3& v = *((Vec3*)mappedValue);
			// 					node[propertyName] = json::array({ v.x, v.y, v.z });
			// 				}
			// 				else if (metaClass == Vec4::GetClassStatic)	//Vec4
			// 				{
			// 					const Vec4& v = *((Vec4*)mappedValue);
			// 					node[propertyName] = json::array({ v.x, v.y, v.z, v.w });
			// 				}
			// 				else if (metaClass == Color::GetClassStatic)	//color
			// 				{
			// 					const Color& color = *((Color*)mappedValue);
			// 					node[propertyName] = json::array({ color.r, color.g, color.b, color.a });
			// 				}
			// 				else if (metaClass == Color32::GetClassStatic)	//color32
			// 				{
			// 					//#Note color32 is stored as 4 floats just like UCore::Color
			// 					const Color color = Color(*((Color32*)mappedValue));
			// 					node[propertyName] = json::array({ color.r, color.g, color.b, color.a });
			// 				}
			// 				else if (metaClass == Name::GetClassStatic)	//Name
			// 				{
			// 					auto str = ((Name*)mappedValue)->CStr();
			// 					node[propertyName] = str ? std::string(str) : std::string();
			// 				}
			// 				else if (metaClass == String::GetClassStatic)	//String
			// 				{
			// 					auto str = ((String*)mappedValue)->CStr();
			// 					node[propertyName] = str ? std::string(str) : std::string();
			// 				}
			// 				else //is not a special class, so parse normally
			// 				{
			// 					node[propertyName] = json::object();
			// 					JSONWriteClass(metaClass, mappedValue, node[propertyName]);
			// 				}
			// 			}
			break;
		default:
			break;
		}
	}

	UCORE void ConfTest()
	{
		ConfNode root;
		root.Add("string", "this is string");
		ConfNode& size = root.Add("size");
		{
			size.Add(nullptr, int(800));
			size.Add(nullptr, int(600));
		}
		ConfNode& position = root.Add("Position");
		{
			position.Add(nullptr, float(0.1));
			position.Add(nullptr, float(0.5));
			position.Add(nullptr, float(-123.12345000));
		}
		// 		root.Add("Positions").Add(nullptr, 
		// 		{
		// 			ConfNode({ ConfNode(int(0)), ConfNode(int(0)) }),
		// 			ConfNode({ ConfNode(int(1)), ConfNode(int(1)) }),
		// 			ConfNode({ ConfNode(int(2)), ConfNode(int(2)) })
		// 		});

		// 		ConfNode root = ConfNode
		// 		(
		// 			nullptr, { ConfNode("bool1", true), ConfNode("bool2", false),
		// 				ConfNode("Position",{ ConfNode(nullptr, int(32)), ConfNode(nullptr, int(64)) }) }
		// 		);
		// 		ConfNode root;
		// 		root.Add(nullptr, bool(false));
		// 		root.Add(nullptr, bool(true));
		// 		root.Add(nullptr, int(456));

		// 		ConfNode n0(bool(false));
		// 		ConfNode n1 = n0;

		std::string str;
		root.ToString(str, 0);
		ULOG_SUCCESS("\n\n%", str.c_str());

		/*
		{
			String = "this is string",
			Size = {100 , 200},
			Position = {0.8, 0.5, 0.3},
			Positions = {
				{
					0,0,
				},
				{
					1,1,
				},
				{
					2,2,
				}
			}
		}
		*/
	}

	UCORE void UConfigClassWrite(const ClassInfo* pClass, const void* pInstance, ConfNode& root)
	{
		UASSERT(pClass && pInstance);

		for (const ClassInfo* classIter : pClass->GetClassChain())
		{
			for (const PropertyInfo* propertyIter : classIter->GetProperties())
			{
				UConfigWriteProperty(propertyIter, INVALID_INDEX, (void*)propertyIter->Map(pInstance), root);
			}
		}
	}
	void UGetPropertyValueAsString_Arithmetic(const EFieldType type, void* mappedValue, String& value)
	{
		value = nullptr;

		switch (type)
		{
		case EFieldType::EPT_bool:
			value = MapAs<bool>(mappedValue) ? "true" : "false";
			break;


		case EFieldType::EPT_int8:
			value = String::MakeFrom(MapAs<int8>(mappedValue));
			break;
		case EFieldType::EPT_uint8:
			value = String::MakeFrom(MapAs<uint8>(mappedValue));
			break;
		case EFieldType::EPT_int16:
			value = String::MakeFrom(MapAs<int16>(mappedValue));
			break;
		case EFieldType::EPT_uint16:
			value = String::MakeFrom(MapAs<uint16>(mappedValue));
			break;
		case EFieldType::EPT_int32:
			value = String::MakeFrom(MapAs<int32>(mappedValue));
			break;
		case EFieldType::EPT_uint32:
			value = String::MakeFrom(MapAs<uint32>(mappedValue));
			break;
		case EFieldType::EPT_int64:
			value = String::MakeFrom(MapAs<int64>(mappedValue));
			break;
		case EFieldType::EPT_uint64:
			value = String::MakeFrom(MapAs<uint64>(mappedValue));
			break;
		}
	}
	UCORE void UConfigClassWrite(const ClassInfo* pClass, const void* pInstance, String& outConfigString)
	{
		UASSERT(pClass && pInstance);

		ConfNode root;
		std::string str;
		UConfigClassWrite(pClass, pInstance, root);
		root.ToString(str, 2);
		outConfigString = str.c_str();
	}
	template<typename T> T& MapAs(void* ptr)
	{
		return *((T*)ptr);
	}
#endif

	//////////////////////////////////////////////////////////////////////////
	UCORE_API void UConfigClassWrite(const ClassInfo* pClass, const void* pInstance, String& outConfigString)
	{
		//#TODO
	}
	//////////////////////////////////////////////////////////////////////////
	UCORE_API void UConfigClassRead(const ClassInfo* pClass, void* pInstance, const String& inConfigString)
	{
		//#TODO
	}
};


