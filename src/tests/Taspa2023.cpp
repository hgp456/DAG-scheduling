#include "dagSched/tests.h"
//* 1.求每个子任务的WCET,后续节点个数
#include "unistd.h"
#include <algorithm>
#include <vector>
#include <execution>
#include <fl/Headers.h>


#define OK 1
#define WRONG 0

#define FINISH true
#define UNFINISH false

#define BUSY 1
#define IDLE 0

#define EDF 0
#define LLF 1
#define DFS 2
struct CORETYPE
{
    int runningtask;
    int status=IDLE;//状态idel busy 空闲或忙碌
    int lefttime;//剩下的时间
    int busytime=0;//忙碌的时间
    int tasknum;//目前在运行的任务号
    std::vector<int> finishjob;
};

namespace dagSched
{
        
    bool RelyFinish1(SubTask * assignTask)
    {
        for(auto task:assignTask->pred)
        {
            if(task->isfinish==false)
            return false;
        }
        return true;
    }
     int ExecMul1(std::vector<std::vector<SubTask *>> AllassignedTask,int coreNum,std::vector<int> &responsetime)
    {
        int time=0;
        CORETYPE  core[coreNum];
        int allstatus=IDLE;
        while(!AllassignedTask.empty()||allstatus==BUSY)
        {
            std::cout << "Taspa!!!:" <<std::endl;
             for (int i = 0; i < coreNum; i++)
             {
                if (core[i].status == BUSY)
                {
                    core[i].busytime++;
                    std::cout << "core[i].busytime:" << core[i].busytime << std::endl;
                    AllassignedTask[core[i].tasknum][core[i].runningtask]->c--;
                    if (AllassignedTask[core[i].tasknum][core[i].runningtask]->c <= 0)
                    {
                        AllassignedTask[core[i].tasknum][core[i].runningtask]->isfinish = true;
                        core[i].finishjob.push_back(core[i].runningtask);
                        core[i].status = IDLE;
                        responsetime[core[i].tasknum]=time;
                        std::cout << "task:" << core[i].tasknum  << " " << "subtask:" << AllassignedTask[core[i].tasknum][core[i].runningtask]->id << " finfished"
                              << " at core:" << i << " at time:" << time << std::endl;
                    }
                }
             }

             for (int i = 0; i < coreNum; i++)
             {
                // cout << "i am in" << endl
                    for (int task = 0; task < AllassignedTask.size(); task++)
                    {
                        for (int subtaskNumNow = 0; subtaskNumNow < AllassignedTask[task].size(); subtaskNumNow++)
                        {
                                    if (RelyFinish1(AllassignedTask[task][subtaskNumNow]) == true && AllassignedTask[task][subtaskNumNow]->isfinish == false && AllassignedTask[task][subtaskNumNow]->isrunning == false)
                                    {
                                        if(core[i].status==IDLE)
                                        {
                                            core[i].tasknum = task;
                                            core[i].runningtask = subtaskNumNow;
                                            // AllassignedTask[core[i].tasknum][core[i].runningtask]->c--;
                                            std::cout <<  "Core[" << i <<"]:task:" <<task << " subtask:" << AllassignedTask[task][subtaskNumNow]->id << " start at time:" << time << std::endl;
                                            // cout <<"xixi1"<<endl;
                                            AllassignedTask[task][subtaskNumNow]->isrunning = true;
                                            core[i].status = BUSY;
                                            break;
                                            
                                        }
                                        else
                                        {
                                            // if(task < core[i].tasknum ||  task==core[i].tasknum && subtaskNumNow < core[i].runningtask  )
                                            if(task < core[i].tasknum)
                                            {
                                                AllassignedTask[core[i].tasknum][core[i].runningtask]->isrunning = false;
                                                core[i].tasknum = task;
                                                core[i].runningtask=subtaskNumNow;
                                                // AllassignedTask[core[i].tasknum][core[i].runningtask]->c--;
                                                AllassignedTask[task][subtaskNumNow]->isrunning = true;
                                                std::cout <<  "Inp!:Core[" << i <<"]:task:" <<task << " subtask:" << AllassignedTask[task][subtaskNumNow]->id << " start at time:" << time << std::endl;

                                            }
                                        }
                                    }
                        }
                        if(core[i].status==BUSY)
                        {
                            break;
                        }
                 }
                
             }
             for (int i = 0; i < coreNum; i++)
             {
                allstatus = IDLE;
                if (core[i].status == BUSY)
                {
                allstatus = BUSY;
                break;
                }
             }
             if (allstatus == IDLE)
                break;
             time++;
        }
    }

    void fuzzyengineEstablish4(fl::Engine &engine)
    {
    // fl::Engine engine("fuzzy-controller");
    engine.setDescription("Dynamic Fuzzy Logic Real Time Scheduling");
    // 创建输入变量 WCET
    
    fl::InputVariable* input1 = new fl::InputVariable;
    input1->setName("TaspaRank");
    input1->setRange(0, 1); // 设置输入范围
    input1->addTerm(new fl::ZShape("LOW", 0,0.5)); // 添加隶属度函数
    input1->addTerm(new fl::Gaussian("MEDIUM",0.5,0.125));
    input1->addTerm(new fl::SShape("HIGH", 0.5,1));
    engine.addInputVariable(input1);

    // 创建输入变量 outdegreeSum
    fl::InputVariable* input2 = new fl::InputVariable;
    input2->setName("Laxity");
    input2->setRange(0, 1); // 设置输入范围
    input2->addTerm(new fl::ZShape("LOW", 0,0.5)); // 添加隶属度函数
    input2->addTerm(new fl::Gaussian("MEDIUM",0.5,0.125));
    input2->addTerm(new fl::SShape("HIGH", 0.5,1));
    engine.addInputVariable(input2);

    fl::InputVariable* input3 = new fl::InputVariable;
    input3->setName("Deadline");
    input3->setRange(0, 1); // 设置输入范围
    input3->addTerm(new fl::ZShape("LOW", 0,0.5)); // 添加隶属度函数
    input3->addTerm(new fl::Gaussian("MEDIUM",0.5,0.125));
    input3->addTerm(new fl::SShape("HIGH", 0.5,1));
    engine.addInputVariable(input3);
    // fl::InputVariable* input3 = new fl::InputVariable;
    // input3->setName("Laxity");
    // input3->setRange(0, 1); // 设置输入范围
    // input3->addTerm(new fl::ZShape("LOW", 0,0.5)); // 添加隶属度函数
    // input3->addTerm(new fl::Gaussian("MEDIUM",0.5,0.125));
    // input3->addTerm(new fl::SShape("HIGH", 0.5,1));
    // engine.addInputVariable(input3);

    // 创建输出变量
    fl::OutputVariable* output1 = new fl::OutputVariable;
    output1->setRange(0, 100); // 设置输出范围
    output1->setName("Preemptive");
    // output1->addTerm(new fl::ZShape("LOW", 0, 50)); // 添加隶属度函数
    // output1->addTerm(new fl::Gaussian("MEDIUM",50,12.5));
    // output1->addTerm(new fl::SShape("HIGH",50,100));
    output1->addTerm(new fl::Trapezoid("VERY_LOW",0,0,10,30)); // 添加隶属度函数
    output1->addTerm(new fl::Triangle("LOW", 10,30,50));
    output1->addTerm(new fl::Triangle("MEDIUM", 30,50,70));
    output1->addTerm(new fl::Triangle("HIGH", 50,70,90));
    output1->addTerm(new fl::Trapezoid("VERY_HIGH", 70,90,100,100));
    output1->setDefuzzifier(new fl::Centroid());
    output1->setAggregation(new fl::Maximum());
    engine.addOutputVariable(output1);

    // 创建模糊规则
    fl::RuleBlock* ruleblock1 = new fl::RuleBlock;
    ruleblock1->setName("");
    ruleblock1->setConjunction(new fl::Minimum());
    ruleblock1->setDisjunction(new fl::Maximum());
    ruleblock1->setActivation(new fl::General());
    ruleblock1->setImplication(new fl::Minimum());
    
// std::cout <<"test1" << std::endl;
    ruleblock1->addRule(fl::Rule::parse("if TaspaRank is HIGH and Deadline is LOW and Laxity is LOW then Preemptive is VERY_HIGH with 1", &engine)); 
    ruleblock1->addRule(fl::Rule::parse("if TaspaRank is HIGH and Deadline is MEDIUM and Laxity is LOW then Preemptive is VERY_HIGH with 0.7", &engine)); 
    ruleblock1->addRule(fl::Rule::parse("if TaspaRank is HIGH and Deadline is HIGH and Laxity is LOW then Preemptive is VERY_HIGH with 0.5", &engine)); 
    // ruleblock1->addRule(fl::Rule::parse("if Laxity is LOW then Preemptive is VERY_HIGH with 1", &engine));


    ruleblock1->addRule(fl::Rule::parse("if TaspaRank is MEDIUM and Deadline is LOW and Laxity is LOW then Preemptive is VERY_HIGH with 0.9", &engine)); 
    ruleblock1->addRule(fl::Rule::parse("if TaspaRank is MEDIUM and Deadline is MEDIUM and Laxity is LOW then Preemptive is VERY_HIGH with 0.5", &engine)); 
    ruleblock1->addRule(fl::Rule::parse("if TaspaRank is MEDIUM and Deadline is HIGH and Laxity is LOW then Preemptive is VERY_HIGH with 0.2", &engine)); 

    ruleblock1->addRule(fl::Rule::parse("if TaspaRank is LOW and Deadline is LOW and Laxity is LOW then Preemptive is VERY_HIGH with 0.8", &engine)); 
    ruleblock1->addRule(fl::Rule::parse("if TaspaRank is LOW and Deadline is MEDIUM and Laxity is LOW then Preemptive is HIGH with 0.4", &engine)); 
    ruleblock1->addRule(fl::Rule::parse("if TaspaRank is LOW and Deadline is HIGH and Laxity is LOW then Preemptive is MEDIUM with 0.5", &engine)); 


    ruleblock1->addRule(fl::Rule::parse("if TaspaRank is HIGH and Deadline is LOW and Laxity is MEDIUM then Preemptive is HIGH with 1", &engine)); 
    ruleblock1->addRule(fl::Rule::parse("if TaspaRank is HIGH and Deadline is MEDIUM and Laxity is MEDIUM then Preemptive is HIGH with 0.5", &engine)); 
    ruleblock1->addRule(fl::Rule::parse("if TaspaRank is HIGH and Deadline is HIGH and Laxity is MEDIUM then Preemptive is MEDIUM with 0.5", &engine)); 

    ruleblock1->addRule(fl::Rule::parse("if TaspaRank is HIGH and Deadline is LOW and Laxity is HIGH then Preemptive is MEDIUM with 1", &engine)); 
    ruleblock1->addRule(fl::Rule::parse("if TaspaRank is HIGH and Deadline is MEDIUM and Laxity is HIGH then Preemptive is MEDIUM with 0.5", &engine)); 
    ruleblock1->addRule(fl::Rule::parse("if TaspaRank is HIGH and Deadline is HIGH and Laxity is HIGH then Preemptive is LOW with 0.5", &engine)); 


    ruleblock1->addRule(fl::Rule::parse("if TaspaRank is MEDIUM and Deadline is LOW and Laxity is MEDIUM then Preemptive is MEDIUM with 0.5", &engine)); 
    ruleblock1->addRule(fl::Rule::parse("if TaspaRank is MEDIUM and Deadline is MEDIUM and Laxity is MEDIUM then Preemptive is MEDIUM with 1", &engine)); 
    ruleblock1->addRule(fl::Rule::parse("if TaspaRank is MEDIUM and Deadline is HIGH and Laxity is MEDIUM then Preemptive is LOW with 0.5", &engine)); 

    ruleblock1->addRule(fl::Rule::parse("if TaspaRank is MEDIUM and Deadline is LOW and Laxity is HIGH then Preemptive is LOW with 0.5", &engine)); 
    ruleblock1->addRule(fl::Rule::parse("if TaspaRank is MEDIUM and Deadline is MEDIUM and Laxity is HIGH then Preemptive is LOW with 1", &engine)); 
    ruleblock1->addRule(fl::Rule::parse("if TaspaRank is MEDIUM and Deadline is HIGH and Laxity is HIGH then Preemptive is VERY_LOW with 0.5", &engine)); 


    ruleblock1->addRule(fl::Rule::parse("if TaspaRank is LOW and Deadline is LOW and Laxity is MEDIUM then Preemptive is MEDIUM with 0.5", &engine)); 
    ruleblock1->addRule(fl::Rule::parse("if TaspaRank is LOW and Deadline is MEDIUM and Laxity is MEDIUM then Preemptive is LOW with 0.7", &engine)); 
    ruleblock1->addRule(fl::Rule::parse("if TaspaRank is LOW and Deadline is HIGH and Laxity is MEDIUM then Preemptive is LOW with 1", &engine)); 

    ruleblock1->addRule(fl::Rule::parse("if TaspaRank is LOW and Deadline is LOW and Laxity is HIGH then Preemptive is VERY_LOW with 0.5", &engine)); 
    ruleblock1->addRule(fl::Rule::parse("if TaspaRank is LOW and Deadline is MEDIUM and Laxity is HIGH then Preemptive is VERY_LOW with 0.7", &engine)); 
    ruleblock1->addRule(fl::Rule::parse("if TaspaRank is LOW and Deadline is HIGH and Laxity is HIGH then Preemptive is VERY_LOW with 1", &engine));

    // ruleblock1->addRule(fl::Rule::parse("if Laxity is LOW then Preemptive is VERY_HIGH with 1", &engine));


// ruleblock1->addRule(fl::Rule::parse("if TaspaRank is HIGH and Laxity is LOW or Deadline is LOW then Preemptive is VERY_HIGH with 1", &engine)); 
// ruleblock1->addRule(fl::Rule::parse("if TaspaRank is MEDIUM and Laxity is MEDIUM and Deadline is MEDIUM then Preemptive is MEDIUM with 1", &engine)); 
// ruleblock1->addRule(fl::Rule::parse("if TaspaRank is LOW and Laxity is HIGH and Deadline is HIGH then Preemptive is VERY_LOW with 1", &engine)); 
// ruleblock1->addRule(fl::Rule::parse("if TaspaRank is LOW and Laxity is HIGH or Deadline is HIGH then Preemptive is LOW with 1", &engine)); 
// ruleblock1->addRule(fl::Rule::parse("if TaspaRank is MEDIUM and Laxity is HIGH or Deadline is HIGH then Preemptive is LOW with 1", &engine));
    // ruleblock1->addRule(fl::Rule::parse("if Laxity is LOW then Preemptive is VERY_HIGH with 1", &engine));
    // ruleblock1->addRule(fl::Rule::parse("if TaspaRank is HIGH and Laxity is MEDIUM and Deadline is LOW then Preemptive is HIGH  with 0.5", &engine));
    // ruleblock1->addRule(fl::Rule::parse("if TaspaRank is MEDIUM and Laxity is MEDIUM and Deadline is LOW then Preemptive is MEDIUM  with 0.7", &engine));

    // ruleblock1->addRule(fl::Rule::parse("if TaspaRank is LOW and Laxity is MEDIUM and Deadline is LOW then Preemptive is MEDIUM  with 0.5", &engine));
    // ruleblock1->addRule(fl::Rule::parse("if TaspaRank is HIGH and Laxity is HIGH and Deadline is LOW then Preemptive is LOW  with 1", &engine));
    // ruleblock1->addRule(fl::Rule::parse("if TaspaRank is LOW and Laxity is LOW then Preemptive is VERY_LOW  with 1", &engine));
    // ruleblock1->addRule(fl::Rule::parse("if TaspaRank is LOW and Laxity is MEDIUM then Preemptive is LOW  with 0.5", &engine));
    // ruleblock1->addRule(fl::Rule::parse("if TaspaRank is LOW and Laxity is HIGH then Preemptive is MEDIUM  with 1", &engine));


    // ruleblock1->addRule(fl::Rule::parse("if TaspaRank is MEDIUM and Laxity is LOW then Preemptive is LOW  with 0.5", &engine));
    // ruleblock1->addRule(fl::Rule::parse("if TaspaRank is MEDIUM and Laxity is MEDIUM then Preemptive is MEDIUM  with 1", &engine));
    // ruleblock1->addRule(fl::Rule::parse("if TaspaRank is MEDIUM and Laxity is HIGH then Preemptive is HIGH  with 0.5", &engine));

    // ruleblock1->addRule(fl::Rule::parse("if TaspaRank is HIGH and Laxity is LOW  then Preemptive is MEDIUM  with 1", &engine));
    // ruleblock1->addRule(fl::Rule::parse("if TaspaRank is HIGH and Laxity is MEDIUM then Preemptive is HIGH  with 0.5", &engine));
    // ruleblock1->addRule(fl::Rule::parse("if TaspaRank is HIGH and Laxity is HIGH  then Preemptive is VERY_HIGH  with 1", &engine));

    // ruleblock1->addRule(fl::Rule::parse("if Deadline is HIGH  then Preemptive is VERY_LOW  with 1", &engine));
    // ruleblock1->addRule(fl::Rule::parse("if Deadline is MEDIUM then Preemptive is MEDIUM  with 1", &engine));
    // ruleblock1->addRule(fl::Rule::parse("if Deadline is LOW  then Preemptive is VERY_HIGH  with 1", &engine));

    // ruleblock1->addRule(fl::Rule::parse("if TaspaRank is HIGH then Preemptive is HIGH  with 1", &engine));
    // ruleblock1->addRule(fl::Rule::parse("if TaspaRank is MEDIUM then Preemptive is MEDIUM with 1", &engine));
    // ruleblock1->addRule(fl::Rule::parse("if TaspaRank is LOW then Preemptive is LOW with 1", &engine));

    // ruleblock1->addRule(fl::Rule::parse("if outdegreeSum is HIGH then output is HIGH with 1", &engine));
    // ruleblock1->addRule(fl::Rule::parse("if outdegreeSum is MEDIUM then output is MEDIUM with 1", &engine));
    // ruleblock1->addRule(fl::Rule::parse("if outdegreeSum is LOW then output is LOW with 1", &engine));

    
    // ruleblock1->addRule(fl::Rule::parse("if Laxity is MEDIUM then Preemptive is MEDIUM with 1", &engine));
    // ruleblock1->addRule(fl::Rule::parse("if Laxity is HIGH then Preemptive is LOW with 1", &engine));


    engine.addRuleBlock(ruleblock1); 
    // std::cout <<"test" << std::endl;
    // 设置模糊控制器的输入变量
    // input1->setValue(WCET);
    // input2->setValue(outdegreeSum);
    // input3->setValue(waitTime);

    // 计算模糊控制器的输出变量
    // engine.process();
    // float outputValue = output1->getValue();

    // 保存 FIS 文件，以便 Matlab 可视化
    fl::FisExporter exporter;
    std::string engineString = exporter.toString(&engine);
    engineString.replace(engineString.find("Version=6"), 9, "Version=2");
    // // 将引擎字符串保11存到文件
    static int count=0;
    std::ofstream file("fuzzy"+ std::to_string(count) +".fis");
    file << engineString;
    file.close();
    }

    float FuzzyCalculate2(float Rankd,float Laxity,float ld,fl::Engine &engine,float maxRankd,float ddlmax,float ddltask)
{
    // 计算模糊控制器的输出变量
    // std::cout << vol << std::endl;
    // std::cout << Rankd  << " " << maxRankd <<" " << Laxity << " " <<ld <<" " << ddltask <<" " << ddlmax << std::endl;
    if(Laxity<=0)
    {
        Laxity=0;
    }
    engine.setInputValue("TaspaRank",Rankd/maxRankd);
    engine.setInputValue("Laxity",Laxity/ld);
    engine.setInputValue("Deadline",ddltask/ddlmax);
    engine.process();
    float outputValue = engine.getOutputValue("Preemptive");

    return outputValue;
}
    int ExecMul3(std::vector<std::vector<SubTask *>> AllassignedTask,int coreNum,std::vector<int> &responsetime,int mode,int maxRankd,float maxDDL,std::vector<float> TaskDeadLine)
    {
        int time=0;
        CORETYPE  core[coreNum];
        int allstatus=IDLE;
        int assignPeriod = 2;
        int nextAssign=0;
        bool assignflag=true;
        std::vector<float> fprivalue;
        auto minnumber = std::min_element(fprivalue.begin(), fprivalue.end());
        // std::cout << "corenum:"<< coreNum <<std::endl;
        // if(mode==DFS)
        // {
        fl::Engine engine("fuzzy-controller");
        if(mode==DFS)
        {
        fuzzyengineEstablish4(engine);
        }
         for(int i=0;i<coreNum;i++)
        {
            core[i].busytime=0;
        }
        // std::cout << "Taspa:!!!" << std::endl;
        while(!AllassignedTask.empty()||allstatus==BUSY)
        {
            
             for (int i = 0; i < coreNum; i++)
             {
                if (core[i].status == BUSY)
                {
                    core[i].busytime++;
                    // std::cout << "core[" << i <<"].busytime:" << core[i].busytime << std::endl;
                    AllassignedTask[core[i].tasknum][core[i].runningtask]->runningtime++;
                    AllassignedTask[core[i].tasknum][core[i].runningtask]->c--;
                    if (AllassignedTask[core[i].tasknum][core[i].runningtask]->c <= 0)
                    {
                        AllassignedTask[core[i].tasknum][core[i].runningtask]->isfinish = true;
                        core[i].finishjob.push_back(core[i].runningtask);
                        core[i].status = IDLE;
                        responsetime[core[i].tasknum]=time;
                        assignflag=true;
                        // std::cout << "task:" << core[i].tasknum  << " " << "subtask:" << AllassignedTask[core[i].tasknum][core[i].runningtask]->id << " finfished" << " at core:" << i << " at time:" << time << std::endl;
                    }
                }
             }

            if (time == nextAssign)
            {
                nextAssign += assignPeriod;
                assignflag=true;
            }
            if(assignflag==true)
            {

                // cout << "i am in" << endl
                fprivalue.clear();
                bool fpricalculate=false;
                for (int task = 0; task < AllassignedTask.size(); task++)
                {
                    for (int subtaskNumNow = 0; subtaskNumNow < AllassignedTask[task].size(); subtaskNumNow++)
                    {
                                if (RelyFinish1(AllassignedTask[task][subtaskNumNow]) == true && AllassignedTask[task][subtaskNumNow]->isfinish == false && AllassignedTask[task][subtaskNumNow]->isrunning == false)
                                {
                                    bool scheduled=false;
                                    for (int i = 0; i < coreNum; i++)
                                    {
                                        if(core[i].status==IDLE)
                                        {
                                            core[i].tasknum = task;
                                            core[i].runningtask = subtaskNumNow;
                                            // std::cout <<  "Core[" << i <<"]:task:" <<task << " subtask:" << AllassignedTask[task][subtaskNumNow]->id << " start at time:" << time << std::endl;

                                            AllassignedTask[task][subtaskNumNow]->isrunning = true;
                                            core[i].status = BUSY;
                                            scheduled=true;
                                            break;
                                        }
                                    }
                                    if(scheduled==true){
                                        continue;
                                    }
                                    else
                                    {
                                        int minIndex = coreNum; 
                                        int maxIndex=0;
                                        // int minnumber=100;
                                        if(mode==DFS&&fpricalculate==false)
                                        {
                                            for (int i = 0; i < coreNum; i++)
                                            {

                                               AllassignedTask[core[i].tasknum][core[i].runningtask]->fpriValue = FuzzyCalculate2(AllassignedTask[core[i].tasknum][core[i].runningtask]->taspaorder,AllassignedTask[core[i].tasknum][core[i].runningtask]->localD-time,AllassignedTask[core[i].tasknum][core[i].runningtask]->localD,engine,maxRankd,maxDDL,TaskDeadLine[core[i].tasknum]);
                                            //    std::cout <<AllassignedTask[core[i].tasknum][core[i].runningtask]->fpriValue << std::endl;
                                               fprivalue.push_back(AllassignedTask[core[i].tasknum][core[i].runningtask]->fpriValue);
                                                // if(AllassignedTask[core[i].tasknum ][core[i].runningtask]->fpriValue<minnumber)
                                                // {
                                                //     minnumber=AllassignedTask[core[i].tasknum][core[i].runningtask]->fpriValue;
                                                //     maxIndex=i;
                                                // }
                                                // std::cout << "calculating!" << std::endl;
                                            }
                                            // minnumber = std::min_element(fprivalue.begin(), fprivalue.end());
                                            // minIndex = std::distance(fprivalue.begin(), minnumber);
                                            fpricalculate=true;
                                        }
                                        
                                        if(mode==DFS)
                                        {
                AllassignedTask[task][subtaskNumNow]->fpriValue = FuzzyCalculate2( AllassignedTask[task][subtaskNumNow]->taspaorder, AllassignedTask[task][subtaskNumNow]->localD-time, AllassignedTask[task][subtaskNumNow]->localD,engine,maxRankd,maxDDL,TaskDeadLine[task]);
                                            // for(int i=0;i <fprivalue.size();i++)
                                            // {
                                            //     std::cout << fprivalue[i] <<" ";
                                            // }
                                        }       


                                        
                                        // std::cout << std::endl;
                                        // sleep(1);
                                        float minprivalue=101;
                                        for (int i = 0; i < coreNum; i++)
                                        {
                                            float minEDF=0;
                                            float maxLLF=0;
                                            // minprivalue=101;
                                            if (mode == EDF && task < core[i].tasknum)
                                            {
                                                if(core[i].tasknum-task>minEDF)
                                                {
                                                    minEDF=core[i].tasknum-task; //!找到差值最大的
                                                    minIndex=i; //! 换进去
                                                }
                                            }
                                            else if (mode == LLF && task != core[i].tasknum && (AllassignedTask[task][subtaskNumNow]->localD - time < AllassignedTask[core[i].tasknum][core[i].runningtask]->localD - time))
                                            {
                                                
                                                if(maxLLF<AllassignedTask[core[i].tasknum][core[i].runningtask]->localD - time)
                                                // if (AllassignedTask[core[minIndex].tasknum][core[minIndex].runningtask]->localD - time  < AllassignedTask[core[i].tasknum][core[i].runningtask]->localD - time)//!如果松弛度大于当前判断任务,选出最大的
                                                
                                                {
                                                    maxLLF=AllassignedTask[core[i].tasknum][core[i].runningtask]->localD - time;
                                                    minIndex = i;
                                                }
                                            }
                                            else if(mode==DFS && task != core[i].tasknum && AllassignedTask[task][subtaskNumNow]->fpriValue > fprivalue[i])
                                            {
                                                if(minprivalue>fprivalue[i])
                                                {
                                                    fprivalue[i]=minprivalue;
                                                    minIndex = i;
                                                    // std:: cout << minprivalue << std::endl;
                                                }
                                            }
                                        }
                                        
                                                float threshold=0.1;
                                                if ( minIndex!=coreNum )
                                                {
                                                    // if(mode==DFS &&AllassignedTask[task][subtaskNumNow]->fpriValue-minprivalue<0.05)
                                                    // {
                                                    //     break;
                                                    // }
                                                    AllassignedTask[core[minIndex].tasknum][core[minIndex].runningtask]->isrunning = false;
                                                    core[minIndex].tasknum = task;
                                                    core[minIndex].runningtask = subtaskNumNow;
                                                    // std::cout << "Inp!:Core[" << minIndex << "]:task:" << task << " subtask:" << AllassignedTask[task][subtaskNumNow]->id << " start at time:" << time << std::endl;

                                                    AllassignedTask[task][subtaskNumNow]->isrunning = true;
                                                    core[minIndex].status = BUSY;
                                                    scheduled = true;
                                                    if(mode==DFS)
                                                    {
                                                        fprivalue[minIndex]=AllassignedTask[task][subtaskNumNow]->fpriValue; //!重置工作
                                                    }
                                                    break;
                                                }
                                                // if( (mode==EDF&&task < core[i].tasknum) || (mode==LLF && task != core[i].tasknum&&(AllassignedTask[task][subtaskNumNow]->localD-time < AllassignedTask[core[i].tasknum][core[i].runningtask]->localD-time) ))
                                                
                                                // //  if(task < core[i].tasknum || task==core[i].tasknum && subtaskNumNow <core[i].runningtask )
                                                // {
                                                //     AllassignedTask[core[i].tasknum][core[i].runningtask]->isrunning = false;
                                                //     core[i].tasknum = task;
                                                //     core[i].runningtask = subtaskNumNow;
                                                //     std::cout <<  "Inp!:Core[" << i <<"]:task:" <<task << " subtask:" << AllassignedTask[task][subtaskNumNow]->id << " start at time:" << time << std::endl;

                                                //     AllassignedTask[task][subtaskNumNow]->isrunning = true;
                                                //     core[i].status = BUSY;
                                                //     scheduled=true;
                                                //     break;
                                                // }
                                            // }
                                        // }
                                    }
                                }
                    }
                    // if(core[i].status==BUSY)
                    // {
                    //     break;
                    // }
                }
                
            }
            assignflag=false;
             for (int i = 0; i < coreNum; i++)
             {
                allstatus = IDLE;
                if (core[i].status == BUSY)
                {
                allstatus = BUSY;
                break;
                }
             }
             if (allstatus == IDLE)
                break;
             time++;
        }

        float buzytimeSum=0;
        float buzyrate=0;
        for(int i=0;i<coreNum;i++)
        {

            buzytimeSum+=core[i].busytime;
            // std:: cout << "core[i].busytime:" << core[i].busytime<< std::endl;
        }
        // std:: cout << "buzytimeSum:" << buzytimeSum<< std::endl;
        // std:: cout << "buzyrate:" << buzytimeSum/(coreNum*time) << std::endl;
        // std:: cout << "Runningtime:" << time  << std::endl;
        return time;
    }
    std::vector<SubTask *> TaspaPriOrder(std::vector<SubTask *> &V,std::vector<float> outdegreeSum)
    {
        std::vector<SubTask *> assignedTask;//* 已经给了优先级的任务
        std::vector<SubTask *> WaitTask;    //* 等待排序的任务
        int prio=0;
        //*找Outdegree中最大的数,他就是V的ID 把和ID匹配的扔到分配任务中即可
        while(V.size()!=assignedTask.size())
        {
            int max=0;
            int id=0;
            for(int i=0;i<outdegreeSum.size();i++)
            {
                if(outdegreeSum[i]>max)
                {
                    max=outdegreeSum[i];
                    id=i;
                }
            }
            for(auto node:V)
            {
                if(node->id==id)//!当前最大的这个点
                {
                    node->prio=prio++; //todo 找一下它和assigned的关系
                    node->isReady=true;
                    outdegreeSum[id]=0;
                    assignedTask.push_back(node);
                    // std::cout << max << " " << id << std::endl;
                    break;
                }
            }
        }
        // sleep(1);
        return assignedTask;
    }

    float maxmaxRankd=0;
    int TaspaPriCalculate(SubTask* V,std::vector<float> &outdegreeSum,std::vector<int> &visit)
    {
    //    std::cout << V->succ.size() << endl;
       if(V->succ.size()==0)
       {
            visit[V->id]=1;
            outdegreeSum[V->id]=V->c;
            return outdegreeSum[V->id];
       }
       else if(visit[V->id]==0)
       {
            outdegreeSum[V->id]=V->c+V->succ.size();
            visit[V->id]=1;
       }
       else if(visit[V->id]==1)
       {
            return outdegreeSum[V->id];
       }
       int max=0;
       for(int i=0;i<V->succ.size();i++)
       {
            if(TaspaPriCalculate(V->succ[i],outdegreeSum,visit)>max)
            {
                max=TaspaPriCalculate(V->succ[i],outdegreeSum,visit);
            }
            
       }
       outdegreeSum[V->id]+=max;
       if(outdegreeSum[V->id] >maxmaxRankd)
        {
            maxmaxRankd=outdegreeSum[V->id];
        }
       visit[V->id]=1;
       V->taspaorder=outdegreeSum[V->id];
       return outdegreeSum[V->id];
    }

    void resetV1(std::vector<std::vector<SubTask *>> V,std::vector<std::vector<float>> WCET)
    {
        for(int j=0;j<V.size();j++)
        {
            for(int i=0;i<V[j].size();i++)
            {
                V[j][i]->c=WCET[j][i];
                V[j][i]->isReady=false;
                V[j][i]->isWait=false;
                V[j][i]->isfinish=false;
                V[j][i]->isrunning=false;
                V[j][i]->fpriValue=0;
                V[j][i]->waitTime=0;
                V[j][i]->prio=0;
            }
        }
    }



    
    std::vector<float> Taspa2023(Taskset taskset, const int m)
    {
        //* 输入：任务集,核心数
        std::sort(taskset.tasks.begin(), taskset.tasks.end(), deadlineMonotonicSorting);
        std::vector<std::vector<SubTask *>> AllassignedTask;
        std::vector<std::vector<float>> WCETSUM;
        std::vector<std::vector<SubTask *>> Vsum;
        std::vector<float> TaskDeadLine;
         float MaxDDL=0;
         std::vector<float> callback;
        for(int i=0;i<2;i++)
        {
            callback.push_back(0);
        }
        for (int i = 0; i < taskset.tasks.size(); ++i)
        {
            //*解码任务
            
            taskset.tasks[i].computeLocalDeadlines();
            // vol=taskset.tasks[i].getVolume();
            std::vector<SubTask *> V = taskset.tasks[i].getVertices();
            Vsum.push_back(V);
            //*解码子任务
           
            
            // for (int j = 0; j < ordIDs.size(); j++)
            // {
            //     std::cout << ordIDs[j] << " ";
            // }

            // std::cout <dagSched std::endl;
            //*得到每个子任务的WCET
            // sleep(1);
            std::vector<float> WCET;
            std::vector<float>outdegreeSum(V.size(),0);
            std::vector<int>visit(V.size(),0);
            for(int idx = 0; idx < V.size(); ++idx ){
                WCET.push_back(V[idx]->c);
            }
            WCETSUM.push_back(WCET);
            //*得到每个任务的出度和
            SimpleTimer timer; 
            
            
            // TaspaPriCalculate(V[0],outdegreeSum,visit);
            // for(auto outdegree:outdegreeSum)
            // {
            //     std::cout << outdegree << " ";
            // }
            // std::cout << std::endl;
            
            
            // sleep(1);
            std::vector<SubTask *> assignedTask;
            


             



            // assignedTask=TaspaPriOrder(V,outdegreeSum);
            AllassignedTask.push_back(V);

        }
        // for(int i=0;i<AllassignedTask.size();i++)
        // {
        //     // std::cout <<"Taspa: ";
        //     for(int j=0;j<AllassignedTask[i].size();j++)
        //     {
        //         std::cout  << AllassignedTask[i][j]->id << " ";
        //     }
        //    std::cout << std::endl;
        // }
        std::cout << std::endl;
        std::vector<int> responsetime(AllassignedTask.size());
        float vol=0;
        callback[1]=ExecMul3(AllassignedTask,m,responsetime,EDF,vol,MaxDDL,TaskDeadLine);
        resetV1(Vsum,WCETSUM);

        // int schdulednumber=0;
        // for(int i=0;i<responsetime.size();i++){
        //     if(responsetime[i]<=taskset.tasks[i].getDeadline())
        //     {
        //         std::cout <<"Taspa2023 suc response Time=" << responsetime[i] <<" Deadline:" <<taskset.tasks[i].getDeadline() << std::endl;
        //         schdulednumber++;
        //     }
        //     else
        //     {
        //         std::cout <<"Taspa2023 Fail response Time=" << responsetime[i] <<" Deadline:" <<taskset.tasks[i].getDeadline() << std::endl;
        //     }
        //  }
        // return schdulednumber;
        // std::cout <<"Taspa response Time=" << responsetime[0] <<" Deadline:" <<taskset.tasks[0].getDeadline() << std::endl;
        for(int i=0;i<responsetime.size();i++)
        {
            
            if(responsetime[i]>taskset.tasks[i].getDeadline())
            {
                // for(int j=0;j<responsetime.size();j++)
                // {
                // std::cout <<"taspsa fail res:"<<responsetime[j] << std::endl;
                // std::cout <<"taspsa fail ddl:"<<taskset.tasks[j].getDeadline()<< std::endl;
                // }
                return callback;
            }
        }
        // sleep(1);
        // for(int i=0;i<responsetime.size();i++)
        // {
        //   std::cout <<"Taspa2023 suc response Time=" << responsetime[i] <<" Deadline:" <<taskset.tasks[i].getDeadline() << std::endl;

        // }
        std::cout << std::endl;
        callback[0]=1;
        return callback;    

    }

    std::vector<float> Taspa2023LLF(Taskset taskset, const int m)
    {
        //* 输入：任务集,核心数
        std::sort(taskset.tasks.begin(), taskset.tasks.end(), deadlineMonotonicSorting);
        std::vector<std::vector<SubTask *>> AllassignedTask;
        std::vector<std::vector<float>> WCETSUM;
        std::vector<std::vector<SubTask *>> Vsum;
        std::vector<float> TaskDeadLine;
         float MaxDDL=0;
        std::vector<float> callback;
        for(int i=0;i<2;i++)
        {
            callback.push_back(0);
        }
        for (int i = 0; i < taskset.tasks.size(); ++i)
        {
            //*解码任务
            taskset.tasks[i].computeLocalDeadlines();
            
            // taskset.tasks[i].computeLocalDeadlines();
            // vol=taskset.tasks[i].getVolume();
            std::vector<SubTask *> V = taskset.tasks[i].getVertices();
            Vsum.push_back(V);
            //*解码子任务
           
            
            // for (int j = 0; j < ordIDs.size(); j++)
            // {
            //     std::cout << ordIDs[j] << " ";
            // }

            // std::cout <dagSched std::endl;
            //*得到每个子任务的WCET
            // sleep(1);
            std::vector<float> WCET;
            std::vector<float>outdegreeSum(V.size(),0);
            std::vector<int>visit(V.size(),0);
            for(int idx = 0; idx < V.size(); ++idx ){
                WCET.push_back(V[idx]->c);
            }
            WCETSUM.push_back(WCET);
            //*得到每个任务的出度和
            SimpleTimer timer; 
            
            
            // TaspaPriCalculate(V[0],outdegreeSum,visit);


            std::vector<SubTask *> assignedTask;
            
            // assignedTask=TaspaPriOrder(V,outdegreeSum);
            AllassignedTask.push_back(V);

        }
        // for(int i=0;i<AllassignedTask.size();i++)
        // {
        //     // std::cout <<"Taspa: ";
        //     for(int j=0;j<AllassignedTask[i].size();j++)
        //     {
        //         std::cout  << AllassignedTask[i][j]->id << " ";
        //     }
        //    std::cout << std::endl;
        // }
        std::cout << std::endl;
        std::vector<int> responsetime(AllassignedTask.size());
        float vol=0;
        callback[1]=ExecMul3(AllassignedTask,m,responsetime,LLF,vol,MaxDDL,TaskDeadLine);
        resetV1(Vsum,WCETSUM);

        // int schdulednumber=0;
        // for(int i=0;i<responsetime.size();i++){
        //     if(responsetime[i]<=taskset.tasks[i].getDeadline())
        //     {
        //         std::cout <<"Taspa2023 suc response Time=" << responsetime[i] <<" Deadline:" <<taskset.tasks[i].getDeadline() << std::endl;
        //         schdulednumber++;
        //     }
        //     else
        //     {
        //         std::cout <<"Taspa2023 Fail response Time=" << responsetime[i] <<" Deadline:" <<taskset.tasks[i].getDeadline() << std::endl;
        //     }
        //  }
        // return schdulednumber;
        // std::cout <<"Taspa response Time=" << responsetime[0] <<" Deadline:" <<taskset.tasks[0].getDeadline() << std::endl;
        for(int i=0;i<responsetime.size();i++)
        {
            
            if(responsetime[i]>taskset.tasks[i].getDeadline())
            {
                // for(int j=0;j<responsetime.size();j++)
                // {
                // std::cout <<"taspsa LLF fail res:"<<responsetime[j] << std::endl;
                // std::cout <<"taspsa LLF fail ddl:"<<taskset.tasks[j].getDeadline()<< std::endl;
                // }
                return callback;
            }
        }
        // sleep(1);
        // for(int i=0;i<responsetime.size();i++)
        // {
        //   std::cout <<"Taspa2023 LLF suc response Time=" << responsetime[i] <<" Deadline:" <<taskset.tasks[i].getDeadline() << std::endl;

        // }
        std::cout << std::endl;
        callback[0]=1;
        return callback;    

    }


std::vector<float> Taspa2023DFS(Taskset taskset, const int m)
    {
        //* 输入：任务集,核心数
        std::sort(taskset.tasks.begin(), taskset.tasks.end(), deadlineMonotonicSorting);
        std::vector<std::vector<SubTask *>> AllassignedTask;
        std::vector<std::vector<float>> WCETSUM;
        std::vector<std::vector<SubTask *>> Vsum;
        std::vector<float> TaskDeadLine;
        std::vector<float> callback;
        for(int i=0;i<2;i++)
        {
            callback.push_back(0);
        }
        float MaxDDL=taskset.tasks[taskset.tasks.size()-1].getDeadline();
        for (int i = 0; i < taskset.tasks.size(); ++i)
        {
            TaskDeadLine.push_back(taskset.tasks[i].getDeadline());
        }
        for (int i = 0; i < taskset.tasks.size(); ++i)
        {
            //*解码任务
            // taskset.tasks[i].computeLocalDeadlines();
            float vol=0;
            taskset.tasks[i].computeLocalDeadlines();
            vol=taskset.tasks[i].getVolume();
            std::vector<SubTask *> V = taskset.tasks[i].getVertices();
            
            // vol=taskset.tasks[i].getVolume();
            Vsum.push_back(V);
            //*解码子任务
           
            //*得到每个子任务的WCET
            // sleep(1);
            std::vector<float> WCET;
            std::vector<float>outdegreeSum(V.size(),0);
            std::vector<int>visit(V.size(),0);
            for(int idx = 0; idx < V.size(); ++idx ){
                WCET.push_back(V[idx]->c);
            }
            WCETSUM.push_back(WCET);
            //*得到每个任务的出度和
            SimpleTimer timer; 
            
            
            TaspaPriCalculate(V[0],outdegreeSum,visit);

            
            // sleep(1);
            std::vector<SubTask *> assignedTask;
            // PriOrder1(TaskDeadLine[i])
            // assignedTask=TaspaPriOrder(V,outdegreeSum);
            AllassignedTask.push_back(V);

        }


        // for(int i=0;i<AllassignedTask.size();i++)
        // {
        //     // std::cout <<"Taspa: ";
        //     for(int j=0;j<AllassignedTask[i].size();j++)
        //     {
        //         std::cout  << AllassignedTask[i][j]->id << " ";
        //     }
        //    std::cout << std::endl;
        // }
        std::cout << std::endl;
        std::vector<int> responsetime(AllassignedTask.size());
        // std::cout << "maxmax:" << maxmaxRankd << std::endl;
        callback[1]=ExecMul3(AllassignedTask,m,responsetime,DFS,maxmaxRankd,MaxDDL,TaskDeadLine);
        resetV1(Vsum,WCETSUM);
        
        
        // int schdulednumber=0;
        // for(int i=0;i<responsetime.size();i++){
        //     if(responsetime[i]<=taskset.tasks[i].getDeadline())
        //     {
        //         std::cout <<"Taspa2023 suc response Time=" << responsetime[i] <<" Deadline:" <<taskset.tasks[i].getDeadline() << std::endl;
        //         schdulednumber++;
        //     }
        //     else
        //     {
        //         std::cout <<"Taspa2023 Fail response Time=" << responsetime[i] <<" Deadline:" <<taskset.tasks[i].getDeadline() << std::endl;
        //     }
        //  }
        // return schdulednumber;
        // std::cout <<"Taspa response Time=" << responsetime[0] <<" Deadline:" <<taskset.tasks[0].getDeadline() << std::endl;
        // std::cout << "time:" << callback[1] << std::endl;
        for(int i=0;i<responsetime.size();i++)
        {
            
            if(responsetime[i]>taskset.tasks[i].getDeadline())
            {
                // for(int j=0;j<responsetime.size();j++)
                // {
                // std::cout <<"taspsa DFS fail res:"<<responsetime[j] << std::endl;
                // std::cout <<"taspsa DFS fail ddl:"<<taskset.tasks[j].getDeadline()<< std::endl;
                // }
                return callback;
            }
        }
        // sleep(1);
        // for(int i=0;i<responsetime.size();i++)
        // {
        //   std::cout <<"Taspa2023 DFS suc response Time=" << responsetime[i] <<" Deadline:" <<taskset.tasks[i].getDeadline() << std::endl;

        // }
        std::cout << std::endl;
        callback[0]=1;
        return callback;    

    }


    std::vector<SubTask *> DFSPriOrder(std::vector<SubTask *> &V,float maxRankd,float ddlmax,float ddltask)
    {
        std::vector<SubTask *> assignedTask;//* 已经给了优先级的任务
        std::vector<SubTask *> WaitTask;    //* 等待排序的任务
        fl::Engine engine("fuzzy-controller");
        fuzzyengineEstablish4(engine);
        int pri=0;
    //* 遍历任务节点,找出前序节点为零的节点,加入等待节点序列
        for(auto node : V)
        {
            if(node->pred.size()==0)
            {
                node->isWait=true;
                WaitTask.push_back(node);
            }
        }
    while(V.size()!=assignedTask.size())
    {
        //* 找出所有前序节点已经在等待序列的节点
        //! 应该是不用全搜的  可以优化 降低代码速度
        //! 这里也可以并行优化
        //! 可以用拓扑排序 如果不满足了  就不用接着找了 提升应该很大（好像也不行) (拓扑排序广度优先算法看着可以 但是可能也有问题)
        for (auto node : V)
        {
            bool allPredecessorsExecuted=true;
            if(node->isWait==true)
                continue;
            for(auto predecessor:node->pred)
            {
                if (!predecessor->isReady) {
                allPredecessorsExecuted = false;
                break;
                }
            }
            if(allPredecessorsExecuted==true)
            {
                node->isWait=true;
                WaitTask.push_back(node);
            }
        }
        float max_fpri=0;
        int id=0;
        int delNum=0;
        //todo 如果以后有FPGA的考量,可以再思考硬件增益、通讯代价也加入模糊化
        for(auto wait:WaitTask)
        {
                 wait->fpriValue=FuzzyCalculate2(wait->taspaorder,(wait->localD-wait->c),wait->localD,engine,maxRankd,ddlmax,ddltask);
                //  float FuzzyCalculate2(float Rankd,float Laxity,float ld,)

        }
        for(int taskNum=0;taskNum<WaitTask.size();taskNum++)//*提取最大的
        {
            if(WaitTask[taskNum]->fpriValue>max_fpri)
            {
                max_fpri=WaitTask[taskNum]->fpriValue;
                id=WaitTask[taskNum]->id;
                delNum=taskNum;
            }
        }
        for(auto node:V)
        {
            if(node->id==id)//!当前最大的这个点
            {
                node->prio=pri++; //todo 找一下它和assigned的关系
                node->isReady=true;
                // node->fpriValue=0;
                assignedTask.push_back(node);
                break;
            }
        }
        WaitTask.erase(WaitTask.begin()+delNum);//!清理
    }
    // sleep(1);
    return assignedTask;

    }


     int Taspa2023DFSFS(Taskset taskset, const int m)
    {
        //* 输入：任务集,核心数
        std::sort(taskset.tasks.begin(), taskset.tasks.end(), deadlineMonotonicSorting);
        std::vector<std::vector<SubTask *>> AllassignedTask;
        std::vector<std::vector<float>> WCETSUM;
        std::vector<std::vector<SubTask *>> Vsum;
        std::vector<float> TaskDeadLine;
        std::vector<float> callback;
        for(int i=0;i<2;i++)
        {
            callback.push_back(0);
        }
        float MaxDDL=taskset.tasks[taskset.tasks.size()-1].getDeadline();
        for (int i = 0; i < taskset.tasks.size(); ++i)
        {
            TaskDeadLine.push_back(taskset.tasks[i].getDeadline());
        }
        for (int i = 0; i < taskset.tasks.size(); ++i)
        {
            //*解码任务
            float vol=0;
            taskset.tasks[i].computeLocalDeadlines();
            vol=taskset.tasks[i].getVolume();
            std::vector<SubTask *> V = taskset.tasks[i].getVertices();

            Vsum.push_back(V);
            //*解码子任务
           
            

            //*得到每个子任务的WCET
            // sleep(1);
            std::vector<float> WCET;
            std::vector<float>outdegreeSum(V.size(),0);
            std::vector<int>visit(V.size(),0);
            for(int idx = 0; idx < V.size(); ++idx ){
                WCET.push_back(V[idx]->c);
            }
            WCETSUM.push_back(WCET);
            //*得到每个任务的出度和
            SimpleTimer timer; 
            
            
            TaspaPriCalculate(V[0],outdegreeSum,visit);

            std::vector<SubTask *> assignedTask;

            assignedTask=DFSPriOrder(V,maxmaxRankd,MaxDDL,TaskDeadLine[i]);



            AllassignedTask.push_back(assignedTask);

        }


        std::vector<int> responsetime(AllassignedTask.size());

        ExecMul3(AllassignedTask,m,responsetime,LLF,maxmaxRankd,MaxDDL,TaskDeadLine);
        resetV1(Vsum,WCETSUM);

        for(int i=0;i<responsetime.size();i++)
        {
            
            if(responsetime[i]>taskset.tasks[i].getDeadline())
            {
                // for(int j=0;j<responsetime.size();j++)
                // {
                // std::cout <<"taspsa DFSFS fail res:"<<responsetime[j] << std::endl;
                // std::cout <<"taspsa DFSFS fail ddl:"<<taskset.tasks[j].getDeadline()<< std::endl;
                // }
                return false;
            }
        }
        // sleep(1);
        // for(int i=0;i<responsetime.size();i++)
        // {
        //   std::cout <<"Taspa2023 DFSFS suc response Time=" << responsetime[i] <<" Deadline:" <<taskset.tasks[i].getDeadline() << std::endl;

        // }
        // std::cout << std::endl;
        return true;    

    }


}