
# Some bits from https://github.com/MegaMosquito/netstuff/blob/master/Makefile
LOCAL_DEFAULT_ROUTE     := $(shell sh -c "ip route | grep default")
LOCAL_ROUTER_ADDRESS    := $(word 3, $(LOCAL_DEFAULT_ROUTE))
LOCAL_IP_ADDRESS        := $(word 7, $(LOCAL_DEFAULT_ROUTE))

build:
	-docker build -t oled . 

dev:
	-docker rm -f oled 2>/dev/null || :
	docker run -it --privileged \
            --name oled --restart unless-stopped \
            -e LOCAL_ROUTER_ADDRESS=$(LOCAL_ROUTER_ADDRESS) \
            -e LOCAL_IP_ADDRESS=$(LOCAL_IP_ADDRESS) \
            -v `pwd`:/outside oled /bin/sh

run:
	-docker rm -f oled 2>/dev/null || :
	docker run -d --privileged \
            --name oled --restart unless-stopped \
            -e LOCAL_ROUTER_ADDRESS=$(LOCAL_ROUTER_ADDRESS) \
            -e LOCAL_IP_ADDRESS=$(LOCAL_IP_ADDRESS) \
            oled


