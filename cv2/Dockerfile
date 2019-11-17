FROM arm32v6/alpine:3.10

RUN apk add --update \
  gcc \
  musl-dev \
  linux-headers \
  && rm -rf /var/cache/apk/*

COPY fonts.c /
COPY oled96.c /
COPY oled96.h /
COPY get_http.c /
COPY get_http.h /
COPY run.c /
COPY run.h /
COPY oled.c /
COPY oled.c /
RUN gcc -c -O2 -s -DNDEBUG -Wall -I/usr/local/include fonts.c 
RUN gcc -c -O2 -s -DNDEBUG -Wall -I/usr/local/include oled96.c 
RUN gcc -c -O2 -s -DNDEBUG -Wall -I/usr/local/include get_http.c 
RUN gcc -c -O2 -s -DNDEBUG -Wall -I/usr/local/include run.c 
RUN gcc -c -O2 -s -DNDEBUG -Wall -I/usr/local/include oled.c 
RUN gcc oled.o run.o get_http.o oled96.o fonts.o -lm -lpthread -o /oled


FROM arm32v6/alpine:3.10

RUN apk add --update \
  jq \
  && rm -rf /var/cache/apk/*

COPY --from=0 /oled /oled

WORKDIR /
CMD /oled

