
#include <stdio.h>

#include <postgresql/libpq-fe.h>

#include "database.h"
#include "teams.h"
#include "results.h"

static PGconn *psql = NULL;

int db_connect(void)
{
	static const char *conninfo = "dbname=shinyavengerdb user=shinyavenger";

	psql = PQconnectdb(conninfo);
	if (PQstatus(psql) != CONNECTION_OK) {
		fprintf(stderr, "Failed to connect to the database: %s\n",
				PQerrorMessage(psql));
		PQfinish(psql);
		return -1;
	}

	return 0;
}

void db_disconnect(void)
{
	PQfinish(psql);
	psql = NULL;
}
