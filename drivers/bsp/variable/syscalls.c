/**
 * @file syscalls.c
 * @brief 系统调用函数实现
 */

#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include "variable.h"

int _isatty(int fd);
int _read(int fd, char *ptr, int len);
int _write(int fd, char *ptr, int len);
int _close(int fd);
int _lseek(int fd, int ptr, int dir);
int _fstat(int fd, struct stat *st);
int _getpid(void);
int _kill(int pid, int sig);
int _getentropy(void *buffer, size_t length);

/**
 * @brief 判断文件描述符是否为终端
 * @param fd 文件描述符
 * @return 如果是终端返回1，否则返回0
 */
__WEAKDEF int _isatty(int fd)
{
    if (fd >= STDIN_FILENO && fd <= STDERR_FILENO)
        return 1;

    errno = EBADF;
    return 0;
}

/**
 * @brief 关闭文件
 * @param fd 文件描述符
 * @return 成功返回0，失败返回-1
 */
__WEAKDEF int _close(int fd)
{
    if (fd >= STDIN_FILENO && fd <= STDERR_FILENO)
        return 0;

    errno = EBADF;
    return -1;
}

/**
 * @brief 设置文件偏移量
 * @param fd 文件描述符
 * @param ptr 偏移量
 * @param dir 偏移方向
 * @return 始终返回-1，表示不支持
 */
__WEAKDEF int _lseek(int fd, int ptr, int dir)
{
    (void)fd;
    (void)ptr;
    (void)dir;

    errno = EBADF;
    return -1;
}

/**
 * @brief 获取文件状态
 * @param fd 文件描述符
 * @param st 文件状态结构体指针
 * @return 成功返回0，失败返回-1
 */
__WEAKDEF int _fstat(int fd, struct stat *st)
{
    if (fd >= STDIN_FILENO && fd <= STDERR_FILENO)
    {
        st->st_mode = S_IFCHR;
        return 0;
    }

    errno = EBADF;
    return 0;
}

/**
 * @brief 获取进程ID
 * @return 固定返回1
 */
__WEAKDEF int _getpid(void)
{
    return 1;
}

/**
 * @brief 发送信号给进程
 * @param pid 进程ID
 * @param sig 信号
 * @return 始终返回-1，表示不支持
 */
__WEAKDEF int _kill(int pid, int sig)
{
    errno = EINVAL;
    return -1;
}

/**
 * @brief 获取随机数据
 * @param buffer 数据缓冲区
 * @param length 数据长度
 * @return 始终返回-1，表示不支持
 */
__WEAKDEF int _getentropy(void *buffer, size_t length)
{
    errno = ENOSYS;
    return -1;
}

/**
 * @brief 写入数据
 * @param fd 文件描述符
 * @param ptr 数据指针
 * @param len 数据长度
 * @return 成功返回写入的字节数，失败返回-1
 */
__WEAKDEF int _write(int fd, char *ptr, int len)
{
    // 检查文件描述符是否为标准输出或标准错误
    if (fd != STDOUT_FILENO && fd != STDERR_FILENO && !g_bUsartInitialized)
    {
        errno = EBADF;
        return -1;
    }

    USART_Send((uint8_t *)ptr, (uint16_t)((len > UINT16_MAX) ? UINT16_MAX : len));

    return len;
}

/**
 * @brief 读取数据
 * @param fd 文件描述符
 * @param ptr 数据指针
 * @param len 数据长度
 * @return 成功返回读取的字节数，失败返回-1
 */
__WEAKDEF int _read(int fd, char *ptr, int len)
{
    // 检查文件描述符是否为标准输入
    if (fd != STDIN_FILENO && !g_bUsartInitialized)
    {
        errno = EBADF;
        return -1;
    }

    // TODO: 实现从USART读取数据的功能
    

    return len;
}

