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

using namespace  dagSched;

struct CORETYPE
{
    int runningtask;
    int status=IDLE;//状态idel busy 空闲或忙碌
    int lefttime;//剩下的时间
    int busytime;//忙碌的时间
    int tasknum;//目前在运行的任务号
    std::vector<int> finishjob;
};

      void resetV(std::vector<std::vector<SubTask *>> V,std::vector<std::vector<float>> WCET)
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
    void fuzzyengineEstablish3(fl::Engine &engine)
    {
    // fl::Engine engine("fuzzy-controller");
    engine.setDescription("WCET and outdegreeSum and waitTime-based fuzzy controller");
    // 创建输入变量 WCET
    
    fl::InputVariable* input1 = new fl::InputVariable;
    input1->setName("WCET");
    input1->setRange(0, 1); // 设置输入范围
    input1->addTerm(new fl::ZShape("LOW", 0,0.5)); // 添加隶属度函数
    input1->addTerm(new fl::Gaussian("MEDIUM",0.5,0.125));
    input1->addTerm(new fl::SShape("HIGH", 0.5,1));
    engine.addInputVariable(input1);

    // 创建输入变量 outdegreeSum
    fl::InputVariable* input2 = new fl::InputVariable;
    input2->setName("outdegreeSum");
    input2->setRange(0, 1); // 设置输入范围
    input2->addTerm(new fl::ZShape("LOW", 0,0.5)); // 添加隶属度函数
    input2->addTerm(new fl::Gaussian("MEDIUM",0.5,0.125));
    input2->addTerm(new fl::SShape("HIGH", 0.5,1));
    engine.addInputVariable(input2);

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
    output1->setName("output");
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
    ruleblock1->addRule(fl::Rule::parse("if WCET is LOW and outdegreeSum is LOW then output is VERY_LOW  with 1", &engine));
    ruleblock1->addRule(fl::Rule::parse("if WCET is LOW and outdegreeSum is MEDIUM then output is LOW  with 0.5", &engine));
    ruleblock1->addRule(fl::Rule::parse("if WCET is LOW and outdegreeSum is HIGH then output is MEDIUM  with 1", &engine));


    ruleblock1->addRule(fl::Rule::parse("if WCET is MEDIUM and outdegreeSum is LOW then output is LOW  with 0.5", &engine));
    ruleblock1->addRule(fl::Rule::parse("if WCET is MEDIUM and outdegreeSum is MEDIUM then output is MEDIUM  with 1", &engine));
    ruleblock1->addRule(fl::Rule::parse("if WCET is MEDIUM and outdegreeSum is HIGH then output is HIGH  with 0.5", &engine));

    ruleblock1->addRule(fl::Rule::parse("if WCET is HIGH and outdegreeSum is LOW  then output is MEDIUM  with 1", &engine));
    ruleblock1->addRule(fl::Rule::parse("if WCET is HIGH and outdegreeSum is MEDIUM then output is HIGH  with 0.5", &engine));
    ruleblock1->addRule(fl::Rule::parse("if WCET is HIGH and outdegreeSum is HIGH  then output is VERY_HIGH  with 1", &engine));

    // ruleblock1->addRule(fl::Rule::parse("if WCET is HIGH then output is HIGH  with 1", &engine));
    // ruleblock1->addRule(fl::Rule::parse("if WCET is MEDIUM then output is MEDIUM with 1", &engine));
    // ruleblock1->addRule(fl::Rule::parse("if WCET is LOW then output is LOW with 1", &engine));

    // ruleblock1->addRule(fl::Rule::parse("if outdegreeSum is HIGH then output is HIGH with 1", &engine));
    // ruleblock1->addRule(fl::Rule::parse("if outdegreeSum is MEDIUM then output is MEDIUM with 1", &engine));
    // ruleblock1->addRule(fl::Rule::parse("if outdegreeSum is LOW then output is LOW with 1", &engine));

    // ruleblock1->addRule(fl::Rule::parse("if Laxity is LOW then output is HIGH with 1", &engine));
    // ruleblock1->addRule(fl::Rule::parse("if Laxity is MEDIUM then output is MEDIUM with 1", &engine));
    // ruleblock1->addRule(fl::Rule::parse("if Laxity is HIGH then output is LOW with 1", &engine));


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
    // fl::FisExporter exporter;
    // std::string engineString = exporter.toString(&engine);
    // engineString.replace(engineString.find("Version=6"), 9, "Version=2");
    // // // 将引擎字符串保11存到文件
    // static int count=0;
    // std::ofstream file("fuzzy"+ std::to_string(count++) +".fis");
    // file << engineString;
    // file.close();
    }
    float FuzzyCalculate1(float WCET,float outdegreeSum,float ld,float c,fl::Engine &engine,float vol,int Vsize)
{
    // 计算模糊控制器的输出变量
    // std::cout << vol << std::endl;
    engine.setInputValue("WCET",WCET/vol);
    engine.setInputValue("outdegreeSum",outdegreeSum/Vsize);
    // engine.setInputValue("Laxity",(ld-c)/ld);
    engine.process();
    float outputValue = engine.getOutputValue("output");

    return outputValue;
}
    bool RelyFinish(SubTask * assignTask)
    {
        for(auto task:assignTask->pred)
        {
            if(task->isfinish==false)
            return false;
        }
        return true;
    }
    int Exec(std::vector<SubTask *> &assignTask,int coreNum)
    {
        int time=0;
        CORETYPE  core[coreNum];
        // for(int i=0;i<assignTask.size();i++)  //* 打印本次要执行的任务
        // {
        //     std::cout <<"ID:" << assignTask[i]->id <<" ";
        // }
        // std::cout << std::endl;

        int allstatus=BUSY;
        while(allstatus==BUSY)
        {
            allstatus=IDLE;
            for(int i=0;i<coreNum;i++)
            {
                if(core[i].status==BUSY)
                {
                    core[i].busytime++;
                    assignTask[core[i].runningtask]->c--;
                    if(assignTask[core[i].runningtask]->c<0)
                    {
                        assignTask[core[i].runningtask]->isfinish=true;
                        core[i].finishjob.push_back(core[i].runningtask);
                        core[i].status=IDLE;
                        // std::cout << "task:" << assignTask[core[i].runningtask]->id <<" finfished" <<" at core:" << i  <<" at time:"  << time << std::endl; 
                    }
                }
            }
            for(int i=0;i<coreNum;i++)
            {
                if(core[i].status==IDLE&&!assignTask.empty())//空闲且不为空
                {
                    // cout << "i am in" << endl;
                    for(int taskNumNow=0;taskNumNow<assignTask.size();taskNumNow++)
                    {
                        if(RelyFinish(assignTask[taskNumNow])==true && assignTask[taskNumNow]->isfinish==false &&assignTask[taskNumNow]->isrunning==false )
                        {

                            core[i].runningtask=taskNumNow;
                            // std::cout << "Core[" << i <<"].runninigtask=" <<assignTask[taskNumNow]->id << " at time:" << time << std::endl;
                            // assignTask.erase(assignTask.begin()+taskNumNow);
                            // cout <<"xixi1"<<endl;
                            assignTask[taskNumNow]->isrunning=true;
                            core[i].status=BUSY;
                            break;
                        }
                        
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
        return time;
    }
    std::vector<SubTask *> PriOrder(std::vector<SubTask *> &V,std::vector<float> WCET,std::vector<int> outdegreeSum,int coreNum,int pri,fl::Engine &engine,int mode,float vol)
    {
        std::vector<SubTask *> assignedTask;//* 已经给了优先级的任务
        std::vector<SubTask *> WaitTask;    //* 等待排序的任务
        std::vector<SubTask*> V_copy(V);
    //* 遍历任务节点,找出前序节点为零的节点,加入等待节点序列
        for(auto node : V)
        {
            if(node->pred.size()==0)
            {
                node->isWait=true;
                WaitTask.push_back(node);
            }
        }
        //*并行执行,第一次特别慢？！！！！
    //     std::for_each(std::execution::par, V_copy.begin(), V_copy.end(), [&](auto node) {
    //     if (node->pred.size() == 0) {
    //         node->isWait = true;
    //         WaitTask.push_back(node);
    //     }
    // });
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
        // std::cout <<"Test" << std::endl;
        for(auto wait:WaitTask)
        {
            // if(wait->isReady==false)
            // {
                 wait->fpriValue=FuzzyCalculate1(wait->wcetSum,outdegreeSum[wait->id],wait->localD,wait->c,engine,vol,V.size());
            // }
           
            // wait->fpriValue=FuzzyCalculate1(WCET[wait->id],outdegreeSum[wait->id],wait->waitTime);
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
        // std::cout<<"ID:" <<id << " Max_FPRI:" << max_fpri<<" taskNum:" <<delNum<< std::endl;
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
        if(mode==0)
        {
            for(auto wait:WaitTask)//!还在等待队列的元素,添加等待时间
            {
                wait->waitTime++;
            }
        }
    }
    // sleep(1);
    return assignedTask;
    //     

    //     //*对可以执行的对象进行模糊优先级排序,以最差执行时间WCET,出度和outdegreeSum作为因素集


    //     //*更新下一次满足条件的节点


    //     //*再次进行排序

    }

    int WCETFunction(SubTask* V,std::vector<int> &visit)
    {
    //    std::cout << V->succ.size() << endl;
       if(V->succ.size()==0)
       {
            visit[V->id]=1;
            V->wcetSum=V->c;
            return V->wcetSum;
       }
       else if(visit[V->id]==0)
       {
            V->wcetSum=V->c;
       }
       else if(visit[V->id]==1)
       {
            return V->wcetSum;
       }
       for(int i=0;i<V->succ.size();i++)
       {
            V->wcetSum+=WCETFunction(V->succ[i],visit);
       }
       visit[V->id]=1;
       return V->wcetSum;
    }


    int outdegreeFunction(SubTask* V,std::vector<int> &outdegreeSum,std::vector<int> &visit)
    {
    //    std::cout << V->succ.size() << endl;
       if(V->succ.size()==0)
       {
            visit[V->id]=1;
            return V->succ.size();
       }
       else if(visit[V->id]==0)
       {
            outdegreeSum[V->id]=V->succ.size();
       }
       else if(visit[V->id]==1)
       {
            return outdegreeSum[V->id];
       }
       for(int i=0;i<V->succ.size();i++)
       {
            outdegreeSum[V->id]+=outdegreeFunction(V->succ[i],outdegreeSum,visit);
       }
       visit[V->id]=1;
       return outdegreeSum[V->id];
    }

   
    int ExecMul(std::vector<std::vector<SubTask *>> AllassignedTask,int coreNum,std::vector<int> &responsetime)
    {
        int time=0;
        CORETYPE  core[coreNum];
        int allstatus=IDLE;
        int assignPeriod = 5;
        int nextAssign=0;
        bool assignflag=true;
        while(!AllassignedTask.empty()||allstatus==BUSY)
        {
             for (int i = 0; i < coreNum; i++)
             {
                if (core[i].status == BUSY)
                {
                    core[i].busytime++;
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
                for (int task = 0; task < AllassignedTask.size(); task++)
                {
                    for (int subtaskNumNow = 0; subtaskNumNow < AllassignedTask[task].size(); subtaskNumNow++)
                    {
                                if (RelyFinish(AllassignedTask[task][subtaskNumNow]) == true && AllassignedTask[task][subtaskNumNow]->isfinish == false && AllassignedTask[task][subtaskNumNow]->isrunning == false)
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
                                        for (int i = 0; i < coreNum; i++)
                                        {
                                            //!在此处更新模糊值,完成抢占
                                            // if((AllassignedTask[task][subtaskNumNow]->localD-time < AllassignedTask[core[i].tasknum][core[i].runningtask]->localD-AllassignedTask[core[i].tasknum][core[i].runningtask]->runningtime-time))
                                            // if((AllassignedTask[task][subtaskNumNow]->fpriValue > AllassignedTask[core[i].tasknum][core[i].runningtask]->fpriValue))
                                            // if(task < core[i].tasknum)
                                                                                        if((AllassignedTask[task][subtaskNumNow]->localD-time < AllassignedTask[core[i].tasknum][core[i].runningtask]->localD-time))

                                            {

                                                AllassignedTask[core[i].tasknum][core[i].runningtask]->isrunning = false;
                                                core[i].tasknum = task;
                                                core[i].runningtask = subtaskNumNow;
                                            //    std::cout <<  "Inp!:Core[" << i <<"]:task:" <<task << " subtask:" << AllassignedTask[task][subtaskNumNow]->id << " start at time:" << time << std::endl;
                                                // cout <<"xixi1"<<endl;
                                                AllassignedTask[task][subtaskNumNow]->isrunning = true;
                                                core[i].status = BUSY;
                                                scheduled=true;
                                                break;
                                            }
                                        }
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
    }
namespace  dagSched
{

    int GP_FP_Huang2025_Multi(Taskset taskset, const int m)
    {
        //* 输入：任务集,核心数
        std::sort(taskset.tasks.begin(), taskset.tasks.end(), deadlineMonotonicSorting);
        float exectime=0;
        std::vector<std::vector<SubTask *>> AllassignedTask;
        std::vector<std::vector<SubTask *>> Vsum;
        std::vector<std::vector<float>> WCETSUM;
        for (int i = 0; i < taskset.tasks.size(); ++i)
        {
            //*解码任务
            float vol=0;
            taskset.tasks[i].computeLocalDeadlines();
            vol=taskset.tasks[i].getVolume();
            std::vector<SubTask *> V = taskset.tasks[i].getVertices();
            Vsum.push_back(V);
            //*解码子任务
            fl::Engine engine("fuzzy-controller1");
            
            //*得到每个子任务的WCET
            std::vector<float> WCET;
            std::vector<int>outdegreeSum(V.size(),0);
            std::vector<int>visit(V.size(),0);
            std::vector<int>visit1(V.size(),0);
            for(int idx = 0; idx < V.size(); ++idx ){
                WCET.push_back(V[idx]->c);
            }
            WCETSUM.push_back(WCET);
            //*得到每个任务的出度和
            WCETFunction(V[0],visit1);
            outdegreeFunction(V[0],outdegreeSum,visit);

            fuzzyengineEstablish3(engine);
            //*进行优先级排序
            // timer.tic();
            
            std::vector<SubTask *> assignedTask;
            assignedTask=PriOrder(V,WCET,outdegreeSum,m,0,engine,0,vol);
            AllassignedTask.push_back(assignedTask);
            //*得到每个任务的等待时间
            //*并行度
        }
        std::vector<int> responsetime(AllassignedTask.size());
        // for(int i=0;i<AllassignedTask.size();i++)
        // {
        //     std::cout <<"Huang: ";
        //     for(int j=0;j<AllassignedTask[i].size();j++)
        //     {
        //         std::cout << AllassignedTask[i][j]->id << " ";
        //     }
        //    std::cout << std::endl;
        // }
        
        ExecMul(AllassignedTask,m,responsetime);

        resetV(Vsum,WCETSUM);
        for(int i=0;i<responsetime.size();i++)
        {
            if(responsetime[i]>taskset.tasks[i].getDeadline())
            {
                for(int j=0;j<responsetime.size();j++)
                {
                std::cout <<"response Time=" << responsetime[j] <<" Deadline:" <<taskset.tasks[j].getDeadline() << std::endl;
                 std::cout <<"Huang2025 fail response Time=" << responsetime[j] <<" Deadline:" <<taskset.tasks[j].getDeadline() << std::endl;
                }
                
                return false;
            }   
        }
        for(int i=0;i<responsetime.size();i++)
        {
            std::cout <<"Huang2025 suc response Time=" << responsetime[i] <<" Deadline:" <<taskset.tasks[i].getDeadline() << std::endl;

        }
        return true;
    }
}

