#include <ace/Log_Msg.h>
#include <ace/OS_NS_stdlib.h>

#include "DataReaderListenerImpl.h"
#include "DDSMessengerTypeSupportC.h"
#include "DDSMessengerTypeSupportImpl.h"

#include <iostream>
#include <functional>

void
DataReaderListenerImpl::on_requested_deadline_missed(
    DDS::DataReader_ptr /*reader*/,
    const DDS::RequestedDeadlineMissedStatus& /*status*/)
{
}

void
DataReaderListenerImpl::on_requested_incompatible_qos(
    DDS::DataReader_ptr /*reader*/,
    const DDS::RequestedIncompatibleQosStatus& /*status*/)
{
}

void
DataReaderListenerImpl::on_sample_rejected(
    DDS::DataReader_ptr /*reader*/,
    const DDS::SampleRejectedStatus& /*status*/)
{
}

void
DataReaderListenerImpl::on_liveliness_changed(
    DDS::DataReader_ptr /*reader*/,
    const DDS::LivelinessChangedStatus& /*status*/)
{
}

void
DataReaderListenerImpl::on_data_available(DDS::DataReader_ptr reader)
{
    DDSMessenger::MessageDataReader_var reader_i =
        DDSMessenger::MessageDataReader::_narrow(reader);

    if (!reader_i) {
        ACE_ERROR((LM_ERROR,
            ACE_TEXT("ERROR: %N:%l: on_data_available() -")
            ACE_TEXT(" _narrow failed!\n")));
        ACE_OS::exit(1);
    }

  //  DDSMessenger::Message message;
//    DDS::SampleInfo info;
    DDSMessenger::MessageSeq messages(10);
    DDS::SampleInfoSeq sampleInfos(10);
  // long id= reader_i->get_subscriber()->get_participant()->get_domain_id();
    DDS::ReturnCode_t error = reader_i->take(messages,
        sampleInfos,
        10,
        DDS::ANY_SAMPLE_STATE,
        DDS::ANY_VIEW_STATE,
        DDS::ANY_INSTANCE_STATE);
   // const DDS::ReturnCode_t error = reader_i->take_next_sample(message, info);

    if (error == DDS::RETCODE_OK) {
        //std::cout << "SampleInfo.sample_rank = " << info.sample_rank << std::endl;
        //std::cout << "SampleInfo.instance_state = " << OpenDDS::DCPS::InstanceState::instance_state_mask_string(info.instance_state) << std::endl;

        //if (info.valid_data) {
        //    // 数据的输出  取决于你的IDL定义 
        //    std::cout << "Message: subject    = " << message.subject.in() << std::endl
        //        << "         subject_id = " << message.subject_id << std::endl
        //        << "         from       = " << message.from.in() << std::endl
        //        << "         count      = " << message.count << std::endl
        //        << "         text       = " << message.text.in() << std::endl;

        //}
        callback(messages);
    }
    else {
        ACE_ERROR((LM_ERROR,
            ACE_TEXT("ERROR: %N:%l: on_data_available() -")
            ACE_TEXT(" take_next_sample failed!\n")));
    }
}

void
DataReaderListenerImpl::on_subscription_matched(
    DDS::DataReader_ptr /*reader*/,
    const DDS::SubscriptionMatchedStatus& /*status*/)
{
}

void
DataReaderListenerImpl::on_sample_lost(
    DDS::DataReader_ptr /*reader*/,
    const DDS::SampleLostStatus& /*status*/)
{
}


void
DataReaderListenerImpl::setcallback(
    std::function<void(DDSMessenger::MessageSeq)> cb)
{
    callback = cb;
}
