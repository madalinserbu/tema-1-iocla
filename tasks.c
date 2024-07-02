#include "tasks.h"
#include "functional.h"
#include "tests.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void prepend_to_list(void *acc, void *elem)
{
	array_t *reversed_list = (array_t *)acc;
	void *new_data = malloc(reversed_list->elem_size * (reversed_list->len + 1));
	memcpy(new_data, elem, reversed_list->elem_size);
	memcpy((char *)new_data + reversed_list->elem_size, reversed_list->data, reversed_list->elem_size * reversed_list->len);
	free(reversed_list->data);
	reversed_list->data = new_data;
	reversed_list->len++;
}

array_t reverse(array_t list)
{
	array_t reversed_list;
	reversed_list.elem_size = list.elem_size;
	reversed_list.len = 0;
	reversed_list.data = malloc(0); // empty list
	reversed_list.destructor = list.destructor;

	reduce(prepend_to_list, &reversed_list, list);

	return reversed_list;
}

boolean is_passing(void *elem)
{
	student_t *student = (student_t *)elem;
	return student->grade >= 5.0;
}

void get_name(void *new_elem, void *old_elem)
{
	student_t *old_student = (student_t *)old_elem;
	char **new_name = (char **)new_elem;
	*new_name = strdup(old_student->name);
}

array_t get_passing_students_names(array_t list)
{
	array_t passing_students = filter(is_passing, list);
	array_t passing_students_names = map(get_name, sizeof(char *), free, passing_students);
	return passing_students_names;
}

array_t check_bigger_sum(array_t list_list, array_t int_list)
{
	(void)list_list;
	(void)int_list;
	return (array_t){0};
}

array_t get_even_indexed_strings(array_t list)
{
	(void)list;
	return (array_t){0};
}

array_t generate_square_matrix(int n)
{
	(void)n;
	return (array_t){0};
}

array_t create_number_array(array_t integer_part, array_t fractional_part)
{
	(void)integer_part;
	(void)fractional_part;
	return (array_t){0};
}
