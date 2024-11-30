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
├── PetAIProject.pro         # Qt项目文件
└── README.md                # 项目说明文件
```