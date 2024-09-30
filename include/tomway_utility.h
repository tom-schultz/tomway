#pragma once

#include <memory>
#include <stdexcept>

#define LOG_INFO(msg, ...) SDL_Log(msg, ##__VA_ARGS__)
#define LOG_ERROR(msg, ...) SDL_LogError(0, msg, ##__VA_ARGS__)

namespace tomway {
	class not_implemented_exception : public std::logic_error {
	public:
		not_implemented_exception();
	};

	template<typename ... Args>
	std::string string_format( const std::string& format, Args ... args )
	{
		int size_s = std::snprintf( nullptr, 0, format.c_str(), args ... ) + 1; // Extra space for '\0'
		if( size_s <= 0 ){ throw std::runtime_error( "Error during formatting." ); }
		auto size = static_cast<size_t>( size_s );
		std::unique_ptr<char[]> buf = std::make_unique<char[]>(size);
		std::snprintf( buf.get(), size, format.c_str(), args ... );
		return std::string( buf.get(), buf.get() + size - 1 ); // We don't want the '\0' inside
	}
}