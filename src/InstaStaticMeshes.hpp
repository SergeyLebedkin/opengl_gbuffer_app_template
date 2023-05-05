#pragma once

// simple quad positions
static const float cInstaQuadBuffer_Positions[] =
{
	-1.0f, -1.0f, +0.0f,
	+1.0f, -1.0f, +0.0f,
	-1.0f, +1.0f, +0.0f,
	+1.0f, +1.0f, +0.0f,
};

// simple quad normals
static const float cInstaQuadBuffer_Normals[] =
{
	+0.0f, +0.0f, +1.0f,
	+0.0f, +0.0f, +1.0f,
	+0.0f, +0.0f, +1.0f,
	+0.0f, +0.0f, +1.0f,
};

// simple quad tex coords
static const float cInstaQuadBuffer_TexCoords[] =
{
	+0.0f, +0.0f,
	+1.0f, +0.0f,
	+0.0f, +1.0f,
	+1.0f, +1.0f,
};

// static meshes functions
void StaticMeshesInit();
void StaticMeshesShutDown();