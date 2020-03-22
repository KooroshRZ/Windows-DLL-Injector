# WindowsDLLInjector
Some DLL Injection techniques written in C++ for both 32bit and 64bit windows OS

# Intro
There are several ways for injecting a dll or generally a process in another target process\
It has many advantages such as accessing that process memory address and extending it's functionality\
In abuse cases it can be used to hide malicious activity in another process and somehow bypass antivirus detection


each one has it's pros and cons

Here we have implemented 2 techniques

- [x] &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;CreatRemoteRhreate (windows Win32 API through windows.h)
- [x] &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;NtCreatThread (wnidows native API through ntdll.dll)
- [ ] &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;QueueUserAPC
- [ ] &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;SetWindowsHookEx
- [ ] &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;RtlCreateUserThread
- [ ] &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;SetThreadContext
- [ ] &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;ReflectiveDllInjection

# CreateRemoteThread
Maybe it's the first google search result for dll injection\
Very well documented and easy to code!

Pros.\
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; Easy to implement (maybe just 5 lines of code)\
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; well documented on internet

Cons.\
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; Easy to detect\
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; not possible on all processes (specially windows NT native processes)

# NtCreateThread
Acually not as easy as CreateRemoteThread method\
it needs some header definitin and exploration through ntdll.dll to find the address of the required function\
A little hard to implement but very effective against windows native system processes

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
