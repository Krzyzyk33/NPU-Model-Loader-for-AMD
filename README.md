# NPU Model Loader for AMD

This project is an ultra-fast, local server compatible with the OpenAI API, designed specifically to fully offload Large Language Model (LLM) inference to the **AMD NPU (XDNA) neural processor**.

By utilizing the NPU directly, you free up your CPU and dedicated/integrated GPU resources. The entire generation process (both prefill and decoding) is executed natively on the coprocessor, saving energy and allowing you to run AI workloads in complete silence.

## Hardware Requirements ("Will this work on my PC?")

**YES**, provided you have an AMD Ryzen processor equipped with a Ryzen AI NPU.

Works flawlessly with:
- **AMD Ryzen™ AI 300 Series (Strix Point)** - 50 TOPS. Achieves maximum speed here (e.g. 10+ tokens/s decoding).
- **AMD Ryzen™ 8040 Series (Hawk Point)** - 16 TOPS.
- **AMD Ryzen™ 7040 Series (Phoenix)** - 10 TOPS.

*Note for older architectures (7040/8040)*: You might need to use a smaller model or adjust the ONNX format due to lower shared memory availability compared to Strix Point.

## Prerequisites (What you need to run this app)

Before you begin, ensure you have the following installed on your Windows machine:
1. **Windows 11** with the latest AMD NPU drivers.
2. **Ryzen AI Software 1.7.1**: The official AMD libraries must be configured.
3. **C++ Build Tools**: Visual Studio 2022/2026 Community with "Desktop development with C++" workload installed (needs MSVC and CMake).
4. **Git** (optional, but recommended for cloning).

## Installation and Setup

This repository contains pure C++ source code to give you full control over your environment.

1. **Download Dependencies**: Before compiling, you must download the official ONNX Runtime GenAI libraries for Ryzen AI. Run the included script:
   ```cmd
   download_deps.bat
   ```
   *(Note: The script downloads a `deps.zip` containing `libs` and `include` directories from the Releases page).*

2. **Build the Project**: Compile the source code using a C++17 compatible compiler (e.g., MSVC) and CMake:
   ```cmd
   cd npu_chat
   mkdir build && cd build
   cmake .. -A x64
   cmake --build . --config Release
   ```

3. **Get an NPU Model**: Download a compatible ONNX format model (e.g., `amd/Qwen2.5-Coder-7B-Instruct_rai_1.7.1_npu_4K`) from HuggingFace to the `models` directory.

4. **Run the Server**: Start the local API server and provide the path to your downloaded model:
   ```cmd
   npu_chat.exe -m "..\..\..\models\Qwen2.5-Coder-7B-4K"
   ```

## Integration and GUI Apps

The server perfectly emulates the OpenAI cloud. It is fully compatible with any modern local LLM interface (e.g., **AnythingLLM**, **SillyTavern**, **Chatbox**, **LM Studio UI**, or IDE extensions like **Continue.dev** in VSCode).

### Setup for any UI App:
1. Ensure the `npu_chat.exe` server is running.
2. Go to your application's **AI Providers** or **Connections** settings.
3. Select the type: `OpenAI API` (or `Custom OpenAI`, `Local OpenAI`).
4. Enter the **Base URL** (API Endpoint):
   ```text
   http://127.0.0.1:8085/v1
   ```
5. Leave the API Key empty or enter anything (e.g., `none`), as this local server does not require authentication.
6. Enjoy 100% NPU-powered inference!

## Examples

In the `examples` folder, you will find simple integration scripts for **Python** and **Node.js**. They demonstrate how to use the official `openai` SDK with **live streaming (Server-Sent Events)** to communicate with your NPU!

---
Created as part of the quest for perfect NPU software. MIT License.
