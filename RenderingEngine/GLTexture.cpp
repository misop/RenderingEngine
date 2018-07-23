#include "stdafx.h"
#include "GLTexture.h"
#include <IL/il.h>
#include <iostream>

#include "hdrloader.h"

using namespace std;
using namespace GL;

#pragma region Init

Texture::Texture(GLenum texType)
{
	glGenTextures(1, &texture);
	glBindTexture(texType, texture);
	target = texType;

	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);    
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
}

Texture::~Texture(void)
{
	glDeleteTextures(1, &texture);
}

#pragma endregion

#pragma region OpenGL Handling

void Texture::Enable() {
	glEnable(target);
}

void Texture::Disable() {
	glDisable(target);
}

void Texture::Bind() {
	glBindTexture(target, texture);
}

void Texture::Unbind() {
	glBindTexture(target, 0);
}

void Texture::UseTexture(GLint loc, int unit) {
	glUniform1i(loc, unit);
}

void Texture::TexParameteri(GLenum pname, GLint param) {
	glTexParameteri(target, pname, param);
}

#pragma endregion

#pragma region Texture Loading from Data

void Texture::Texture2D(int width, int height, GLenum internalFormat, GLenum format, GLenum type, GLubyte *data) {
	glTexImage2D(target, 0, internalFormat, width, height, 0, format, type, data);
}

void Texture::FunctionTexture(int width, int height, float *data) {
	glTexImage2D(target, 0, GL_R32F, width, height, 0, GL_RED, GL_FLOAT, data);
}

void Texture::RGBATexture(int width, int height, GLenum format, GLenum type, GLubyte *data) {
	GLenum internalFormat = GL_RGBA32F;
	if (format == GL_UNSIGNED_BYTE)
		internalFormat = GL_RGBA;
	glTexImage2D(target, 0, internalFormat, width, height, 0, format, type, data);
}

void Texture::DepthTexture(int width, int height, GLenum type) {
	glTexImage2D(target, 0, GL_DEPTH_COMPONENT32, width, height, 0, GL_DEPTH_COMPONENT, type, 0);
}

#pragma endregion

#pragma region Texture Loading from File

void Texture::LoadRGBATextureFromImage(string img) {
	ilInit();

	ILuint texIL;
	ilGenImages(1, &texIL);
	ilBindImage(texIL);
	// nacitanie textury zo suboru
	//wstring widestr(img.begin(), img.end());
	ILboolean result = ilLoadImage((ILstring)img.c_str());
	if (!result) {
		ilDeleteImages(1, &texIL);
		return;
	}
	ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);
	// generovanie OpenGL textur
	GLuint tex;
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	// nastavenie parametrov textury
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	Bind();
	TexParameteri(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	TexParameteri(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	TexParameteri(GL_TEXTURE_WRAP_S, GL_REPEAT);
	TexParameteri(GL_TEXTURE_WRAP_T, GL_REPEAT);
	RGBATexture(ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT), ilGetInteger(IL_IMAGE_FORMAT), GL_UNSIGNED_BYTE, ilGetData());

	// zmazanie OpenIL textury
	ilDeleteImages(1, &texIL);
}

void Texture::LoadCubeTextureFromImages(string posxFile, string negxFile, string posyFile, string negyFile, string poszFile, string negzFile) {
    ilInit();
    ILuint cubeTexIL;
    ilGenImages(1, &cubeTexIL);
    ilBindImage(cubeTexIL);

    // vytvorenie OpenGL cube map textury
	Bind();
    TexParameteri(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    TexParameteri(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    TexParameteri(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    TexParameteri(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // nacitanie textury pre posx stenu zo suboru a do OpenGL
	ILboolean result = ilLoadImage((ILstring)posxFile.c_str());
    if (!result) {
        ilDeleteImages(1, &cubeTexIL);
        return;
    }
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGB, ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT), 
                 0, ilGetInteger(IL_IMAGE_FORMAT), GL_UNSIGNED_BYTE, ilGetData());

    // nacitanie textury pre negx stenu zo suboru a do OpenGL
	result = ilLoadImage((ILstring)negxFile.c_str());
    if (!result) {
        ilDeleteImages(1, &cubeTexIL);
        return;
    }
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGB, ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT), 
                 0, ilGetInteger(IL_IMAGE_FORMAT), GL_UNSIGNED_BYTE, ilGetData());

    // nacitanie textury pre posy stenu zo suboru a do OpenGL
	result = ilLoadImage((ILstring)posyFile.c_str());
    if (!result) {
        ilDeleteImages(1, &cubeTexIL);
        return;
    }
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGB, ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT), 
                 0, ilGetInteger(IL_IMAGE_FORMAT), GL_UNSIGNED_BYTE, ilGetData());

    // nacitanie textury pre negy stenu zo suboru a do OpenGL
	result = ilLoadImage((ILstring)negyFile.c_str());
    if (!result) {
        ilDeleteImages(1, &cubeTexIL);
        return;
    }
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGB, ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT), 
                 0, ilGetInteger(IL_IMAGE_FORMAT), GL_UNSIGNED_BYTE, ilGetData());

    // nacitanie textury pre posz stenu zo suboru a do OpenGL
	result = ilLoadImage((ILstring)poszFile.c_str());
    if (!result) {
        ilDeleteImages(1, &cubeTexIL);
        return;
    }
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGB, ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT), 
                 0, ilGetInteger(IL_IMAGE_FORMAT), GL_UNSIGNED_BYTE, ilGetData());

    // nacitanie textury pre negz stenu zo suboru a do OpenGL
	result = ilLoadImage((ILstring)negzFile.c_str());
    if (!result) {
        ilDeleteImages(1, &cubeTexIL);
        return;
    }
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGB, ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT), 
                 0, ilGetInteger(IL_IMAGE_FORMAT), GL_UNSIGNED_BYTE, ilGetData());
}

void Texture::LoadHDRCubeTextureFromImages(string posxFile, string negxFile, string posyFile, string negyFile, string poszFile, string negzFile) {
	// vytvorenie OpenGL cube map textury
	Bind();
	TexParameteri(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	TexParameteri(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	TexParameteri(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	TexParameteri(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// nacitanie textury pre posx stenu zo suboru a do OpenGL
	HDRLoaderResult posxRes;
	bool result = HDRLoader::load(posxFile.c_str(), posxRes);
	if (!result) {
		return;
	}
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGB32F, posxRes.width, posxRes.height, 0, GL_RGB, GL_FLOAT, (GLvoid*)posxRes.cols);

	// nacitanie textury pre negx stenu zo suboru a do OpenGL
	HDRLoaderResult negxRes;
	result = HDRLoader::load(negxFile.c_str(), negxRes);
	if (!result) {
		return;
	}
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGB32F, negxRes.width, negxRes.height, 0, GL_RGB, GL_FLOAT, (GLvoid*)negxRes.cols);

	// nacitanie textury pre posy stenu zo suboru a do OpenGL
	HDRLoaderResult posyRes;
	result = HDRLoader::load(posyFile.c_str(), posyRes);
	if (!result) {
		return;
	}
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGB32F, posyRes.width, posyRes.height, 0, GL_RGB, GL_FLOAT, (GLvoid*)posyRes.cols);

	// nacitanie textury pre negy stenu zo suboru a do OpenGL
	HDRLoaderResult negyRes;
	result = HDRLoader::load(negyFile.c_str(), negyRes);
	if (!result) {
		return;
	}
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGB32F, negyRes.width, negyRes.height, 0, GL_RGB, GL_FLOAT, (GLvoid*)negyRes.cols);

	// nacitanie textury pre posz stenu zo suboru a do OpenGL
	HDRLoaderResult poszRes;
	result = HDRLoader::load(poszFile.c_str(), poszRes);
	if (!result) {
		return;
	}
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGB32F, poszRes.width, poszRes.height, 0, GL_RGB, GL_FLOAT, (GLvoid*)poszRes.cols);

	// nacitanie textury pre negz stenu zo suboru a do OpenGL
	HDRLoaderResult negzRes;
	result = HDRLoader::load(negzFile.c_str(), negzRes);
	if (!result) {
		return;
	}
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGB32F, negzRes.width, negzRes.height, 0, GL_RGB, GL_FLOAT, (GLvoid*)negzRes.cols);
}

#pragma endregion