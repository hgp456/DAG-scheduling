#include "dagSched/tests.h"
// //* 1.求每个子任务的WCET,后续节点个数
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


struct CORETYPE
{
    int runningtask;
    int status=IDLE;//状态idel busy 空闲或忙碌
    int lefttime;//剩下的时间
    int busytime;//忙碌的时间
    int tasknum;//目前在运行的任务号
    std::vector<int> finishjob;
};


namespace dagSched
{
    bool RelyFinish2(SubTask * assignTask)
    {
        for(auto task:assignTask->pred)
        {
            if(task->isfinish==false)
            return false;
        }
        return true;
    }



    std::vector<SubTask *> TopologicalPriOrder(std::vector<SubTask *> &V)
    {
        std::vector<SubTask *> assignedTask;//* 已经给了优先级的任务
        std::vector<SubTask *> WaitTask;    //* 等待排序的任务
        int prio=0;
        //*找Outdegree中最大的数,他就是V的ID 把和ID匹配的扔到分配任务中即可
        while(V.size()!=assignedTask.size())
        {
            int min=V.size()+1;
            int id=0;
            for(int i=0;i<V.size();i++)
            {
                if(V[i]->prio<min)
                {
                    min=V[i]->prio;
                    id=i;
                }
            }
            for(auto node:V)
            {
                if(node->id==id)//!当前最大的这个点
                {
                    // node->prio=prio++; //todo 找一下它和assigned的关系
                    node->isReady=true;
                    node->prio=V.size()+1;
                    assignedTask.push_back(node);
                    // std::cout << max << " " << id << std::endl;
                    break;
                }
            }
        }
        // sleep(1);
        return assignedTask;
    }
    int ExecMul4(std::vector<std::vector<SubTask *>> AllassignedTask,int coreNum,std::vector<int> &responsetime)
    {
        int time=0;
        CORETYPE  core[coreNum];
        int allstatus=IDLE;
        int assignPeriod = 5;
        int nextAssign=0;
        bool assignflag=true;
        std::cout << "Taspa:!!!" << std::endl;
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
                                if (RelyFinish2(AllassignedTask[task][subtaskNumNow]) == true && AllassignedTask[task][subtaskNumNow]->isfinish == false && AllassignedTask[task][subtaskNumNow]->isrunning == false)
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
                                            if(AllassignedTask[task][subtaskNumNow]->localD-time < AllassignedTask[core[i].tasknum][core[i].runningtask]->localD-time)
                                            //  if(task < core[i].tasknum || task==core[i].tasknum && subtaskNumNow <core[i].runningtask )
                                            {
                                                AllassignedTask[core[i].tasknum][core[i].runningtask]->isrunning = false;
                                                core[i].tasknum = task;
                                                core[i].runningtask = subtaskNumNow;
                                                // std::cout <<  "Inp!:Core[" << i <<"]:task:" <<task << " subtask:" << AllassignedTask[task][subtaskNumNow]->id << " start at time:" << time << std::endl;

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
    int ExecMul2(std::vector<std::vector<SubTask *>> AllassignedTask,int coreNum,std::vector<int> &responsetime)
    {
        int time=0;
        CORETYPE  core[coreNum];
        int allstatus=IDLE;
        while(!AllassignedTask.empty()||allstatus==BUSY)
        {
             for (int i = 0; i < coreNum; i++)
             {
                if (core[i].status == BUSY)
                {
                    core[i].busytime++;
                    AllassignedTask[core[i].tasknum][core[i].runningtask]->c--;
                    if (AllassignedTask[core[i].tasknum][core[i].runningtask]->c <= 0)
                    {
                        AllassignedTask[core[i].tasknum][core[i].runningtask]->isfinish = true;
                        core[i].finishjob.push_back(core[i].runningtask);
                        core[i].status = IDLE;
                        responsetime[core[i].tasknum]=time;
                        // std::cout << "task:" << core[i].tasknum  << " " << "subtask:" << AllassignedTask[core[i].tasknum][core[i].runningtask]->id << " finfished"
                        //       << " at core:" << i << " at time:" << time << std::endl;
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
                                    if (RelyFinish2(AllassignedTask[task][subtaskNumNow]) == true && AllassignedTask[task][subtaskNumNow]->isfinish == false && AllassignedTask[task][subtaskNumNow]->isrunning == false)
                                    {
                                        if(core[i].status==IDLE)
                                        {
                                            core[i].tasknum = task;
                                            core[i].runningtask = subtaskNumNow;
                                            // AllassignedTask[core[i].tasknum][core[i].runningtask]->c--;
                                            // std::cout <<  "Core[" << i <<"]:task:" <<task << " subtask:" << AllassignedTask[task][subtaskNumNow]->id << " start at time:" << time << std::endl;
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
                                                // std::cout <<  "Inp!:Core[" << i <<"]:task:" <<task << " subtask:" << AllassignedTask[task][subtaskNumNow]->id << " start at time:" << time << std::endl;

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

    void resetV2(std::vector<std::vector<SubTask *>> V,std::vector<std::vector<float>> WCET)
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

    float max_pred(std::vector<SubTask *> pred,std::vector<float> &lf)
    {
        float max=0;
        for(auto pre:pred)
        {
            if(lf[pre->id]>max)
            {
                max=lf[pre->id];
            }
        }
        return max;
    }

    float max_succ(std::vector<SubTask *> succ,std::vector<float> &lb)
    {
        float max=0;
        for(auto suc:succ)
        {
            if(lb[suc->id]>max)
            {
                max=lb[suc->id];
            }
        }
        return max;
    }
    void compute_length(std::vector<SubTask *> V,std::vector<float> &lf,std::vector<float> &lb,std::vector<int> ordIDs,std::vector<float> &l)
    {
        lf[0]=V[0]->c;
        for(auto id:ordIDs)
        {
            if(id!=0)//! 不是源节点
            {
                lf[id]=V[id]->c+max_pred(V[id]->pred,lf);
            }
        }
        lb[ordIDs[ordIDs.size()-1]]=V[ordIDs[ordIDs.size()-1]]->c; //!拓扑排序最后的节点
        for(int succid=ordIDs.size()-1;succid>=0;succid--)
        {
            if(ordIDs[succid]!=1)//!不是末尾节点
            {
                lb[ordIDs[succid]]=V[ordIDs[succid]]->c + max_succ(V[ordIDs[succid]]->succ,lb);
            }
        }
        // for(auto lff:lf)
        // {
        //     std::cout <<  lff <<" ";
        // }
        // std::cout << std::endl;
        // for(auto lbb:lb)
        // {
        //     std::cout <<  lbb <<" ";
        // }
        // std::cout << std::endl;
        for(int i=0;i<ordIDs.size();i++)
        {
            l[i]=lf[i]+lb[i]-V[i]->c;
            // std::cout << l[i] <<" ";
        }
        // std::cout << std::endl;
        // sleep(1);
    }
    //! 有点小问题,就是还没做后继优先
    void criticalPriorder(std::vector<SubTask *> V,std::vector<float> &lf,std::vector<float> &lb,std::vector<float> &l,std::vector<SubTask *> &assignedTask)
    {
        int prio=0;
        // std::vector<SubTask *> assignedTask;//* 已经给了优先级的任务
        std::vector<SubTask *> WaitTask;    //* 等待排序的任务
    //* 遍历任务节点,找出前序节点为零的节点,加入等待节点序列
        for(auto node : V)
        {
            if(node->pred.size()==0&&node->isWait==false)
            {
                node->isWait=true;
                WaitTask.push_back(node);
                // std::cout << "push id:" <<node->id << std::endl;
            }
        }
        while(assignedTask.size()!=V.size())
        {
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
            float max=0;
            int id=0;
            int delNum=0;
            // std::cout << WaitTask.size() << std::endl; 
            for(int taskNum=0;taskNum<WaitTask.size();taskNum++)//*提取最大的
            {
                if(l[WaitTask[taskNum]->id]>max)
                {
                    max=l[WaitTask[taskNum]->id];
                    // std::cout << "max:"<<max << std::endl; 
                    id=WaitTask[taskNum]->id;
                    delNum=taskNum;
                }
            }
            for(auto node:V)
            {
                if(node->id==id)//!当前最大的这个点
                {
                    node->prio=prio++; //todo 找一下它和assigned的关系
                    node->isReady=true;
                    // node->fpriValue=0;
                    l[WaitTask[delNum]->id]=0;
                    // std::cout << "push id:" <<node->id << std::endl;
                    assignedTask.push_back(node);
                    break;
                }
            }
            WaitTask.erase(WaitTask.begin()+delNum);//!清理
            // for(int i=0;i<V[id]->succ.size();i++)
            // {
            //     std::cout << "succ:" <<V[id]->succ[i]->id  << " ";
            // }
        }
        // std::cout <<"i am out " << std::endl;
    }

    bool critical_path(Taskset taskset, const int m)
    {
        float exectime=0;
        std::vector<std::vector<SubTask *>> AllassignedTask;
        std::vector<std::vector<float>> WCETSUM;
        std::vector<std::vector<SubTask *>> Vsum;
        std::sort(taskset.tasks.begin(), taskset.tasks.end(), deadlineMonotonicSorting);
        for(int i=0; i<taskset.tasks.size(); ++i){
            std::vector<SubTask *> V = taskset.tasks[i].getVertices();
            Vsum.push_back(V);
            std::vector<int> ordIDs = taskset.tasks[i].getTopologicalOrder();

            std::vector<float> WCET;
            for(int idx = 0; idx < V.size(); ++idx ){
                WCET.push_back(V[idx]->c);
            }
            WCETSUM.push_back(WCET);


            std::vector<float> lf(V.size(),0);
            std::vector<float> lb(V.size(),0);
            std::vector<float> l(V.size(),0);
            compute_length(V,lf,lb,ordIDs,l);

            std::vector<SubTask *> assignedTask;
            criticalPriorder(V,lf,lb,l,assignedTask);
            AllassignedTask.push_back(assignedTask);
        }
        std::vector<int> responsetime(AllassignedTask.size());
        ExecMul4(AllassignedTask,m,responsetime);
        resetV2(Vsum,WCETSUM);
        for(int i=0;i<responsetime.size();i++)
        {
            if(responsetime[i]>taskset.tasks[i].getDeadline())
            return false;
        }
        return true;
    }




    bool TopologicalScheduling(Taskset taskset, const int m)
    {
        float exectime=0;
        std::vector<std::vector<SubTask *>> AllassignedTask;
        std::vector<std::vector<float>> WCETSUM;
        std::vector<std::vector<SubTask *>> Vsum;
        std::sort(taskset.tasks.begin(), taskset.tasks.end(), deadlineMonotonicSorting);
        for(int i=0; i<taskset.tasks.size(); ++i){
            std::vector<SubTask *> V = taskset.tasks[i].getVertices();
            Vsum.push_back(V);
            std::vector<int> ordIDs = taskset.tasks[i].getTopologicalOrder();
            std::vector<float> WCET;
            for(int idx = 0; idx < V.size(); ++idx ){
                WCET.push_back(V[idx]->c);
            }
            WCETSUM.push_back(WCET);
            for(int idx = 0, i; idx < ordIDs.size() ; ++idx ){
                i = ordIDs[idx]; // vertex index
                V[i]->prio = idx;
            }
            std::vector<SubTask *> assignedTask;
            assignedTask=TopologicalPriOrder(V);
            AllassignedTask.push_back(assignedTask);

            
        }
        std::vector<int> responsetime(AllassignedTask.size());
        ExecMul4(AllassignedTask,m,responsetime);
        resetV2(Vsum,WCETSUM);

        //  std::cout <<"To response Time=" << responsetime[0] <<" Deadline:" <<taskset.tasks[0].getDeadline() << std::endl;
        for(int i=0;i<responsetime.size();i++)
        {
            // std::cout << responsetime[i] << std::endl;
            
            if(responsetime[i]>taskset.tasks[i].getDeadline())
            return false;
        }
        // sleep(1);
        return true;    
    }
}