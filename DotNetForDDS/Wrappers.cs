using System.Runtime.InteropServices;
using System.Text;

namespace DotNetForDDS
{
    public class Wrappers
    {
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

        public static  string GetDCPSInfoRepoTopic(long domainid)
        {
            IntPtr outPut = Marshal.AllocHGlobal(2048);
            int r= GetDCPSInfoRepoTopic(domainid, outPut);
            string retStr = Marshal.PtrToStringAnsi(outPut);
            return retStr;
        }

        public static string GetOpenDDSParticipantLocation(long domainid)
        {
            IntPtr outPut = Marshal.AllocHGlobal(2048);
            int r = GetOpenDDSParticipantLocation(domainid, outPut);
            string retStr = Marshal.PtrToStringAnsi(outPut);
            return retStr;
        }
        public static string GetOpenDDSConnectionRecord(long domainid)
        {
            IntPtr outPut = Marshal.AllocHGlobal(2048);
            int r = GetOpenDDSConnectionRecord(domainid, outPut);
            string retStr = Marshal.PtrToStringAnsi(outPut);
            return retStr;
        }


    }
}
