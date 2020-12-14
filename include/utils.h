#ifndef UTILS_H
#define UTILS_H

#include<set>
#include<algorithm>

#define REPRODUCIBLE 1

#define FatalError(s) {                                                \
    std::stringstream _where, _message;                                \
    _where << __FILE__ << ':' << __LINE__;                             \
    _message << std::string(s) + "\n" << __FILE__ << ':' << __LINE__;\
    std::cerr << _message.str() << "\nAborting...\n";                  \
    exit(EXIT_FAILURE);                                                \
}

// Colored output
#define COL_END "\033[0m"
#define COL_CYANB "\033[1;36m"

#define TIME_VERBOSE 1

#define TSTART timespec start, end;                               \
                    clock_gettime(CLOCK_MONOTONIC, &start);            

#define TSTOP_C(col, show)  clock_gettime(CLOCK_MONOTONIC, &end);       \
    double t_ns = ((double)(end.tv_sec - start.tv_sec) * 1.0e9 +       \
                  (double)(end.tv_nsec - start.tv_nsec))/1.0e6;        \
    if(show) std::cout<<col<<"Time:"<<std::setw(16)<<t_ns<<" ms\n"<<COL_END; 

#define TSTOP TSTOP_C(COL_CYANB, TIME_VERBOSE)


/*
Given sets a and b it computes a \ b and save it in a
*/
template<typename T>
void set_difference_inplace(std::set<T>& a, const std::set<T>& b)
{
    std::set<T> c;
    std::set_difference(a.begin(), a.end(),b.begin(), b.end(), std::inserter(c, c.begin()) );
    a = c;
}

#endif /*UTILS_H*/