#ifndef _PROC_OS_SYSTEM_H_
#define _PROC_OS_SYSTEM_H_

#if defined(_WIN32) || defined(_WIN64)

#else if()
#endif // _WIN32
typedef struct system_os_handler_t {
	void* handle_;
}system_os_handler_t;
void process_release_game_os();
void sec_sleep(double time_);
#endif // !_PROC_OS_SYSTEM_H_
