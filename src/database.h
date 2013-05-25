#ifndef DATABASE_H
#define DATABASE_H

extern int db_connect(void);
extern void db_disconnect(void);
extern void db_add_teams(void);
extern void db_add_results(void);

#endif
