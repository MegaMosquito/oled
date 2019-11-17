FROM arm32v6/alpine:3.10

RUN apk add --update \
  curl \
  wiringpi \
  wiringpi-dev \
  git \
  gcc \
  make \
  musl-dev \
  linux-headers \
  && rm -rf /var/cache/apk/*

COPY ssd1306_i2c.c /
COPY ssd1306_i2c.h /
COPY get_http.c /
COPY get_http.h /
COPY run.c /
COPY run.h /
COPY oled.c /
COPY oled.c /
RUN gcc -c -O2 -s -DNDEBUG -Wall -I/usr/local/include ssd1306_i2c.c 
RUN gcc -c -O2 -s -DNDEBUG -Wall -I/usr/local/include get_http.c 
RUN gcc -c -O2 -s -DNDEBUG -Wall -I/usr/local/include run.c 
RUN gcc -c -O2 -s -DNDEBUG -Wall -I/usr/local/include oled.c 
RUN gcc oled.o run.o get_http.o ssd1306_i2c.o -L/usr/local/lib -lwiringPi -lcrypt -lpthread -lm -lrt -o /oled


FROM arm32v6/alpine:3.10

RUN apk add --update \
  curl \
  wiringpi \
  && rm -rf /var/cache/apk/*

COPY --from=0 /oled /oled

WORKDIR /
CMD /oled
