#pragma once

#include <iostream>

namespace aqua_libc {
	extern "C" {
		#define NO_ENTRY
		#include <../c/root.h>
	}

	int m4nGl3D___native_entry(void) {
		int rv = -1;

		try {
			rv = main();
		}

		catch (std::exception& e) {
			std::cerr << "[C++ AQUA library] UNHANDLED EXCEPTION: " << e.what() << std::endl;
		}

		return rv;
	}

	extern "C" int __native_entry(void) {
		return m4nGl3D___native_entry();
	}
}
