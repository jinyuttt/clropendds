#include "OpenddsExportForSo.h"
#include <thread>

static map<string, FuncPtr> callback;

FuncPtr funcall=nullptr;

int addQueue = 0;//默认0；
std::queue<MsgQueue> task_queue;
std::mutex mtx;
std::condition_variable cv;
bool done = false;
int queuesize = 0;
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
 int Addition(int a, int b)
{
    return a+b;
}

 int getString(char* b)
{
    char* cstr = "Hello!";
    std::string outputString = "my：";
    strcpy(b, outputString.c_str());
  
    return 0;
}

/*
* 初始化创建
*/
 int Initopendds(int argc, char* argv[])
{
    return createFactory(argc, argv);
}

/*
* 创建域
*/
 int Createdomain(long id)
{
    auto domain= createDomainParticipant(id);
    if (domain == nullptr) {
        return -1;
    }
    return 0;
}
int SetQueueMsg()
{
    addQueue = 1;
    return 0;
}
/*
* 发布数据
*/
 int PublishMsg(const  char* topicname,const unsigned char* content,int len, int status, const char* from)
{
    std::cerr << "PublishMsg!" << std::endl;
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
    int ret= publish(topicname, content,len, status, from);
     std::cerr << "Publish return!" << std::endl;
     return ret;
}

/*
* 指定域发布
*/
 int PublishDomainMsg(long domainid, const  char* topicname, const unsigned  char* content, int lengh, int status, const char* from)
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
// <summary>
/// 数据存储
/// </summary>
/// <param name="msg"></param>
static void producer(MsgQueue msg) {
   
        std::lock_guard<std::mutex> lock(mtx);
        task_queue.push(msg);
        queuesize++;
        cv.notify_one(); // 通知一个等待的线程
       // std::lock_guard<std::mutex> lock(mtx);
    //    done = true;
    //
    //cv.notify_all(); // 通知所有等待的线程
}
static void PrintExport(const std::string& topic, unsigned char* content, long len, long id, const std::string& from){
    if (addQueue == 1) {
        MsgQueue msg;
        msg.topic = topic;
        msg.content = content;
        msg.len = len;
        msg.id = id;
        msg.from = from;
        producer(msg);
    }
    else
    {
      auto pos = callback.find(topic);
    if (pos != callback.end()) {
      
        FuncPtr cur = pos->second;
        cur(topic.c_str(), content, len, id, from.c_str());
    }
    }

}



/// <summary>
/// 小端转换
/// </summary>
/// <param name="value"></param>
/// <param name="bytes"></param>
void longToBytes(long value, unsigned char* bytes) {
    memcpy(bytes, &value, sizeof(long));
}
  void intToUnsignedChar(int value, unsigned char* bytes) {
     
      std::memcpy(bytes, &value, sizeof(int));
  

}


/// <summary>
/// 转换
/// </summary>
/// <param name="str"></param>
/// <returns></returns>
unsigned char* stringToUnsignedChar(const std::string& str) {
    size_t len = str.length();
    auto arrPtr = std::make_unique< unsigned char[]>(len + 1);
    unsigned char* buffer = new unsigned char[len + 1];  // 额外的空间用于'\0'
    strncpy(reinterpret_cast<char*>(buffer), str.c_str(), len);
    buffer[len] = '\0';  // 添加字符串终止符
    return buffer;

}

/// <summary>
/// 数据获取
/// </summary>
/// <param name="msg"></param>
/// <param name="len"></param>
void Consumer(unsigned char** msg,long&len) {
   
        len = 0;
        long offset = 0;
        long resultLen = 0;
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [] { return !task_queue.empty() || done; }); // 等待队列非空或完成标志为真
        
        MsgQueue value = task_queue.front();
        task_queue.pop();
        queuesize--;
        lock.unlock(); // 在解锁后处理值，避免潜在的死锁风险
        //topic长度(int)+topic+消息长度（long)+消息+id（long)+来源长度(int)+来源
        resultLen = sizeof(int)+ value.topic.length() + sizeof(long)+value.len + sizeof(int)+value.from.length() + sizeof(long)+1;
        //
      // unsigned char* v = new  unsigned char[len];
       unsigned char* result = static_cast<unsigned char*>(malloc(resultLen)); // 分配内存
       
         unsigned char* tlen = new unsigned char [sizeof(int)];
           intToUnsignedChar(value.topic.length(),tlen);
        std::memcpy(result, tlen, sizeof(int));//复制topic长度
        unsigned char* topic = stringToUnsignedChar(value.topic);//topic内容转换
        offset += sizeof(int);
        std::memcpy(result+offset, topic, value.topic.length());//复制topic内容
        unsigned char* clen = new  unsigned char[sizeof(int)];
       // longToBytes(value.len, clen);//转换消息长度；
        intToUnsignedChar((int)value.len, tlen);
        offset += value.topic.length();
        std::memcpy(result+offset, tlen, sizeof(int));//复制内容长度
        offset += sizeof(int);
        std::memcpy(result+offset, value.content, value.len);//复制内容
        //
        offset += value.len;
        intToUnsignedChar((int)value.id, tlen);
        //longToBytes(value.id, clen);
        std::memcpy(result + offset, tlen, sizeof(int));//复制id
        //
        offset += sizeof(int);
        intToUnsignedChar(value.from.length(), tlen);
        std::memcpy(result+offset, tlen, sizeof(int));//复制from长度
        offset += sizeof(int);
       const unsigned char* from = stringToUnsignedChar(value.from);
   
        std::memcpy(result + offset, from, value.from.length());// 复制from
      
        *msg = result;
       
        len = resultLen;
        delete tlen;
 delete from;
}


int GetQueueMsgSize()
{
    return queuesize;
}

/*
* 订阅主题
*/
 int SubscribeMsg(const  char* topicname, FuncPtr ptr)
{
   std::cerr << "SubscribeMsg!" << std::endl;
 
    // std::function<void(const std::string&, unsigned char*,long ,long, const std::string&)> f = [func](const std::string& s1, unsigned char* c,long len,long id, const std::string& s2) {
      //   func(s1.c_str(), c,len,id, s2.c_str());
      //   };
      //   std::cerr << "SubscribeMsg! end" << std::endl;
       callback[topicname] = ptr;
       funcall=ptr;
         const char* p="test";
unsigned char * pstr = (unsigned char*)p;
   funcall("test",pstr,4,0,"0");
    return subscriber(topicname, PrintExport);
}

/*
* 指定域订阅
*/
 int SubscribeDomainMsg(long domainid, const  char* topicname, void (*func)(const char*, unsigned char*,long ,long, const char*))
{
    std::function<void(const std::string&, unsigned char*, long,long, const std::string&)> f = [func](const std::string& s1, unsigned char* c, long len,long id, const std::string& s2) {
        func(s1.c_str(), c, len,id, s2.c_str());
        };
    return subscriber(domainid,topicname, f);
}



 int OpenTestTopic()
{
    isTestTopic = true;
    return 0;
}

 int GetDCPSInfoRepoTopic(long id, char* ret)
{
   string retsult= GetDCPSInfoRepoTopic(id);
    return 0;
}

 int GetOpenDDSParticipantLocation(long id, char* ret)
{
    string retsult = GetOpenDDSParticipantLocation(id);
    return 0;
}

 int GetOpenDDSConnectionRecord(long id, char* ret)
{
    string retsult = GetOpenDDSConnectionRecord(id);
    return 0;
}

 int SetReliabilityQosPolicyKind(const  char* topicname, int value)
{
    DDS::Topic_var topic = createTopic(last, topicname);
    DDS::TopicQos topic_qos;
    if (value == 1) {
        topic_qos.liveliness.kind = DDS::LivelinessQosPolicyKind::AUTOMATIC_LIVELINESS_QOS;
    }
    else if (value == 2) {
        topic_qos.liveliness.kind = DDS::LivelinessQosPolicyKind::MANUAL_BY_TOPIC_LIVELINESS_QOS;
    }
    else  if (value == 3) {
        topic_qos.liveliness.kind = DDS::LivelinessQosPolicyKind::MANUAL_BY_PARTICIPANT_LIVELINESS_QOS;
    }
    setQos(topic, topic_qos);
    return 0;
}

 int SetHistoryQosPolicyKind(const  char* topicname, int value)
{

    DDS::Topic_var topic = createTopic(last, topicname);
    DDS::TopicQos topic_qos;
    if (value == 1) {
        topic_qos.history.kind = DDS::HistoryQosPolicyKind::KEEP_ALL_HISTORY_QOS;
    }
    else if (value == 2) {
        topic_qos.history.kind = DDS::HistoryQosPolicyKind::KEEP_LAST_HISTORY_QOS;
    }
   

    setQos(topic, topic_qos);
    return 0;
}

int SetDurabilityQosPolicyKind(const  char* topicname, int value)
{
    DDS::Topic_var topic = createTopic(last, topicname);
    DDS::TopicQos topic_qos;
    if (value == 1) {
        topic_qos.durability.kind = DDS::DurabilityQosPolicyKind::VOLATILE_DURABILITY_QOS;
    }
    else if (value == 2) {
        topic_qos.durability.kind = DDS::DurabilityQosPolicyKind::TRANSIENT_LOCAL_DURABILITY_QOS;
    }
    else  if (value == 3) {
        topic_qos.durability.kind = DDS::DurabilityQosPolicyKind::TRANSIENT_DURABILITY_QOS;
    }
    else  if (value == 4) {
        topic_qos.durability.kind = DDS::DurabilityQosPolicyKind::PERSISTENT_DURABILITY_QOS;
    }

    setQos(topic, topic_qos);
    return 0;
}


