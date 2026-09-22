# 微软官方 C/C++ 编译器 (MSVC) 开发模版

这是一个基于微软官方 C/C++ 编译器 (MSVC) 和 CMake 构建工具搭建的 C/C++ 项目模版。

## 🛠️ 环境要求

- **操作系统**: Windows 10 / 11
- **编译器**: MSVC (Visual Studio 2022 / Build Tools 2022)
- **构建工具**: CMake (3.20+)

## 🚀 快速开始

### 1. 使用 Developer Command Prompt
打开 `Developer Command Prompt for VS 2022` 并运行以下命令：

```cmd
# 配置项目
cmake -B build -G "Visual Studio 17 2022" -A x64

# 编译项目
cmake --build build --config Release
