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

    // 3. 检查 RTCWCoop.x64.exe 是否存在
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

    // 4. 【解决抢焦点】模拟按下 Win + M，将所有文件夹和后台窗口最小化
    keybd_event(VK_LWIN, 0, 0, 0);
    keybd_event('M', 0, 0, 0);
    keybd_event('M', 0, KEYEVENTF_KEYUP, 0);
    keybd_event(VK_LWIN, 0, KEYEVENTF_KEYUP, 0);

    // 延时 100ms 确保窗口最小化动作执行完毕
    Sleep(100);

    // 5. 构建带参数的命令行字符串 (必须传入 +set fs_game ET)
    snprintf(cmdArgs, sizeof(cmdArgs), "\"%s\" +set fs_game ET", exePath);

    // 6. 初始化进程结构体
    STARTUPINFOA si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    // 7. 启动游戏主程序
    if (CreateProcessA(
            exePath,     // 目标可执行文件路径
            cmdArgs,     // 传递命令行参数 "+set fs_game ET"
            NULL,        // 进程安全属性
            NULL,        // 线程安全属性
            FALSE,       // 句柄继承
            0,           // 创建标志
            NULL,        // 环境变量
            workDir,     // 工作目录 (确保声音/绘图驱动 DLL 能加载)
            &si, 
            &pi)) {
        
        // 启动成功，关闭句柄并退出 launcher
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        return 0;
    } else {
        MessageBoxA(NULL, "无法启动 RTCWCoop.x64.exe，请检查系统权限或依赖环境！", "错误", MB_OK | MB_ICONERROR);
        return 1;
    }
}
