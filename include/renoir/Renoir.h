#pragma once

#include <stddef.h>
#include <stdint.h>

#if __cplusplus
extern "C" {
#endif

// Enums
typedef enum RENOIR_CLEAR {
	RENOIR_CLEAR_COLOR = 1 << 0,
	RENOIR_CLEAR_DEPTH = 1 << 1
} RENOIR_CLEAR;

typedef enum RENOIR_SHADER {
	RENOIR_SHADER_NONE,
	RENOIR_SHADER_VERTEX,
	RENOIR_SHADER_PIXEL,
	RENOIR_SHADER_GEOMETRY,
	RENOIR_SHADER_COMPUTE
} RENOIR_SHADER;

typedef enum RENOIR_BUFFER {
	RENOIR_BUFFER_NONE,
	RENOIR_BUFFER_VERTEX,
	RENOIR_BUFFER_INDEX,
	RENOIR_BUFFER_UNIFORM,
	RENOIR_BUFFER_COMPUTE
} RENOIR_BUFFER;

typedef enum RENOIR_USAGE {
	RENOIR_USAGE_NONE,
	RENOIR_USAGE_STATIC,
	RENOIR_USAGE_DYNAMIC
} RENOIR_USAGE;

typedef enum RENOIR_ACCESS {
	RENOIR_ACCESS_NONE,
	RENOIR_ACCESS_READ,
	RENOIR_ACCESS_WRITE,
	RENOIR_ACCESS_READ_WRITE
} RENOIR_ACCESS;

typedef enum RENOIR_PIXELFORMAT {
	RENOIR_PIXELFORMAT_NONE,
	RENOIR_PIXELFORMAT_RGBA,
	RENOIR_PIXELFORMAT_R16I,
	RENOIR_PIXELFORMAT_R16F,
	RENOIR_PIXELFORMAT_R32F,
	RENOIR_PIXELFORMAT_R32G32F,
	RENOIR_PIXELFORMAT_D24S8,
	RENOIR_PIXELFORMAT_D32,
	RENOIR_PIXELFORMAT_R8
} RENOIR_PIXELFORMAT;

typedef enum RENOIR_TYPE {
	RENOIR_TYPE_NONE,
	RENOIR_TYPE_UBYTE,
	RENOIR_TYPE_UNSIGNED_SHORT,
	RENOIR_TYPE_SHORT,
	RENOIR_TYPE_INT,
	RENOIR_TYPE_FLOAT,
	RENOIR_TYPE_FLOAT2,
	RENOIR_TYPE_FLOAT3,
	RENOIR_TYPE_FLOAT4
} RENOIR_TYPE;

typedef enum RENOIR_FACE {
	RENOIR_FACE_NONE,
	RENOIR_FACE_BACK,
	RENOIR_FACE_FRONT,
	RENOIR_FACE_FRONT_BACK
} RENOIR_FACE;

typedef enum RENOIR_BLEND {
	RENOIR_BLEND_NONE,
	RENOIR_BLEND_ZERO,
	RENOIR_BLEND_ONE,
	RENOIR_BLEND_SRC_COLOR,
	RENOIR_BLEND_ONE_MINUS_SRC_COLOR,
	RENOIR_BLEND_DST_COLOR,
	RENOIR_BLEND_ONE_MINUS_DST_COLOR,
	RENOIR_BLEND_SRC_ALPHA,
	RENOIR_BLEND_ONE_MINUS_SRC_ALPHA
} RENOIR_BLEND;

typedef enum RENOIR_BLEND_EQ {
	RENOIR_BLEND_EQ_NONE,
	RENOIR_BLEND_EQ_ADD,
	RENOIR_BLEND_EQ_SUBTRACT,
	RENOIR_BLEND_EQ_MIN,
	RENOIR_BLEND_EQ_MAX
} RENOIR_BLEND_EQ;

typedef enum RENOIR_ORIENTATION {
	RENOIR_ORIENTATION_NONE,
	RENOIR_ORIENTATION_CCW,
	RENOIR_ORIENTATION_CW
} RENOIR_ORIENTATION;

typedef enum RENOIR_SEMANTIC {
	RENOIR_SEMANTIC_NONE,
	RENOIR_SEMANTIC_POSITION,
	RENOIR_SEMANTIC_POSITION0 = RENOIR_SEMANTIC_POSITION,
	RENOIR_SEMANTIC_POSITION1,
	RENOIR_SEMANTIC_COLOR,
	RENOIR_SEMANTIC_TEXCOORD,
	RENOIR_SEMANTIC_NORMAL,
	RENOIR_SEMANTIC_PSIZE
} RENOIR_SEMANTIC;

typedef enum RENOIR_COMPARE {
	RENOIR_COMPARE_NONE,
	RENOIR_COMPARE_NEVER,
	RENOIR_COMPARE_LESS,
	RENOIR_COMPARE_EQUAL,
	RENOIR_COMPARE_LESS_EQUAL,
	RENOIR_COMPARE_GREATER,
	RENOIR_COMPARE_NOT_EQUAL,
	RENOIR_COMPARE_GREATER_EQUAL,
	RENOIR_COMPARE_ALWAYS
} RENOIR_COMPARE;

typedef enum RENOIR_TEXMODE {
	RENOIR_TEXMODE_NONE,
	RENOIR_TEXMODE_CLAMP,
	RENOIR_TEXMODE_WRAP,
	RENOIR_TEXMODE_BORDER,
	RENOIR_TEXMODE_MIRROR
} RENOIR_TEXMODE;

typedef enum RENOIR_FILTER {
	RENOIR_FILTER_NONE,
	RENOIR_FILTER_POINT,
	RENOIR_FILTER_LINEAR
} RENOIR_FILTER;

typedef enum RENOIR_PRIMITIVE {
	RENOIR_PRIMITIVE_POINTS,
	RENOIR_PRIMITIVE_LINES,
	RENOIR_PRIMITIVE_TRIANGLES,
} RENOIR_PRIMITIVE;

typedef enum RENOIR_SWITCH {
	RENOIR_SWITCH_DEFAULT,
	RENOIR_SWITCH_ENABLE,
	RENOIR_SWITCH_DISABLE
} RENOIR_ACTIVE;

typedef enum RENOIR_PIPELINE_MODE {
	RENOIR_PIPELINE_MODE_RASTER,
	RENOIR_PIPELINE_MODE_COMPUTE
} RENOIR_PIPELINE;

typedef enum RENOIR_MSAA_MODE {
	RENOIR_MSAA_MODE_NONE,
	RENOIR_MSAA_MODE_2,
	RENOIR_MSAA_MODE_4,
	RENOIR_MSAA_MODE_8,
} RENOIR_MSAA_MODE;

// Handles
typedef struct Renoir_Buffer { void* handle; } Renoir_Buffer;
typedef struct Renoir_Pipeline { void* handle; } Renoir_Pipeline;
typedef struct Renoir_Texture { void* handle; } Renoir_Texture;
typedef struct Renoir_Sampler { void* handle; } Renoir_Sampler;
typedef struct Renoir_Program { void* handle; } Renoir_Program;
typedef struct Renoir_Compute { void* handle; } Renoir_Compute;
typedef struct Renoir_Geometry { void* handle; } Renoir_Geometry;
typedef struct Renoir_Pass { void* handle; } Renoir_Pass;
typedef struct Renoir_View { void* handle; } Renoir_View;


// Descriptons
typedef struct Renoir_Size {
	int width, height, depth;
} Renoir_Size;

typedef struct Renoir_Color {
	float r, g, b, a;
} Renoir_Color;

typedef struct Renoir_Settings {
	bool defer_api_calls;
	RENOIR_MSAA_MODE msaa;
} Renoir_Settings;

typedef struct Renoir_Pipeline_Desc {
	RENOIR_PIPELINE_MODE mode;
	union
	{
		struct
		{
			RENOIR_SWITCH cull;
			RENOIR_FACE cull_face;
			RENOIR_ORIENTATION cull_front;

			RENOIR_SWITCH depth;

			RENOIR_SWITCH blend;
			RENOIR_BLEND src_rgb, dst_rgb, src_alpha, dst_alpha;
			RENOIR_BLEND_EQ eq_rgb, eq_alpha;

			Renoir_Program shader;
		} raster;

		struct
		{
			Renoir_Compute shader;
		} compute;
	};
} Pipeline_Desc;

typedef struct Renoir_Buffer_Desc {
	RENOIR_BUFFER type;
	RENOIR_USAGE usage;
	RENOIR_ACCESS access;
	void* data;
	size_t data_size;
} Renoir_Buffer_Desc;

typedef struct Renoir_Texture_Desc {
	Renoir_Size size;
	RENOIR_USAGE usage;
	RENOIR_ACCESS access;
	RENOIR_PIXELFORMAT pixel_format;
	RENOIR_TYPE pixel_type;
	void* data;
	size_t data_size;
} Renoir_Texture_Desc;

typedef struct Renoir_Sampler_Desc {
	RENOIR_FILTER filter;
	RENOIR_TEXMODE u;
	RENOIR_TEXMODE v;
	RENOIR_TEXMODE w;
	RENOIR_COMPARE compare;
	Renoir_Color border;
} Renoir_Sampler_Desc;

typedef struct Renoir_Shader_Blob {
	const char* bytes;
	size_t size;
} Renoir_Shader_Blob;

typedef struct Renoir_Program_Desc {
	Renoir_Shader_Blob vertex;
	Renoir_Shader_Blob pixel;
	Renoir_Shader_Blob geometry;
} Renoir_Program_Desc;

typedef struct Renoir_Compute_Desc {
	Renoir_Shader_Blob compute;
} Renoir_Compute_Desc;

typedef struct Renoir_Clear_Desc {
	RENOIR_CLEAR flags;
	Renoir_Color color;
	float depth;
	uint8_t stencil;
} Renoir_Clear_Desc;

typedef struct Renoir_Vertex_Desc {
	Renoir_Buffer buffer;
	RENOIR_TYPE type;
	size_t stride;
	size_t offset;
} Renoir_Vertex_Desc;

typedef struct Renoir_Draw_Desc {
	RENOIR_PRIMITIVE primitive;
	int base_element;
	int elements_count;
	int instances_count;
	Renoir_Vertex_Desc vertex_buffers[10];
	Renoir_Buffer index_buffer;
	RENOIR_TYPE index_type;
} Renoir_Draw_Desc;

typedef struct Renoir_Texture_Edit_Desc {
	int x, y, z;
	int width, height, depth;
	void* bytes;
	size_t bytes_size;
} Renoir_Texture_Read_Desc;

struct IRenoir;

typedef struct Renoir
{
	struct IRenoir* ctx;

	bool (*init)(struct Renoir* self, Renoir_Settings settings);
	void (*dispose)(struct Renoir* self);

	Renoir_View (*view_window_new)(struct Renoir* api, int width, int height, void* window, void* display);
	void (*view_free)(struct Renoir* api, Renoir_View view);
	void (*view_resize)(struct Renoir* api, Renoir_View view, int width, int height);
	void (*view_present)(struct Renoir* api, Renoir_View view);

	Renoir_Buffer (*buffer_new)(struct Renoir* api, Renoir_Buffer_Desc desc);
	void (*buffer_free)(struct Renoir* api, Renoir_Buffer buffer);

	Renoir_Texture (*texture_new)(struct Renoir* api, Renoir_Texture_Desc desc);
	void (*texture_free)(struct Renoir* api, Renoir_Texture texture);

	Renoir_Sampler (*sampler_new)(struct Renoir* api, Renoir_Sampler_Desc desc);
	void (*sampler_free)(struct Renoir* api, Renoir_Sampler sampler);

	Renoir_Program (*program_new)(struct Renoir* api, Renoir_Program_Desc desc);
	void (*program_free)(struct Renoir* api, Renoir_Program program);

	Renoir_Compute (*compute_new)(struct Renoir* api, Renoir_Compute_Desc desc);
	void (*compute_free)(struct Renoir* api, Renoir_Compute compute);

	Renoir_Pipeline (*pipeline_new)(struct Renoir* api, Renoir_Pipeline_Desc desc);
	void (*pipeline_free)(struct Renoir* api, Renoir_Pipeline pipeline);

	Renoir_Pass (*pass_new)(struct Renoir* api);
	void (*pass_free)(struct Renoir* api, Renoir_Pass pass);

	// Graphics Commands
	void (*pass_begin)(struct Renoir* api, Renoir_Pass pass, Renoir_View view);
	void (*pass_end)(struct Renoir* api, Renoir_Pass pass);
	void (*clear)(struct Renoir* api, Renoir_Pass pass, Renoir_Clear_Desc desc);
	void (*use_pipeline)(struct Renoir* api, Renoir_Pass pass, Renoir_Pipeline pipeline);
	// Write Functions
	void (*buffer_write)(struct Renoir* api, Renoir_Pass pass, Renoir_Buffer buffer, size_t offset, void* bytes, size_t bytes_size);
	void (*texture_write)(struct Renoir* api, Renoir_Pass pass, Renoir_Texture texture, Renoir_Texture_Edit_Desc desc);
	// Read Functions
	void (*buffer_read)(struct Renoir* api, Renoir_Buffer buffer, size_t offset, void* bytes, size_t bytes_size);
	void (*texture_read)(struct Renoir* api, Renoir_Texture texture, Renoir_Texture_Edit_Desc desc);
	// Bind Functions
	void (*buffer_bind)(struct Renoir* api, Renoir_Pass pass, Renoir_Buffer buffer, RENOIR_SHADER shader, int slot);
	void (*texture_bind)(struct Renoir* api, Renoir_Pass pass, Renoir_Texture texture, RENOIR_SHADER shader, int slot);
	void (*sampler_bind)(struct Renoir* api, Renoir_Pass pass, Renoir_Sampler sampler, RENOIR_SHADER shader, int slot);
	// Draw
	void (*draw)(struct Renoir* api, Renoir_Pass pass, Renoir_Draw_Desc desc);
} Renoir;

#if __cplusplus
}
#endif
