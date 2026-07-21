#include "proc_os_system.h"
#if defined(_WIN32) || defined(_WIN64)
#include <Windows.h>
#include <stdint.h>
#define sleep_sec_system(time) Sleep((DWORD)(time))
#define connect_free_system_console() (void)FreeConsole()
#define view_system_console_hide() ShowWindow(GetConsoleWindow(), SW_HIDE)
#define view_system_console_show() ShowWindow(GetConsoleWindow(), SW_SHOW)
#define get_timer() 0
#else
#include <unistd.h>
#endif
#define TARGET_ONE_SEC_TIME 1.0

typedef struct system_handler_fps_t {
	double delta_fps_;
	double time_;
	double last_time_;
	double frame_time_;
	uint32_t fps_;
	uint32_t fps_count_;
	uint32_t now_fps_;
	uint32_t fps_status_;
}system_handler_fps_t;
typedef struct system_os_handler_access_t {
	system_handler_fps_t fps_;
}system_os_handler_access_t;
void sec_sleep(double time_) {
	sleep_sec_system(time_);
}

void process_release_game_os() {
	connect_free_system_console();
}
void init_fps_handler(system_os_handler_t* sys_, uint32_t target_fps) {
	system_os_handler_access_t* sys_acc_ =
		(system_os_handler_access_t*)sys_->handle_;
	double target_frame_time = 1.0 / (double)target_fps;
	sys_acc_->fps_.fps_ = target_fps;
	sys_acc_->fps_.frame_time_ = target_frame_time;
}
void control_fps_timer(system_os_handler_t * sys_) {
	system_os_handler_access_t* sys_acc_ =
		(system_os_handler_access_t*)sys_->handle_;
	double c_time_ = get_timer();
	double delta_time_ = c_time_ - sys_acc_->fps_.last_time_;
	double idle_time_ = sys_acc_->fps_.delta_fps_ - delta_time_;
	if (idle_time_ > 0.0) {

		if (sys_acc_->fps_.frame_time_ + delta_time_ + idle_time_ > TARGET_ONE_SEC_TIME) {
			idle_time_ =
				TARGET_ONE_SEC_TIME - sys_acc_->fps_.frame_time_ - delta_time_;
		}
		if (idle_time_ > 0.0) {
			sec_sleep(idle_time_ * 1000.0f);
			c_time_ = get_timer();
			delta_time_ = c_time_ - sys_acc_->fps_.last_time_;
		}
	}
	sys_acc_->fps_.last_time_ = c_time_;
	sys_acc_->fps_.frame_time_ += delta_time_;
	sys_acc_->fps_.fps_count_++;
	if (sys_acc_->fps_.frame_time_ > TARGET_ONE_SEC_TIME) {
		sys_acc_->fps_.frame_time_ = 0.0;
		sys_acc_->fps_.now_fps_ = sys_acc_->fps_.fps_count_;
		sys_acc_->fps_.fps_count_ = 0;
	}
}