//#define DEBUG

#ifdef DEBUG

#include "raylib.h"
#include "buttons.h"
#include <iostream>
#include <string>
int main() {
    const int screenWidth = 800;
    const int screenHeight = 600;
    InitWindow(screenWidth, screenHeight, "Test");
    SetTargetFPS(60);

    // ============ 终极验证代码 START ============
    TraceLog(LOG_INFO, "开始终极验证：生成、保存并加载图像...");

    // 1. 在内存中生成一个简单的彩色图像
    Image testImage = GenImageColor(200, 100, BLUE);
    TraceLog(LOG_INFO, "内存图像生成成功。");

    // 2. 将其保存为BMP文件到当前目录（或临时目录）
    std::string testFilePath = "debug_test_image.bmp";
    // 注意：确保你有写权限，这里保存到当前工作目录
    ExportImage(testImage, testFilePath.c_str());
    UnloadImage(testImage); // 卸载内存图像，现在只依赖文件
    TraceLog(LOG_INFO, "图像已保存至文件: %s", testFilePath.c_str());

    // 3. 立即尝试加载这个刚刚创建的文件
    TraceLog(LOG_INFO, "正在尝试加载刚保存的文件...");
    Texture2D testTexture = LoadTexture(testFilePath.c_str());

    if (testTexture.id == 0) {
        TraceLog(LOG_FATAL, "终极验证失败：无法加载Raylib自己生成的BMP文件！");
        TraceLog(LOG_FATAL, "这证明MSYS2的Raylib库或相关DLL存在严重问题。");
        system("pause");
        // 程序可以在这里暂停或退出
        CloseWindow();
        return -1;
    } else {
        TraceLog(LOG_INFO, "终极验证成功！加载的纹理ID: %u", testTexture.id);
        // 简单绘制一下，证明它真的能用
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawTexture(testTexture, 100, 100, WHITE);
        DrawText("SUCCESS: Self-made BMP loaded!", 120, 220, 20, DARKGRAY);
        EndDrawing();
        WaitTime(2); // 显示2秒
        UnloadTexture(testTexture);
    }
    TraceLog(LOG_INFO, "终极验证结束。");
    // ============ 终极验证代码 END ============
    UnloadTexture(testTexture);
    CloseWindow();
    return 0;
}

#else
#define WIN32_LEAN_AND_MEAN
#define NOGDI
#define NOUSER
#define NOCOMM
#define NOMINMAX

#include "raylib.h"
#include "buttons.h"
#include "callai.h"
#include "split.h"
#include <iostream>
#include <fstream>
#include <string>

std::string getDamagePrompt="角色与任务：\n请你扮演一位资深语文教育专家兼评分规则制定者。\n你的任务是：基于对“写作常见问题”的深刻理解，为下方提供的《负面特征模式库》中的每一项，评估其对一篇中学作文质量的伤害程度，并为之设定一个合理的扣分权重（单次出现）。\n核心指令：\n理解特征：请仔细阅读下表，理解每种“负面特征模式”所对应的“问题本质”。\n评估伤害：请从结构破坏性、内容无效性、立意干扰性三个维度，评估每种问题的严重程度。\n设定权重：\n权重范围为 -0.1 至 -3.0分。\n最轻微的口语赘余可接近-0.1，最严重的离题或逻辑硬伤可接近-3.0。\n权重必须体现差异性：例如，“离题冗余”的扣分应显著重于“常识堆砌”。\n**输出格式：共一行一个负浮点数，表示总计扣分权重，除此之外不要输出任何内容，除此之外不要输出任何内容，除此之外不要输出任何内容！**\n负面特征模式库：\n模式编号	负面特征模式	示例关键词/句法模式	问题本质  扣分权重\n1	空洞评价	“我觉得…”、“真是…”、“特别…”	缺乏具体细节与论证，内容浮于表面。  -0.5\n2	逻辑断桥	“对了，…”、“还想到…”、“突然想起…”	思维跳跃，破坏段落与篇章的连贯性与逻辑链。  -1.0\n3	离题冗余	与核心主题明显无关的个人琐事叙述	材料选择彻底失败，内容无效，严重稀释主题。  -1.5\n4	常识堆砌	“盐是NaCl”、“可以化冰”、“用来刷牙”	提供无新意的公共知识，缺乏个人视角或信息增量。  -2.0\n5	强行升华	“可见…”、“所以我们要…”、“这说明了…”（与前文脱节时）	立意与主体内容脱节，结论生硬、牵强，破坏整体真实感。  -0.5\n6	口语化冗余	“这个东西吧”、“就是说”、“那个时候吧”	语言啰嗦，不简洁，损害书面语的严谨与美感。  -1.0\n";
int main(void)
{
    SetConsoleOutputCP(CP_UTF8);
    const int screenWidth=1600;
    const int screenHeight=900;
    InitWindow(screenWidth,screenHeight,"Ｔｈｅ　ａｗｅｓｏｍｅ　ｓｃｏｒｉｎｇ　ｓｙｓ");
    SetTargetFPS(60);
    Rectangle testbtnRect={screenWidth/2-100,screenHeight/2-50,200,100};
    std::string WorkingDirectory=GetWorkingDirectory();
    Texture2D btnTexture=LoadTexture((WorkingDirectory+"/assets/button.bmp").c_str());
    Button testbtn(testbtnRect,btnTexture);
    while(!WindowShouldClose())
    {
        updateMousePos();
        if(testbtn.isLeftClicked()){
            TraceLog(LOG_INFO,"testbtn is clicked");
            std::ifstream file("./writing.txt");
            std::string writing="";
            if(file.is_open()){
                std::string line;
                while(std::getline(file,line)){
                    writing+=line+"\n";
                }
                file.close();
            }
            std::vector<std::string> paragraphs=split(writing,"\n");
            TraceLog(LOG_INFO,"writing: %s",writing.c_str());
            for(std::string paragraph:paragraphs){
                TraceLog(LOG_INFO,"paragraph: %s",paragraph.c_str());
                TraceLog(LOG_INFO,"\n\n");
            }
            std::string title=paragraphs[0];
            paragraphs.erase(paragraphs.begin());
            std::vector<std::string> paragraphScoring;
            for(std::string paragraph:paragraphs){
                TraceLog(LOG_INFO,"paragraph: %s",paragraph.c_str());
                TraceLog(LOG_INFO,"\n\n");
            }
            Response damageResponse=callai("qwen3:14b",getDamagePrompt+"下为文章\n"+writing,"",0.9,0.6,512,1024);
            Response scoreResponse1=callai("qwen3:14b","请以老师的身份，以高考标准**严格**，根据扣分权重："+damageResponse.getShortContent()+"，此值越小，分数应当越小，为下面文章打分，满分600分，得分请在270~370或470~570之间，如果文章较好，选择后一段区间，否则选择前一段，**眼光批判些**，不要给一篇差的文章高分，文章："+writing+"\n注意你必须输出一个整数表示得分，**不要输出任何其他东西，不要对你的评分做出解释，更不要分块打分，只输出一个整数，只输出一个整数，只输出一个整数！**","",1.2,0.6,512,1024);
            Response scoreResponse2=callai("qwen3:14b","请以老师的身份，以高考标准**严格**，根据扣分权重："+damageResponse.getShortContent()+"，此值越小，分数应当越小，为下面文章打分，满分600分，得分请在400~500之间，但要结合文章内容，文章："+writing+"\n注意你必须输出一个整数表示得分，**不要输出任何其他东西，不要对你的评分做出解释，更不要分块打分，只输出一个整数，只输出一个整数，只输出一个整数！**","",1.2,0.6,256,1024);
            Response scoreResponse3=callai("qwen3:14b","请以老师的身份，以高考标准**严格**，根据扣分权重："+damageResponse.getShortContent()+"，此值越小，分数应当越小，为下面文章打分，满分600分，得分请在420~480之间，但要结合文章内容，文章："+writing+"\n注意你必须输出一个整数表示得分，**不要输出任何其他东西，不要对你的评分做出解释，更不要分块打分，只输出一个整数，只输出一个整数，只输出一个整数！**","",1.2,0.6,256,1024);
            int score=std::stoi(scoreResponse1.getShortContent());
            int score2=std::stoi(scoreResponse2.getShortContent());
            int score3=std::stoi(scoreResponse3.getShortContent());
            TraceLog(LOG_INFO,"score: %d",score);
            TraceLog(LOG_INFO,"score2: %d",score2);
            TraceLog(LOG_INFO,"score3: %d",score3);
            for(auto para:paragraphs){
                if(para==""){
                    continue;
                }
                std::string prompt="请为下面这个段落写段评，标题为："+title+"，段落为："+para+"\n以markdown格式输出，但不需要\"```markdown\"和\"```\"，也不要包含标题，\"段评：\"，注意你也必须写出这一段的不足之处，不能一味夸赞，不要包含其他内容，格式：优点+\"但是\"+不足处";
                Response response=callai("qwen3:14b",prompt,"",1.2,0.6,512,1024);
                TraceLog(LOG_INFO,"response: %s",response.getShortContent().c_str());
                paragraphScoring.push_back(response.getShortContent());
            }
            Response responseWholeWriting=callai("qwen3:14b","请为下面文章写总评，标题为："+title+"，文章为："+writing+"\n以markdown格式输出，但不需要\"```markdown\"和\"```\"，也不要包含标题，\"段评：\"，注意你也必须写出这篇文章的不足之处，不能一味夸赞，不要包含其他内容，格式：优点+\"但是\"+不足处，请都写得长一点","",1.2,0.6,1024,2048);
            std::ofstream outfile("./scoring.md");
            outfile<<"# "<<title<<"       得分："<<(score+score2+score3)/30.0<<"/60\n\n";
            try{
                for(int i=0;i<paragraphScoring.size();i++){
                    outfile<<paragraphs[i]<<"\n\n*"<<paragraphScoring[i]<<"*\n\n\n";
                }
            }catch(const std::exception& e){
                TraceLog(LOG_ERROR,"paragraphs size: %d",paragraphs.size());
                TraceLog(LOG_ERROR,"paragraphScoring size: %d",paragraphScoring.size());
                TraceLog(LOG_ERROR,"%s",e.what());
            }
            outfile<<"# 总体评价:\n\n\n"<<responseWholeWriting.getShortContent();
        }
        BeginDrawing();
            ClearBackground(RAYWHITE);
            testbtn.draw(0);
        EndDrawing();
    }
    CloseWindow();
    return 0;
}
#endif