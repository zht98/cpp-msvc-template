#include <windows.h>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    const char* workDir = "I:\\SteamLibrary\\steamapps\\common\\RealRTCW";
    const char* exePath = "I:\\SteamLibrary\\steamapps\\common\\RealRTCW\\RealRTCW.x64.exe";

    STARTUPINFOA si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    // 使用 CreateProcessA 启动目标程序并指定工作目录
    if (CreateProcessA(
            exePath,     // 可执行文件全路径
            NULL,        // 命令行参数
            NULL,        // 进程安全属性
            NULL,        // 线程安全属性
            FALSE,       // 句柄继承
            0,           // 创建标志
            NULL,        // 使用默认环境变量
            workDir,     // 设置工作目录
            &si, 
            &pi)) {
        
        // 成功启动后关闭不必要的句柄
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        return 0;
    }

    return 1;
}
