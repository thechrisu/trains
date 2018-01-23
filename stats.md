# Performance stats
Runs of `usr/test/test_message_benchmark.c`

## 2018-01-23
Before any optimization was done on the kernel, this was the round-trip time for messages.

| Optimization | Average | Worst | Variance | Send before receive ? | Message size |
------------------------------------------------------------------------------------
| O0           | 866     | 874   | 0        | False                 | 4            |
| O0           | 896     | 897   | 0        | True                  | 4            |
| O0           | 1104    | 1111  | 0        | False                 | 64           |
| O0           | 1135    | 1409  | 0        | True                  | 64           |
| O2           | 416     | 418   | 0        | False                 | 4            |
| O2           | 428     | 431   | 0        | True                  | 4            | 
| O2           | 486     | 487   | 0        | False                 | 64           |
| O2           | 500     | 629   | 0        | True                  | 64           |

