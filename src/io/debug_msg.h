
#ifndef __DEBUG_MSG_H_
#define __DEBUG_MSG_H_

#define DEBUG_MSG(FKT_NAME, MSG) \
    qDebug("%s l.%d: %s: %s", __FILE__, __LINE__, FKT_NAME, MSG); 

#endif
