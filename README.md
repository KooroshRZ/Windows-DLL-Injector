# WindowsDLLInjector
Some DLL Injection techniques written in C++ for both 32bit and 64bit windows OS

# Intro
There are several ways for injecting a dll or generally a process in another target process\
It has many advantages such as accessing that process memory address and extending it's functionality\
In abuse cases it can be used to hide malicious activity in another process and somehow bypass antivirus detection


each one has it's pros and cons

Here we have implemented 4/7 techniques

- [x] &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;CreateRemoteThread (windows Win32 API through windows.h)
- [x] &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;NtCreateThread (windows native API through ntdll.dll)
- [x] &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;QueueUserAPC
- [x] &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;SetWindowsHookEx
- [x] &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;RtlCreateUserThread (windows native API through ntdll.dll)
- [ ] &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;SetThreadContext
- [ ] &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;ReflectiveDllInjection

# CreateRemoteThread
Maybe it's the first google search result for dll injection\
This API is mapped to NtCreateThread native API 
Very well documented and easy to code!

Pros.\
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; Easy to implement (maybe just 5 lines of code)\
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; well documented on internet

Cons.\
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; Easy to detect\
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; not possible on all processes (specially windows NT native processes)

# NtCreateThread
Acually not as easy as CreateRemoteThread method\
It needs some header definitin and function pointer casting to get the address of NtCreateThread in ntdll.dll\
A little hard to implement but very effective against windows native system processes

Pros.\
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; Can be used for windows native NT processes (like : svchost, smss, ...)\
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; Harder to be detected

Cons.\
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; A little hard to implement and debug\
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; no official documentation

# QueueUserAPC
This method is about hijacking and using a thread in target process\
As documented in msdn, the function call QueueUserAPC() adds user-mode asychronous procedure call(APC) object to the APC queue of specified thread\
Actually we don't create our own thread and just use the target process's threads for loading our dll into the process address space\
Notice that to make this method work, The remote target thread should be in suspended state and with alertable flag (Example SleepEx() in TargetProgram.cpp)

Pros.\
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; No need for creating new Threads\
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; It adds some stealthy abilities for injection (cause no new thead would be created)

Cons.\
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; Not always possible on all threads (just suspended threads with alertable flag)

# SetWindowsHookEx
This method actually is for setting Windows Hook for specific events like keyboard or mouse event\
But since the hook procedure should be in form of exported function in a dll file in the target process,\
This makes it possible to use it as a dll injection method\
This one is kinda different one because neither thread nor APC is created but A hook is set in target process for a specific thread

Pros.\
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; No need for creating new Threads or APC call\
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; Easy to use spcially with thread ID equals 0 :)

Cons.\
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; Too suspicious for antivirus detection because of using hooks


# RtlCreatUserThread
Same as NtCreateThread with some possible wrapping

Pros.\
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; Can be used for windows native NT processes (like : svchost, smss, ...)\
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; Harder to be detected

Cons.\
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; A little hard to implement and debug\
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; no official documentation


<br><br>
# Credit
Many thanks for **fdiskyou** and his **InjectAllTheThings** (nice name) repository\
the website : http://deniable.org/misc/inject-all-the-things
