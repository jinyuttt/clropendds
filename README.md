# clropendds
opendds的c#绑定，依赖opendds库

文件  
DDSMessenger.idl
DDS.mpc

这是原始文件，根据opendds，使用MPC工具生成构成项目。
生成项目后添加各种c++文件，然后导出接口供c#使用。

项目说明
项目中使用的方法主要还是c++17标准（visual studio默认)。

特殊处理
根据测试发现rtps.ini的配置，第一包数据会接收不到，我没有研究过原因，应该是第一包数据会初始化丢失。
但是根据rtps_udp.ini的配置，会正常。
针对丢失第一包数据的情况，项目中做了延迟，如果设调用了OpenTestTopic方法，内部会判断是不是第一次发布主题数据，
如果是则会先发布该主题的一个消息，延迟1秒后再发送数据，消息内容是：opendds字符串。
这种方法不太友好可能，但是可以使用。

c#封装DotNetForDDS

        /// <summary>
        /// 接收回调
        /// </summary>
        /// <param name="topic">主题</param>
        /// <param name="data">数据</param>
        /// <param name="datalen">数据长度</param>
        /// <param name="id">域ID</param>
        /// <param name="from">来源</param>
        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        public delegate void FuncPtr(string topic, IntPtr data,long datalen,long id,string from);

        /// <summary>
        /// 测试函数
        /// </summary>
        /// <param name="a"></param>
        /// <param name="b"></param>
        /// <returns></returns>
        [DllImport("OpenddsWrappers.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern int Addition(int a, int b);

        /// <summary>
        /// 测试函数
        /// </summary>
        /// <param name="a"></param>
        /// <param name="b"></param>
        /// <returns></returns>
        [DllImport("OpenddsWrappers.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern int getString(IntPtr  ret);

        /// <summary>
        /// 初始化，程序只调用一次
        /// </summary>
        /// <param name="argc"></param>
        /// <param name="agv"></param>
        /// <returns></returns>
        [DllImport("OpenddsWrappers.dll", CallingConvention = CallingConvention.Cdecl,CharSet =CharSet.Ansi)]
        public static extern int Initopendds(int argc, string[] agv);

        /// <summary>
        /// 创建域
        /// </summary>
        /// <param name="id">域ID</param>
        /// <returns></returns>
        [DllImport("OpenddsWrappers.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern int Createdomain(long id);

        /// <summary>
        /// 发布数据，在最新创建的域发布，一般程序中唯一域时
        /// </summary>
        /// <param name="topicname">主题</param>
        /// <param name="content">内容</param>
        /// <param name="len">内容长度</param>
        /// <param name="status">发布方式，0立即发布，1等待有订阅方初始化</param>
        /// <param name="from">标记发布源</param>
        /// <returns></returns>
        [DllImport("OpenddsWrappers.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern int PublishMsg(string topicname, byte[] content,int len, int status, string from);

        /// <summary>
        /// 指定域发布数据，没有域时会自动创建
        /// </summary>
        /// <param name="domainid">域ID</param>
        /// <param name="topicname">主题</param>
        /// <param name="content">内容</param>
        /// 
        /// <param name="status">发布方式</param>
        /// <param name="from">标记源</param>
        /// <returns></returns>
        [DllImport("OpenddsWrappers.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern int PublishDomainMsg(long domainid, string topicname, byte[] content,int len, int status, string from);

        /// <summary>
        /// 订阅数据，最新创建域上订阅，一般一个域时
        /// </summary>
        /// <param name="topicname">主题</param>
        /// <param name="func">接收回调</param>
        [DllImport("OpenddsWrappers.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern int SubscribeMsg(string topicname, FuncPtr func);

        /// <summary>
        /// 指定域上订阅
        /// </summary>
        /// <param name="domainid">域ID</param>
        /// <param name="topicname">主题</param>
        /// <param name="func">接收回调</param>

        [DllImport("OpenddsWrappers.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern int SubscribeDomainMsg(long domainid, string topicname, FuncPtr func);

        [DllImport("OpenddsWrappers.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern int OpenTestTopic();


        [DllImport("OpenddsWrappers.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern int GetDCPSInfoRepoTopic(long domainid, IntPtr ret);

        [DllImport("OpenddsWrappers.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern int GetOpenDDSParticipantLocation(long domainid, IntPtr ret);

        [DllImport("OpenddsWrappers.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern int GetOpenDDSConnectionRecord(long domainid, IntPtr ret);




