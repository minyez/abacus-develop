void setupmpi(int argc,char **argv,int &nproc, int &myrank);
void divide_pools(const int &nproc, const int &myrank,int &nproc_in_pool, int &npool, int&mypool, int &rank_in_pool);