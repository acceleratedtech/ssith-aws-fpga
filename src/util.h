
int copyFile(char *buffer, const char *filename, size_t buffer_size);

void setEnableDebugLog(int enable);
int debugLog(const char * __restrict format, ...) __attribute__((__format__ (__printf__, 1, 2)));
