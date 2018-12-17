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
# define PATH "/sys/kernel/debug/fortytwo/"

bool	test_read_id(uint64_t read_size)
{
	int	    fd;
	static char buf[PAGE_SIZE];
	uint64_t    current_pos = 0;
	ssize_t	    ret;
	bool	    success = true;

	if (-1 == (fd = open(PATH "id", O_RDONLY)))
		return false;
	bzero(buf, sizeof(buf));
	while ((ret = read(fd, buf + current_pos, read_size)) > 0)
	{
		printf("ret = %ld, read: %s\n", ret, buf);
		current_pos += ret;
	}
	printf("Total string read: %s\n", buf);
	if (!(0 == memcmp(login, buf, sizeof(login) - 1)))
	{
		printf("Read buffer: %s, which differs from %s\n", buf, login);
		success = false;
	}
	if (strlen(login) != strlen(buf))
	{
		printf("The length of the read buffer differs from the length of %s\n", login);
		success = false;
	}
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

bool	test_write_id(char *buffer, uint64_t size, bool should_fail)
{
	int	    fd;
	uint64_t    current_pos;
	ssize_t	    ret;
	bool	    success = true;

	if (-1 == (fd = open(PATH "id", O_RDWR)))
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

void	test_jiffies(void)
{
	static char	buffer[65];
	uint64_t	i;
	int		fd;

	i = 0;
	while (i < 100)
	{
		if (-1 == (fd = open(PATH "jiffies", O_RDONLY)))
		{
			printf("Failed to open: " PATH "jiffies\n");
			abort();
		}
		int64_t ret = read(fd, buffer, 64);
		if (ret < 0)
			abort();
		usleep(10000);
		buffer[ret] = 0;
		printf("current_jiffies: %s\n", buffer);
		close(fd);
		i++;
	}
}

bool	test_write_foo(char *buffer, uint64_t size, bool should_fail)
{
	int	fd;
	ssize_t	ret;

	if (-1 == (fd = open(PATH "foo", O_WRONLY)))
		abort();
	ret = write(fd, buffer, size);
	if (should_fail && ret < 0)
		return (true);
	else if (!should_fail && ret < 0)
		return (false);
	if (ret != size)
	{
		printf("wrote %lld instead of %llu\n", ret, size);
	}
	close(fd);
	return true;
}

bool	test_read_foo(char *buffer, uint64_t size)
{
	static char	buf[4096];
	int		fd;
	ssize_t		ret;

	if (-1 == (fd = open(PATH "foo", O_RDONLY)))
		abort();
	ret = read(fd, buf, sizeof(buf));
	if (ret != size)
	{
		printf("read %lld instead of %llu\n", ret, size);
	}
	if (0 != memcmp(buffer, buf, ret))
		return false;
	close(fd);
	return true;
}

int	 main(void)
{
	uint64_t    current_write_size;
	char	    *buffer;

	assert(test_read_id(1));
	assert(test_read_id(2));
	assert(test_read_id(PAGE_SIZE));

	current_write_size = 1;
	while (current_write_size < PAGE_SIZE)
	{
		if (NULL == (buffer = get_random_buffer(current_write_size)))
			return EXIT_FAILURE;
		assert(test_write_id(buffer, current_write_size, true));
		free(buffer);
		current_write_size++;
	}
	assert(test_write_id(login, sizeof(login) - 1, false));
	test_jiffies();

	if (NULL == (buffer = get_random_buffer(8000)))
		return EXIT_FAILURE;
	assert(test_write_foo(buffer, 8000, true));
	assert(test_write_foo(buffer, 4096, false));
	assert(test_read_foo(buffer, 4096));
	return 0;
}
