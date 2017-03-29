#### 2017-03-29T09:00:07+0800
```
$ docker-compose build
$ docker-compose up -d
Creating farmbotwebapi_postgres_1
Creating farmbotwebapi_fapi_1

$ docker-compose ps
          Name                        Command              State    Ports
---------------------------------------------------------------------------
farmbotwebapi_fapi_1       tail -f /dev/null               Up
farmbotwebapi_postgres_1   docker-entrypoint.sh postgres   Up      5432/tcp

$ docker-compose exec fapi bash
$ bash-4.3# gosu postgres psql -h postgres -U postgres
Password for user postgres:
mysecretpassword

postgres=# \pset format aligned
Output format is aligned.
postgres=# \list
                                      List of databases
        Name         |  Owner   | Encoding |  Collate   |   Ctype    |   Access privileges
---------------------+----------+----------+------------+------------+-----------------------
 farmbot_development | postgres | UTF8     | en_US.utf8 | en_US.utf8 |
 postgres            | postgres | UTF8     | en_US.utf8 | en_US.utf8 |
 template0           | postgres | UTF8     | en_US.utf8 | en_US.utf8 | =c/postgres          +
                     |          |          |            |            | postgres=CTc/postgres
 template1           | postgres | UTF8     | en_US.utf8 | en_US.utf8 | =c/postgres          +
                     |          |          |            |            | postgres=CTc/postgres
(4 rows)
\q

# curl http://localhost:3000
<!DOCTYPE html>
<html lang="en">

<head>
  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width,initial-scale=1">
  <meta content="width=device-width, initial-scale=1" name="viewport">
  <title>FarmBot</title>
  <link rel="stylesheet" href="https://maxcdn.bootstrapcdn.com/bootstrap/3.3.5/css/bootstrap.min.css">
  <link rel="stylesheet" href="https://maxcdn.bootstrapcdn.com/font-awesome/4.4.0/css/font-awesome.min.css">
  <link rel="stylesheet" href="/dist/styles.css">

</head>

<body>
  <script type="text/javascript" src="/dist/front_page.b24d612e42e4f4cbe164.js"></script>
</body>

</html>
// email: "notos@notos.com",
// password: "password123",
// WebSocket connection to 'ws://0.0.0.0:3002/' failed: Error in connection establishment:

```

#### 2017-03-21T11:40:19+0800
```
$ docker-compose build
$ docker-compose up -d
$ docker-compose exec fapi bash
bash-4.3# ruby --version
ruby 2.3.3p222 (2016-11-21 revision 56859) [x86_64-linux]
bash-4.3# postgres --version
postgres (PostgreSQL) 9.5.6
bash-4.3# gosu postgres psql -h postgres -U postgres
Password for user postgres:
psql (9.5.6, server 9.6.2)
WARNING: psql major version 9.5, server major version 9.6.
         Some psql features might not work.
Type "help" for help.

postgres=# select 1;
 ?column?
----------
        1
(1 row)
\q

bash-4.3# rake db:create && rake db:schema:load && rake db:seed
```

#### postgres image
* library/postgres - Docker Hub https://hub.docker.com/_/postgres/
```
$ docker-compose up -d
$ docker-compose exec postgres bash
root@9db42f07b1ca:/# gosu postgres psql
postgres=# select 1;
 ?column?
----------
        1
(1 row)

postgres=#
\q
could not save history to file "/home/postgres/.psql_history": No such file or directory
root@9db42f07b1ca:/# exit
```
