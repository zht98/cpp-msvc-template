#include <windows.h>
#include <shellapi.h>
#include <stdio.h>

int main() {
    const char* workDir = "I:\\SteamLibrary\\steamapps\\common\\RealRTCW";
    const char* exePath = "I:\\SteamLibrary\\steamapps\\common\\RealRTCW\\RealRTCW.x64.exe";

    // 1. 设置工作目录
    if (!SetCurrentDirectoryA(workDir)) {
        printf("Error: Directory not found.\n");
        return 1;
    }

    // 2. 使用绝对路径启动游戏
    HINSTANCE result = ShellExecuteA(NULL, "open", exePath, NULL, workDir, SW_SHOW);

    // ShellExecute 返回值大于 32 代表启动成功
    if ((INT_PTR)result <= 32) {
        printf("Error: Failed to launch game.\n");
        return 1;
    }

    return 0;
}
