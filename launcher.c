#include <windows.h>
#include <string.h>
#include <stdio.h>

// 强制将指定窗口拉到系统最前台并获取焦点的函数
void ForceForegroundWindow(HWND hwnd) {
    DWORD currentThreadID = GetCurrentThreadId();
    DWORD foregroundThreadID = GetWindowThreadProcessId(GetForegroundWindow(), NULL);

    if (currentThreadID != foregroundThreadID) {
        AttachThreadInput(currentThreadID, foregroundThreadID, TRUE);
        SetForegroundWindow(hwnd);
        SetFocus(hwnd);
        AttachThreadInput(currentThreadID, foregroundThreadID, FALSE);
    } else {
        SetForegroundWindow(hwnd);
        SetFocus(hwnd);
    }

    if (IsIconic(hwnd)) {
        ShowWindow(hwnd, SW_RESTORE);
    } else {
        ShowWindow(hwnd, SW_SHOW);
    }
}

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

    // 3. 锁定工作目录（保证声音正常）
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

    // 5. 保持验证成功的画质/音频兼容启动参数
    snprintf(cmdArgs, sizeof(cmdArgs), 
        "\"%s\" +set fs_game ET +set r_fullscreen 0 +set r_mode -1 +set r_customwidth 1280 +set r_customheight 1024 +set s_driver dsound +set s_initsound 1 +set s_khz 44", 
        exePath);

    // 6. 初始化进程结构体
    STARTUPINFOA si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_SHOWMAXIMIZED;
    ZeroMemory(&pi, sizeof(pi));

    // 7. 启动游戏主程序
    if (CreateProcessA(
            exePath,     // 目标可执行文件路径
            cmdArgs,     // 命令行参数
            NULL,        // 进程安全属性
            NULL,        // 线程安全属性
            FALSE,       // 句柄继承
            0,           // 创建标志
            NULL,        // 环境变量
            workDir,     // 工作目录
            &si, 
            &pi)) {
        
        // 8. 循环等待游戏创建窗口，并强行将焦点拉到最前台
        HWND hwndGame = NULL;
        for (int i = 0; i < 20; i++) { // 最多尝试等待 2 秒 (20 * 100ms)
            Sleep(100);
            
            // 查找 RTCWCoop 游戏窗口
            hwndGame = FindWindowA(NULL, "RTCWCoop");
            if (!hwndGame) {
                // 如果按窗口名没找到，按窗口类名查找
                hwndGame = FindWindowA("RTCWCoop", NULL);
            }

            if (hwndGame) {
                // 找到窗口，使用线程附加技术强行夺取前台焦点
                ForceForegroundWindow(hwndGame);
                break;
            }
        }

        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        return 0;
    } else {
        MessageBoxA(NULL, "无法启动 RTCWCoop.x64.exe！", "错误", MB_OK | MB_ICONERROR);
        return 1;
    }
}
