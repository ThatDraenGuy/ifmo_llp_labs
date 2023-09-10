//
// Created by draen on 06.09.23.
//

#ifndef LLP_LAB_INCLUDE_PUBLIC_UTIL_LINKED_LIST_H
#define LLP_LAB_INCLUDE_PUBLIC_UTIL_LINKED_LIST_H

struct linked_list;

void linked_list_destroy(struct linked_list *linked_list);
void linked_list_push_back(struct linked_list *linked_list, void *data);

#endif // LLP_LAB_INCLUDE_PUBLIC_UTIL_LINKED_LIST_H
