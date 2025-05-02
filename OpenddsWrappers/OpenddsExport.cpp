#include "OpenddsExport.h"
#include <thread>

/*
* 测试函数
*/
void callFunction(std::function<void(string, char*, string)> func) {
    std::string str1 = "Hello";
    char c = 'A';
    std::string str2 = "World";
    func(str1, &c, str2);
}
/*
* 测试函数
*/
void CallManagedFunction(void (*func)(const char*, char*, const char*)) {
    std::function<void(const std::string&, char*, const std::string&)> f = [func](const std::string& s1, char* c, const std::string& s2) {
        func(s1.c_str(), c, s2.c_str());
        };
    callFunction(f);
}

/*
* 测试函数
*/
MYLIB_API int Addition(int a, int b)
{
    return a+b;
}

MYLIB_API int getString(char* b)
{
    char* cstr = "Hello!";
    std::string outputString = "输入内容为：";
    strcpy(b, outputString.c_str());
  
    return 0;
}

/*
* 初始化创建
*/
MYLIB_API int Initopendds(int argc, char* argv[])
{
    return createFactory(argc, argv);
}

/*
* 创建域
*/
MYLIB_API int Createdomain(long id)
{
    auto domain= createDomainParticipant(id);
    if (domain == nullptr) {
        return -1;
    }
    return 0;
}

/*
* 发布数据
*/
MYLIB_API int PublishMsg(const  char* topicname,const unsigned char* content,int len, int status, const char* from)
{
   
    if (isTestTopic) {
       auto pos= dictTestTopic.find(topicname);
       if (pos == dictTestTopic.end()) {
          
           const char* str = "opendds";
           const unsigned char* ustr = (const unsigned char*)str;
           publish(topicname, ustr, 7, 0, from);
           dictTestTopic[topicname] = 0;
           auto start = std::chrono::high_resolution_clock::now();
           std::this_thread::sleep_for(std::chrono::milliseconds(1000));
           auto end = std::chrono::high_resolution_clock::now();
           std::chrono::duration<double, std::milli> elapsed = end - start;
         
       }
   }
    return publish(topicname, content,len, status, from);
}

/*
* 指定域发布
*/
MYLIB_API int PublishDomainMsg(long domainid, const  char* topicname, const unsigned  char* content, int lengh, int status, const char* from)
{
    if (isTestTopic) {
        auto pos = dictTestTopic.find(topicname);
        if (pos == dictTestTopic.end()) {

            const char* str = "opendds";
            const unsigned char* ustr = (const unsigned char*)str;
            publish(domainid,topicname, ustr, 7, 0, from);
            dictTestTopic[topicname] = 0;
            auto start = std::chrono::high_resolution_clock::now();
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            auto end = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double, std::milli> elapsed = end - start;

        }
    }
    return publish(domainid,topicname, content, lengh, status, from);
}

/*
* 订阅主题
*/
MYLIB_API int SubscribeMsg(const  char* topicname, void (*func)(const char*, unsigned char*,long ,long, const char*))
{
     std::function<void(const std::string&, unsigned char*,long ,long, const std::string&)> f = [func](const std::string& s1, unsigned char* c,long len,long id, const std::string& s2) {
         func(s1.c_str(), c,len,id, s2.c_str());
         };
    return subscriber(topicname, f);
}

/*
* 指定域订阅
*/
MYLIB_API int SubscribeDomainMsg(long domainid, const  char* topicname, void (*func)(const char*, unsigned char*,long ,long, const char*))
{
    std::function<void(const std::string&, unsigned char*, long,long, const std::string&)> f = [func](const std::string& s1, unsigned char* c, long len,long id, const std::string& s2) {
        func(s1.c_str(), c, len,id, s2.c_str());
        };
    return subscriber(domainid,topicname, f);
}

MYLIB_API int OpenTestTopic()
{
    isTestTopic = true;
    return 0;
}

MYLIB_API int GetDCPSInfoRepoTopic(long id, char* ret)
{
   string retsult= GetDCPSInfoRepoTopic(id);
    return 0;
}

MYLIB_API int GetOpenDDSParticipantLocation(long id, char* ret)
{
    string retsult = GetOpenDDSParticipantLocation(id);
    return 0;
}

MYLIB_API int GetOpenDDSConnectionRecord(long id, char* ret)
{
    string retsult = GetOpenDDSConnectionRecord(id);
    return 0;
}
