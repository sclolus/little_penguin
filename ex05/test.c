#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include <strings.h>
#include <string.h>
#include <errno.h>

static char *login = "sclolus";

# define PAGE_SIZE 1UL << 7UL

bool	test_read(uint64_t read_size)
{
	int	    fd;
	static char buf[PAGE_SIZE];
	uint64_t    current_pos = 0;
	ssize_t	    ret;
	bool	    success = true;

	if (-1 == (fd = open("/dev/fortytwo", O_RDWR)))
		return false;
	bzero(buf, sizeof(buf));
	while ((ret = read(fd, buf + current_pos, read_size)) > 0)
	{
		printf("ret = %ld, read: %s\n", ret, buf);
		current_pos += ret;
	}
	printf("Total string read: %s\n", buf);
	success &= (0 == memcmp(login, buf, sizeof(login) - 1));
	success &= (strlen(login) == strlen(buf));
	if (-1 == close(fd))
		return false;
	return success;
}

char	*get_random_buffer(uint64_t size)
{
	uint64_t    count;
	int	    fd;
	char	    *buffer;
	ssize_t	    ret;

	buffer = NULL;
	if (-1 == (fd = open("/dev/urandom", O_RDONLY)))
		goto err;
	if (NULL == (buffer = malloc(size)))
		goto err;
	count = 0;
	while (count < size)
	{
		if (-1 == (ret = read(fd, buffer + count, size - count)))
			goto err;
		count += (uint64_t)ret;
	}
	close(fd);
	return buffer;
err:
	printf("Error in get_random_buffer()\n");
	free(buffer);
	close(fd);
	return NULL;
}

bool	test_write(char	*buffer, uint64_t size, bool should_fail)
{
	int	    fd;
	uint64_t    current_pos;
	ssize_t	    ret;
	bool	    success = true;

	if (-1 == (fd = open("/dev/fortytwo", O_RDWR)))
		return false;
	ret = write(fd, buffer, size);
	if (ret == -1 && should_fail)
		success &= (errno == EINVAL);
	else if (should_fail == false && ret >= 0)
		     ;
	else
	{
		printf("Got a ret of %lld where it should have been -1, errno: %d, current_size: %llu\n", ret, errno, size);
		success &= false;
	}
	if (-1 == close(fd))
		return false;
	return success;
}

int	 main(void)
{
	uint64_t    current_write_size;
	char	    *buffer;

	assert(test_read(1));
	assert(test_read(2));
	assert(test_read(PAGE_SIZE));

	current_write_size = 1;
	while (current_write_size < PAGE_SIZE)
	{
		if (NULL == (buffer = get_random_buffer(current_write_size)))
			return EXIT_FAILURE;
		assert(test_write(buffer, current_write_size, true));
		free(buffer);
		current_write_size++;
	}
	assert(test_write(login, sizeof(login) - 1, false));
	return 0;
}
