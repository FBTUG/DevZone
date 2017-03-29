#!/bin/bash
set -e
cd /opt/Farmbot-Web-API && \
   rake db:create && \
   rake db:schema:load && \
   rake db:seed && \
   rails server -p 3000 -b 0.0.0.0
