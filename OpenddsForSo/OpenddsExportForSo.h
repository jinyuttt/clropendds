#pragma once


#include"publisher.h"
#include"Subscriber.h"
#include"OpenddsWarp.h"
extern "C" {
	struct  MsgQueue
	{
		string topic;
		unsigned char* content;
		long len;
		long id;
		string from;
	};

     typedef  void (*FuncPtr)(const char*, unsigned char*, long, long, const char*);
	__attribute__ ((visibility("default")))  int Addition(int a, int b);  
	__attribute__ ((visibility("default")))   int getString(char* b);
	__attribute__ ((visibility("default")))   int Initopendds(int argc, char* argv[]);
	__attribute__ ((visibility("default")))  int Createdomain(long id);
	__attribute__ ((visibility("default")))  int PublishMsg(const  char* topicname,const unsigned  char* content,int lengh, int status, const char* from);
	__attribute__ ((visibility("default")))   int PublishDomainMsg(long domainid, const  char* topicname, const unsigned  char* content, int lengh, int status, const char* from);
	
	__attribute__ ((visibility("default")))  int SetQueueMsg();
__attribute__ ((visibility("default")))  void Consumer(unsigned char** msg, long& len);
__attribute__ ((visibility("default")))  int GetQueueMsgSize();
	
	__attribute__ ((visibility("default")))  int SubscribeMsg(const  char* topicname, void (*func)(const char* , unsigned char*,long,long, const char* ));
	__attribute__ ((visibility("default")))   int SubscribeDomainMsg(long domainid, const  char* topicname, void (*func)(const char*, unsigned char*,long ,long, const char*));

	__attribute__ ((visibility("default")))   int OpenTestTopic();
	__attribute__ ((visibility("default")))   int GetDCPSInfoRepoTopic(long id, char* ret);
	__attribute__ ((visibility("default")))  int GetOpenDDSParticipantLocation(long id, char* ret);
	__attribute__ ((visibility("default")))  int GetOpenDDSConnectionRecord(long id, char* ret);


	__attribute__ ((visibility("default")))  int GetOpenDDSConnectionRecord(long id, char* ret);

	__attribute__ ((visibility("default")))  int SetReliabilityQosPolicyKind(const  char* topicname,  int value);
	__attribute__ ((visibility("default")))  int SetHistoryQosPolicyKindconst(const char* topicname,  int value);
	__attribute__ ((visibility("default")))  int SetDurabilityQosPolicyKind(const  char* topicname,  int value);


}

