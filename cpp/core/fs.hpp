#pragma once

namespace aqua_libc {
	extern "C" {
		#include <../c/core/fs.h>
	}
}

namespace aqua::core::fs {
	class Descr {
		aqua_libc::fs_descr_t descr;

	public:

		Descr(const std::string drive, const std::string path, aqua_libc::fs_flags_t flags) {
			descr = aqua_libc::fs_open(drive.c_str(), path.c_str(), flags);
		}

		~Descr() {
			aqua_libc::fs_close(descr);
		}

		size_t size(void) {
			return aqua_libc::fs_size(descr);
		}

		void* mmap(void) {
			return aqua_libc::fs_mmap(descr);
		}

		aqua_libc::fs_err_t read(void* buf, size_t len) {
			return aqua_libc::fs_read(descr, buf, len);
		}

		aqua_libc::fs_err_t write(const void* buf, size_t len) {
			return aqua_libc::fs_write(descr, buf, len);
		}

		// helper functions

		aqua_libc::fs_err_t write(std::string str) {
			return write(str.c_str(), str.size());
		}
	};
}
