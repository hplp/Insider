#ifndef CONST_H_
#define CONST_H_

#define FILE_ROW_NUM (268435456) // 2147483648 = 2^31    268435456
#define FILE_COL_NUM (17)

#define READ_BUF_LINES (512)
#define READ_BUF_SIZE (READ_BUF_LINES * FILE_COL_NUM)
#define READ_BUF_ITERATIONS (FILE_ROW_NUM / READ_BUF_LINES)
#define MAX_AES_Nr_PARAM_SIZE (1024)

#endif