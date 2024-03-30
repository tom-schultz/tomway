#pragma once

#include <stdexcept>

namespace hagl {
	class NotImplementedException : public std::logic_error {
	public:
		NotImplementedException();
	};
}