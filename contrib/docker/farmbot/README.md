### Prerequisites

* docker
* docker-compose

### Usage

* docker host IP address (example): 192.168.2.73
* farmbot web url: http://192.168.2.73:3000
* account admin email: notos@notos.com
* account admin password: password123

```
$ git clone --depth=1 https://github.com/FBTUG/DevZone.git
$ cd DevZone/contrib/docker/frambot

// Find and replace APT_HOST/MQTT_HOST with the IP address of the docker host in the docker-compose.yml

$ docker-compose build
$ docker-copmose up -d
$ docker-compose ps
$ docker-compose logs mqtt
...
mqtt_1      | === Incoming Message ===
mqtt_1      | bot/device_1/from_clients
mqtt_1      | {
mqtt_1      |   "kind": "rpc_request",
mqtt_1      |   "args": {
mqtt_1      |     "label": "08ac1430-44c6-4c1a-b506-dac4a0271298"
mqtt_1      |   },
mqtt_1      |   "body": [
mqtt_1      |     {
mqtt_1      |       "kind": "move_relative",
mqtt_1      |       "args": {
mqtt_1      |         "x": 100,
mqtt_1      |         "y": 0,
mqtt_1      |         "z": 0,
mqtt_1      |         "speed": 800
mqtt_1      |       }
mqtt_1      |     }
mqtt_1      |   ]
mqtt_1      | }

$ docker-compose logs fapi
...
fapi_1      |    (0.2ms)  SELECT "tool_bays"."id" FROM "tool_bays" WHERE "tool_bays"."device_id" = $1  [["device_id", 1]]
fapi_1      |   ToolSlot Load (0.3ms)  SELECT "tool_slots".* FROM "tool_slots" WHERE "tool_slots"."tool_bay_id" = 2
fapi_1      | Completed 200 OK in 14ms (Views: 1.0ms | ActiveRecord: 3.3ms)

```

### References
* FarmBot/Farmbot-Web-API: Setup, customize, and control FarmBot from any device https://github.com/FarmBot/Farmbot-Web-API
* FBTUG-FarmBot 軟體設計 - https://hadad.hackpad.com/FBTUG-FarmBot--2MokQLnci2Q
* FarmBot/mqtt-gateway: Realtime messaging server for Farmbot (MQTT broker). https://github.com/FarmBot/mqtt-gateway
