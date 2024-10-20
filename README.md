# loggerDB

a simple, stupid "database" thats just a b+tree inspired linked list backed by your filesystem.

Only really works if:
- you are using an atomic filesystem
- datasets are identified using an integer
- identifiers are monotonically increasing

more specifically, this system is designed to be used with [littlefs](https://github.com/littlefs-project/littlefs) on a microcontroller


### License

loggerDB is licensed under MIT
