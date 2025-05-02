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
static DDS::Publisher_var pub;

int publish(string topicname, const unsigned char* content, int len, int status,string from) {
	if (factory == nullptr) {
		return -1;
	}
	if (pub == nullptr) {
		pub = createPublisher(last);
	}

	// 填充my_data数组...
	CORBA::Octet* buffer=new CORBA::Octet[len];
	// std::unique_ptr<CORBA::Octet> ptr2 = std::make_unique<CORBA::Octet>(len+1);
	memcpy(buffer, content, len); // 使用memcpy进行数组复制
	DDSMessenger::Message::_tao_seq_CORBA_Octet_  sequence(len+1,len, buffer);
	
	DDS::Topic_var topic = createTopic(last, topicname);
	DDSMessenger::MessageDataWriter_var message_writer = createWriter(pub, topic);
	DDSMessenger::Message message;
	 message.subject_id = increment();
	 message.from = from.c_str();
	 message.subject = topicname.c_str();
	 message.text = "object";
	 message.content = sequence;
	 message.domain_id = topic->get_participant()->get_domain_id();
	 publishData(message_writer, message, status);
	 delete buffer;

}

int publish(int domainid,string topicname,const unsigned char* content,int len, int status,string from) {
	if (factory == nullptr) {
		return -1;
	}
	//
	DDS::DomainParticipant_var participant=createDomainParticipant(domainid);
	DDS::Publisher_var curpub= createPublisher(participant);
	// 填充my_data数组...
	CORBA::Octet* buffer = new CORBA::Octet[len];
	memcpy(buffer, content, len); // 使用memcpy进行数组复制
	DDSMessenger::Message::_tao_seq_CORBA_Octet_  sequence(len + 1, len, buffer);

	DDS::Topic_var topic = createTopic(participant, topicname);
	DDSMessenger::MessageDataWriter_var message_writer = createWriter(curpub, topic);
	DDSMessenger::Message message;
	message.subject_id = increment();
	message.from = from.c_str();
	message.subject = topicname.c_str();
	message.text = "object";
	message.content = sequence;
	message.domain_id = topic->get_participant()->get_domain_id();
	
	publishData(message_writer, message, status);

	delete buffer;

}
