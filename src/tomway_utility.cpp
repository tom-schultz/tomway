#include "tomway_utility.h"

tomway::not_implemented_exception::not_implemented_exception() : std::logic_error("Not implemented.") {}