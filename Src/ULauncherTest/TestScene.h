#pragma once

#include "Base.h"
#include "GameInput.h"
#include "StaticMesh.h"
#include "../Engine/StaticMesh.h"


using namespace UEngine;

struct TestCamera
{
	Vec3 mPosition = Vec3(0, 0, -0);
	Vec3 mRotationEuler = Vec3(0, 0, 0);
	float mFOVDegree = 60;
	float mZNear = 0.01;
	float mZFar = 100;
	float mCameraRotationSpeed = 2;

	Matrix4 mMatrixProjection = Matrix4::IDENTITY;
	Matrix4 mMatrixView = Matrix4::IDENTITY;
	Matrix4 mMatrixWorldToClip = Matrix4::IDENTITY;
	
	void Update(float delta);
};

struct TestGrid
{
	void Render();
};
struct TestEntityMesh
{
	TestStaticMesh*	mMesh = nullptr;

	Matrix4 mTransform;

	TestEntityMesh()
	{
		mTransform = Matrix4Translation(Vec3(0, 0, 0));
	}
};

struct CanvasImpl
{
	Matrix4 mProjectionMatrix;

	struct LineVertex
	{
		Vec2 position;
		Color32 color;
	};


	void AddLine(Vec2 a, Vec2 b, Color32 color) {}

};


struct TestScene
{
	struct CBPerObject
	{
		Matrix4 mLocalToWorld;
	};
	struct CBPerCamera
	{
		Matrix4 mWorldToClip;
	};

	TestCamera mCamera;
	AStaticMesh* mMonkeyMesh = nullptr;
	
	ID3D12Resource* mRenderTargetTexture = nullptr;
	DescHandleRTV mRTV; //view of the render target
	DescHandleCSU mSRV;	//srv of the render target

	GFXPiplineState* mScreenQudsPS = nullptr;
	ID3D12Resource* mScreenQuadsVB = nullptr;
	GFXInputLayout* mScreenQuadsIL = nullptr;

	TestScene();

	struct SQVertex
	{
		float x, y, u, v;
		unsigned index;
	};

	static void GenScreenQuadVertices(float x, float y, float w, float h, unsigned index, TArray<SQVertex>& out)
	{
		SQVertex tl = { x, y, 0, 0 };
		SQVertex tr = { x + w, y, 0, 1 };
		SQVertex br = { x + w, y + h, 1, 1 };
		SQVertex bl = { x, y + h, 0, 1 };

		out.Add(tr); out.Add(br); out.Add(bl);
		out.Add(tl); out.Add(tr); out.Add(bl);
	}
	void CrearScreenQuadResources()
	{
		////////////IL
		mScreenQuadsIL = new GFXInputLayout;
		mScreenQuadsIL->mElements.AddDefault(3);

		mScreenQuadsIL->mElements[0].mFormat = EGFXFormat::R32G32_FLOAT;
		mScreenQuadsIL->mElements[0].mSemanticName = "POSITION";
		mScreenQuadsIL->mElements[0].mAlignedByteOffset = 0;


		mScreenQuadsIL->mElements[1].mFormat = EGFXFormat::R32G32_FLOAT;
		mScreenQuadsIL->mElements[1].mSemanticName = "UV";
		mScreenQuadsIL->mElements[1].mAlignedByteOffset = sizeof(float[2]);

		mScreenQuadsIL->mElements[2].mFormat = EGFXFormat::R32_UINT;
		mScreenQuadsIL->mElements[2].mSemanticName = "INDEX";
		mScreenQuadsIL->mElements[2].mAlignedByteOffset = sizeof(float[4]);

		///////////////
		TArray<SQVertex> verts;
		float offset = 0.04;
		float w = 0.2;
		float h = 0.2;
		GenScreenQuadVertices(offset, offset, w, h, 0, verts);
		GenScreenQuadVertices(offset, offset + h, w, h, 1, verts);
	}
	void Update(float delta);
	void Render(CmdList* cmdList);
};