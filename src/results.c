
#include "results.h"

struct result results[MAX_RESULTS];
int num_results = 0;

static void shift(struct result *a, int start, int end)
{
	int root = start;
	int child;
	int swap;
	struct result temp;

	while ((root*2) + 1 <= end) {
		child = (root*2) + 1;
		swap = root;

		if (a[swap].date < a[child].date)
			swap = child;

		if (child+1 <= end && a[swap].date < a[child+1].date)
			swap = child + 1;

		if (swap != root) {
			temp = a[root];
			a[root] = a[swap];
			a[swap] = temp;
		} else
			return;
	}
}

static void heapify(struct result *a, int count)
{
	int start = (count-1) / 2;

	while (start >= 0) {
		shift(a, start, count-1);
		start--;
	}
}

void sort_results(void)
{
	struct result temp;
	int end;

	heapify(results, num_results);

	end = num_results - 1;
	while (end > 0) {
		temp = results[end];
		results[end] = results[0];
		results[0] = temp;

		end--;
		shift(results, 0, end);
	}
}
