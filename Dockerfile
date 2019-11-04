FROM arm32v6/alpine:3.10

RUN apk add --update \
  gcc \
  musl-dev \
  linux-headers \
  python3 \
  python3-dev \
  py3-pip \
  py3-pillow \
  && pip3 install adafruit-circuitpython-ssd1306 \
  && pip3 install RPi.GPIO \
  && apk add curl \
  && apk del \
    gcc \
    musl-dev \
    linux-headers \
  && rm -rf /var/cache/apk/*

COPY oled.py /
WORKDIR /
CMD python3 oled.py

