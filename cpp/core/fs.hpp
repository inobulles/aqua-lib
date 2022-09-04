#pragma once

namespace aqua_libc {
	extern "C" {
		#include <../c/core/fs.h>
	}
}

namespace aqua::core::fs {
	enum Flags {
		FLAGS_READ   = aqua_libc::FS_FLAGS_READ,
		FLAGS_WRITE  = aqua_libc::FS_FLAGS_WRITE,
		FLAGS_EXEC   = aqua_libc::FS_FLAGS_EXEC,
		FLAGS_APPEND = aqua_libc::FS_FLAGS_APPEND,
		FLAGS_CREATE = aqua_libc::FS_FLAGS_CREATE,
	};

	inline Flags operator |(Flags a, Flags b) {
		return static_cast<Flags>(static_cast<int>(a) | static_cast<int>(b));
	}

	class Generic_error : public std::exception {
		std::string msg;

	public:

		Generic_error(std::string msg) : msg(msg) {
		}

		const char* what(void) const noexcept {
			return this->msg.c_str();
		}
	};

	class Descr {
		aqua_libc::fs_descr_t descr;

	public:

		Descr(const std::string drive, const std::string path, Flags flags) {
			descr = aqua_libc::fs_open(drive.c_str(), path.c_str(), static_cast<aqua_libc::fs_flags_t>(flags));

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
