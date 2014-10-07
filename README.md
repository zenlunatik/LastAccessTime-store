Sessions typically have a Last Access Time field which is updated at a high frequency.
It may not make sense to push these updates to a back end db that handles data with more
consistency guarantees and higher latency

LATStore is a memcached based last access time store. The session id can be used as a key.
