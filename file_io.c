#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>

void read_file(const char *path, void *data, size_t size) {
#ifdef __DJGPP__
	int fd = open( path, O_RDONLY | O_BINARY );
#else
	int fd = open( path, O_RDONLY );
#endif

	int nr = read( fd, data, size );
	close( fd );//TODO: error checking
}

void write_file(const char *path, const void *data, size_t size) {
#ifdef __DJGPP__
	int fd = open( path, O_WRONLY | O_CREAT | O_BINARY, 0664);
#else
	int fd = open( path, O_WRONLY | O_CREAT, 0664);
#endif
	int nr = write( fd, data, size );
	close( fd );//TODO: error checking
}
