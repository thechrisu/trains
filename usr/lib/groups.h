/**
 * @file
 * @brief Train groups.
 */
#ifndef TRAIN_GROUPS_H
#define TRAIN_GROUPS_H

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
  char group_name[MAX_GROUP_NAME_LEN];
} train_group;

train_group tr_groups[6];
int num_groups;

#endif /* TRAIN_GROUPS_H */
