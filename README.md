## 项目介绍 Project Description

一个轻量的作文批改项目，调用本地ollama模型来评分~~（话说都下载大模型了还轻量吗？）~~
A lightweight writing scorer, call local ollama models to give score~~(Could it really be lightweight after downloading models?)~~

当前正在开发阶段...
Under development...

## 如何使用 How to Use

首先，确保你准备了2个非思考大模型和2个思考大模型(一般标注于ollama官网)
First, make sure that you've pulled 2 non-thinking models and 2 thinking models(which is usually indicated in its description on ollama website)

将非思考模型名称填入config.json的`"scoreModel1"`与`"scoreModel2"`字段，将思考模型名称填入`"scoreModelThinking"`与`"commentingModel"`字段
Enter your non-thinking models' names to the fields `"scoreModel1"` and `"scoreModel2"` in config.json,and thinking models into the fields `"scoreModelThinking"` and `"commentingModel"`

然后运行exe文件，点击"score"字样的按钮，就可以开始评分了
Then run the executable, click the button with text "score", the scoring will begin

所有的程序运行信息会详细记录在日志里，你可以通过`./logs/log_[日期]`找到日志
All the running status and messages will be logged, you can find them in path `./logs/log_[date]`

## 故障排除 Troubleshooting

常见故障：
common accidents:

1.模型名称错误
1.model name error

**这是最常见的错误**，请检查你是否将`-`打成了`_`或其他输入错误
**This is the most common situation**, please check if you accidentally typed `-` as `_` or other typing error

2.模型输出不符合预期
2.model output is not expected

是的，这偶尔会发生，如果模型的输出过于诡异，可能导致程序无法解析
Yes, this happens from time to time, if the model's output is too weird, the program won't understand it

如果你发现没有对应日期的日志，但找到了`console.log`，而且你确定并非上面的情况，那么恭喜！你发现了一个除上面以外会让程序崩溃的bug
If you didn't find the log of the date,but find `console.log`, and you are sure that none of above is your situation, then congrats! You found an bug that make the program break down and didn't included above

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
Please check the log first, most of the problems could be accidents

## 闲话 Gossip

我的代码风格很烂说实话:(
My code style sucks actually:(