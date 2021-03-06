//
// Created by mauro on 4/9/21.
//
#pragma once

#include <stdexcept>
#include <iostream>
#include <csignal>
#include <fstream>
#include <rapidjson/document.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/writer.h>
#include <spdlog/spdlog.h>
#include <boost/program_options.hpp>
#include <cppkafka/consumer.h>
#include <cppkafka/configuration.h>
#include <cppkafka/utils/consumer_dispatcher.h>
#include <cpr/response.h>
#include <elasticlient/client.h>
#include <elasticlient/bulk.h>
#include "sole.h"
#include <date/date.h>
#include "ThreadPool.h"

using std::string;
using std::exception;
using std::cout;
using std::endl;
using std::function;

using cppkafka::Consumer;
using cppkafka::ConsumerDispatcher;
using cppkafka::Configuration;
using cppkafka::Message;
using cppkafka::TopicPartition;
using cppkafka::TopicPartitionList;
using cppkafka::Error;

namespace po = boost::program_options;
using namespace std;

class Streaming {
private:
    static inline std::string getEnvVar(std::string const &key) {
        char const *val = getenv(key.c_str());
        return val == nullptr ? std::string() : std::string(val);
    }

    ThreadPool pool_elasticsearch{static_cast<size_t>(std::stoi(getEnvVar("THREADPOOL")))};

    bool system_debug_{false};
    std::string el_index_;
    unsigned int bulk_size_;
    std::shared_ptr<elasticlient::Client> client_;

    void PushToElasticSearch(const std::string &index, const std::vector<std::string> &data);

public:
    Streaming();

    ~Streaming();

    void start();
};