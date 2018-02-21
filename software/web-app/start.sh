#!/bin/bash

cd /opt/Farmbot-Web-App
set -e
secret=`rake secret`

# for official, not working right now
# sed -i s/changeme-io/$HOSTIP/g /etc/nginx/sites-enabled/farmbot.conf

sed -i s/changeme-io/$HOSTIP/g config/application.yml

sed -i s/localhost/$HOSTIP/g app/models/transport.rb
sed -i s/localhost/$HOSTIP/g config/webpack.dev.js

sed -i s/rake-secret/$secret/g config/application.yml
sed -i s/pgsql-host/$PGHOST/g config/application.yml
sed -i s/pgsql-port/$PGPORT/g config/application.yml
sed -i s/pgsql-user/$PGUSER/g config/application.yml
sed -i s/pgsql-pass/$PGPASS/g config/application.yml
sed -i s/pgsql-dbname/$PGDBNAME/g config/application.yml
sed -i s/ws-mqtt/$HOSTIP:$WSPORT/g config/application.yml

sed -i s/pgsql-host/$PGHOST/g config/database.yml
sed -i s/pgsql-port/$PGPORT/g config/database.yml
sed -i s/pgsql-user/$PGUSER/g config/database.yml
sed -i s/pgsql-pass/$PGPASS/g config/database.yml
sed -i s/pgsql-dbname/$PGDBNAME/g config/database.yml

#avoid can't start server
if [ -f "/opt/Farmbot-Web-App/tmp/pids/server.pid" ]; then
    rm /opt/Farmbot-Web-App/tmp/pids/server.pid
fi

RAILS_ENV=development
#nohup /sbin/entrypoint.sh > /postgresql.log 2>&1 &

#start postgresql
/etc/init.d/postgresql restart
echo "Start Postgresql service..."

if [ ! -f "/init_finish" ]; then
   echo "init DB"
   sudo -u postgres psql -c "CREATE USER farmbot WITH PASSWORD 'farmbot1234'; ALTER USER farmbot WITH SUPERUSER;"
fi

if [ ! -f "/init_finish" ]; then
   rake db:create:all db:migrate db:seed
fi

echo "init" > /init_finish

rails api:start
