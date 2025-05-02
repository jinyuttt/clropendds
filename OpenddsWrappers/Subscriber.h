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
/**
* 所有数据都会在这里返回
* 这里已经没有域的概念，所有主题都会显示
*/
void  ddscallback(DDSMessenger::MessageSeq msg) {
	for (size_t i = 0; i < msg.length(); i++)
	{
		DDSMessenger::Message message = msg[i];
		string topic = message.subject;
		string from = message.from;
		long id = message.domain_id;
		const long len = message.content.length();
		unsigned char* content = new unsigned char[len];
		memcpy(content,  message.content.get_buffer(),len); // 使用memcpy进行数组复制
		auto pos = call.find(topic);
		if (pos != call.end()) {
			std::function<void(string, unsigned char*,long,long,string)> cur = pos->second;
			cur(topic, content,len,id, from);
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
}

int subscriber(int domainid,string topicname, std::function<void(string, unsigned char*,long ,long, string)> cb) {
	if (factory == nullptr) {
		return -1;
	}
	
	if (lister == nullptr) {
		lister = createLister(ddscallback);
	}
	DDS::DomainParticipant_var participant = createDomainParticipant(domainid);
		

	DDS::Subscriber_var cursub = createSubscriber(participant);
	DDS::Topic_var topic = createTopic(participant, topicname);
	DDS::DataReader_var  readr = createDataReader(cursub, topic, lister);
	call[topicname] = cb;
}