// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Renderer.h"
#include "Texture.h"
#include "Mesh.h"
#include <algorithm>
#include "Shader.h"
#include "VertexArray.h"
#include "SpriteComponent.h"
#include "MeshComponent.h"
#include <GL/glew.h>

Renderer::Renderer(Game* game)
	:mGame(game)
	,mSpriteShader(nullptr)
	, mBasicMeshShader(nullptr)
	, mPhongShader(nullptr)
	, mPointLightCount(0)
{
}

Renderer::~Renderer()
{
}

bool Renderer::Initialize(float screenWidth, float screenHeight)
{
	mScreenWidth = screenWidth;
	mScreenHeight = screenHeight;

	// Set OpenGL attributes
	// Use the core OpenGL profile
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	// Specify version 3.3
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	// Request a color buffer with 8-bits per RGBA channel
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	// Enable double buffering
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	// Force OpenGL to use hardware acceleration
	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);

	mWindow = SDL_CreateWindow("Game Programming in C++ (Chapter 6)", 100, 100,
		static_cast<int>(mScreenWidth), static_cast<int>(mScreenHeight), SDL_WINDOW_OPENGL);
	if (!mWindow)
	{
		SDL_Log("Failed to create window: %s", SDL_GetError());
		return false;
	}

	// Create an OpenGL context
	mContext = SDL_GL_CreateContext(mWindow);

	// Initialize GLEW
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		SDL_Log("Failed to initialize GLEW.");
		return false;
	}

	// On some platforms, GLEW will emit a benign error code,
	// so clear it
	glGetError();

	// Make sure we can create/compile shaders
	if (!LoadShaders())
	{
		SDL_Log("Failed to load shaders.");
		return false;
	}

	// Create quad for drawing sprites
	CreateSpriteVerts();

	/* Create 4 inert point lights, that calling code can create later */
	for (unsigned int i = 0; i < POINT_LIGHT_MAX; i++)
	{
		mPointLight[i].mPosition = Vector3::Zero;
		mPointLight[i].mDiffuseColor = Vector3::Zero;
		mPointLight[i].mSpecColor = Vector3::Zero;
		mPointLight[i].mSpecPower = 0.0f;
		mPointLight[i].mRadius = 0.0f;
	}

	return true;
}

void Renderer::Shutdown()
{
	delete mSpriteVerts;
	mSpriteShader->Unload();
	delete mSpriteShader;
	mBasicMeshShader->Unload();
	mPhongShader->Unload();
	delete mBasicMeshShader;
	delete mPhongShader;
	SDL_GL_DeleteContext(mContext);
	SDL_DestroyWindow(mWindow);
}

void Renderer::UnloadData()
{
	// Destroy textures
	for (auto i : mTextures)
	{
		i.second->Unload();
		delete i.second;
	}
	mTextures.clear();

	// Destroy meshes
	for (auto i : mMeshes)
	{
		i.second->Unload();
		delete i.second;
	}
	mMeshes.clear();
}

void Renderer::Draw()
{
	// Set the clear color to light grey
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	// Clear the color buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Draw mesh components
	// Enable depth buffering/disable alpha blend
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);

	// Set the mesh shader active
	mBasicMeshShader->SetActive();
	// Update view-projection matrix
	mBasicMeshShader->SetMatrixUniform("uViewProj", mView * mProjection);
	// Update lighting uniforms
	SetLightUniforms(mBasicMeshShader);
	for (auto mc : mBasicMeshComps)
	{
		mc->Draw(mBasicMeshShader);
	}

	mPhongShader->SetActive();
	mPhongShader->SetMatrixUniform("uViewProj", mView * mProjection); // either this, or basic mesh, is broken
	SetLightUniforms(mPhongShader);// or this
	for (auto mc : mPhongComps)
	{
		mc->Draw(mPhongShader);
	}

	// Draw all sprite components
	// Disable depth buffering
	glDisable(GL_DEPTH_TEST);
	// Enable alpha blending on the color buffer
	glEnable(GL_BLEND);
	glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
	glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);

	// Set shader/vao as active
	mSpriteShader->SetActive();
	mSpriteVerts->SetActive();
	for (auto sprite : mSprites)
	{
		sprite->Draw(mSpriteShader);
	}

	// Swap the buffers
	SDL_GL_SwapWindow(mWindow);
}

void Renderer::AddSprite(SpriteComponent* sprite)
{
	// Find the insertion point in the sorted vector
	// (The first element with a higher draw order than me)
	int myDrawOrder = sprite->GetDrawOrder();
	auto iter = mSprites.begin();
	for (;
		iter != mSprites.end();
		++iter)
	{
		if (myDrawOrder < (*iter)->GetDrawOrder())
		{
			break;
		}
	}

	// Inserts element before position of iterator
	mSprites.insert(iter, sprite);
}

void Renderer::RemoveSprite(SpriteComponent* sprite)
{
	auto iter = std::find(mSprites.begin(), mSprites.end(), sprite);
	mSprites.erase(iter);
}

void Renderer::AddMeshComp(MeshComponent* mesh)
{
	std::string s = mesh->GetMesh()->GetShaderName();
	if (mesh->GetMesh()->GetShaderName() == "BasicMesh")
	{
		mBasicMeshComps.emplace_back(mesh);
	}
	else if (mesh->GetMesh()->GetShaderName() == "Phong")
	{
		mPhongComps.emplace_back(mesh);
	}
	else
	{
		mBasicMeshComps.emplace_back(mesh); /* Fallback incase I can't spell */
	}
}

void Renderer::RemoveMeshComp(MeshComponent* mesh)
{
	if (mesh->GetMesh()->GetShaderName() == "BasicMesh")
	{
		auto iterBasicMesh = std::find(mBasicMeshComps.begin(), mBasicMeshComps.end(), mesh);
		mBasicMeshComps.erase(iterBasicMesh);
	}
	else if (mesh->GetMesh()->GetShaderName() == "Phong")
	{
		auto iterPhong = std::find(mPhongComps.begin(), mPhongComps.end(), mesh);
		mPhongComps.erase(iterPhong);
	}
	/* No else case here, no fallback possible */
}

Texture* Renderer::GetTexture(const std::string& fileName)
{
	Texture* tex = nullptr;
	auto iter = mTextures.find(fileName);
	if (iter != mTextures.end())
	{
		tex = iter->second;
	}
	else
	{
		tex = new Texture();
		if (tex->Load(fileName))
		{
			mTextures.emplace(fileName, tex);
		}
		else
		{
			delete tex;
			tex = nullptr;
		}
	}
	return tex;
}

Mesh* Renderer::GetMesh(const std::string & fileName)
{
	Mesh* m = nullptr;
	auto iter = mMeshes.find(fileName);
	if (iter != mMeshes.end())
	{
		m = iter->second;
	}
	else
	{
		m = new Mesh();
		if (m->Load(fileName, this))
		{
			mMeshes.emplace(fileName, m);
		}
		else
		{
			delete m;
			m = nullptr;
		}
	}
	return m;
}

bool Renderer::LoadShaders()
{
	// Create sprite shader
	mSpriteShader = new Shader();
	if (!mSpriteShader->Load("Shaders/Sprite.vert", "Shaders/Sprite.frag"))
	{
		return false;
	}

	mSpriteShader->SetActive();
	// Set the view-projection matrix
	Matrix4 viewProj = Matrix4::CreateSimpleViewProj(mScreenWidth, mScreenHeight);
	mSpriteShader->SetMatrixUniform("uViewProj", viewProj);

	// Create basic mesh shader
	mBasicMeshShader = new Shader();
	if (!mBasicMeshShader->Load("Shaders/BasicMesh.vert", "Shaders/BasicMesh.frag"))
	{
		return false;
	}

	// Create basic mesh shader
	mPhongShader = new Shader();
	if (!mPhongShader->Load("Shaders/Phong.vert", "Shaders/Phong.frag"))
	{
		return false;
	}

	mBasicMeshShader->SetActive();
	// Set the view-projection matrix
	mView = Matrix4::CreateLookAt(Vector3::Zero, Vector3::UnitX, Vector3::UnitZ);
	mProjection = Matrix4::CreatePerspectiveFOV(Math::ToRadians(70.0f),
		mScreenWidth, mScreenHeight, 25.0f, 10000.0f);
	mBasicMeshShader->SetMatrixUniform("uViewProj", mView * mProjection);
	mPhongShader->SetMatrixUniform("uViewProj", mView * mProjection);
	return true;
}

void Renderer::CreateSpriteVerts()
{
	float vertices[] = {
		-0.5f, 0.5f, 0.f, 0.f, 0.f, 0.0f, 0.f, 0.f, // top left
		0.5f, 0.5f, 0.f, 0.f, 0.f, 0.0f, 1.f, 0.f, // top right
		0.5f,-0.5f, 0.f, 0.f, 0.f, 0.0f, 1.f, 1.f, // bottom right
		-0.5f,-0.5f, 0.f, 0.f, 0.f, 0.0f, 0.f, 1.f  // bottom left
	};

	unsigned int indices[] = {
		0, 1, 2,
		2, 3, 0
	};

	mSpriteVerts = new VertexArray(vertices, 4, indices, 6);
}

void Renderer::SetLightUniforms(Shader* shader)
{
	// Camera position is from inverted view
	Matrix4 invView = mView;
	invView.Invert();
	shader->SetVectorUniform("uCameraPos", invView.GetTranslation());
	// Ambient light
	shader->SetVectorUniform("uAmbientLight", mAmbientLight);
	// Directional light
	shader->SetVectorUniform("uDirLight.mDirection",
		mDirLight.mDirection);
	shader->SetVectorUniform("uDirLight.mDiffuseColor",
		mDirLight.mDiffuseColor);
	shader->SetVectorUniform("uDirLight.mSpecColor",
		mDirLight.mSpecColor);

	/* Point lights */
	shader->SetVectorUniform("uPointLight[0].mPosition",     mPointLight[0].mPosition);
	shader->SetVectorUniform("uPointLight[0].mDiffuseColor", mPointLight[0].mDiffuseColor);
	shader->SetVectorUniform("uPointLight[0].mSpecColor",    mPointLight[0].mSpecColor);
	shader->SetFloatUniform( "uPointLight[0].mSpecPower",    mPointLight[0].mSpecPower);
	shader->SetFloatUniform( "uPointLight[0].mRadius",       mPointLight[0].mRadius);
	shader->SetVectorUniform("uPointLight[1].mPosition",     mPointLight[1].mPosition);
	shader->SetVectorUniform("uPointLight[1].mDiffuseColor", mPointLight[1].mDiffuseColor);
	shader->SetVectorUniform("uPointLight[1].mSpecColor",    mPointLight[1].mSpecColor);
	shader->SetFloatUniform( "uPointLight[1].mSpecPower",    mPointLight[1].mSpecPower);
	shader->SetFloatUniform( "uPointLight[1].mRadius",       mPointLight[1].mRadius);
	shader->SetVectorUniform("uPointLight[2].mPosition",     mPointLight[2].mPosition);
	shader->SetVectorUniform("uPointLight[2].mDiffuseColor", mPointLight[2].mDiffuseColor);
	shader->SetVectorUniform("uPointLight[2].mSpecColor",    mPointLight[2].mSpecColor);
	shader->SetFloatUniform( "uPointLight[2].mSpecPower",    mPointLight[2].mSpecPower);
	shader->SetFloatUniform( "uPointLight[2].mRadius",       mPointLight[2].mRadius);
	shader->SetVectorUniform("uPointLight[3].mPosition",     mPointLight[3].mPosition);
	shader->SetVectorUniform("uPointLight[3].mDiffuseColor", mPointLight[3].mDiffuseColor);
	shader->SetVectorUniform("uPointLight[3].mSpecColor",    mPointLight[3].mSpecColor);
	shader->SetFloatUniform( "uPointLight[3].mSpecPower",    mPointLight[3].mSpecPower);
	shader->SetFloatUniform( "uPointLight[3].mRadius",       mPointLight[3].mRadius);
}

void Renderer::SetPointLight(PointLight point)
{
	if (mPointLightCount < POINT_LIGHT_MAX)
	{
		mPointLight[mPointLightCount].mPosition = point.mPosition;
		mPointLight[mPointLightCount].mDiffuseColor = point.mDiffuseColor;
		mPointLight[mPointLightCount].mSpecColor = point.mSpecColor;
		mPointLight[mPointLightCount].mSpecPower = point.mSpecPower;
		mPointLight[mPointLightCount].mRadius = point.mRadius;
		mPointLightCount++;
	}
}