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

    // 3. 锁定工作目录（解决音频驱动 DLL 找不到导致的无声问题）
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

    // 5. 构建带兼容性参数的命令行
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

    // 7. 允许刚启动的子进程抢占前台焦点
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
            workDir,     // 工作目录
            &si, 
            &pi)) {
        
        // 【关键修复：避免 HDMI 丢信号】
        // 给予游戏 500ms 创建窗口和初始化 DWM 渲染，并强行赋予前台焦点
        Sleep(500);
        AllowSetForegroundWindow(pi.dwProcessId);
        
        // 尝试获取窗口句柄并置顶
        HWND hwndGame = FindWindowA(NULL, "RTCWCoop");
        if (hwndGame) {
            SetForegroundWindow(hwndGame);
            ShowWindow(hwndGame, SW_SHOW);
        }

        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        return 0;
    } else {
        MessageBoxA(NULL, "无法启动 RTCWCoop.x64.exe！", "错误", MB_OK | MB_ICONERROR);
        return 1;
    }
}
