FROM ubuntu:latest

LABEL maintainer="Mauro Delazeri <mauro@zinnion.com>"

WORKDIR /home/bb

RUN ln -fs /usr/share/zoneinfo/America/New_York /etc/localtime
RUN apt-get update && apt-get install -y tzdata
RUN dpkg-reconfigure --frontend noninteractive tzdata

RUN apt-get update && apt-get -y install libssl-dev git wget zlib1g-dev autoconf cmake automake curl make unzip build-essential libtool pkg-config libjsoncpp-dev libboost-all-dev libcurl4-openssl-dev

ENV LD_LIBRARY_PATH=/home/bb/lib:/usr/local/lib/

ENV LIBKAFKA "1.6.1-TST2"
RUN wget https://github.com/edenhill/librdkafka/archive/refs/tags/v$LIBKAFKA.tar.gz -O /tmp/v$LIBKAFKA.tar.gz \
    && tar xvf /tmp/v$LIBKAFKA.tar.gz -C /tmp \
    && cd /tmp/librdkafka-$LIBKAFKA \
    && cmake . \
    && make -j$(nproc) \
    && make install \
    && rm -rf /tmp/librdkafka*
    
RUN git clone https://github.com/mfontanini/cppkafka.git /tmp/cppkafka \
    && cd /tmp/cppkafka \ 
    && cmake . \ 
    && make -j4 \ 
    && make install \
    && rm -rf /tmp/cppkafka

RUN git clone https://github.com/gabime/spdlog.git /tmp/spdlog \
    && cd /tmp/spdlog \
    && cmake . \
    && make -j$(nproc) \
    && make install \ 
    && rm -rf /tmp/spdlog

RUN git clone https://github.com/seznam/elasticlient.git /tmp/elasticlient \
    && cd /tmp/elasticlient \
    && git submodule update --init --recursive \
    && cmake -DBUILD_ELASTICLIENT_TESTS=NO . \
    && make -j$(nproc) \
    && make install \ 
    && rm -rf /tmp/elasticlient

RUN VERSION=`git ls-remote https://github.com/maurodelazeri/mandarinfish | grep refs/tags | grep -oE "[0-9]+\.[0-9]+\.[0-9]+$" | sort --version-sort | tail -n 1` \
    && sh -c "curl -L https://github.com/maurodelazeri/mandarinfish/releases/download/${VERSION}/mandarinfish > /usr/local/bin/mandarinfish" \
    && chmod +x /usr/local/bin/mandarinfish

#RUN git clone  https://github.com/maurodelazeri/mandarinfish.git /tmp/mandarinfish \
#    && cd /tmp/mandarinfish \
#    && cmake . \
#    && make -j4 \
#    && rm -rf /tmp/mandarinfish
