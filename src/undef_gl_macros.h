#pragma once

// ƒанный файл предназначен дл€ выключени€ надоедливых макросов OpenGL.
// ѕоскольку дл€ OpenGL используетс€ glbinding - https://github.com/cginternals/glbinding ,
// который оборачивает константы GL в enum class и consteypr выражаени€,
// макросы только мешают. 

#undef GL_TEXTURE0
#undef GL_TEXTURE_2D
#undef GL_TEXTURE_WRAP_S
#undef GL_TEXTURE_WRAP_T
#undef GL_TEXTURE_BORDER_COLOR
#undef GL_TEXTURE_MIN_FILTER
#undef GL_TEXTURE_MAG_FILTER
#undef GL_LINEAR
#undef GL_NEAREST
#undef GL_RGB
#undef GL_RGBA
#undef GL_NEAREST
#undef GL_FLOAT
#undef GL_REPEAT
#undef GL_CLAMP_TO_BORDER


#undef GL_TRIANGLES
#undef GL_UNSIGNED_INT
#undef GL_FLOAT
#undef GL_UNSIGNED_BYTE
#undef GL_COLOR_BUFFER_BIT
#undef GL_DEBUG_OUTPUT
#undef GL_DEBUG_OUTPUT_SYNCHRONOUS

#undef GL_BLEND
#undef GL_SRC_ALPHA
#undef GL_ONE_MINUS_SRC_ALPHA
#undef GL_DEPTH_TEST
#undef GL_NEVER

#undef GL_DEBUG_SOURCE_API
#undef GL_DEBUG_SOURCE_WINDOW_SYSTEM
#undef GL_DEBUG_SOURCE_SHADER_COMPILER
#undef GL_DEBUG_SOURCE_THIRD_PARTY
#undef GL_DEBUG_SOURCE_APPLICATION
#undef GL_DEBUG_SOURCE_OTHER

#undef GL_DEPTH_BUFFER_BIT
#undef GL_LESS