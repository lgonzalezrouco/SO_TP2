#ifndef _TYPES_H_U_
#define _TYPES_H_U_

typedef enum status {
  INVALID_PID = -7,
  SAME_STATUS,
  INVALID_STATUS,
  SAME_PRIORITY,
  INVALID_PRIORITY,
  NOT_FOUND,
  INVALID_PROCESS,
  SUCCESS
} status;

#endif