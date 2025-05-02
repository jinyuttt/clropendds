using DotNetForDDS;
using System.Runtime.InteropServices;
using System.Text;

namespace Test
{
    internal class Program
    {
        static void Main(string[] args)
        {
            for(int i = 0; i < args.Length; i++)
            {
                Console.WriteLine(args[i]);
            }
            IntPtr outPut = System.Runtime.InteropServices.Marshal.AllocHGlobal(2048);
            Wrappers.getString(outPut);
            string retStr = System.Runtime.InteropServices.Marshal.PtrToStringAnsi(outPut);
            int ret = Wrappers.Addition(3, 3);
            string[] strs= new string[2];
            strs[0] = "-DCPSConfigFile";
            strs[1] = "rtps.ini";
            Wrappers.Initopendds(2, strs);
            Wrappers.Createdomain(43);
            Wrappers.SubscribeMsg("test", MyAction);
            Wrappers.OpenTestTopic();
            for (int i = 0; i < 10; i++)
            { 
               var bytes= Encoding.UTF8.GetBytes("我在测试"+i);
                Wrappers.PublishMsg("test", bytes,bytes.Length, 1, "sss");
              
             //   Thread.Sleep(1000);
            }
            Console.WriteLine(ret);
            Console.ReadLine();
        }
       

        static void MyAction(string a, IntPtr data,long datalen,long id, string b)
        {
            byte[] bytes = new byte[datalen];
            Marshal.Copy(data, bytes, 0, (int)datalen);
           string str= Encoding.UTF8.GetString(bytes);
            Console.WriteLine($"{a},{str},{id},{b}");
        }
    }
}
