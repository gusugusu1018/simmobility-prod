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
#RUN wget https://dl.bintray.com/boostorg/release/1.73.0/source/boost_1_73_0.tar.bz2 \
#   && tar --bzip2 -xf boost_1_73_0.tar.bz2 \
#   && cd boost_1_73_0 \
#   && bash bootstrap.sh \
#   && ./b2 install
#RUN cmake ../ -DCMAKE_BUILD_TYPE:STRING=Release \
#    -DBUILD_SHORT:BOOL=ON \
#    -DBUILD_MEDIUM:BOOL=ON \
#    -DBUILD_LONG:BOOL=ON \
#    -DBUILD_TESTS_LONG:BOOL=OFF
#dpkg -s libboost-dev | grep 'Version'

RUN git clone https://github.com/smart-fm/simmobility-prod.git
