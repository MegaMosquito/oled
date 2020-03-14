
LOCAL_ROUTER_ADDRESS    := $(shell netstat -r | egrep default | head -1 | awk '{ print $$2 }')
LOCAL_IP_ADDRESS        := $(shell hostname -I | awk '{ print $$1 }')

all: build run

build:
	-docker build -t oled . 

dev:
	-docker rm -f oled 2>/dev/null || :
	docker run -it --privileged \
            --name oled --restart unless-stopped \
            -e LOCAL_ROUTER_ADDRESS=$(LOCAL_ROUTER_ADDRESS) \
            -e LOCAL_IP_ADDRESS=$(LOCAL_IP_ADDRESS) \
            --net=host -v /usr:/usr2 \
            -v `pwd`:/outside oled /bin/sh

run:
	-docker rm -f oled 2>/dev/null || :
	docker run -d --privileged \
            --name oled --restart unless-stopped \
            -e LOCAL_ROUTER_ADDRESS=$(LOCAL_ROUTER_ADDRESS) \
            -e LOCAL_IP_ADDRESS=$(LOCAL_IP_ADDRESS) \
            --net=host -v /usr:/usr2 \
            oled

IMAGE := ibmosquito/oled:1.0.0
publish:
	docker build -t $(IMAGE) .
	docker push $(IMAGE)
