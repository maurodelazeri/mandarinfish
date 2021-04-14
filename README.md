# mandarinfish
Kafka pipeline to Elasticsearch

### Compilation to build a new release

```
mkdir build
cd build
cmake ..
make
```

### Build docker image

Note that you need to build a [release](https://github.com/maurodelazeri/mandarinfish/releases) first

```
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
