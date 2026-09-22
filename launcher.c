#include <windows.h>
#include <string.h>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    char exePath[MAX_PATH];
    char workDir[MAX_PATH];

    // 1. 获取 launcher.exe 自身的绝对路径
    if (GetModuleFileNameA(NULL, exePath, MAX_PATH) == 0) {
        MessageBoxA(NULL, "无法获取当前程序路径！", "错误", MB_OK | MB_ICONERROR);
        return 1;
    }

    // 2. 找到最后一个反斜杠 '\'，裁剪出文件夹目录路径
    char* lastSlash = strrchr(exePath, '\\');
    if (lastSlash != NULL) {
        *lastSlash = '\0';         // 截断字符串，使得 exePath 变为当前目录
        strcpy(workDir, exePath);  // 保存工作目录
        
        // 拼接出主程序的绝对路径
        strcat(exePath, "\\RealRTCW.x64.exe");
    } else {
        MessageBoxA(NULL, "路径解析失败！", "错误", MB_OK | MB_ICONERROR);
        return 1;
    }

    // 3. 【核心保护】检查 RealRTCW.x64.exe 是否存在
    DWORD dwAttrib = GetFileAttributesA(exePath);
    if (dwAttrib == INVALID_FILE_ATTRIBUTES || (dwAttrib & FILE_ATTRIBUTE_DIRECTORY)) {
        // 文件不存在或为文件夹，弹出提示框并退出
        MessageBoxA(
            NULL, 
            "未找到游戏主程序 RealRTCW.x64.exe！\n\n请确保 launcher.exe 已放置在 RealRTCW 游戏根目录下。", 
            "启动失败", 
            MB_OK | MB_ICONERROR
        );
        return 1;
    }

    // 4. 初始化进程结构体
    STARTUPINFOA si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    // 5. 启动游戏主程序
    if (CreateProcessA(
            exePath,     // 目标可执行文件路径
            NULL,        // 命令行参数
            NULL,        // 进程安全属性
            NULL,        // 线程安全属性
            FALSE,       // 句柄继承
            0,           // 创建标志
            NULL,        // 环境变量
            workDir,     // 工作目录
            &si, 
            &pi)) {
        
        // 启动成功，关闭句柄并退出 launcher
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        return 0;
    } else {
        MessageBoxA(NULL, "无法启动 RealRTCW.x64.exe，请检查系统权限或依赖环境！", "错误", MB_OK | MB_ICONERROR);
        return 1;
    }
}
