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

    std::string bulk_size = getEnvVar("BULK_INSERT_SIZE");
    if (bulk_size.empty()) {
        spdlog::info("Please specify export the variable BULK_INSERT_SIZE");
        exit(1);
    }

    el_index_ = getEnvVar("EL_INDEX");
    if (el_index_.empty()) {
        spdlog::info("Please specify export the variable EL_INDEX");
        exit(1);
    }

    bulk_inser_size_ = std::stoi(bulk_size);
    if (bulk_inser_size_ <= 0) {
        spdlog::info("Please specify export the variable BULK_INSERT_SIZE as an integer");
        exit(1);
    }
}

Streaming::~Streaming() {}

void Streaming::start() {
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

    elasticlient::SameIndexBulkData bulk(el_index_, bulk_inser_size_);
    elasticlient::Bulk bulkIndexer(client_);

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

                bulk.indexDocument("docType", sole::uuid4().str(), msg.get_payload());
                if (bulk.size() == bulk_inser_size_ - 1) {
                    size_t errors = bulkIndexer.perform(bulk);

                    if (system_debug_) {
                        std::cout << "When indexing " << bulk.size() << " documents, "
                                  << errors << " errors occured" << std::endl;
                    }

                    bulk.clear();
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
}
