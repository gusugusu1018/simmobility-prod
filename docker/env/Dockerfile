FROM ubuntu:18.04

RUN apt-get update
RUN DEBIAN_FRONTEND="noninteractive" apt-get -y install tzdata
RUN apt-get install -y \
         libtinyxml-dev \
         libpq-dev \
         libxerces-c-dev \
         libjsoncpp-dev \
         xsdcxx \
         liblua5.2-dev \
         g++ \
         git \
         qgit \
         cmake \
         libcppunit-dev \
         doxygen \
         libglpk-dev \
         libssl-dev \
         libsoci-dev \
         postgresql \
         binutils \
         libproj-dev \
         gdal-bin \
         python \
         python-pip \
         libssl-dev \
         python-pandas \
         python-matplotlib \
         python-numpy \
         python-crypto \
         python-psycopg2 \
         wget

# libboost-all-dev \
RUN wget https://dl.bintray.com/boostorg/release/1.68.0/source/boost_1_68_0.tar.bz2 \
   && tar --bzip2 -xf boost_1_68_0.tar.bz2 \
   && cd boost_1_68_0 \
   && bash bootstrap.sh \
   && ./b2 install

RUN cd / && rm boost_1_68_0.tar.bz2
