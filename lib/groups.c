#include "groups.h"

train_group_info tr_groups[6];
int num_groups;

train_group_info *find_group(char name[MAX_GROUP_NAME_LEN]) {
  for (int i = 0; i < num_groups; i++) {
    train_group_info *group = &tr_groups[i];
    if (tstrcmp(name, group->group_name)) {
      return group;
    }
  }

  return NULL_TRAIN_GROUP_INFO;
}
