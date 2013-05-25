
#include <stdio.h>
#include <string.h>

#include <postgresql/libpq-fe.h>

#include "database.h"
#include "teams.h"
#include "results.h"

static PGconn *psql = NULL;

static const char *teams_table_sql =
	"CREATE TABLE teams ("
	"key integer,"
	"name varchar(128),"
	"winper float,"
	"rpi float,"
	"elo smallint)";

int db_connect(void)
{
	static const char *conninfo = "dbname=shinyavengerdb user=shinyavenger";

	psql = PQconnectdb(conninfo);
	if (PQstatus(psql) != CONNECTION_OK) {
		fprintf(stderr, "Failed to connect to the database: %s\n",
				PQerrorMessage(psql));
		db_disconnect();
		return -1;
	}

	return 0;
}

void db_disconnect(void)
{
	if (psql) {
		PQfinish(psql);
		psql = NULL;
	}
}

void db_add_teams(void)
{
	PGresult *res;
	int i;
	char buf[512];

	res = PQexec(psql, "BEGIN");
	if (PQresultStatus(res) != PGRES_COMMAND_OK) {
		fprintf(stderr, "SQL BEGIN failed: %s\n",
				PQerrorMessage(psql));
		PQclear(res);
		db_disconnect();
		return;
	}
	PQclear(res);

	res = PQexec(psql, "DROP TABLE IF EXISTS teams");
	PQclear(res);

	res = PQexec(psql, teams_table_sql);
	if (PQresultStatus(res) != PGRES_COMMAND_OK) {
		fprintf(stderr, "SQL CREATE TABLE failed: %s\n",
				PQerrorMessage(psql));
		PQclear(res);
		db_disconnect();
		return;
	}
	PQclear(res);

	for (i = 0; i < MAX_TEAMS; i++) {
		if (teams[i].key == 0)
			continue;

		sprintf(buf, "INSERT INTO teams VALUES(%d, '%s', %f, %f, %d)",
				teams[i].key,
				teams[i].name,
				teams[i].winper,
				teams[i].rpi,
				teams[i].elo);

		res = PQexec(psql, buf);
		if (PQresultStatus(res) != PGRES_COMMAND_OK) {
			fprintf(stderr, "SQL INSERT failed: %s\n",
					PQerrorMessage(psql));
			PQclear(res);
			db_disconnect();
			return;
		}
	}

	res = PQexec(psql, "END");
	if (PQresultStatus(res) != PGRES_COMMAND_OK) {
		fprintf(stderr, "SQL END failed: %s\n",
				PQerrorMessage(psql));
		PQclear(res);
		db_disconnect();
	}
}
