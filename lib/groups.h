/**
 * @file
 * @brief Train groups.
 */
#ifndef TRAIN_GROUPS_H
#define TRAIN_GROUPS_H

#include "tstdlib.h"

#define MAX_GROUP_MEMBERS 6
#define MAX_GROUP_NAME_LEN 4
#define MAX_GROUPS 6

/**
 * The idea is to address train groups via their name.
 * This way, we can just keep the groups contiguously
 * in the first <code>num_groups</code> elements of the tr_groups array,
 * as opposed to having to use a bool-map if we used the array index as
 * the group identifier.
 */
typedef struct {
  int members[MAX_GROUP_MEMBERS];
  int num_members;
} train_group;

typedef struct {
  train_group g;
  char group_name[MAX_GROUP_NAME_LEN];
  int tid;
} train_group_info;

#define NULL_TRAIN_GROUP_INFO (train_group_info *)0

extern train_group_info tr_groups[6];
extern int num_groups;

/**
 * @param   name The name of a group.
 * @returns A pointer to the group.
 */
train_group_info *find_group(char name[MAX_GROUP_NAME_LEN]);

#endif /* TRAIN_GROUPS_H */
