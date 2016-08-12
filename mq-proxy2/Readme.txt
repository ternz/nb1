问题：
1. 一端socket出错关闭或者正常关闭时，销毁数据导致另一端socket fdhandle内存释放，
另一端socket fd在epoll_wait返回时，fdhandle指向失效的内存，再次应用引起segment fault
2. 使用计算线程处理数据包转换，尽管使用二级fdhandle指针标示数据包处理好后该往哪写。若fdhandle
释放于二级fdhandle指针null检查和使用的时间窗口间，也会引起segment fault

解决方案：
1. 使用map存储fdhandle, key为fd+fdhandle地址+随机数, value就是fdhandle。epoll_event结构体里data不再设为
fdhandle指针，而是fd，通过查找map拿到fdhandle指针。
2. 使用shared_ptr等智能指针消灭 new/delete。
3. 数据包转换线程里做两点防护，一是屏蔽SIGSEGV信号；而是对可能出问题的地方 try-catch。