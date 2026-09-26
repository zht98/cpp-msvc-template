#include <windows.h>
#include <string.h>
#include <stdio.h>

// 模拟按下 Win + D，收起桌面上所有干扰窗口
void MinimizeAllWindows() {
    keybd_event(VK_LWIN, 0, 0, 0);
    keybd_event('D', 0, 0, 0);
    keybd_event('D', 0, KEYEVENTF_KEYUP, 0);
    keybd_event(VK_LWIN, 0, KEYEVENTF_KEYUP, 0);
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

    // 2. 裁剪工作目录并拼接目标程序名
    char* lastSlash = strrchr(exePath, '\\');
    if (lastSlash != NULL) {
        *lastSlash = '\0';         // 截断字符串，使 exePath 变成目录
        strcpy(workDir, exePath);  // 保存工作目录
        
        // 拼接 RTCWCoop.x64.exe 路径
        strcat(exePath, "\\RTCWCoop.x64.exe");
    } else {
        MessageBoxA(NULL, "路径解析失败！", "错误", MB_OK | MB_ICONERROR);
        return 1;
    }

    // 3. 锁定工作目录（保证声音与资源加载正常）
    SetCurrentDirectoryA(workDir);

    // 4. 检查游戏主程序是否存在
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

    // 5. 稳定验证成功的画质/音频兼容启动参数
    snprintf(cmdArgs, sizeof(cmdArgs), 
        "\"%s\" +set fs_game ET +set r_fullscreen 0 +set r_mode -1 +set r_customwidth 1280 +set r_customheight 1024 +set s_driver dsound +set s_initsound 1 +set s_khz 44", 
        exePath);

    // 6. 清场：收起所有后台窗口，并给予 300ms 缓冲，防止与显卡初始化冲突
    MinimizeAllWindows();
    Sleep(300);

    // 7. 初始化进程结构体
    STARTUPINFOA si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_SHOWMAXIMIZED;
    ZeroMemory(&pi, sizeof(pi));

    // 8. 启动游戏主程序
    if (CreateProcessA(
            exePath, 
            cmdArgs, 
            NULL, 
            NULL, 
            FALSE, 
            0, 
            NULL, 
            workDir, 
            &si, 
            &pi)) {
        
        // 启动后直接释放句柄退出，让游戏平稳进行 OpenGL 初始化，不再手动去抓取窗口
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        return 0;
    } else {
        MessageBoxA(NULL, "无法启动 RTCWCoop.x64.exe！", "错误", MB_OK | MB_ICONERROR);
        return 1;
    }
}
