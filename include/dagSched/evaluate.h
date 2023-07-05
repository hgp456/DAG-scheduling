#ifndef EVALUATE_H
#define EVALUATE_H

#include "dagSched/utils.h"
#include "dagSched/GeneratorParams.h"
#include "dagSched/Taskset.h"
#include "dagSched/tests.h"
#include "dagSched/plot_utils.h"

namespace dagSched{

void evaluate(const std::string& genparams_path, const std::string& output_fig_path, const bool show_plots){
    GeneratorParams gp;
    gp.readFromYaml(genparams_path);
    gp.configureParams(gp.gType);

    std::vector<float> AVGDFS;
    std::vector<float> AVGLLF;
    std::vector<float> AVGEDF;
    AVGDFS.push_back(0);
    AVGDFS.push_back(0);
    AVGLLF.push_back(0);
    AVGLLF.push_back(0);
    AVGEDF.push_back(0);
    AVGEDF.push_back(0);

    float U_curr = gp.Utot;
    if(gp.gType == GenerationType_t::VARYING_U)
        U_curr = gp.Umin;

    int m = gp.m;
    std::cout <<"m:" << m << std::endl;
    if(gp.gType == GenerationType_t::VARYING_M)
        m = gp.mMin - 1;

    std::cout <<"m:" << m << std::endl;
    int n_tasks = gp.nTasks;
    if(gp.gType == GenerationType_t::VARYING_N)
        n_tasks = gp.nMin - 1;

    std::map<std::string,std::vector<float>> sched_res;
    std::map<std::string,std::vector<double>> time_res;
    std::vector<float> x;

    int test_idx = -1;
    SimpleTimer timer;

    int min_V_all  = 100;
    int max_V_all  = 0;
    bool first=false;
    for(int i=0; i<gp.nTasksets; ++i){
        if(gp.gType == GenerationType_t::VARYING_U && i % gp.tasksetPerVarFactor == 0){
            U_curr += gp.stepU;
            x.push_back(U_curr);
            test_idx++;
        }
        else if(gp.gType == GenerationType_t::VARYING_N && i % gp.tasksetPerVarFactor == 0){
            n_tasks += gp.stepN;
            x.push_back(n_tasks);
            test_idx++;
        }
        else if(gp.gType == GenerationType_t::VARYING_M && i % gp.tasksetPerVarFactor == 0){
            m += gp.stepM;
            x.push_back(m);
            test_idx++;
            std::cout <<"m: "<< m-1 << " avg dfs time=" << AVGDFS[0]/AVGDFS[1] << " AllTime:" << AVGDFS[0]<< " Tasknum:" << AVGDFS[1] << std::endl;
            std::cout <<"m: "<< m-1 << " avg llf time=" << AVGLLF[0]/AVGLLF[1] << " AllTime:" << AVGLLF[0]<< " Tasknum:" << AVGLLF[1] << std::endl;
            std::cout <<"m: "<< m-1 << " avg edf time=" << AVGEDF[0]/AVGEDF[1] << " AllTime:" << AVGEDF[0]<< " Tasknum:" << AVGEDF[1] << std::endl;
            AVGDFS.clear();
            AVGLLF.clear();
            AVGEDF.clear();
        }
        
        Taskset task_set;
    // if(first==false)
    // {

    
        task_set.generate_taskset_Melani(n_tasks, U_curr, m, gp);
        // first=true;
    // }
        int max_v_size = 0;
        for(int ii=0; ii<task_set.tasks.size(); ++ii)
            if(task_set.tasks[ii].getVertices().size() > max_v_size)
                max_v_size = task_set.tasks[ii].getVertices().size();
        if (max_v_size > max_V_all) max_V_all = max_v_size;
        if (max_v_size < min_V_all) min_V_all = max_v_size;

        std::cout<<"taskset: "<<i<<" U: "<<U_curr<<" ntasks: "<<task_set.tasks.size()<<" max|V|: "<<max_V_all<<" min|V|: "<<min_V_all<<" m:"<<m<< " test_idx: "<<test_idx<<std::endl;

        // for(int x=0; x<task_set.tasks.size();++x){
        //     task_set.tasks[x].saveAsDot("task"+std::to_string(i)+"test"+std::to_string(x)+".dot");
        //     std::string dot_command = "dot -Tpng task"+std::to_string(i)+"test"+std::to_string(x)+".dot > task"+ std::to_string(i)+"test"+std::to_string(x)+".png";
        //     system(dot_command.c_str());
        // }

        if(gp.sType == SchedulingType_t::GLOBAL){
            switch (gp.dtype){
            case DeadlinesType_t::CONSTRAINED: case DeadlinesType_t::IMPLICIT:
                if(gp.aType == AlgorithmType_t::EDF && gp.DAGType == DAGType_t::DAG ){
                    if(i % gp.tasksetPerVarFactor == 0){
                        sched_res["Qamhieh2013"].push_back(0);
                        // sched_res["Baruah2014"].push_back(0);
                        sched_res["Melani2015"].push_back(0);
                        sched_res["Huang2023"].push_back(0);
                    }

                    timer.tic();
                    sched_res["Qamhieh2013"][test_idx] += GP_FP_EDF_Qamhieh2013_C(task_set, m);
                    time_res["Qamhieh2013"].push_back(timer.toc());

                    // timer.tic();
                    // sched_res["Baruah2014"][test_idx] += GP_FP_EDF_Baruah2014_C(task_set, m);
                    // time_res["Baruah2014"].push_back(timer.toc());

                    timer.tic();
                    sched_res["Melani2015"][test_idx] += GP_FP_EDF_Melani2015_C(task_set, m);
                    time_res["Melani2015"].push_back(timer.toc());
                    
                    if(gp.wType == workloadType_t::SINGLE_DAG){
                        if(i % gp.tasksetPerVarFactor == 0)
                            sched_res["Baruah2012"].push_back(0);
                        
                        timer.tic();
                        sched_res["Baruah2012"][test_idx] += GP_FP_EDF_Baruah2012_C(task_set.tasks[0], m);
                        time_res["Baruah2012"].push_back(timer.toc());
                    }

                    if(gp.dtype != DeadlinesType_t::CONSTRAINED){
                        if(i % gp.tasksetPerVarFactor == 0)
                            sched_res["Li2013"].push_back(0);
                        
                        timer.tic();
                        sched_res["Li2013"][test_idx] += GP_FP_EDF_Li2013_I(task_set, m);
                        time_res["Li2013"].push_back(timer.toc());
                    }

                    timer.tic();
                    sched_res["Huang2023"][test_idx] +=  GP_FP_Huang2023_Multi(task_set,m);
                    time_res["Huang2023"].push_back(timer.toc());
                }
                else if(gp.aType == AlgorithmType_t::FTP && gp.DAGType ==DAGType_t::DAG){
                    if(i % gp.tasksetPerVarFactor == 0){
                        sched_res["Bonifaci2013"].push_back(0);
                        sched_res["Melani2015"].push_back(0);
                        sched_res["Serrano2016"].push_back(0);
                        sched_res["Pathan2017"].push_back(0);
                        sched_res["Fonseca2017"].push_back(0);
                        sched_res["Fonseca2019"].push_back(0);
                        sched_res["Nasri2019"].push_back(0);
                        sched_res["He2019"].push_back(0);
                    }

                    timer.tic();
                    sched_res["Bonifaci2013"][test_idx] +=  GP_FP_DM_Bonifaci2013_C(task_set, m);
                    time_res["Bon2013"].push_back(timer.toc());

                    timer.tic();
                    sched_res["Melani2015"][test_idx] +=  GP_FP_FTP_Melani2015_C(task_set, m);
                    time_res["Mel2015"].push_back(timer.toc());

                    timer.tic();
                    sched_res["Serrano2016"][test_idx] += GP_LP_FTP_Serrano16_C(task_set, m);
                    time_res["Ser2016"].push_back(timer.toc());

                    timer.tic();
                    sched_res["Pathan2017"][test_idx] +=  GP_FP_DM_Pathan2017_C(task_set, m);
                    time_res["Pat2017"].push_back(timer.toc());

                    timer.tic();
                    sched_res["Fonseca2017"][test_idx] +=  GP_FP_FTP_Fonseca2017_C(task_set, m);
                    time_res["Fon2017"].push_back(timer.toc());

                    timer.tic();
                    sched_res["Fonseca2019"][test_idx] +=  GP_FP_FTP_Fonseca2019(task_set, m);
                    time_res["Fon2019"].push_back(timer.toc());

                    timer.tic();
                    sched_res["Nasri2019"][test_idx] +=  G_LP_FTP_Nasri2019_C(task_set, m);
                    time_res["Nas2019"].push_back(timer.toc());

                    timer.tic();
                    sched_res["He2019"][test_idx] +=  GP_FP_FTP_He2019_C(task_set, m);
                    time_res["He2019"].push_back(timer.toc());
                }

                else if(gp.aType == AlgorithmType_t::EDF && gp.DAGType ==DAGType_t::CDAG ){
                    if(i % gp.tasksetPerVarFactor == 0)
                        sched_res["Melani2015"].push_back(0);

                    timer.tic();
                    sched_res["Melani2015"][test_idx] +=  GP_FP_EDF_Melani2015_C(task_set, m);
                    time_res["Melani2015"].push_back(timer.toc());
                }
                else if(gp.aType == AlgorithmType_t::FTP && gp.DAGType ==DAGType_t::CDAG){
                    if(i % gp.tasksetPerVarFactor == 0){
                        sched_res["Melani2015"].push_back(0);
                        sched_res["Pathan2017"].push_back(0);
                    }

                    timer.tic();
                    sched_res["Melani2015"][test_idx] +=  GP_FP_FTP_Melani2015_C(task_set, m);
                    time_res["Melani2015"].push_back(timer.toc());

                    timer.tic();
                    sched_res["Pathan2017"][test_idx] +=  GP_FP_DM_Pathan2017_C(task_set, m);
                    time_res["Pathan2017"].push_back(timer.toc());
                }

                else if(gp.aType == AlgorithmType_t::FTP && gp.DAGType ==DAGType_t::TDAG && gp.wType == workloadType_t::SINGLE_DAG){
                    if(i % gp.tasksetPerVarFactor == 0)
                        sched_res["Han2019"].push_back(0);

                    timer.tic();
                    sched_res["Han2019"][test_idx] +=  GP_FP_Han2019_C_1(task_set.tasks[0], gp.typedProc);
                    time_res["Han2019"].push_back(timer.toc());
                }

                
                break;
            case DeadlinesType_t::ARBITRARY:
                if(gp.aType == AlgorithmType_t::EDF && gp.DAGType ==DAGType_t::DAG ){
                    if(i % gp.tasksetPerVarFactor == 0)
                    {
                        sched_res["Huang2023"].push_back(0);
                        sched_res["Taspa2023"].push_back(0);
                        sched_res["Topological"].push_back(0);
                    // sched_res["critical_path"].push_back(0);
                    }
                    
                    // timer.tic();
                    // sched_res["Bonifaci2013"][test_idx] += GP_FP_EDF_Bonifaci2013_A(task_set, m);
                    // time_res["Bonifaci2013"].push_back(timer.toc());

                    // if(gp.wType == workloadType_t::SINGLE_DAG){
                    //     if(i % gp.tasksetPerVarFactor == 0)
                    //         sched_res["Baruah2012"].push_back(0);
                        
                    //     timer.tic();
                    //     sched_res["Baruah2012"][test_idx] += GP_FP_EDF_Baruah2012_A(task_set.tasks[0], m);
                    //     time_res["Baruah2012"].push_back(timer.toc());
                    // }

                    
                    // timer.tic();
                    // sched_res["Taspa"][test_idx] +=  Taspa2023(task_set,m);
                    // time_res["Taspa"].push_back(timer.toc());

                    timer.tic();
                    sched_res["Huang2023"][test_idx] +=  GP_FP_Huang2023_Multi(task_set,m);
                    time_res["Huang2023"].push_back(timer.toc());
                

                    timer.tic();
                    sched_res["Topological"][test_idx] +=  TopologicalScheduling(task_set,m);
                    time_res["Topological"].push_back(timer.toc());
                    
                    // timer.tic();
                    // sched_res["critical_path"][test_idx] +=  critical_path(task_set,m);
                    // time_res["critical_path"].push_back(timer.toc());
                

                }
                else if(gp.aType == AlgorithmType_t::FTP && gp.DAGType ==DAGType_t::DAG ){
                    if(i % gp.tasksetPerVarFactor == 0){
                        sched_res["Bonifaci2013"].push_back(0);
                        sched_res["Fonseca2019"].push_back(0);
                    }

                    timer.tic();
                    sched_res["Bonifaci2013"][test_idx] += GP_FP_DM_Bonifaci2013_A(task_set, m);
                    time_res["Bonifaci2013"].push_back(timer.toc());

                    timer.tic();
                    sched_res["Fonseca2019"][test_idx] += GP_FP_FTP_Fonseca2019(task_set, m, false);
                    time_res["Fonseca2019"].push_back(timer.toc());

                    if(gp.wType == workloadType_t::SINGLE_DAG){
                        if(i % gp.tasksetPerVarFactor == 0)
                            sched_res["Graham1969"].push_back(0);

                        timer.tic();
                        sched_res["Graham1969"][test_idx] += Graham1969(task_set.tasks[0], m);
                        time_res["Graham1969"].push_back(timer.toc());
                    }
                }
                break;
            }
        }
        else if(gp.sType == SchedulingType_t::PARTITIONED){
            
            if(gp.aType == AlgorithmType_t::FTP && gp.DAGType == DAGType_t::DAG && gp.dtype != DeadlinesType_t::ARBITRARY){
                 if(i % gp.tasksetPerVarFactor == 0){
                    sched_res["Fonseca2016"].push_back(0);
                    sched_res["Casini2018"].push_back(0);
                    // sched_res["Casini2018_W"].push_back(0);
                    // sched_res["Casini2018_B"].push_back(0);
                    // sched_res["Casini2018_F"].push_back(0);
                    // sched_res["Casini2018_P"].push_back(0);
                    #ifdef ZAHAF2019
                    sched_res["Zahaf2019"].push_back(0);
                    #endif
                 }
                
                WorstFitProcessorsAssignment(task_set, m);

                timer.tic();
                sched_res["Fonseca2016"][test_idx] += P_FP_FTP_Fonseca2016_C(task_set, m);
                time_res["Fonseca2016"].push_back(timer.toc());

                timer.tic();
                sched_res["Casini2018"][test_idx] += P_LP_FTP_Casini2018_C(task_set, m);
                time_res["Casini2018"].push_back(timer.toc());
                
                // timer.tic();
                // int worst = P_LP_FTP_Casini2018_C_withAssignment(task_set, m, PartitioningCoresOrder_t::WORST_FIT);
                // sched_res["Casini2018_W"][test_idx] += worst;
                // time_res["Casini2018_W"].push_back(timer.toc());

                // timer.tic();
                // int best = P_LP_FTP_Casini2018_C_withAssignment(task_set, m, PartitioningCoresOrder_t::BEST_FIT);
                // sched_res["Casini2018_B"][test_idx] += best;
                // time_res["Casini2018_B"].push_back(timer.toc());

                // timer.tic();
                // int first = P_LP_FTP_Casini2018_C_withAssignment(task_set, m, PartitioningCoresOrder_t::FIRST_FIT);
                // sched_res["Casini2018_F"][test_idx] += first;
                // time_res["Casini2018_F"].push_back(timer.toc());

                // std::cout<<"PARTITIONED: "<<(first || best || worst)<<std::endl;
                // sched_res["Casini2018_P"][test_idx] += (int) (first || best || worst);
                // time_res["Casini2018_P"].push_back(time_res["Casini2018_F"].back() + time_res["Casini2018_B"].back() + time_res["Casini2018_W"].back());

                #ifdef ZAHAF2019
                timer.tic();
                sched_res["Zahaf2019"][test_idx] += P_LP_EDF_Zahaf2019_C(task_set, m);
                time_res["Zahaf2019"].push_back(timer.toc());
                #endif
            }
        }

        else if(gp.sType == SchedulingType_t::SOTA){
            if(gp.aType == AlgorithmType_t::FTP && gp.DAGType == DAGType_t::DAG && gp.dtype != DeadlinesType_t::ARBITRARY){
                if(i % gp.tasksetPerVarFactor == 0){
                    // sched_res["Melani2015"].push_back(0);
                    // // // sched_res["Fonseca2016"].push_back(0);
                    // sched_res["Serrano2016"].push_back(0);
                    
                    // sched_res["Casini2018"].push_back(0);
                    // // // sched_res["Fonseca2019"].push_back(0);
                    // sched_res["Nasri2019"].push_back(0);
                    // sched_res["He2019"].push_back(0);
                    // sched_res["Huang2023EDFFP"].push_back(0);
                    sched_res["EDF"].push_back(0);
                    sched_res["LLF"].push_back(0);
                    sched_res["DFPDA"].push_back(0);
                    // sched_res["test"].push_back(0);
                    // sched_res["Huang2025Multi"].push_back(0);

                    // sched_res["Topological"].push_back(0);
                    // sched_res["critical_path"].push_back(0);
                }

                WorstFitProcessorsAssignment(task_set, m);

                // timer.tic();
                // sched_res["Melani2015"][test_idx] +=  GP_FP_FTP_Melani2015_C(task_set, m);
                // time_res["Mel2015"].push_back(timer.toc());

                // timer.tic();
                // sched_res["Serrano2016"][test_idx] += GP_LP_FTP_Serrano16_C(task_set, m);
                // time_res["Serrano2016"].push_back(timer.toc());
                // // std::cout << "1:"<<timer.toc() << std::endl;
                // timer.tic();
                // sched_res["Fonseca2016"][test_idx] += P_FP_FTP_Fonseca2016_C(task_set, m);
                // time_res["Fonseca2016"].push_back(timer.toc());

                // timer.tic();
                // sched_res["Huang2023WCET"][test_idx] +=  GP_FP_Huang2023(task_set,m);
                // time_res["Huang2023WCET"].push_back(timer.toc());


                // timer.tic();
                // sched_res["Casini2018"][test_idx] += P_LP_FTP_Casini2018_C(task_set, m);
                // time_res["Casini2018"].push_back(timer.toc());

                // // timer.tic();
                // // sched_res["Fonseca2019"][test_idx] +=  GP_FP_FTP_Fonseca2019(task_set, m);
                // // time_res["Fon2019"].push_back(timer.toc());

                // timer.tic();
                // sched_res["Nasri2019"][test_idx] +=  G_LP_FTP_Nasri2019_C(task_set, m);
                // time_res["Nasri2019"].push_back(timer.toc());
                // // std::cout << timer.toc() << std::endl;
                // // std::cout <<"get" <<  std::endl;
                
                // timer.tic();
                // sched_res["He2019"][test_idx] +=  GP_FP_FTP_He2019_C(task_set, m);
                // time_res["He2019"].push_back(timer.toc());
                // std::cout << timer.toc() << std::endl;
                // std::cout << "4:"<<timer.toc() << std::endl;
                
                // timer.tic();
                // sched_res["Huang2023WCET"][test_idx] +=  GP_FP_Huang2023_Multi_only_WCET(task_set,m);
                // time_res["Huang2023WCET"].push_back(timer.toc());

                // std::cout <<"get out " << std::endl;
               

                // timer.tic();
                // sched_res["Huang2023EDFNP"][test_idx] +=  GP_FP_Huang2023(task_set,m);
                // time_res["Huang2023EDFNP"].push_back(timer.toc());
                
                // timer.tic();
                // sched_res["Huang2023EDFFP"][test_idx] +=  GP_FP_Huang2023_Multi(task_set,m);
                // time_res["Huang2023EDFFP"].push_back(timer.toc());

                std::vector<float> temp;

                // for(int i=m;i<12;i++)
                // {
                    timer.tic();
                    temp=Taspa2023DFS(task_set,m);
                    // sched_res["Proposed"][test_idx] +=  Taspa2023DFS(task_set,m)[0];
                    sched_res["DFPDA"][test_idx] +=  temp[0];
                    time_res["DFPDA"].push_back(timer.toc());

                    // std::cout << "m=" << m <<" time=" << temp[1] << " task" << task_set.tasks.size() <<  " avgtime=:" <<std::endl;
                    // std::cout << "time:" << temp[1] << std::endl;
                    // sleep(1);
                    AVGDFS[0]+=temp[1];
                    AVGDFS[1]+=task_set.tasks.size();


                    timer.tic();
                    temp=Taspa2023LLF(task_set,m);
                    // sched_res["Proposed"][test_idx] +=  Taspa2023DFS(task_set,m)[0];
                    sched_res["LLF"][test_idx] +=  temp[0];
                    time_res["LLF"].push_back(timer.toc());


                    // timer.tic();
                    // sched_res["LLF"][test_idx] +=  Taspa2023LLF(task_set,m);
                    // time_res["LLF"].push_back(timer.toc());

                    AVGLLF[0]+=temp[1];
                    AVGLLF[1]+=task_set.tasks.size();


                    timer.tic();
                    temp=Taspa2023(task_set,m);
                    // sched_res["Proposed"][test_idx] +=  Taspa2023DFS(task_set,m)[0];
                    sched_res["EDF"][test_idx] +=  temp[0];
                    time_res["EDF"].push_back(timer.toc());

                    AVGEDF[0]+=temp[1];
                    AVGEDF[1]+=task_set.tasks.size();


                    // timer.tic();
                    // sched_res["EDF"][test_idx] +=  Taspa2023(task_set,m);
                    // time_res["EDF"].push_back(timer.toc());
                
                // }
                // timer.tic();
                // sched_res["test"][test_idx] +=  Taspa2023DFSFS(task_set,m);
                // time_res["test"].push_back(timer.toc());

                

                // timer.tic();
                // sched_res["Huang2025Multi"][test_idx] +=  GP_FP_Huang2025_Multi(task_set,m);
                // time_res["Huang2025Multi"].push_back(timer.toc());

                
                // timer.tic();
                // sched_res["Topological"][test_idx] +=  TopologicalScheduling(task_set,m);
                // time_res["Topological"].push_back(timer.toc());
                    
                // timer.tic();
                // sched_res["critical_path"][test_idx] +=  critical_path(task_set,m);
                // time_res["critical_path"].push_back(timer.toc());
                
                // std::cout << "2:"<<timer.toc() << std::endl;
                // break;
                // sleep(1);
            }
        }

        // std::cout << "avg time=" << AVGDFS[0]/AVGDFS[1] << std::endl;

        for(auto &t:task_set.tasks) //!这里很重要！！！
            t.destroyVerices();
        // return 0;

        
    }

    std::cout <<"m: "<< m << " avg dfs time=" << AVGDFS[0]/AVGDFS[1] << " AllTime:" << AVGDFS[0]<< " Tasknum:" << AVGDFS[1] << std::endl;
    std::cout <<"m: "<< m << " avg llf time=" << AVGLLF[0]/AVGLLF[1] << " AllTime:" << AVGLLF[0]<< " Tasknum:" << AVGLLF[1] << std::endl;
    std::cout <<"m: "<< m << " avg edf time=" << AVGEDF[0]/AVGEDF[1] << " AllTime:" << AVGEDF[0]<< " Tasknum:" << AVGEDF[1] << std::endl;


    std::string x_axis_label;
    if(gp.gType == GenerationType_t::VARYING_M)
        x_axis_label = "Number of cores";
    else if(gp.gType == GenerationType_t::VARYING_M)
        x_axis_label = "Number of tasks";
    else if(gp.gType == GenerationType_t::VARYING_U)
        x_axis_label = "Taskset utilization";

    plotResults(sched_res, x, x_axis_label, "Taskset scheduled", output_fig_path, show_plots);
    plotTimes(time_res, output_fig_path, show_plots);

}

}


#endif /* EVALUATE_H */