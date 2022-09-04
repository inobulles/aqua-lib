#pragma once

#include <root.hpp>

namespace aqua_libc {
	extern "C" {
		#include <../c/aquabsd/alps/win.h>
	}
}

namespace aqua::aquabsd::alps::win {
	enum Cb {
		CB_DRAW   = aqua_libc::WIN_CB_DRAW,
		CB_RESIZE = aqua_libc::WIN_CB_RESIZE,
	};

	enum State {
		STATE_TRANSIENT  = aqua_libc::WIN_STATE_TRANSIENT,
		STATE_FULLSCREEN = aqua_libc::WIN_STATE_FULLSCREEN,
	};

	inline State operator |(State a, State b) {
		return static_cast<State>(static_cast<int>(a) | static_cast<int>(b));
	}

	class Generic_error : std::exception {
		std::string msg;

	public:

		Generic_error(std::string msg) : msg(msg) {
		}

		const char* what(void) const noexcept {
			return this->msg.c_str();
		}
	};

	class Win {
		aqua_libc::win_t* win;

	public:

		float x_pos, y_pos;
		float width, height;

		unsigned    x_res,    y_res;
		unsigned wm_x_res, wm_y_res;

		Win(unsigned x_res, unsigned y_res) :
			x_res(x_res),
			y_res(y_res)
		{
			win = aqua_libc::win_create(x_res, y_res);

			if (!win) {
				throw Generic_error("Failed to create window");
			}
		}

		Win(aqua_libc::win_t* win) : win(win) {
		}

		Win() {
		}

		~Win() {
			if (!win) {
				return;
			}

			aqua_libc::win_delete(win);
		}

		void set_caption(std::string caption) {
			aqua_libc::win_set_caption(win, caption.c_str());
		}

		std::string get_caption(void) {
			return aqua_libc::win_get_caption(win);
		}

		State get_state(void) {
			return static_cast<State>(aqua_libc::win_get_state(win));
		}

		void register_cb(Cb type, int (*cb) (), void* param) {
			auto c_type = static_cast<aqua_libc::win_cb_t>(type);

			if (aqua_libc::win_register_cb(win, c_type, cb, param) < 0) {
				throw Generic_error("Failed to register window callback");
			}
		}

		void register_cb(Cb type, int (*cb) (uint64_t, uint64_t, uint64_t), void* param) {
			this->register_cb(type, reinterpret_cast<int (*) ()>(cb), param);
		}

		void register_cb(Cb type, int (*cb) (uint64_t, uint64_t, uint64_t, uint64_t), void* param) {
			this->register_cb(type, reinterpret_cast<int (*) ()>(cb), param);
		}

		void loop(void) {
			aqua_libc::win_loop(win);
		}

		void sync(void) {
			aqua_libc::win_sync(win);

			x_pos    = win->x_pos;
			y_pos    = win->y_pos;

			width    = win->width;
			height   = win->height;

			x_res    = win->x_res;
			y_res    = win->y_res;

			wm_x_res = win->wm_x_res;
			wm_y_res = win->wm_y_res;
		}

		void close(void) {
			aqua_libc::win_close(win);
		}

		void grab_focus(void) {
			aqua_libc::win_grab_focus(win);
		}

		void modify(float x, float y, unsigned x_res, unsigned y_res) {
			aqua_libc::win_modify(win, x, y, x_res, y_res);
		}

		// AQUA DWD protocol stuff

		void set_dwd_close_pos(float x, float y) {
			aqua_libc::win_set_dwd_close_pos(win, x, y);
		}

		bool supports_dwd(void) {
			return aqua_libc::win_supports_dwd(win);
		}

		float get_dwd_close_pos_x(void) {
			return aqua_libc::win_get_dwd_close_pos_x(win);
		}

		float get_dwd_close_pos_y(void) {
			return aqua_libc::win_get_dwd_close_pos_y(win);
		}
	};
}
