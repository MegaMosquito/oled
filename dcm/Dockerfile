FROM arm32v6/alpine:3.10

RUN apk add --update \
  gcc \
  musl-dev \
  dateutils \
  linux-headers \
  && rm -rf /var/cache/apk/*

COPY fonts.c /
RUN gcc -c -O2 -s -DNDEBUG -Wall -I/usr/local/include fonts.c 

COPY oled96.c /
COPY oled96.h /
RUN gcc -c -O2 -s -DNDEBUG -Wall -I/usr/local/include oled96.c 

COPY oled.c /
RUN gcc -c -O2 -s -DNDEBUG -Wall -I/usr/local/include oled.c 

RUN gcc oled.o oled96.o fonts.o -lm -lpthread -o /oled

FROM arm32v6/alpine:3.10

RUN apk add --update \
  jq \
  && rm -rf /var/cache/apk/*

COPY --from=0 /oled /oled

WORKDIR /
CMD /oled

