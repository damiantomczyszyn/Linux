(int offset, int next, int bucket, int size)
{
	chain_hlocks[offset] = (next >> 16) | CHAIN_BLK_FLAG;
	chain_hlocks[offset + 1] = (u16)next;

	if (size && !bucket) {
		chain_hlocks[off