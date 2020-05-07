FROM gusugusu1018/simmob:env

RUN mkdir simmobility-prod
COPY . simmobility-prod/

RUN cd simmobility-prod/dev/Basic \
   && mkdir Release \
   && cd Release \
   && cmake .. \
    -DCMAKE_BUILD_TYPE:STRING=Release \
    -DBUILD_SHORT:BOOL=ON \
    -DBUILD_MEDIUM:BOOL=ON \
    -DBUILD_LONG:BOOL=ON \
    -DBUILD_TESTS_LONG:BOOL=OFF \
   && make -j4
