//
// Created by mauro on 4/9/21.
//

#include "streaming.h"

Streaming::Streaming() {
    // Prepare Client for nodes of one Elasticsearch cluster
    std::string el_cluster_addrs = getEnvVar("EL_CLUSTER_ADDRS");
    if (el_cluster_addrs.empty()) {
        spdlog::info("Please specify export the variable EL_CLUSTER_ADDRS");
        exit(1);
    }

    system_debug_ = (strcasecmp("true", getEnvVar("DEBUG").c_str()) == 0);

    client_ = std::make_shared<elasticlient::Client>(
            std::vector<std::string>({el_cluster_addrs}));  // last / is mandatory

    el_index_ = getEnvVar("EL_INDEX");
    if (el_index_.empty()) {
        spdlog::info("Please specify export the variable EL_INDEX");
        exit(1);
    }
}

Streaming::~Streaming() {}

void Streaming::start() {
    try {
        std::string topic_name = getEnvVar("TOPIC");
        if (topic_name.empty()) {
            spdlog::info("Please specify export the variable TOPIC");
            exit(1);
        }

        std::string broker_list = getEnvVar("BROKERS_LIST");
        if (topic_name.empty()) {
            spdlog::info("Please specify export the variable BROKERS_LIST");
            exit(1);
        }

        std::string group_id = getEnvVar("GROUP_ID");
        if (topic_name.empty()) {
            spdlog::info("Please specify export the variable GROUP_ID");
            exit(1);
        }

        // Construct the configuration
        Configuration config = {
                {"metadata.broker.list", broker_list},
                {"group.id",             group_id},
                // Disable auto commit
                {"enable.auto.commit",   false}
        };

        // Create the consumer
        Consumer consumer(config);

        // Print the assigned partitions on assignment
        consumer.set_assignment_callback([](const TopicPartitionList &partitions) {
            cout << "Got assigned: " << partitions << endl;
        });

        // Print the revoked partitions on revocation
        consumer.set_revocation_callback([](const TopicPartitionList &partitions) {
            cout << "Got revoked: " << partitions << endl;
        });

        // Subscribe to the topic
        consumer.subscribe({topic_name});
        spdlog::info("Consuming messages from topic:{}", topic_name);

        // Create a consumer dispatcher
        ConsumerDispatcher dispatcher(consumer);

        // Now run the dispatcher, providing a callback to handle messages, one to handle
        // errors and another one to handle EOF on a partition
        dispatcher.run(
                // Callback executed whenever a new message is consumed
                [&](Message msg) {
                    // Print the key (if any)
                    if (system_debug_) {
                        if (msg.get_key()) {
                            cout << msg.get_key() << " -> ";
                        }
                        // Print the payload
//                    if (system_debug_) {
//                        cout << msg.get_payload() << endl;
//                    }
                    }

                    std::string payload = msg.get_payload();
                    rapidjson::Document document;
                    // Parse the JSON
                    if (document.Parse(payload.c_str()).HasParseError()) {
                        spdlog::error("Document parse error: {}", payload);
                        return;
                    }

                    if (!document.IsObject()) {
                        spdlog::error("Error: {}", "No data");
                        return;
                    }

                    std::string time;
                    if (document.HasMember("@timestamp")) {
                        const rapidjson::Value &timestamp = document["@timestamp"];
                        time = timestamp.GetString();
                        time = time.substr(0, 10);
                        std::replace(time.begin(), time.end(), '-', '.');
                    } else {
                        spdlog::error("Field @timestamp does not exist.");
                        return;
                    }

//                    cout << "updating data on the index " << el_index_ + time << " " << msg.get_payload() << endl;

                    cpr::Response indexResponse = client_->index(el_index_ + time, "docType", sole::uuid4().str(),
                                                                 msg.get_payload());
                    if (indexResponse.status_code != 200) {
                        spdlog::error("Error while inserting on elasticsearch: Status code {} Message {}",
                                      indexResponse.status_code, indexResponse.text);
                        return;
                    }

                    // Now commit the message
                    consumer.commit(msg);
                },
                // Whenever there's an error (other than the EOF soft error)
                [](Error error) {
                    cout << "[+] Received error notification: " << error << endl;
                },
                // Whenever EOF is reached on a partition, print this
                [](ConsumerDispatcher::EndOfFile, const TopicPartition &topic_partition) {
                    cout << "Reached EOF on partition " << topic_partition << endl;
                }
        );
    } catch (std::exception &e) {
        spdlog::error("Streaming error: {}", e.what());
    }
}
