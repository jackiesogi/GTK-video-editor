#include <iostream>
#include <sstream>
#include <string>
#include<stdlib.h>
#include<algorithm>
#include"project.hpp"

using namespace std;

std::string inputfile()
{
    FILE* pipe = popen("zenity --entry --text=\"input your project name\"", "r");
    if (!pipe) {
        std::cerr << "Error opening pipe." << std::endl;
        //return EXIT_FAILURE;
    }

    // 读取zenity的输出
    char buffer[128];
    std::ostringstream result_stream;
    while (fgets(buffer, sizeof(buffer), pipe) != NULL) {
        result_stream << buffer;
    }

    // 关闭zenity进程
    pclose(pipe);

    // 获取用户输入
    string result = result_stream.str();

    // 打印用户输入
    cout << "您输入的内容是: " << result;
    return result;
}


void openfile() {
    FILE* pipe = popen("zenity --file-selection --title=\"select your project\"", "r");
    if (!pipe) {
        std::cerr << "打开管道时发生错误。" << std::endl;
        return;
    }

    char buffer[128];
    std::string result = "";
    while (fgets(buffer, sizeof(buffer), pipe) != NULL) {
        result += buffer;
    }

    // 关闭zenity进程
    pclose(pipe);

    // 去除末尾的换行符
    result.erase(std::remove_if(result.begin(), result.end(), [](char c) { return c == '\n'; }), result.end());

    // 打印用户选择的文件路径
    std::cout << "您选择的文件路径是: " << result << std::endl;
}


int selectproject()
{
    std::string command = "zenity --list --text=\"start\" --radiolist --column=\"\" --column=\"choice\" TRUE \"open a porject\" FALSE \"create a new project\"";
    
    // 打開命令並讀取輸出
    FILE* pipe = popen(command.c_str(), "r");
    if (!pipe) {
        std::cerr << "無法執行 Zenity 命令\n";
        return EXIT_FAILURE;
    }

    char buffer[128];
    std::string result = "";
    
    // 讀取命令輸出
    while (!feof(pipe)) {
        if (fgets(buffer, 128, pipe) != NULL)
            result += buffer;
    }

    // 關閉管道
    pclose(pipe);

    


    // 處理選擇結果
    if (result == "大雄\n") {
        std::cout << "你選擇了大雄\n";
    } else if (result == "多啦A夢\n") {
        std::cout << "你選擇了多啦A夢\n";
    } else if (result == "胖虎\n") {
        std::cout << "你選擇了胖虎\n";
    } else if (result == "小福\n") {
        std::cout << "你選擇了小福\n";
    } else {
        std::cout << "未知選擇\n";
    }


//zenity --list --text="請選擇一個卡通人物" --radiolist --column="" --column="姓名" TRUE "大雄" FALSE "多啦A夢" FALSE "胖虎" FALSE "小福"

}