#pragma once

#include "renoir/Renoir.h"

#include <atomic>

#include <GL/glew.h>

struct Renoir_Command;

enum RENOIR_HANDLE_KIND
{
	RENOIR_HANDLE_KIND_NONE,
	RENOIR_HANDLE_KIND_SWAPCHAIN,
	RENOIR_HANDLE_KIND_PASS,
	RENOIR_HANDLE_KIND_BUFFER,
	RENOIR_HANDLE_KIND_TEXTURE,
	RENOIR_HANDLE_KIND_SAMPLER,
	RENOIR_HANDLE_KIND_PROGRAM,
	RENOIR_HANDLE_KIND_COMPUTE,
	RENOIR_HANDLE_KIND_PIPELINE,
};

struct Renoir_Handle
{
	RENOIR_HANDLE_KIND kind;
	std::atomic<int> rc;
	union
	{
		struct
		{
			int width;
			int height;
			void* handle;
			void* display;
			void* hdc;
		} swapchain;

		struct
		{
			Renoir_Command *command_list_head;
			Renoir_Command *command_list_tail;
			// used when rendering is done on screen/window
			Renoir_Handle* swapchain;
			// used when rendering is done off screen
			GLuint fb;
			int width, height;
			Renoir_Pass_Offscreen_Desc offscreen;
		} pass;

		struct
		{
			GLuint id;
			RENOIR_BUFFER type;
			RENOIR_USAGE usage;
			RENOIR_ACCESS access;
		} buffer;

		struct
		{
			GLuint id;
			Renoir_Size size;
			RENOIR_USAGE usage;
			RENOIR_ACCESS access;
			Renoir_Sampler_Desc default_sampler_desc;
			RENOIR_PIXELFORMAT pixel_format;
			bool cube_map;
			bool mipmaps;
			bool render_target;
			RENOIR_MSAA_MODE msaa;
			GLuint render_buffer;
		} texture;

		struct
		{
			GLuint id;
			Renoir_Sampler_Desc desc;
		} sampler;

		struct
		{
			GLuint id;
		} program;

		struct
		{
			GLuint id;
		} compute;

		struct
		{
			Renoir_Pipeline_Desc desc;
		} pipeline;
	};
};
