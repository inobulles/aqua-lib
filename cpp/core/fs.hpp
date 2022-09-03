#pragma once

namespace aqua_libc {
	extern "C" {
		#include <../c/core/fs.h>
	}
}

namespace aqua::core::fs {
	class Generic_error : public std::exception {
		std::string msg;

	public:

		Generic_error(std::string msg) : msg(msg) {
		}

		std::string what(void) {
			return "Generic AQUA filesystem error: " + this->msg;
		}
	};

	class Descr {
		aqua_libc::fs_descr_t descr;

	public:

		Descr(const std::string drive, const std::string path, aqua_libc::fs_flags_t flags) {
			descr = aqua_libc::fs_open(drive.c_str(), path.c_str(), flags);

			if (!descr) {
				throw Generic_error("Failed to open '" + drive + ":" + path + "'");
			}
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

		void read(void* buf, size_t len) {
			aqua_libc::fs_err_t err = aqua_libc::fs_read(descr, buf, len);

			if (err != aqua_libc::FS_ERR_SUCCESS) {
				throw Generic_error("Failed to read from file");
			}
		}

		void write(const void* buf, size_t len) {
			aqua_libc::fs_err_t err = aqua_libc::fs_write(descr, buf, len);

			if (err != aqua_libc::FS_ERR_SUCCESS) {
				throw Generic_error("Failed to write to file");
			}
		}

		// helper functions

		void write(std::string str) {
			write(str.c_str(), str.size());
		}
	};
}
