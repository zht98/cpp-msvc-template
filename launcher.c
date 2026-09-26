#include <windows.h>
#include <string.h>
#include <stdio.h>

// 模拟按下 Win + D 键，一键最小化桌面上所有的窗口（包括浏览器、文件夹等）
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

    // 6. 【核心干预】：向系统发送 Win + D，强制最小化包括浏览器在内的所有前台窗口
    MinimizeAllWindows();
    Sleep(200); // 给系统 200ms 完成所有窗口收起动画

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
        
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        return 0;
    } else {
        MessageBoxA(NULL, "无法启动 RTCWCoop.x64.exe！", "错误", MB_OK | MB_ICONERROR);
        return 1;
    }
}
