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
