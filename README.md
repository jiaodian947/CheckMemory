# CheckMemory
CheckMemory

Add global variables CLeakChecker g_LeakChecker

#ifdef _WIN32
#ifdef _DEBUG
#pragma init_seg(lib)        //Make sure the first initializes it   
#include <memory_leak_report.h>

const char* g_leak_checker_file = "global_logic_memory.log";      // out file name
CLeakChecker g_LeakChecker;

#endif // _DEBUG
#endif // _WIN32
