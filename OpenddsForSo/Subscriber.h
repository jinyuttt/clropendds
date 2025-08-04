#pragma once
#include "OpenddsWarp.h"
#include "DDSMessengerTypeSupportImpl.h"

#include <dds/DCPS/Marked_Default_Qos.h>
#include <dds/DCPS/Service_Participant.h>
#include <dds/DCPS/WaitSet.h>
#include <dds/DCPS/StaticIncludes.h>
#include <iostream>
#include <vector>
#include <map>
#include "Utils.h"
#include <iostream>
#include <functional>

static DDS::Subscriber_var sub;
static DDS::DataReaderListener_var lister;
static map<string, std::function<void(string, unsigned char*,long,long,  string)>> call;
 std::function<void(string, unsigned char*, long, long, string)> callprint;


 void PrintMsgCall(const char* topic, unsigned char* content, long len, long id, const char* from) {
	std::cerr << "PrintMsgCall" << std::endl;
	std::cerr << topic << std::endl;
	std::cerr << content << std::endl;
}

void PrintMsg(string topic, unsigned char* content, long len, long id, string  from) {
	std::cerr << "PrintMsg" << std::endl;
	std::cerr << topic << std::endl;
	std::cerr << content << std::endl;
}
void Print(const std::string& s1, unsigned char* c, long len, long id, const std::string& s2) {
       std::cerr << "Print" << std::endl;
	
 }
/**
* 所有数据都会在这里返回
* 这里已经没有域的概念，所有主题都会显示
*/
void  ddscallback(DDSMessenger::MessageSeq msg) {
     
        std::cerr << msg.length() << std::endl;
	for (size_t i = 0; i < msg.length(); i++)
	{
	     
		DDSMessenger::Message message = msg[i];
		
		
		string topic = message.subject._retn();
		
		string from = message.from._retn();
		
		long id = message.domain_id;
		const long len = message.content.length();
		unsigned char* content = new unsigned char[len];
		memcpy(content,  message.content.get_buffer(),len); // 使用memcpy进行数组复制
		auto pos = call.find(topic);
		
		if (pos != call.end()) {
		
			std::function<void(string, unsigned char*,long,long,string)> cur = pos->second;
			
		//	if (callprint) {
	//callprint(topic, content, len, id, from);
//}
//else {
	//func = PrintMsgCall;
	//std::function<void(const std::string&, unsigned char*, long, long, const std::string&)> f = Print;
	//callprint = f;
	//callprint(topic, content, len, id, from);
//}
			//if(cur==nullptr)
			//{
			 // std::cerr << "!null"<< std::endl;
			//}
			if(cur)
			{
			//  std::cerr << "callback"<< std::endl;
			//  std::cerr << topic<< std::endl;
			//  std::cerr << content<< std::endl;
			//  std::cerr << len<< std::endl;
			//  std::cerr << id<< std::endl;
			//  std::cerr << from<< std::endl;
			  
			  cur(topic, content,len,id, from);
			}
			  //else{std::cerr << "?????/?"<< std::endl;
			//}
			
		}
		
	}
}


int subscriber(string topicname, std::function<void(string, unsigned char*,long ,long,string)> cb) {
	if (factory == nullptr) {
		return -1;
	}
	if (sub == nullptr) {
		sub = createSubscriber(last);
	}
	if (lister == nullptr) {
		lister = createLister(ddscallback);
	}
	DDS::Topic_var topic = createTopic(last, topicname);
	DDS::DataReader_var  readr = createDataReader(sub, topic, lister);
	call[topicname] = cb;
	return 0;
}

int subscriber(int domainid,string topicname, std::function<void(string, unsigned char*,long ,long, string)> cb) {
	if (factory == nullptr) {
		return -1;
	}
	
	if (lister == nullptr) {
		lister = createLister(ddscallback);
	}
	
	DDS::DomainParticipant_var participant = createDomainParticipant(domainid);
	    std::cerr << "subscriber" << std::endl;

	DDS::Subscriber_var cursub = createSubscriber(participant);
	DDS::Topic_var topic = createTopic(participant, topicname);
	DDS::DataReader_var  readr = createDataReader(cursub, topic, lister);
	call[topicname] = cb;
	std::cerr << "subscriber end!" << std::endl;
	return 0;
}
