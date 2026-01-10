#define WIN32_LEAN_AND_MEAN
#define NOGDI
#define NOUSER
#define NOCOMM
#define NOMINMAX

#include "raylib.h"
#include "buttons.h"
#include "callai.h"
#include "safelog.h"
#include "gadgets.h"
#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include <stdio.h>
#include <atomic>
#include <chrono>
#include <mutex>
#include <unistd.h>

std::mutex titleMutex;
std::mutex paragraphsMutex;
std::mutex writingMutex;
std::mutex promptMutex;
std::mutex paragraphScoringMutex;
std::mutex wholeWritingScoringMutex;

const std::string promptScoring="<background>你是一位作文批改老师，请以严格的考试作文标准，对以下文章进行评分。<writing>文章的标题是：%s，文章的内容是：%s。<scoring standard>评分标准是：1.文章的结构是否合理，2.文章的内容是否符合题目要求，3.文章的语言是否通顺，是否书面化，4.文章的语法是否正确，5.文章的情感是否符合题目要求，6.文章的长度是否符合题目要求。请对文章进行评分，上述每一点满分10分，其中6分及格，总体评分范围是0-60分，0分表示完全不符合要求，60分表示完全符合要求。请确保你的回复中只包含一个0~60的整数";
int main(void)
{
    curl_global_init(CURL_GLOBAL_ALL);
    FILE* fp=stdout;
    int x=200,y=200;
    freopen("./logs/console.log","w",stdout);
    SetConsoleOutputCP(CP_UTF8);
    const int screenWidth=1600;
    const int screenHeight=900;
    InitWindow(screenWidth,screenHeight,"Ｔｈｅ　ａｗｅｓｏｍｅ　ｓｃｏｒｉｎｇ　ｓｙｓ");
    SetTargetFPS(60);
    Rectangle testbtnRect={screenWidth/2-100,screenHeight/2-50,200,100};
    Rectangle savebtnRect={screenWidth/2-100,screenHeight/2+50,200,100};
    std::string WorkingDirectory=GetWorkingDirectory();
    Texture2D btnTexture=LoadTexture((WorkingDirectory+"/assets/button.bmp").c_str());
    Texture2D savebtnTexture=LoadTexture((WorkingDirectory+"/assets/savebtn.bmp").c_str());
    Button testbtn(testbtnRect,btnTexture);
    Button savebtn(savebtnRect,savebtnTexture);
    int progress=0;
    int score=0;
    int score2=0;
    int score3=0;
    std::string title="";
    std::vector<std::string> paragraphs;
    std::vector<std::string> paragraphScoring;
    std::string responseWholeWritingStr="";
    std::string scoreResponseStr="";
    std::string scoreResponseStr2="";
    std::string scoreResponseStr3="";
    std::atomic<bool> callDone(false);
    std::atomic<bool> analysisDone(false);
    std::atomic<bool> wholeAnalysisDone(false);
    auto start=std::chrono::high_resolution_clock::now();
    auto end=std::chrono::high_resolution_clock::now();
    std::string writing="";
    std::string prompt="";
    bool wholeProcessDone=false;
    while(!WindowShouldClose())
    {
        updateMousePos();
        if(testbtn.isLeftClicked()){
            start=std::chrono::high_resolution_clock::now();
            safeLog(LOG_INFO,"testbtn is clicked");
            std::ifstream file("./writing.txt");
            if(file.is_open()){
                std::string line;
                while(std::getline(file,line)){
                    writing+=line+"\n";
                }
                file.close();
            }
            paragraphs=split(writing,"\n");
            safeLog(LOG_INFO,"writing: %s",writing.c_str());
            for(std::string paragraph:paragraphs){
                safeLog(LOG_INFO,"paragraph: %s",paragraph.c_str());
                safeLog(LOG_INFO,"\n\n");
            }
            title=paragraphs[0];
            paragraphs.erase(paragraphs.begin());
            for(std::string paragraph:paragraphs){
                safeLog(LOG_INFO,"paragraph: %s",paragraph.c_str());
                safeLog(LOG_INFO,"\n\n");
            }
            std::vector<std::string> splitedPrompt=split(promptScoring,"%s");
            prompt=splitedPrompt[0]+title+splitedPrompt[1]+writing+splitedPrompt[2];
            safeLog(LOG_INFO,"prompt: %s",prompt.c_str());
            Response scoreResponse1;
            Response scoreResponse2;
            Response scoreResponse3;
            std::thread thread1([&](){
                promptMutex.lock();
                std::string promptStr=prompt;
                promptMutex.unlock();
                scoreResponse1=callai("gemma3:12b",promptStr,"",0.3,0.6,512,1024,false);
                scoreResponseStr=scoreResponse1.getShortContent();
                safeLog(LOG_INFO,"scoreResponse1 done: %s",scoreResponseStr.c_str());
                while(scoreResponseStr.find_last_not_of("0123456789")!=std::string::npos){
                    scoreResponseStr.erase(scoreResponseStr.find_last_not_of("0123456789"));
                }
                score=std::stoi(extractFirstNumber(scoreResponseStr));
                scoreResponse2=callai("phi3:14b",promptStr,"",0.3,0.6,512,1024,false);
                scoreResponseStr2=scoreResponse2.getShortContent();
                safeLog(LOG_INFO,"scoreResponse2 done: %s",scoreResponseStr2.c_str());
                while(scoreResponseStr2.find_last_not_of("0123456789")!=std::string::npos){
                    scoreResponseStr2.erase(scoreResponseStr2.find_last_not_of("0123456789"));
                }
                score2=std::stoi(extractFirstNumber(scoreResponseStr2));
                scoreResponse3=callai("qwen3:14b",promptStr,"",0.3,0.6,512,1024,false);
                scoreResponseStr3=scoreResponse3.getShortContent();
                safeLog(LOG_INFO,"scoreResponse3 done: %s",scoreResponseStr3.c_str());
                while(scoreResponseStr3.find_last_not_of("0123456789")!=std::string::npos){
                    scoreResponseStr3.erase(scoreResponseStr3.find_last_not_of("0123456789"));
                }
                score3=std::stoi(extractFirstNumber(scoreResponseStr3));
                callDone.store(true);
            });
            thread1.detach();
            std::thread paragraphAnalysis([&](){
                paragraphsMutex.lock();
                std::vector<std::string> paragraphsCopy=paragraphs;
                paragraphsMutex.unlock();
                for(auto para:paragraphsCopy){
                    if(para==""){
                        continue;
                    }
                    std::string prompt="请为下面这个段落写段评，标题为："+title+"，段落为："+para+"\n以markdown格式输出，但不需要\"```markdown\"和\"```\"，也不要包含标题，\"段评：\"，注意你也必须写出这一段的不足之处，不能一味夸赞，不要包含其他内容，格式：优点+\"但是\"+不足处";
                    Response response=callai("qwen3:14b",prompt,"",1.2,0.6,512,1024,true);
                    paragraphScoringMutex.lock();
                    safeLog(LOG_INFO,"response: %s",response.getShortContent().c_str());
                    paragraphScoring.push_back(response.getShortContent());
                    paragraphScoringMutex.unlock();
                }
                safeLog(LOG_INFO,"paragraphAnalysis done");
                analysisDone.store(true);
            });
            paragraphAnalysis.detach();
            std::thread wholeAnalysisThread([&](){
                titleMutex.lock();
                std::string titleStr=title;
                titleMutex.unlock();
                writingMutex.lock();
                std::string writingStr=writing;
                writingMutex.unlock();
                Response responseWholeWriting=callai("qwen3:14b","请为下面文章写总评，标题为："+titleStr+"，文章为："+writingStr+"\n以markdown格式输出，但不需要\"```markdown\"和\"```\"，也不要包含标题，\"段评：\"，注意你也必须写出这篇文章的不足之处，不能一味夸赞，不要包含其他内容，格式：优点+\"但是\"+不足处，请都写得长一点","",1.2,0.6,1024,2048,true);
                safeLog(LOG_INFO,"responseWholeWriting: %s",responseWholeWriting.getShortContent().c_str());
                wholeWritingScoringMutex.lock();
                responseWholeWritingStr=responseWholeWriting.getShortContent();
                safeLog(LOG_INFO,"responseWholeWritingStr: %s",responseWholeWritingStr.c_str());
                wholeWritingScoringMutex.unlock();
                wholeAnalysisDone.store(true);
            });
            wholeAnalysisThread.detach();
        }
        if(savebtn.isLeftClicked()){
            std::ofstream outfile("./scoring.md");
            outfile<<"# "<<title<<"       得分："<<(score+score2+score3+90.0)/3.0<<"/60\n\n";
            try{
                paragraphScoringMutex.lock();
                for(int i=0;i<paragraphScoring.size();i++){
                    outfile<<paragraphs[i]<<"\n\n*"<<paragraphScoring[i]<<"*\n\n\n";
                }
                paragraphScoringMutex.unlock();
                wholeWritingScoringMutex.lock();
                outfile<<"# 总体评价:\n\n\n"<<responseWholeWritingStr;
                wholeWritingScoringMutex.unlock();
            }catch(const std::exception& e){
                safeLog(LOG_ERROR,"paragraphs size: %d",paragraphs.size());
                safeLog(LOG_ERROR,"paragraphScoring size: %d",paragraphScoring.size());
                safeLog(LOG_ERROR,"%s",e.what());
            }
            outfile.close();
        }
        if(callDone.load()&&analysisDone.load()&&wholeAnalysisDone.load()){
            wholeProcessDone=true;
        }
        progress=callDone.load()?1:0;
        progress+=analysisDone.load()?1:0;
        progress+=wholeAnalysisDone.load()?1:0;
        auto duration=std::chrono::duration_cast<std::chrono::seconds>(end-start);
        BeginDrawing();
            ClearBackground(RAYWHITE);
            DrawText(("progress: "+std::to_string(progress)).c_str(),100,100,20,BLACK);
            testbtn.draw(0);
            savebtn.draw(0);
            x++;
            if(x>400){
                x=200;
                y+=50;
            }
            if(y>400){
                y=200;
            }
            DrawCircle(x,y,50,BLUE);
            DrawText("If the circle is moving, the program is not blocked",200,200,20,BLACK);
            if(wholeProcessDone){
                DrawText("The whole process is done",200,300,20,BLACK);
                end=std::chrono::high_resolution_clock::now();
                safeLog(LOG_INFO,"duration: %ld",duration.count());
                DrawText(("time: "+std::to_string(duration.count())+"s").c_str(),200,400,20,BLACK);
            }
        EndDrawing();
    }
    auto now=std::chrono::system_clock::now();
    std::time_t now_c=std::chrono::system_clock::to_time_t(now);
    std::string now_str=std::ctime(&now_c);
    now_str.erase(now_str.find_last_not_of(" \n\r\t")+1);
    while(now_str.find_first_of(" :")!=std::string::npos){
        now_str.replace(now_str.find_first_of(" :"),1,"_");
    }
    now_str="ren .\\logs\\console.log log_"+now_str+".log";
    safeLog(LOG_INFO,"now_str: %s",now_str.c_str());
    safeLog(LOG_INFO,"WorkingDirectory: %s",WorkingDirectory.c_str());
    std::string cdStr="cd "+WorkingDirectory;
    std::string finalShellStr=cdStr+" && "+now_str;
    safeLog(LOG_INFO,"finalShellStr: %s",finalShellStr.c_str());
    fclose(stdout);
    system(finalShellStr.c_str());
    CloseWindow();
    return 0;
}