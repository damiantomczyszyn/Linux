m, char c, int offset, int length)
{
	int i;

	for (i = 0; i < offset; i++)
		seq_puts(m, " ");
	for (i = 0; i < length; i++)
		seq_printf(m, "%c", c);
	seq_puts(m, "\n");
}

static void snprint_time(char *buf, size_t bufsiz, s64 nr)
{
	s64 div;
	s32 rem;

	nr += 5; /* for display rounding */
	div = div_s64_rem(nr, 1000, &rem);
	snprintf(buf, bufsiz, "%lld.%02d", (long long)div, (int)rem/10);
}

static void seq_time(struct seq_file *m, s64 time)
{
	char num[15];

	snprint_time(num, sizeof(num), time);
	seq_printf(m, " %14s", num);
}

static void seq_lock_time(struct seq_file *m, struct lock_time *lt)
{
	seq_p