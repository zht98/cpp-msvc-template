#include <windows.h>
#include <string.h>
#include <stdio.h>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    char exePath[MAX_PATH];
    char workDir[MAX_PATH];
    char cmdArgs[MAX_PATH * 2];

    // 1. 获取当前 launcher 自身的绝对路径
    if (GetModuleFileNameA(NULL, exePath, MAX_PATH) == 0) {
        MessageBoxA(NULL, "无法获取当前程序路径！", "错误", MB_OK | MB_ICONERROR);
        return 1;
    }

    // 2. 找到最后一个反斜杠 '\'，裁剪出工作目录并拼接目标程序名
    char* lastSlash = strrchr(exePath, '\\');
    if (lastSlash != NULL) {
        *lastSlash = '\0';         // 截断字符串，使得 exePath 变为当前目录
        strcpy(workDir, exePath);  // 保存工作目录
        
        // 拼接出 RTCWCoop.x64.exe 的绝对路径
        strcat(exePath, "\\RTCWCoop.x64.exe");
    } else {
        MessageBoxA(NULL, "路径解析失败！", "错误", MB_OK | MB_ICONERROR);
        return 1;
    }

    // 3. 强制锁定工作目录为当前文件夹（解决桌面快捷方式无声）
    SetCurrentDirectoryA(workDir);

    // 4. 检查 RTCWCoop.x64.exe 是否存在
    DWORD dwAttrib = GetFileAttributesA(exePath);
    if (dwAttrib == INVALID_FILE_ATTRIBUTES || (dwAttrib & FILE_ATTRIBUTE_DIRECTORY)) {
        MessageBoxA(
            NULL, 
            "未找到游戏主程序 RTCWCoop.x64.exe！\n\n请确保启动器已放置在游戏 coop 目录下。", 
            "启动失败", 
            MB_OK | MB_ICONERROR
        );
        return 1;
    }

    // 5. 构建带全套兼容性参数的命令行：
    // +set fs_game ET              加载合作模组
    // +set r_mode -1 +set r_customwidth 1280 +set r_customheight 1024 设定分辨率
    // +set r_fullscreen 0 +set r_noborder 1  无边框全屏（防 HDMI 掉信号）
    // +set s_driver dsound +set s_initsound 1 +set s_khz 44  DirectSound 驱动（保证声音）
    snprintf(cmdArgs, sizeof(cmdArgs), 
        "\"%s\" +set fs_game ET +set r_mode -1 +set r_customwidth 1280 +set r_customheight 1024 +set r_fullscreen 0 +set r_noborder 1 +set s_driver dsound +set s_initsound 1 +set s_khz 44", 
        exePath);

    // 6. 初始化进程结构体
    STARTUPINFOA si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_SHOW;
    ZeroMemory(&pi, sizeof(pi));

    // 7. 允许刚启动的进程抢占前台焦点（解决文件夹遮挡）
    AllowSetForegroundWindow(ASFW_ANY);

    // 8. 启动游戏主程序
    if (CreateProcessA(
            exePath,     // 目标可执行文件路径
            cmdArgs,     // 完整兼容命令行参数
            NULL,        // 进程安全属性
            NULL,        // 线程安全属性
            FALSE,       // 句柄继承
            0,           // 创建标志
            NULL,        // 环境变量
            workDir,     // 显式指定工作目录
            &si, 
            &pi)) {
        
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        return 0;
    } else {
        MessageBoxA(NULL, "无法启动 RTCWCoop.x64.exe！", "错误", MB_OK | MB_ICONERROR);
        return 1;
    }
}
