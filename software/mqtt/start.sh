#!/bin/bash
c=`echo $PKAPIURL|sed 's/\//\\\\\//g'`
d=`echo $VHOST|sed 's/\//\\\\\//g'`
sed -i s/FARMBOT_PUBLIC_KEY_URL/$c/g /etc/rabbitmq/rabbitmq.config
sed -i s/VHOST/$d/g /etc/rabbitmq/rabbitmq.config

count=1
while [ $count -le 500 ]
do
    http_key_api_response=`curl -o /dev/null -I -s -w "%{http_code}\n" $PKAPIURL`
    echo "HTTP:"$http_key_api_response

    if [ $http_key_api_response = "200" ]; then
        rabbitmq-server
    else
        echo "mqtt fail!"
    fi

    count=$((count + 1))
    sleep 2
done
