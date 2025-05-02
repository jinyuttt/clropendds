#include "OpenddsExport.h"
#include <thread>

/*
* ���Ժ���
*/
void callFunction(std::function<void(string, char*, string)> func) {
    std::string str1 = "Hello";
    char c = 'A';
    std::string str2 = "World";
    func(str1, &c, str2);
}
/*
* ���Ժ���
*/
void CallManagedFunction(void (*func)(const char*, char*, const char*)) {
    std::function<void(const std::string&, char*, const std::string&)> f = [func](const std::string& s1, char* c, const std::string& s2) {
        func(s1.c_str(), c, s2.c_str());
        };
    callFunction(f);
}

/*
* ���Ժ���
*/
MYLIB_API int Addition(int a, int b)
{
    return a+b;
}

MYLIB_API int getString(char* b)
{
    char* cstr = "Hello!";
    std::string outputString = "��������Ϊ��";
    strcpy(b, outputString.c_str());
  
    return 0;
}

/*
* ��ʼ������
*/
MYLIB_API int Initopendds(int argc, char* argv[])
{
    return createFactory(argc, argv);
}

/*
* ������
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
* ��������
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
* ָ���򷢲�
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
* ��������
*/
MYLIB_API int SubscribeMsg(const  char* topicname, void (*func)(const char*, unsigned char*,long ,long, const char*))
{
     std::function<void(const std::string&, unsigned char*,long ,long, const std::string&)> f = [func](const std::string& s1, unsigned char* c,long len,long id, const std::string& s2) {
         func(s1.c_str(), c,len,id, s2.c_str());
         };
    return subscriber(topicname, f);
}

/*
* ָ������
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
