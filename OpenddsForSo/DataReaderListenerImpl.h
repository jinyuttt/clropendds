#pragma once

#ifndef DATAREADER_LISTENER_IMPL_H
#define DATAREADER_LISTENER_IMPL_H
#include "DDSMessengerTypeSupportImpl.h"
#include <ace/Global_Macros.h>

#include <dds/DCPS/LocalObject.h>
#include <dds/DCPS/Definitions.h>
#include <dds/DCPS/Service_Participant.h>
#include <dds/DCPS/Marked_Default_Qos.h>
#include <dds/DCPS/PublisherImpl.h>
#include <ace/streams.h>

#include "dds/DCPS/StaticIncludes.h"

#include <stdio.h>
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

#ifdef _WIN32
#  define SLEEP(a)    Sleep(a*1000)
#else
#  define SLEEP(a)    sleep(a);
#endif
class DataReaderListenerImpl
    : public virtual OpenDDS::DCPS::LocalObject<DDS::DataReaderListener> {
public:
    std::function<void(DDSMessenger::MessageSeq)> callback;
public:
    virtual void on_requested_deadline_missed(
        DDS::DataReader_ptr reader,
        const DDS::RequestedDeadlineMissedStatus& status);

    virtual void on_requested_incompatible_qos(
        DDS::DataReader_ptr reader,
        const DDS::RequestedIncompatibleQosStatus& status);

    virtual void on_sample_rejected(
        DDS::DataReader_ptr reader,
        const DDS::SampleRejectedStatus& status);

    virtual void on_liveliness_changed(
        DDS::DataReader_ptr reader,
        const DDS::LivelinessChangedStatus& status);

    virtual void on_data_available(
        DDS::DataReader_ptr reader);

    virtual void on_subscription_matched(
        DDS::DataReader_ptr reader,
        const DDS::SubscriptionMatchedStatus& status);

    virtual void on_sample_lost(
        DDS::DataReader_ptr reader,
        const DDS::SampleLostStatus& status);

    virtual void setcallback(
        std::function<void(DDSMessenger::MessageSeq)> cb);
};

#endif /* DATAREADER_LISTENER_IMPL_H */


