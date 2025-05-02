#pragma once
#include "DDSMessengerTypeSupportImpl.h"

#include <dds/DCPS/Marked_Default_Qos.h>
#include <dds/DCPS/Service_Participant.h>
#include <dds/DCPS/WaitSet.h>
#include <dds/DCPS/StaticIncludes.h>
#include <dds/DCPS/BuiltInTopicDataReaderImpls.h>
#include <dds/DCPS/ConnectionRecords.h>
#include <dds/DCPS/BuiltInTopicUtils.h>
#include <iostream>
#include <vector>
#include <map>
#include "DataReaderListenerImpl.h"
#include <ace/Init_ACE.h>
using namespace std;
#if OPENDDS_DO_MANUAL_STATIC_INCLUDES
#  include <dds/DCPS/RTPS/RtpsDiscovery.h>
#  include <dds/DCPS/transport/rtps_udp/RtpsUdp.h>
#endif

#include <dds/DdsDcpsInfrastructureC.h>
#include <dds/DdsDcpsPublicationC.h>
#include <thread>
#include <mutex>
#include <shared_mutex>
#include <ace/Log_Msg.h>

#define enum_to_string(x) #x
  map<long, DDS::DomainParticipant_var> dict;
  map<long, DDSMessenger::MessageTypeSupport_var> dictmsg;
  static DDS::DomainParticipantFactory_var factory;//Ψһ
  static  DDS::DomainParticipant_var last;//����һ����������
  static  DDSMessenger::MessageTypeSupport_var msg;//Ψһ
  static  bool isTestTopic = false;
  map<long,map<string, DDS::Topic_var>> dictTopics;//ÿ�����ÿ���������
  map<long,map<string, DDSMessenger::MessageDataWriter_var>> dictwrite;
  map<long, DDS::Publisher_var> dictpublisher;//ÿ����һ��������
  map<long, DDS::Subscriber_var> dictsubscriber;//ÿ����һ��������
  map<long, map<string, DDS::DataReader_var>> dictreader;//ÿ����ÿ������һ����ȡ��
  map<string,int>  dictTestTopic ;//ÿ����һ��������
  std::mutex mutex_var;
  std::mutex mutexpublisher_var;
  std::mutex mutexsubscribe_var;
  std::mutex mutexdomain_var;
  std::shared_mutex mtxtopic_var;
  /*
  * ����������Ψһ
  */
  int createFactory(int argc, char* argv[]) {
      //����Ҳ������RAII˼��
      std::lock_guard<std::mutex> lock(mutex_var);
      if (factory == nullptr) {
          ACE::init();
          DDS::DomainParticipantFactory_var dpf = TheParticipantFactoryWithArgs(argc, argv);
          factory = dpf;
          if (!dpf) {
              std::cerr << "DomainParticipantFactory_var failed." << std::endl;
              return 1;
          }
      }
      return 0;
  }

  /*
  * ������
  */
  DDS::DomainParticipant_var createDomainParticipant(long id) {
      std::lock_guard<std::mutex> lock(mutexdomain_var);
     DDS::DomainParticipantFactory_var dpf=factory;
     auto pos = dict.find(id);
     if(pos != dict.end()) {
         return pos->second;
     }
     DDS::DomainParticipant_var participant =
         dpf->create_participant(id, // domain ID
             PARTICIPANT_QOS_DEFAULT,
             0,  // No listener required
             OpenDDS::DCPS::DEFAULT_STATUS_MASK);
     if (!participant) {
         std::cerr << "create_participant failed." << std::endl;
         return nullptr;
     }

     dict.insert(pair<int, DDS::DomainParticipant_var>(id
         , participant));
     DDSMessenger::MessageTypeSupport_var mts =
         new DDSMessenger::MessageTypeSupportImpl();
     if (DDS::RETCODE_OK != mts->register_type(participant, "")) {
         std::cerr << "register_type failed." << std::endl;
         return nullptr;
     }
     dictmsg[id] = mts;
     last = participant;
     msg = mts;
     last = participant;
     return participant;

}

  /*
  * ��ȡ�Ѿ����ڵ�
  * ������
  */
  DDS::Topic_var readTopic(long domainid,string topicname) {
      std::shared_lock<std::shared_mutex> lock(mtxtopic_var);
          auto mapfind = dictTopics.find(domainid);
          if (mapfind != dictTopics.end()) {
              map<string, DDS::Topic_var> maptopics = mapfind->second;
              auto topics = maptopics.find(topicname);
              if (topics != maptopics.end()) {
                  DDS::Topic_var cur = topics->second;
                  return cur;
              }
          }
      return nullptr;
  }

  /*
  * ��Ӹ���
  * д����
  */
  DDS::Topic_var writeTopic(DDS::DomainParticipant_var participant, string topicname) {
      //�½�
      std::unique_lock<std::shared_mutex> lock(mtxtopic_var);
       const  long id = participant->get_domain_id();
      auto pos = dictmsg.find(id);
      if (pos == dictmsg.end()) {
          std::cerr << "no create domain when create topic." << std::endl;
          return nullptr;
      }
      DDSMessenger::MessageTypeSupport_var mts = pos->second;
      CORBA::String_var type_name = mts->get_type_name();
      DDS::Topic_var topic =
          participant->create_topic(topicname.c_str(),
              type_name,
              TOPIC_QOS_DEFAULT,
              0,  // No listener required
              OpenDDS::DCPS::DEFAULT_STATUS_MASK);
      if (!topic) {
          std::cerr << "create_topic failed." << std::endl;
          return nullptr;
      }
     auto domainpos=  dictTopics.find(id);
     if (domainpos != dictTopics.end()) {
         //�Ѿ�����
         domainpos->second[topicname] = topic;
     }
     else {
         map<string, DDS::Topic_var> newtopic;
         newtopic[topicname] = topic;
         dictTopics[id] = newtopic;
     }
      return topic;
  }

  /*
  * ��������
  */
 DDS::Topic_var createTopic(DDS::DomainParticipant_var participant, string topicname) {
  
   const  long id = participant->get_domain_id();
     DDS::Topic_var topic;
     topic = readTopic(id, topicname);
     if (!topic) {
         topic = writeTopic(participant, topicname);
     }
     return topic;
     
 }


 /*
 * ��������
 */
 DDS::Publisher_var createPublisher(DDS::DomainParticipant_var participant) {
   const  long id= participant->get_domain_id();
     auto pos=  dictpublisher.find(id);
     if (pos != dictpublisher.end()) {
         return pos->second;
     }

     std::lock_guard<std::mutex> lock(mutexpublisher_var);
     DDS::Publisher_var pub =
         participant->create_publisher(PUBLISHER_QOS_DEFAULT,
             0,  // No listener required
             OpenDDS::DCPS::DEFAULT_STATUS_MASK);
     if (!pub) {
         std::cerr << "create_publisher failed." << std::endl;
         return nullptr;
     }
     dictpublisher[id] = pub;
     return pub;

 }

 /*
 * ������Ϣ
 */
 DDSMessenger::MessageDataWriter_var   createWriter(DDS::Publisher_var pub, DDS::Topic_var topic) {
     // ����һ��DataWriter
     long id = topic->get_participant()->get_domain_id();
     string name = topic->get_name();
   auto domain=  dictwrite.find(id);
   if (domain != dictwrite.end()) {
       map<string, DDSMessenger::MessageDataWriter_var>  mapwrite = domain->second;
      auto pos= mapwrite.find(name);
      if (pos != mapwrite.end()) {
          DDSMessenger::MessageDataWriter_var curwriter = pos->second;
          return curwriter;
      }
   }
     DDS::DataWriter_var writer =
         pub->create_datawriter(topic,
             DATAWRITER_QOS_DEFAULT,
             0,  // No listener required
             OpenDDS::DCPS::DEFAULT_STATUS_MASK);
     if (!writer) {
         std::cerr << "create_datawriter failed." << std::endl;
         return nullptr;
     }
     DDSMessenger::MessageDataWriter_var message_writer =
         DDSMessenger::MessageDataWriter::_narrow(writer);
     map<string, DDSMessenger::MessageDataWriter_var> dictdataw;
     dictdataw[name] = message_writer;
     dictwrite[id] = dictdataw;
     return message_writer;
 }

 /*
 * �����߷������ݣ�status=0����������1�ȴ�15�뷢��
 */
 int publishData(DDSMessenger::MessageDataWriter_var message_writer, DDSMessenger::Message message, int status) {
  
     if (status == 0) {
         DDS::InstanceHandle_t handle = message_writer->register_instance(message);
         DDS::ReturnCode_t ret = message_writer->write(message, handle);
        // DDS::ReturnCode_t error = message_writer->write(message, DDS::HANDLE_NIL);
         
         ++message.subject_id;
         if (ret != DDS::RETCODE_OK) {
             // Log or otherwise handle the error condition
             return 1;
         }
         return 0;
     }
     else {
         // ��ϣ�ֱ��Subscriber����

         // 1.�����Ǵ�����DataWriter�л�ȡ״̬����
         DDS::StatusCondition_var condition = message_writer->get_statuscondition();
         // 2.��conditions �����÷���ƥ��
         condition->set_enabled_statuses(DDS::PUBLICATION_MATCHED_STATUS);
         // 3.����һ��wait���� 
         DDS::WaitSet_var ws = new DDS::WaitSet;
         // 4.��״̬�������ӵ�wait������
         ws->attach_condition(condition);

         while (true) {
             DDS::PublicationMatchedStatus matches;
             // 5.��÷������ƥ��״̬
             if (message_writer->get_publication_matched_status(matches) != DDS::RETCODE_OK) {
                 std::cerr << "get_publication_matched_status failed!"
                     << std::endl;
                 return 1;
             }
             // 6.�����ǰ��ƥ������һ������࣬��conditions ��wait �����з����������������
             if (matches.current_count >= 1) {
                 break;
             }

             DDS::ConditionSeq conditions;
             DDS::Duration_t timeout = { 15, 0 };
             // 7.��wait�����ϵȴ���������һ��ָ����ʱ������޶���
             if (ws->wait(conditions, timeout) != DDS::RETCODE_OK) {
                 std::cerr << "wait failed!" << std::endl;
                 return 1;
             }

         }
         ws->detach_condition(condition);
         publishData(message_writer, message, 0);
     }
 }


 /*
 * ����������
 */
 DDS::Subscriber_var createSubscriber(DDS::DomainParticipant_var participant ) {
     // Create the subscriber

   const long id = participant->get_domain_id();
     auto pos = dictsubscriber.find(id);
     if (pos != dictsubscriber.end()) {
         return pos->second;
     }

     std::lock_guard<std::mutex> lock(mutexsubscribe_var);
     DDS::Subscriber_var sub =
         participant->create_subscriber(SUBSCRIBER_QOS_DEFAULT,
             0,  // No listener required
             OpenDDS::DCPS::DEFAULT_STATUS_MASK);
     if (!sub) {
         std::cerr << "Failed to create_subscriber." << std::endl;
         return nullptr;
     }
     dictsubscriber[id] = sub;
     return sub;

 }

 /*
 * ������ȡ��
 */
 DDS::DataReader_var createDataReader(DDS::Subscriber_var sub, DDS::Topic_var topic, DDS::DataReaderListener_var listener) {
     // Create the Datareader
    const long id = topic->get_participant()->get_domain_id();
     string name = topic->get_name();
     bool isfinddomain = false;
     auto domain=  dictreader.find(id);
     if (domain != dictreader.end()) {
         isfinddomain = true;
         map<string, DDS::DataReader_var> mapreader = domain->second;
        auto pos=  mapreader.find(name);
        if (pos != mapreader.end()) {
            return pos->second;
        }
     }
     DDS::DataReader_var dr =
         sub->create_datareader(topic,
             DATAREADER_QOS_DEFAULT,
             listener,
             OpenDDS::DCPS::DEFAULT_STATUS_MASK);
     if (!dr) {
         std::cerr << "create_datareader failed." << std::endl;
         return nullptr;
     }
     if (isfinddomain) {
         domain->second[name] = dr;
     }
     else {
         map<string, DDS::DataReader_var> dic;
         dic[name] = dr;
         dictreader[id] = dic;
     }
     return dr;
 }

 /*
 * ��������
 */
 DDS::DataReaderListener_var createLister(std::function<void(DDSMessenger::MessageSeq)> callback) {
    
     DataReaderListenerImpl* tmp = new DataReaderListenerImpl;
     tmp->setcallback(callback);
     DDS::DataReaderListener_var listener(tmp);
     return listener;
 }

 /*
 * ����
 */
 int clean(DDS::DomainParticipant_var participant) {
     participant->delete_contained_entities();
     factory->delete_participant(participant);
     TheServiceParticipant->shutdown();
     return 0;
 }

 int setQos(DDS::Topic_var topic, DDS::TopicQos topic_qos) {
     // Get default Publisher QoS from a DomainParticipant:
    
     DDS::ReturnCode_t ret;
     ret=topic->set_qos(topic_qos);
     if (DDS::RETCODE_OK != ret) {
         std::cerr << "Could not get default topic QoS" << std::endl;
         return 1;
     }
     return 0;
   
 }

 int setQos(DDS::Publisher_var pub, DDS::PublisherQos pub_qos) {
   
     DDS::ReturnCode_t ret;
    
     ret=pub->set_qos(pub_qos);
     // Get default DomainParticipant QoS from a DomainParticipantFactory:
  
     if (DDS::RETCODE_OK != ret) {
         std::cerr << "Could not get default participant QoS" << std::endl;
         return 1;
     }
     return 0;
    
 }
 int setQos(DDS::Subscriber_var sub, DDS::SubscriberQos sub_qos) {
    
     DDS::ReturnCode_t ret;
   
     ret=sub->set_qos(sub_qos);
    
     if (DDS::RETCODE_OK != ret) {
         std::cerr << "Could not get default subscriber QoS" << std::endl;
         return 1;
     }

     return 0;
 }
 int setQos(DDS::DomainParticipant_var participant, DDS::DomainParticipantQos dp_qos) {
   
     DDS::ReturnCode_t ret;
   ret=  participant->set_qos(dp_qos);
    
     if (DDS::RETCODE_OK != ret) {
         std::cerr << "Could not get default participant QoS" << std::endl;
         return 1;
     }
     return 0;
    
 }
 int setQos(DDSMessenger::MessageDataWriter_var writer, DDS::DataWriterQos dw_qos) {
     // Get default Publisher QoS from a DomainParticipant:
   
     DDS::ReturnCode_t ret;
  
     // Get default DataWriter QoS from a Publisher:
    
     ret=writer->set_qos(dw_qos);
    
     if (DDS::RETCODE_OK != ret) {
         std::cerr << "Could not get default data writer QoS" << std::endl;
         return 1;
     }
     return 0;
 }
 int setQos(DDS::DataReader_var reader, DDS::DataReaderQos dr_qos) {
   
     DDS::ReturnCode_t ret;

     ret=reader->set_qos(dr_qos);
   
     if (DDS::RETCODE_OK != ret) {
         std::cerr << "Could not get default data reader QoS" << std::endl;
         return 1;
     }

     return 0;
 }

 string GetDCPSInfoRepoTopic(long id) {
   auto pos=  dict.find(id);
   if (pos != dict.end()) {
       DDS::DomainParticipant_var participant = pos->second;
       DDS::Subscriber_var bit_subscriber = participant->get_builtin_subscriber();
       //������
       DDS::DataReader_var dr =
           bit_subscriber->lookup_datareader(OpenDDS::DCPS::BUILT_IN_PARTICIPANT_TOPIC);
       DDS::ParticipantBuiltinTopicDataDataReader_var part_dr =
           DDS::ParticipantBuiltinTopicDataDataReader::_narrow(dr);

       
       DDS::ParticipantBuiltinTopicDataSeq part_data;
       DDS::SampleInfoSeq infos;
       DDS::ReturnCode_t ret = part_dr->read(part_data, infos, 20,
           DDS::ANY_SAMPLE_STATE,
           DDS::ANY_VIEW_STATE,
           DDS::ANY_INSTANCE_STATE);

       // Check return status and read the participant data
       //��������
       DDS::DataReader_var drpub =
           bit_subscriber->lookup_datareader(OpenDDS::DCPS::BUILT_IN_PUBLICATION_TOPIC);
       DDS::PublicationBuiltinTopicDataDataReader_var pub_dr =
           DDS::PublicationBuiltinTopicDataDataReader::_narrow(drpub);

       DDS::PublicationBuiltinTopicDataSeq  pub_data;
       DDS::SampleInfoSeq infos_pub;
       DDS::ReturnCode_t ret_pub = pub_dr->read(pub_data, infos_pub, 20,
           DDS::ANY_SAMPLE_STATE,
           DDS::ANY_VIEW_STATE,
           DDS::ANY_INSTANCE_STATE);
       //��������
       DDS::DataReader_var drsub =
           bit_subscriber->lookup_datareader(OpenDDS::DCPS::BUILT_IN_PUBLICATION_TOPIC);
       DDS::SubscriptionBuiltinTopicDataDataReader_var sub_dr =
           DDS::SubscriptionBuiltinTopicDataDataReader::_narrow(drsub);

       DDS::SubscriptionBuiltinTopicDataSeq  sub_data;
       DDS::SampleInfoSeq infos_sub;
       DDS::ReturnCode_t ret_sub = sub_dr->read(sub_data, infos_sub, 20,
           DDS::ANY_SAMPLE_STATE,
           DDS::ANY_VIEW_STATE,
           DDS::ANY_INSTANCE_STATE);

       //ÿ��ȡ����Ӧ�����ݣ�ȡ�����µ�
       string domain;
       DDS::ParticipantBuiltinTopicData part= part_data[part_data.length()-1];
       domain.append("{");
       domain.append("\"ReliabilityQosPolicy\":");
      const  int len =(int) part.user_data.value.length();
       unsigned char* content = new unsigned char[len];
       memcpy(content, part.user_data.value.get_buffer(), len); // ʹ��memcpy�������鸴��
       domain.append(reinterpret_cast<char*>(content), len);
       domain.append("}");
       //
       string pub;
       DDS::PublicationBuiltinTopicData pubdata = pub_data[pub_data.length()-1];
       pub.append("{");
       pub.append("\"ReliabilityQosPolicy\":\"");

     
       pub.append(enum_to_string(pubdata.reliability.kind));
       pub.append("\"}");
       //
       string sub;
       DDS::SubscriptionBuiltinTopicData subdata = sub_data[sub_data.length()-1];
       sub.append("{");
       sub.append("\"ReliabilityQosPolicy\":");
    
       sub.append(enum_to_string(subdata.reliability.kind));
       sub.append("\"}");
       string result;

       result.append("{");
       result.append("\"Participan\":");
       result.append(domain);
       result.append(",");
      
       result.append("\"Publication\":");
       result.append(pub);
       result.append(",");
      
       result.append("\"Subscription\":");
       result.append(sub);
       result.append("}");
       return result;
   }
 }
 

 string GetOpenDDSParticipantLocation(long id) {
     auto pos = dict.find(id);
     if (pos != dict.end()) {
         DDS::DomainParticipant_var participant = pos->second;
         DDS::Subscriber_var bit_subscriber = participant->get_builtin_subscriber();
         //������
         DDS::DataReader_var dr =
             bit_subscriber->lookup_datareader(OpenDDS::DCPS::BUILT_IN_PARTICIPANT_LOCATION_TOPIC);

         OpenDDS::DCPS::ParticipantLocationBuiltinTopicDataDataReader_var loc_dr =
             OpenDDS::DCPS::ParticipantLocationBuiltinTopicDataDataReader::_narrow(dr);

         OpenDDS::DCPS::ParticipantLocationBuiltinTopicDataSeq  loc_data;
         DDS::SampleInfoSeq infos;
         DDS::ReturnCode_t ret = loc_dr->read(loc_data, infos, 20,
             DDS::ANY_SAMPLE_STATE,
             DDS::ANY_VIEW_STATE,
             DDS::ANY_INSTANCE_STATE);

         OpenDDS::DCPS::ParticipantLocationBuiltinTopicData data = loc_data[loc_data.length()-1];
      
         string addr = data.local_addr;
         return addr;


     }
     return "";
 }

 string GetOpenDDSConnectionRecord(long id) {
     auto pos = dict.find(id);
     if (pos != dict.end()) {
         DDS::DomainParticipant_var participant = pos->second;
         DDS::Subscriber_var bit_subscriber = participant->get_builtin_subscriber();
         //������
         DDS::DataReader_var dr =
             bit_subscriber->lookup_datareader(OpenDDS::DCPS::BUILT_IN_CONNECTION_RECORD_TOPIC);

         OpenDDS::DCPS::ConnectionRecordDataReader_var record_dr =
             OpenDDS::DCPS::ConnectionRecordDataReader::_narrow(dr);

         OpenDDS::DCPS::ConnectionRecordSeq  record_data;
         DDS::SampleInfoSeq infos;
         DDS::ReturnCode_t ret = record_dr->read(record_data, infos, 20,
             DDS::ANY_SAMPLE_STATE,
             DDS::ANY_VIEW_STATE,
             DDS::ANY_INSTANCE_STATE);

         OpenDDS::DCPS::ConnectionRecord data = record_data[record_data.length() - 1];

         string addr = data.address;
         return addr;


     }
     return "";
 }