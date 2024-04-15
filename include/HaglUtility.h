#pragma once

#include <stdexcept>

#define LOG_INFO(msg, ...) SDL_Log(msg, ##__VA_ARGS__)
#define LOG_ERROR(msg, ...) SDL_LogError(0, msg, ##__VA_ARGS__)

namespace hagl {
	class NotImplementedException : public std::logic_error {
	public:
		NotImplementedException();
	};
}