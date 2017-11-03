#include "Vector.h"
#include "ByteSerializer.h"
#include "Meta.h"


namespace UCore
{
		UCLASS_BEGIN_IMPL(Vec2)
		UCLASS_END_IMPL(Vec2)

		UCLASS_BEGIN_IMPL(Vec3)
		UCLASS_END_IMPL(Vec3)

		UCLASS_BEGIN_IMPL(Vec4)
		UCLASS_END_IMPL(Vec4)

		UCLASS_BEGIN_IMPL(Color32)
		UCLASS_END_IMPL(Color32)

		UCLASS_BEGIN_IMPL(Color)
		UCLASS_END_IMPL(Color)
};


namespace UCore
{


	//////////////////////////////////////////////////////////////////////////
	UCORE_API const Color32 Color32::BLACK(0, 0, 0, 255);

	Color32::Color32(const Color& color)
	{
		mRGBA[0] = (uint8)(color.r * 255.0f);
		mRGBA[1] = (uint8)(color.g * 255.0f);
		mRGBA[2] = (uint8)(color.b * 255.0f);
		mRGBA[3] = (uint8)(color.a * 255.0f);
	}

	const Color32 Color32::WHITE(255, 255, 255, 255);
	const Color32 Color32::RED(255, 0, 0, 255);
	const Color32 Color32::GREEN(0, 255, 0, 255);
	const Color32 Color32::BLUE(0, 0, 255, 255);
	const Color32 Color32::YELLOW(255, 255, 0, 255);

	const Vec2 Vec2::ZERO(0);
	const Vec2 Vec2::ONE(1);


	void Vec2::MetaSerialize(ByteSerializer& ser)
	{
		ser.Bytes(this, sizeof(float[2]));
	}
	void Vec2::MetaDeserialize(ByteDeserializer& ser)
	{
		ser.Bytes(this, sizeof(float[2]));
	}


	const Vec3 Vec3::ZERO(0.0f);
	const Vec3 Vec3::ONE(1.0f);


	void Vec3::MetaSerialize(ByteSerializer& ser)
	{
		ser.Bytes(this, sizeof(float[3]));
	}
	void Vec3::MetaDeserialize(ByteDeserializer& ser)
	{
		ser.Bytes(this, sizeof(float[3]));
	}



	const Vec4 Vec4::Zero(0);
	const Vec4 Vec4::One(1);

	void Vec4::MetaSerialize(ByteSerializer& ser)
	{
		ser.Bytes(this, sizeof(float[4]));
	}
	void Vec4::MetaDeserialize(ByteDeserializer& ser)
	{
		ser.Bytes(this, sizeof(float[4]));
	}


	void Color::MetaSerialize(ByteSerializer& ser)
	{
		ser.Bytes(this, sizeof(float[4]));
	}
	void Color::MetaDeserialize(ByteDeserializer& ser)
	{
		ser.Bytes(this, sizeof(float[4]));
	}



	const Color Color::WHITE(1,1,1,1);
	const Color Color::RED(1,0,0,1);
	const Color Color::GREEN(0,1,0,1);
	const Color Color::BLUE(0,0,1,1);
	const Color Color::YELLOW(1,1,0,1);
	const Color Color::BLACK(0,0,0,1);
	const Color Color::PURPLE(160.0f / 255.0f,  32 / 255.0f, 240 / 255.0f, 1);
	const Color Color::PINK(1, 20 / 255.0f, 147 / 255.0f, 1);




	void Color32::MetaSerialize(ByteSerializer& ser)
	{
		ser << mColor;
	}
	void Color32::MetaDeserialize(ByteDeserializer& ser)
	{
		ser >> mColor;
	}


	
	StringStreamOut& operator<<(StringStreamOut& stream, const Vec2& v)
	{
		stream << "{" << v.x << ", " << v.y << "}";
		return stream;
	}
	StringStreamOut& operator<<(StringStreamOut& stream, const Vec3& v)
	{
		stream << "{" << v.x << ", " << v.y << ", " << v.z << "}";
		return stream;
	}
	StringStreamOut& operator<<(StringStreamOut& stream, const Vec4& v)
	{
		stream << "{" << v.x << ", " << v.y << ", " << v.z << ", " << v.w << "}";
		return stream;
	}
	StringStreamOut& operator<<(StringStreamOut& stream, const Color& c)
	{
		stream << "{" << c.r << ", " << c.g << ", " << c.b << ", " << c.a << "}";
		return stream;
	}
	StringStreamOut& operator<<(StringStreamOut& stream, Color32 c)
	{
		stream << "{" << c.mRGBA[0] << ", " << c.mRGBA[1] << ", " << c.mRGBA[2] << ", " << c.mRGBA[3] << "}";
		return stream;
	}
	StringStreamOut& operator<<(StringStreamOut& stream, Vec2I v)
	{
		stream << "{" << v.x << ", " << v.y << "}";
		return stream;
	}
	


};