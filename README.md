# loggerDB

a simple. stupid, time-series database designed specifically for use with [littlefs](https://github.com/littlefs-project/littlefs).

loggerDB aims to achieve the following:
- fast writes
- fast lookups
- support for metadata
- be able to read and write at the same time (not to the same file)

with littlefs these were implemented as follows:
- data is stored in "nodes" which describes a hierarchical folder structure according to the timestamp (`YYYY/MM/DD/HH/MM`)
- querying a node according to a timestamp gives a handle that allows storing data in named fields
- fields are capable of storing arbitrary data and may be read, written or appended to depending on the use case
- data metadata is stored in a field within a node with a size constraint so it roughly fits inline into a metadata pair

### License

loggerDB is licensed under MIT
