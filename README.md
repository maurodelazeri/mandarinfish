# mandarinfish
Kafka pipeline to Elasticsearch

### Build docker image

```
mkdir build
cd build
cmake ..
make
cd ..
docker build -t mandarinfish .
```

### Kubernetes

`deployment.yaml` is available


### Dependencies

https://github.com/edenhill/librdkafka

https://github.com/mfontanini/cppkafka

https://github.com/gabime/spdlog

https://github.com/seznam/elasticlient 


### Create timestamp on insert

```json

curl -XPUT 'localhost:9200/my_index' -H 'Content-Type: application/json' -d '
{
  "settings": {
    "default_pipeline": "my_timestamp_pipeline"
  }
}
'

curl -XPUT 'localhost:9200/_ingest/pipeline/my_timestamp_pipeline' -H 'Content-Type: application/json' -d '
{
  "description": "Adds a field to a document with the time of ingestion",
  "processors": [
    {
      "set": {
        "field": "ingest_timestamp",
        "value": "{{_ingest.timestamp}}"
      }
    }
  ]
}
'
```
