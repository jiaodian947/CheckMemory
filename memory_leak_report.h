/*=============================================================================
	memory_leak_report.h : memory leak define
	Copyright 2013-2013 Sininm Games, Inc. All Rights Reserved.
	Revision history:
=============================================================================*/
#ifndef _MEMORY_LEAK_REPORT_H_
#define _MEMORY_LEAK_REPORT_H_

#include <windows.h>
#include <crtdbg.h>
#include <string.h>

#ifndef NDEBUG

// 内存记录文件名
extern const char* g_leak_checker_file;

// 内存泄漏检测

class CLeakChecker
{
public:
	struct leak_info_t
	{
		char strInfo[128];
	};	
	
	// 信息输出函数
	static int OurReportingFunction(int reportType, char* userMessage, 
		int* retVal)
	{
		// 缓冲一下以提高写入文件的速度
		static leak_info_t s_LeakInfos[256];
		static size_t s_nInfoCount = 0;
		
		size_t msg_len = strlen(userMessage);
		
		if (msg_len < 128)
		{
			memcpy(s_LeakInfos[s_nInfoCount].strInfo, userMessage, 
				msg_len + 1);
		}
		else
		{
			memcpy(s_LeakInfos[s_nInfoCount].strInfo, userMessage, 127);
			s_LeakInfos[s_nInfoCount].strInfo[127] = 0;
		}
		
		++s_nInfoCount;
		
		if ((s_nInfoCount == 256) 
			|| (strcmp(userMessage, "Object dump complete.\n") == 0))
		{
			try
			{
				HANDLE handle = CreateFile(g_leak_checker_file,
					GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, 0, NULL);
				
				if (handle != INVALID_HANDLE_VALUE)
				{
					SetFilePointer(handle, 0, NULL, FILE_END);
					
					DWORD writen = 0;
					
					for (size_t i = 0; i < s_nInfoCount; ++i)
					{
						char* info = s_LeakInfos[i].strInfo;
						size_t len = strlen(info);
						
						if (0 == len)
						{
							continue;
						}
						
						WriteFile(handle, info, DWORD(len - 1), &writen, NULL);
						
						if (info[len - 1] == '\n')
						{
							WriteFile(handle, "\r\n", 2, &writen, NULL);
						}
					}
					
					CloseHandle(handle);
				}
			}
			catch (...)
			{
				s_nInfoCount = 0;
				return FALSE;
			}
		
			s_nInfoCount = 0;
		}
			
		/*
		try
		{
			HANDLE handle = CreateFile(g_leak_checker_file,
				GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, 0, NULL);
			
			if (handle != INVALID_HANDLE_VALUE)
			{
				SetFilePointer(handle, 0, NULL, FILE_END);
				
				DWORD writen = 0;
				
				if (strlen(userMessage) > 0)
				{
					WriteFile(handle, userMessage, 
						DWORD(strlen(userMessage) - 1), &writen, NULL);
					
					if (userMessage[strlen(userMessage) - 1] == '\n')
					{
						const char* end_line = "\r\n";
						
						WriteFile(handle, end_line, DWORD(strlen(end_line)), 
							&writen, NULL);
					}
				}
				
				CloseHandle(handle);
			}
		}
		catch (...)
		{
			return FALSE;
		}
		*/
		
		retVal = 0;
		
		if (reportType == _CRT_ASSERT)
		{
			return TRUE;
		} 
		else 
		{
			return FALSE;
		}
	}

public:
	CLeakChecker()
	{
		old_hook_ = NULL;
		initialize();
	}

	~CLeakChecker()
	{
		dump_memory();
		shut_down();
	}

	// 初始化
	void initialize()
	{
		_CrtMemCheckpoint(&mem_state_);

		_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF); 
		
		old_hook_ = _CrtSetReportHook(OurReportingFunction);
	}

	// 关闭
	void shut_down()
	{
		_CrtSetReportHook(old_hook_);
	}

	// 导出内存泄露信息
	void dump_memory()
	{
		_CrtMemState mem_state;
		
		_CrtMemCheckpoint(&mem_state);
		
		_CrtMemState diff;
		
		_CrtMemDifference(&diff, &mem_state_, &mem_state);
		
		//if (NULL == diff.pBlockHeader)
		//{
		//	return;
		//}
		
		DeleteFile(g_leak_checker_file);
		
		_CrtMemDumpStatistics(&diff);
		_CrtMemDumpAllObjectsSince(&diff);
	}

private:
	_CrtMemState mem_state_;
	_CRT_REPORT_HOOK old_hook_; 
};

#endif // !NDEBUG

#endif // end of _MEMORY_LEAK_REPORT_H_
