## 项目介绍 Project Overview

一个轻量的作文批改项目，调用本地Ollama模型来评分（话说都下载大模型了还轻量吗？）

A lightweight writing scorer that calls local Ollama models to give score(Could it really be lightweight after downloading models?)

当前正在开发阶段...

Under development...

目前应该只适配Windows，因为使用了raylib以及system()

Currently, it is only compatible with Windows because I used raylib and system() function

logs文件夹可以安全删除，但是如果你想看看程序的爆炸案例和成功案例可以留着

The logs folder could be deleted safely, but if you want to see the cases which the program explodes and the ones that program successes, you could save that

## 如何使用 How to Use

首先，确保你准备了2个非思考大模型和2个思考大模型(一般标注于ollama官网)

First, make sure that you've pulled 2 non-thinking models and 2 thinking models(which is usually indicated in its description on ollama website)

将非思考模型名称填入config.json的`"scoreModel1"`与`"scoreModel2"`字段，将思考模型名称填入`"scoreModelThinking"`与`"commentingModel"`字段

Enter your non-thinking models' names into the fields `"scoreModel1"` and `"scoreModel2"` in config.json,and thinking models into the fields `"scoreModelThinking"` and `"commentingModel"`

然后将你的写作填入`writing.txt`

Then enter you writing into `writing.txt`

最后运行exe文件，点击"score"字样的按钮，就可以开始评分了

Finally run the executable, click the button with text "score", the scoring will begin

所有的程序运行信息会详细记录在日志里，你可以通过`./logs/log_[日期]`找到日志（虽然我的日志也有bug...）

All the running status and messages will be logged, you can find them in path `./logs/log_[date]`,(even though my logs also have bugs...)

## 项目的优势和劣势 Advantages and Disadvantages of this program

首先，这个项目很轻，目前exe只有约1MB的大小

First of all, this program is lightweight, up to now, this executable is only about 1MB

其次，我尽力用程序设计使得它节省时间。我将评分，段评，总评分为3个线程（`std::thread`），平均运行时间~450s（但是上下浮动较大）

Second, I tried my best to design the program for efficiency. I splited the scoring, paragraph commenting and whole commenting into 3 threads(`std::thread`), average score time ~450s(but it floats up and down a lot)

劣势部分在于提示词设计，所以有时，大模型会给出非预期的输出导致崩溃（如果你在这方面比较强，请提交一个issue!）

The disadvantage part lies in **prompt designing**, so sometimes, the model will give unexpected output and cause a crash(if you are good at it, feel free to submit an issue!)

## 故障排除 Troubleshooting

常见故障：

common issues:

1.模型名称错误

1.model name error

**这是最常见的错误**，请检查你是否将`-`打成了`_`或其他输入错误

**This is the most common error**, please check if you accidentally typed `-` as `_` or other typing error

2.模型输出不符合预期

2.unexpected model output

是的，这偶尔会发生，如果模型的输出过于诡异，可能导致程序无法解析

Yes, this happens from time to time, if the model's output is too weird, the program won't parse  it correctly

3.在配置文件中出现过多`%s`

3.too much `%s` in config

检查`config.json`，`scoringPrompt`和`formalPrompt`中均应当**恰好**包含2个`%s`

check your `config.json`, the fields `scoringPrompt` and `formalPrompt` should contain **exactly** 2 `%s`

如果你发现没有对应日期的日志，但找到了`console.log`，而且你确定并非上面的情况，那么恭喜！你发现了一个除上面以外会让程序崩溃的bug

If you didn't find the log of the date,but find `console.log`, and you are sure that none of above is your situation, then congrats! You found a crash bug that didn't included above

请提交一个issue

Please submit an issue

\*tip:
在你

Before you

```cpp
try{
    throw issue;
}
catch(reply)
{
    turn(😡)
}
```

之前

\[translated before]

请先检查日志，大部分的问题可能是偶然事件

Please check the log first -- most of the problems could be just temporary glitches

## 库依赖 Lib dependences

使用`raylib`图形库，[网址](https://www.raylib.com/)

Using `raylib`, [website](https://www.raylib.com/)

- **许可证**：Raylib 使用 **zlib/libpng License**。
- **版权**：© Ramon Santamaria and contributors
- **项目地址**：<https://github.com/raysan5/raylib>

- **LICENSE**: Raylib uses **zlib/libpng License**.
- **COPYRIGHT**: © Ramon Santamaria and contributors
- **ADDRESS**: <https://github.com/raysan5/raylib>

请前往`NOTICE.md`获取更多信息

Please go to `NOTICE.md` for more information

## 闲话 Gossip

我的代码风格很烂说实话:(

My code style sucks actually:(

我英语也不好，所以如果这个README里有翻译错误，欢迎捉虫（）

My English is also imperfect, so if there's any spelling or grammar error, feel free to catch my bugs()