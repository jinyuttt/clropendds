

using DotNetForDDSSo;
using System.Runtime.InteropServices;
using System.Text;

namespace TestForSo
{
    internal class Program
    {
        static void Main(string[] args)
        {
            for(int i = 0; i < args.Length; i++)
            {
                Console.WriteLine(args[i]);
            }
          //  IntPtr outPut = System.Runtime.InteropServices.Marshal.AllocHGlobal(2048);
          // Wrappers.getString(outPut);
         // string retStr = System.Runtime.InteropServices.Marshal.PtrToStringAnsi(outPut);
         // Console.WriteLine(retStr);
           int ret = Wrappers.Addition(3, 3);
            string[] strs= new string[2];
            strs[0] = "-DCPSConfigFile";
            strs[1] = "rtps.ini";
            Wrappers.Initopendds(2, strs);
            Wrappers.Createdomain(43);
           
          Wrappers.SubscribeMsg("test", MyAction);
           Wrappers.SetQueueMsg();
          Wrappers.OpenTestTopic();
          GetMsg();
            for (int i = 0; i < 10; i++)
            { 
               var bytes= Encoding.UTF8.GetBytes("我在测试mm"+i);
                int r=Wrappers.PublishMsg("test", bytes,bytes.Length, 1, "sss");

              Thread.Sleep(1000);
            }
            Console.WriteLine(ret);
            Console.ReadLine();
        }
       

       static  void MyAction(string a, IntPtr data,long datalen,long id, string b)
        {
            byte[] bytes = new byte[datalen];
            Marshal.Copy(data, bytes, 0, (int)datalen);
           string str= Encoding.UTF8.GetString(bytes);
            Console.WriteLine($"{a},{str},{id},{b}");
        }

         static void GetMsg()
{
    Thread thread = new Thread(() =>
    {
        while (true)
        {
            IntPtr ptr = IntPtr.Zero;
            long len = 20;
            Wrappers.Consumer(ref ptr, ref len);
            if(len != 0)
            {
            byte[] bytes = new byte[len];
            Marshal.Copy(ptr, bytes, 0, (int)len);
            int tlen = BitConverter.ToInt32(bytes, 0);
            string topic = Encoding.Default.GetString(bytes, 4, tlen);
            long msgLen = BitConverter.ToInt32(bytes, 4 + tlen);
            string msg = Encoding.UTF8.GetString(bytes, 4 + tlen + 4, (int)msgLen);
            int id = BitConverter.ToInt32(bytes, 4 + tlen + 4 + (int)msgLen);
            int fromLen = BitConverter.ToInt32(bytes, 4 + tlen + 4 + (int)msgLen + 4);
            string from = Encoding.Default.GetString(bytes, 4 + tlen + 4 + (int)msgLen + 4 + 4, fromLen);

            Console.WriteLine($"{topic},{msg},{id},{from}");
            }
           int num= Wrappers.GetQueueMsgSize();
           Console.WriteLine("数据量："+num);
            Thread.Sleep(1000);
        }
    });
    thread.Start();
}
    }
}
