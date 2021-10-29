#include <bits/stdc++.h>
using namespace std;

#define fi first
#define se second
#define pb push_back
#define ALL(v) (v).begin(), (v).end()
#define SZ(v) (int)((v).size())

typedef long long LL;
typedef long double LD;
typedef pair<int, int> PII;
typedef vector<int> VI;

const int MN = 200005, P2 = (1 << 18), MTS = 2 * P2 + 5;
const LL INF = (LL)1e18;

struct Node
{
	LL suma = 0, dod = 0, mini = INF;
};

Node tree[MTS];
LL A[MN];

void propagate(int v, int len)
{
	len >>= 1;
	int lewy = (v << 1), prawy = lewy + 1;
	tree[lewy].suma += tree[v].dod * len;
	tree[prawy].suma += tree[v].dod * len;
	tree[lewy].dod += tree[v].dod;
	tree[prawy].dod += tree[v].dod;
	tree[lewy].mini += tree[v].dod;
	tree[prawy].mini += tree[v].dod;
	tree[v].dod = 0;
}

void add(int l, int r, LL val, int v = 1, int a = 0, int b = P2 - 1)
{
	if(l == a && r == b)
	{
		tree[v].suma += val * (b - a + 1);
		tree[v].dod += val;
		tree[v].mini += val;
		return;
	}
	propagate(v, b - a + 1);
	int mid = (a + b) >> 1, lewy = (v << 1), prawy = lewy + 1;
	if(l <= mid)
		add(l, min(mid, r), val, lewy, a, mid);
	if(r > mid)
		add(max(mid + 1, l), r, val, prawy, mid + 1, b);
	tree[v].suma = tree[lewy].suma + tree[prawy].suma;
	tree[v].mini = min(tree[lewy].mini, tree[prawy].mini);
}

LL suma(int l, int r, int v = 1, int a = 0, int b = P2 - 1)
{
	if(l == a && r == b)
		return tree[v].suma;
	propagate(v, b - a + 1);
	int mid = (a + b) >> 1, lewy = (v << 1), prawy = lewy + 1;
	LL ret = 0LL;
	if(l <= mid)
		ret += suma(l, min(mid, r), lewy, a, mid);
	if(r > mid)
		ret += suma(max(mid + 1, l), r, prawy, mid + 1, b);
	return ret;
}

LL mini(int l, int r, int v = 1, int a = 0, int b = P2 - 1)
{
	if(l == a && r == b)
		return tree[v].mini;
	propagate(v, b - a + 1);
	int mid = (a + b) >> 1, lewy = (v << 1), prawy = lewy + 1;
	LL ret = INF;
	if(l <= mid)
		ret = min(ret, mini(l, min(mid, r), lewy, a, mid));
	if(r > mid)
		ret = min(ret, mini(max(mid + 1, l), r, prawy, mid + 1, b));
	return ret;
}

void build(int v, int a, int b)
{
	if(v >= P2)
	{
		if(v >= P2 + MN)
			return;
		tree[v].suma = A[v - P2];
		tree[v].mini = A[v - P2];
		return;
	}
	int mid = (a + b) >> 1, lewy = (v << 1), prawy = lewy + 1;
	build(lewy, a, mid);
	build(prawy, mid + 1, b);
	tree[v].suma = tree[lewy].suma + tree[prawy].suma;
	tree[v].mini = min(tree[lewy].mini, tree[prawy].mini);
}

int main()
{
	int n, q;
	scanf("%d%d", &n, &q);
	for(int i = 1; i <= n; ++i)
		scanf("%lld", &A[i]);
	build(1, 0, P2 - 1);
	for(int i = 1; i <= q; ++i)
	{
		int a, b;
		LL c;
		char typ[3];
		scanf("%s", typ);
		if(typ[0] == 'M')
		{
			scanf("%d%d", &a, &b);
			printf("%lld\n", mini(a, b));
		}
		if(typ[0] == 'P')
		{
			scanf("%d%d%lld", &a, &b, &c);
			add(a, b, c);
		}
		if(typ[0] == 'S')
		{
			scanf("%d%d", &a, &b);
			printf("%lld\n", suma(a, b));
		}
	}
}
