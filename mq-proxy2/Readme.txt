���⣺
1. һ��socket����رջ��������ر�ʱ���������ݵ�����һ��socket fdhandle�ڴ��ͷţ�
��һ��socket fd��epoll_wait����ʱ��fdhandleָ��ʧЧ���ڴ棬�ٴ�Ӧ������segment fault
2. ʹ�ü����̴߳������ݰ�ת��������ʹ�ö���fdhandleָ���ʾ���ݰ�����ú������д����fdhandle
�ͷ��ڶ���fdhandleָ��null����ʹ�õ�ʱ�䴰�ڼ䣬Ҳ������segment fault

���������
1. ʹ��map�洢fdhandle, keyΪfd+fdhandle��ַ+�����, value����fdhandle��epoll_event�ṹ����data������Ϊ
fdhandleָ�룬����fd��ͨ������map�õ�fdhandleָ�롣
2. ʹ��shared_ptr������ָ������ new/delete��
3. ���ݰ�ת���߳��������������һ������SIGSEGV�źţ����ǶԿ��ܳ�����ĵط� try-catch��