# STAR_CPP
**S**tandard tes**T** dat**A** generato**R** for **C** **P**lus **P**lus  
**by bhcbhc001** 地址：<https://github.com/bhcbhc001/star_cpp>  

这是为C++而生的测试数据生成器！  
洛谷的**CYaRon**非常好用，但可惜并不支持C++，于是本人写了这个生成器。

目前支持的功能有 ~~（是的只有这一点点功能）~~ ：
- 建一个随机图
- 建一个随机树
- 从字典生成随机字符串
- 使用以上功能生成的数据和您其他地方下载的测试数据方便地进行程序对拍
- ……

下载：直接去 Github 上下载即可  
教程将会日后在 Github 和洛谷上同步上线！

**使用范例：**  
```cpp
// P1001 测试数据生成器
#include <iostream>
#include "STAR_CPP.h" // 引入 STAR_CPP 头文件
using namespace std;

int main() {
  // 生成10组测试数据
  for (int i = 1; i <= 10; i++) {
    // 绑定输入输出文件，如果没有则自动创建
    STAR_CPP :: IO io("data"+to_string(i)+".in","data"+to_string(i)+".out");
    STAR_CPP :: Random rand; // 创建 Rand 类以使用随机数

    // 向输入文件里写入2个随机数
    io << rand.randint(0, 1000) << " " << rand.randint(0, 1000);
    io.createof("D:/main.exe"); // 根据标程路径（绝对路径）创建输出文件，并写入数据
  }
  return 0;
}
```
**运行前：**  

![](https://cdn.luogu.com.cn/upload/image_hosting/ctn2rt4z.png)

**运行后：**

![](https://cdn.luogu.com.cn/upload/image_hosting/8qj8zs6m.png)

**贡献**  
本项目目前由[bhcbhc001](../user/1368917)完全独自开发。  
当然，如果你想对 STAR_CPP 做出贡献，我（们）乐意接受，也希望有人可以继续提出不足，我（们）将及时改正。

**希望这个小小的头文件可以对你有所帮助！**
