#include <windows.h>
#include <stdio.h>

int main() {
    const char* workDir = "I:\\SteamLibrary\\steamapps\\common\\RealRTCW";
    const char* exePath = "I:\\SteamLibrary\\steamapps\\common\\RealRTCW\\RealRTCW.x64.exe";

    STARTUPINFOA si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    // 使用 CreateProcess 启动程序
    if (CreateProcessA(
            exePath,     // 可执行文件路径
            NULL,        // 命令行参数
            NULL,        // 进程安全属性
            NULL,        // 线程安全属性
            FALSE,       // 句柄继承
            0,           // 创建标志
            NULL,        // 使用父进程环境变量
            workDir,     // 设置工作目录
            &si, 
            &pi)) {
        
        // 关闭不必要的句柄
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        return 0;
    } else {
        printf("Error launching process: %lu\n", GetLastError());
        return 1;
    }
}
