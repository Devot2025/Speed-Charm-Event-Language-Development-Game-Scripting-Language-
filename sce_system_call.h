#ifdef SYSTEM_OS_	

#if defined(_WIN32) && defined(_WIN64)

#include <Windows.h>

#define SYSTEM_CALL_WRITE__(fd, buf, size) \
WriteFile(fd, buf, (DWORD)size, NULL, NULL)

#define SYSTEM_CALL_READ__(fd, buf, size) \
ReadFile(fd, buf, (DWORD)size, NULL, NULL)

#define SYSTEM_CALL_SEEK__(fd, offset, origin) \
SetFilePointer(fd, offset, NULL, origin)

#define SYSTEM_CALL_TELL__(fd) \
SetFilePointer(fd, 0, NULL, FILE_CURRENT)

#define SYSTEM_CALL_CLOSE__(fd) \
CloseHandle(fd)

#define SYSTEM_CALL_OPEN_READ__(fd, path) \
((fd) = CreateFileA(path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL))

#define SYSTEM_CALL_OPEN_WRITE__(fd, path) \
((fd) = CreateFileA(path, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL))

#define SYSTEM_CALL_OPEN_APPEND__(fd, path) \
((fd) = CreateFileA(path, FILE_APPEND_DATA, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL))

#elif defined(__unix__) || defined(__unix) || defined(__APPLE__) || defined(__MACH__) || defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__) || defined(__sun)

#include <unistd.h>
#include <fcntl.h>

#define SYSTEM_CALL_WRITE__(fd, buf, size) \
write(fd, buf, size)

#define SYSTEM_CALL_READ__(fd, buf, size) \
read(fd, buf, size)

#define SYSTEM_CALL_SEEK__(fd, offset, origin) \
lseek(fd, offset, origin)

#define SYSTEM_CALL_TELL__(fd) \
lseek(fd, 0, SEEK_CUR)

#define SYSTEM_CALL_CLOSE__(fd) \
close(fd)

#define SYSTEM_CALL_OPEN_READ__(fd, path) \
((fd) = open(path, O_RDONLY))

#define SYSTEM_CALL_OPEN_WRITE__(fd, path) \
((fd) = open(path, O_WRONLY | O_CREAT | O_TRUNC, 0644))

#define SYSTEM_CALL_OPEN_APPEND__(fd, path) \
((fd) = open(path, O_WRONLY | O_CREAT | O_APPEND, 0644))

#endif // OS branch


#else 
// When it's troublesome (lol)
#include <stdio.h>
#include <stdint.h>

#define SYSTEM_CALL_WRITE__(fp, buf, size) \
fwrite(buf, sizeof(uint8_t), size, fp)

#define SYSTEM_CALL_READ__(fp, buf, size) \
fread(buf, sizeof(uint8_t), size, fp)

#define SYSTEM_CALL_SEEK__(fp, offset, origin) \
fseek(fp, offset, origin)

#define SYSTEM_CALL_TELL__(fp) \
ftell(fp)

#define SYSTEM_CALL_CLOSE__(fp) \
fclose(fp)

#define SYSTEM_CALL_OPEN_READ__(fp, path) \
((fp) = fopen(path, "rb"))

#define SYSTEM_CALL_OPEN_WRITE__(fp, path) \
((fp) = fopen(path, "wb"))

#define SYSTEM_CALL_OPEN_APPEND__(fp, path) \
((fp) = fopen(path, "ab"))

#endif // SYSTEM_OS_