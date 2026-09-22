#include <windows.h>
#include <shellapi.h>

int main() {
    // 1. 设置工作目录为游戏文件夹
    const char* workDir = "I:\\SteamLibrary\\steamapps\\common\\RealRTCW";
    SetCurrentDirectoryA(workDir);

    // 2. 启动游戏程序
    ShellExecuteA(NULL, "open", "RealRTCW.x64.exe", NULL, workDir, SW_SHOW);

    return 0;
}
