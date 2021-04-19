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

https://github.com/Tencent/rapidjson
