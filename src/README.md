### MOMODeskMind项目文件结构
```
MOMODeskMind/
│
├── src/                     # 源代码目录
│   ├── main.cpp             # 程序入口
│   ├── mainwindow.cpp       # 主窗口实现
│   ├── mainwindow.ui        # 主窗口UI设计
│   ├── petai.cpp            # 宠物AI类
│   ├── petai.h              # 宠物AI类头文件
│   ├── weatherapi.cpp       # 天气API调用类
│   ├── weatherapi.h         # 天气API调用类头文件
│   ├── reminder.cpp         # 日程提醒功能实现
│   ├── reminder.h           # 日程提醒功能头文件
│   ├── messagewidget.cpp    # 待办事项功能实现
│   ├── messagewidget.h      # 待办事项头文件
│   └── settings.cpp         # 设置管理功能
│
├── ui/                      # UI资源文件目录
│   ├── pet.ui               # 宠物UI设计文件
│   └── settings.ui          # 设置页面UI设计文件
│
├── resources/               # 资源文件目录
│   ├── pet_images/          # 宠物外观相关图像文件
│   └── sounds/              # 宠物的声音资源
│
├── CMakeLists.txt           # CMake构建文件
└── README.md                # 项目说明文件
```
### 项目说明

#### 编译与运行

1. **请选择 Qt 6.8 版本** 来进行编译与运行，以确保项目的兼容性,linux可以采用GCC12.2.0x86_64-linux-gnu编译。

2. 在编译和运行之前，您需要**申请一个天气 API 接口**。可以通过以下步骤获取您的 API 密钥：

   - 访问天气 API 提供商的网站（[weatherprovider](https://api.weatherprovider.com)）。
   - 注册并申请 API 密钥。

3. 获取到您的 API 密钥后，您需要进行如下配置：

   - 打开文件 `~/momodesk-mind/src/config/weather-config.json`。
   - 在文件中修改 API 配置，填写您的 API 密钥。

   例如：

   ```json
    {
        "WEATHER_API_KEY": "your-api-key-here",
        "API_URL": "https://api.weatherapi.com/v1/current.json"
    }
    ```
