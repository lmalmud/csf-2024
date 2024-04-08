#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

int compare_i64(const void *left_, const void *right_) {
  int64_t left = *(int64_t *)left_;
  int64_t right = *(int64_t *)right_;
  if (left < right) return -1;
  if (left > right) return 1;
  return 0;
}

void seq_sort(int64_t *arr, size_t begin, size_t end) {
  size_t num_elements = end - begin;
	// printf("base: %lu \t nmemb: %zu\n", *(arr + begin), num_elements);
  qsort(arr + begin, num_elements, sizeof(int64_t), compare_i64);
}

// Merge the elements in the sorted ranges [begin, mid) and [mid, end),
// copying the result into temparr.
void merge(int64_t *arr, size_t begin, size_t mid, size_t end, int64_t *temparr) {
  int64_t *endl = arr + mid;
  int64_t *endr = arr + end;
  int64_t *left = arr + begin, *right = arr + mid, *dst = temparr;

  for (;;) {
    int at_end_l = left >= endl;
    int at_end_r = right >= endr;

    if (at_end_l && at_end_r) break;

    if (at_end_l)
      *dst++ = *right++;
    else if (at_end_r)
      *dst++ = *left++;
    else {
      int cmp = compare_i64(left, right);
      if (cmp <= 0)
        *dst++ = *left++;
      else
        *dst++ = *right++;
    }
  }
}

void fatal(const char *msg) __attribute__ ((noreturn));

void fatal(const char *msg) {
  fprintf(stderr, "Error: %s\n", msg);
  exit(1);
}

void merge_sort(int64_t *arr, size_t begin, size_t end, size_t threshold) {
  assert(end >= begin);
  size_t size = end - begin;

  if (size <= threshold) {
    seq_sort(arr, begin, end);
    return;
  }

  // recursively sort halves in parallel

  size_t mid = begin + size/2;
	// printf("fork!\n");

  // TODO: parallelize the recursive sorting
  pid_t left_child = fork();
  pid_t right_child;
  if (left_child == -1) {
    fatal("failure to create left child process");
  }
  if (left_child == 0) {
    merge_sort(arr, begin, mid, threshold);
    exit(0);
  } else {
    right_child = fork();
    if (right_child == -1) {
      fatal("failure to create right child process");
    }
    if (right_child == 0) {
      merge_sort(arr, mid, end, threshold); // sort second half in parent process
      exit(0);
    }
  }
  int wstatus_left; // blocks until the process indentified by pid_to_wait_for completes
  pid_t actual_pid_left = waitpid(left_child, &wstatus_left, 0);
  int wstatus_right;
  pid_t actual_pid_right = waitpid(right_child, &wstatus_right, 0);
    if (actual_pid_left == -1) {
    fatal("unable to wait for left child process");
  }
  if (!WIFEXITED(wstatus_left)) { // subprocess crashed, was interrupted, or did not exit normally
		// printf("size: %zu\n", size);
    fatal("left child did not exit normally");
  }
  if (WEXITSTATUS(wstatus_left) != 0) { // subprocess returned a non-zero exit code
      fatal("left child returned non-zero exit code");
  }
    if (actual_pid_right == -1) {
    fatal("unable to wait for right child process");
  }
  if (!WIFEXITED(wstatus_right)) { // subprocess crashed, was interrupted, or did not exit normally
    fatal("right child did not exit normally");
  }
  if (WEXITSTATUS(wstatus_right) != 0) { // subprocess returned a non-zero exit code
      // printf("size: %zu\n", size);
			fatal("right child returned non-zero exit code");
  }

  /*
  pid_t pid_left = fork();
  if (pid_left == -1) {
    fatal("failure to create left child process");
  } else if (pid_left == 0) {
    merge_sort(arr, begin, mid, threshold); // sort first half in child process
		// printf("returned size: %zu\n", size);
    exit(0); // child process ends
  }

  int wstatus_left; // blocks until the process indentified by pid_to_wait_for completes
  pid_t actual_pid_left = waitpid(pid_left, &wstatus_left, 0);
  if (actual_pid_left == -1) {
    fatal("unable to wait for left child process");
  }
  if (!WIFEXITED(wstatus_left)) { // subprocess crashed, was interrupted, or did not exit normally
		// printf("size: %zu\n", size);
    fatal("left child did not exit normally");
  }
  if (WEXITSTATUS(wstatus_left) != 0) { // subprocess returned a non-zero exit code
      fatal("left child returned non-zero exit code");
  }
  
	pid_t pid_right = fork();
  if (pid_right == -1) {
    fatal("failure to create right child process");
  } else if (pid_right == 0) {
    merge_sort(arr, mid, end, threshold); // sort second half in parent process
    exit(0);
  }
  int wstatus_right;
  pid_t actual_pid_right = waitpid(pid_right, &wstatus_right, 0);
  if (actual_pid_right == -1) {
    fatal("unable to wait for right child process");
  }
  if (!WIFEXITED(wstatus_right)) { // subprocess crashed, was interrupted, or did not exit normally
    fatal("right child did not exit normally");
  }
  if (WEXITSTATUS(wstatus_right) != 0) { // subprocess returned a non-zero exit code
      // printf("size: %zu\n", size);
			fatal("right child returned non-zero exit code");
  }
  */

  // allocate temp array now, so we can avoid unnecessary work
  // if the malloc fails
  int64_t *temp_arr = (int64_t *) malloc(size * sizeof(int64_t));
  if (temp_arr == NULL)
    fatal("malloc() failed");

  // child processes completed successfully, so in theory
  // we should be able to merge their results
  merge(arr, begin, mid, end, temp_arr);
	// printf("arrays merged: %zu\t%zu\n", begin, end);

  // copy data back to main array
  for (size_t i = 0; i < size; i++)
    arr[begin + i] = temp_arr[i];

  // now we can free the temp array
  free(temp_arr);

  // success!
}

int main(int argc, char **argv) {
  // check for correct number of command line arguments
  if (argc != 3) {
    fprintf(stderr, "Usage: %s <filename> <sequential threshold>\n", argv[0]);
    return 1;
  }

  // process command line arguments
  const char *filename = argv[1];
  char *end;
  size_t threshold = (size_t) strtoul(argv[2], &end, 10);
  if (end != argv[2] + strlen(argv[2])) {
    fatal("threshold value is invalid");
    // TODO: report an error (threshold value is invalid) - DONE
  }

  // TODO: open the file - DONE
  int fd = open(filename, O_RDWR);
  if (fd < 0) { // file was not sucessfully opened
    fatal("file could not be opened");
  }

  // TODO: use fstat to determine the size of the file - DONE
  struct stat statbuf;
  int rc = fstat(fd, &statbuf); // gets file status
  if (rc != 0) {
		close(fd);
    fatal("could not get file status");
  }
  size_t file_size_in_bytes = statbuf.st_size;

  // TODO: map the file into memory using mmap
  int64_t *data = mmap(NULL, file_size_in_bytes, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	int close_result = close(fd); // immediately close file descriptor, since mmap has a separate reference
  if (close_result < 0) {
    fatal("could not close file");
  }
  if (data == MAP_FAILED) { // handle mmap error and exit
    fatal("could not map error");
  }
  // *data now behaves like a standard array of int64_t

  // TODO: sort the data! - DONE
  merge_sort(data, 0, file_size_in_bytes/8, threshold);

  // TODO: unmap and close the file - DONE
  int unmap_result = munmap(data, file_size_in_bytes);
  if (unmap_result < 0) {
    fatal("could not unmap memory");
  }

  // TODO: exit with a 0 exit code if sort was successful - DONE
  exit(0);
}
