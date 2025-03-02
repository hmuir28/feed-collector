# COMMAND(WIP)

## Run the following

### Environment variables setup

#### Create a file called config.env

```
CASSANDRA_IP=127.0.0.1
```

### Build an executable

```
g++ -std=c++17 webapp.cpp -o rss_reader \
    -I/opt/homebrew/include -L/opt/homebrew/lib \
    -lcurl -lpugixml -lcassandra -arch arm64
```

### Run the executable

```
./rss_reader
```
