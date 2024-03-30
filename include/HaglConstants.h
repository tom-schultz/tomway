#pragma once

#define LOG_INFO(msg, ...) SDL_Log(msg, ##__VA_ARGS__)
#define LOG_ERROR(msg, ...) SDL_LogError(msg, ##__VA_ARGS__)

namespace hagl {
	extern const char* APP_NAME;
}