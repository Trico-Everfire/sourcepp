#include "sourcepp.h"

#ifdef GAMEPP
#include "gamepp.h"
#endif

#ifdef STEAMPP
#include "steampp.h"
#endif

#ifdef VCRYPTPP
#include "vcryptpp.h"
#endif

PYBIND11_MODULE(_sourcepp_python, m) {
	m.doc() = "SourcePP: A Python wrapper around several modern C++20 libraries for sanely parsing Valve's formats.";

	m.attr("__author__") = "craftablescience";
	m.attr("__version__") = "dev";

	sourcepp::register_python(m);

#ifdef GAMEPP
	gamepp::register_python(m);
#else
	m.def_submodule("gamepp");
#endif

#ifdef STEAMPP
	steampp::register_python(m);
#else
	m.def_submodule("steampp");
#endif

#ifdef VCRYPTPP
	vcryptpp::register_python(m);
#else
	m.def_submodule("vcryptpp");
#endif
}