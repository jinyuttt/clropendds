#pragma once
#ifdef MYLIB_EXPORTS
#define MYLIB_API __declspec(dllexport)
#else
#define MYLIB_API __declspec(dllimport)
#endif

#include"publisher.h"
#include"Subscriber.h"
#include"OpenddsWarp.h"
extern "C" {
	MYLIB_API int Addition(int a, int b);  
	MYLIB_API int getString(char* b);
	MYLIB_API int Initopendds(int argc, char* argv[]);
	MYLIB_API int Createdomain(long id);
	MYLIB_API int PublishMsg(const  char* topicname,const unsigned  char* content,int lengh, int status, const char* from);
	MYLIB_API int PublishDomainMsg(long domainid, const  char* topicname, const unsigned  char* content, int lengh, int status, const char* from);
	MYLIB_API int SubscribeMsg(const  char* topicname, void (*func)(const char* , unsigned char*,long,long, const char* ));
	MYLIB_API int SubscribeDomainMsg(long domainid, const  char* topicname, void (*func)(const char*, unsigned char*,long ,long, const char*));

	MYLIB_API int OpenTestTopic();
	MYLIB_API int GetDCPSInfoRepoTopic(long id, char* ret);
	MYLIB_API int GetOpenDDSParticipantLocation(long id, char* ret);
	MYLIB_API int GetOpenDDSConnectionRecord(long id, char* ret);


	MYLIB_API int GetOpenDDSConnectionRecord(long id, char* ret);

	MYLIB_API int SetReliabilityQosPolicyKind(const  char* topicname,  int value);
	MYLIB_API int SetHistoryQosPolicyKindconst(const char* topicname,  int value);
	MYLIB_API int SetDurabilityQosPolicyKind(const  char* topicname,  int value);


}

