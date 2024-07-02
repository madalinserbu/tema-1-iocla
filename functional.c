#include "functional.h"
#include <limits.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
void for_each(void (*func)(void *), array_t list)
{
	char *ptr = (char *)list.data;
	for (int i = 0; i < list.len; ++i) {
		func(ptr);
		ptr += list.elem_size;
	}
}

array_t map(void (*func)(void *, void *), int new_list_elem_size, void (*new_list_destructor)(void *), array_t list)
{
	array_t new_list;
	new_list.elem_size = new_list_elem_size;
	new_list.len = list.len;
	new_list.destructor = new_list_destructor;
	new_list.data = malloc(new_list.elem_size * new_list.len);
	if (new_list.data == NULL) {
		fprintf(stderr, "Memory allocation failed\n");
		exit(EXIT_FAILURE);
	}
	char *old_ptr = (char *)list.data;
	char *new_ptr = (char *)new_list.data;
	for (int i = 0; i < list.len; ++i) {
		func(new_ptr, old_ptr);
		old_ptr += list.elem_size;
		new_ptr += new_list.elem_size;
	}

	if (list.destructor != NULL) {
		old_ptr = (char *)list.data;
		for (int i = 0; i < list.len; ++i) {
			list.destructor(old_ptr);
			old_ptr += list.elem_size;
		}
	}
	free(list.data);

	return new_list;
}

array_t filter(boolean (*func)(void *elem), array_t list)
{
	array_t new_list;
	int count = 0;
	for (int i = 0; i < list.len; ++i) {
		if (func((char *)list.data + i * list.elem_size))
			count++;
	}
	new_list.elem_size = list.elem_size;
	new_list.len = count;
	new_list.destructor = list.destructor;
	new_list.data = malloc(new_list.elem_size * new_list.len);
	if (new_list.data == NULL) {
		fprintf(stderr, "Memory allocation failed\n");
		exit(EXIT_FAILURE);
	}
	char *old_ptr = (char *)list.data;
	char *new_ptr = (char *)new_list.data;
	for (int i = 0; i < list.len; ++i) {
		if (func(old_ptr)) {
			memcpy(new_ptr, old_ptr, list.elem_size);
			new_ptr += list.elem_size;
		}
		old_ptr += list.elem_size;
	}

	if (list.destructor != NULL) {
		old_ptr = (char *)list.data;
		for (int i = 0; i < list.len; ++i) {
			if (!func(old_ptr))
				list.destructor(old_ptr);
			old_ptr += list.elem_size;
		}
	}
	free(list.data);

	return new_list;
}

void *reduce(void (*func)(void *, void *), void *acc, array_t list)
{
	char *ptr = (char *)list.data;
	for (int i = 0; i < list.len; ++i) {
		func(acc, ptr);
		ptr += list.elem_size;
	}
	// No memory leaks in this function

	return acc;
}

void for_each_multiple(void (*func)(void **), int varg_c, ...)
{
	va_list args;
	va_start(args, varg_c);

	int min_len = INT_MAX;
	array_t *lists = malloc(varg_c * sizeof(array_t));
	for (int i = 0; i < varg_c; i++) {
		lists[i] = va_arg(args, array_t);
		if (lists[i].len < min_len)
			min_len = lists[i].len;
	}

	for (int i = 0; i < min_len; i++) {
		void *elements[varg_c];
		for (int j = 0; j < varg_c; j++)
			elements[j] = lists[j].data + i * lists[j].elem_size;
		func(elements);
	}

	free(lists);
	va_end(args);
}

array_t map_multiple(void (*func)(void *, void **), int new_list_elem_size, void (*new_list_destructor)(void *), int varg_c, ...)
{
	va_list args;
	va_start(args, varg_c);

	int min_len = INT_MAX;
	array_t *lists = malloc(varg_c * sizeof(array_t));
	for (int i = 0; i < varg_c; i++) {
		lists[i] = va_arg(args, array_t);
		if (lists[i].len < min_len)
			min_len = lists[i].len;
	}

	array_t result;
	result.elem_size = new_list_elem_size;
	result.len = min_len;
	result.data = malloc(min_len * new_list_elem_size);
	result.destructor = new_list_destructor;

	for (int i = 0; i < min_len; i++) {
		void *elements[varg_c];
		for (int j = 0; j < varg_c; j++)
			elements[j] = lists[j].data + i * lists[j].elem_size;
		func(result.data + i * new_list_elem_size, elements);
	}

	for (int i = 0; i < varg_c; i++) {
		if (lists[i].destructor) {
			for (int j = 0; j < lists[i].len; j++) {
				void *element = lists[i].data + j * lists[i].elem_size;
				lists[i].destructor(element);
			}
		}
		free(lists[i].data);
	}

	free(lists);

	va_end(args);

	return result;
}

void *reduce_multiple(void (*func)(void *, void **), void *acc, int varg_c, ...)
{
	va_list args;
	va_start(args, varg_c);

	int min_len = INT_MAX;
	array_t *lists = malloc(varg_c * sizeof(array_t));
	for (int i = 0; i < varg_c; i++) {
		lists[i] = va_arg(args, array_t);
		if (lists[i].len < min_len)
			min_len = lists[i].len;
	}

	for (int i = 0; i < min_len; i++) {
		void *elements[varg_c];
		for (int j = 0; j < varg_c; j++) {
			elements[j] = lists[j].data + i * lists[j].elem_size;
		}
		func(acc, elements);
	}

	free(lists);
	va_end(args);

	return acc;
}