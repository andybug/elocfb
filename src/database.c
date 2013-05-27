
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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

static void psql_check_error(PGresult *res, const char *msg)
{
	if (PQresultStatus(res) != PGRES_COMMAND_OK) {
		fprintf(stderr, "%s: %s\n", msg, PQerrorMessage(psql));
		PQclear(res);
		db_disconnect();
		exit(EXIT_FAILURE);
	} else
		PQclear(res);
}

void db_add_teams(void)
{
	PGresult *res;
	int i;
	char buf[512];

	res = PQexec(psql, "BEGIN");
	psql_check_error(res, "SQL BEGIN failed");

	res = PQexec(psql, "DROP TABLE IF EXISTS teams");
	psql_check_error(res, "SQL DROP TABLE failed");

	res = PQexec(psql, teams_table_sql);
	psql_check_error(res, "SQL CREATE TABLE failed");

	for (i = 0; i < num_teams; i++) {
		snprintf(buf, 512, "INSERT INTO teams VALUES(%d, '%s', %f, %f, %d)",
				teams[i].key,
				teams[i].name,
				teams[i].winper,
				teams[i].rpi,
				teams[i].elo);

		res = PQexec(psql, buf);
		psql_check_error(res, "SQL INSERT failed");
	}

	res = PQexec(psql, "END");
	psql_check_error(res, "SQL END failed");
}
