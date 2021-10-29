#include <bits/stdc++.h>

#define mp make_pair
#define fi first
#define se second
#define pb push_back
#define all(x) (x).begin(),(x).end()

using namespace std;

typedef long long LL;
typedef pair<int,int> PII;
typedef vector < int > VI;
typedef double D;

const int MN = 100005, inf = 1000000005, mod = 1000000007;
const LL INF = 1000000000000000005LL;

struct Node
{
	int prior, value, cnt, mini;
	bool rev;
	Node *l, *r;
	Node(int _value = 0)
	{
		prior = rand();
		value = _value;
		cnt = 1;
		mini = _value;
		rev = false;
		l = r = 0;
	}
};

int cnt(Node *it)
{
	return it ? it->cnt : 0;
}

int mini(Node *it)
{
	return it ? it->mini : inf;
}

void upd(Node *it)
{
	if(it)
	{
		it->cnt = cnt(it->l) + cnt(it->r) + 1;
		it->mini = min(it->value, min(mini(it->l), mini(it->r)));
	}
}

void push(Node *it)
{
	if(it && it->rev)
	{
		it->rev = false;
		swap(it->l, it->r);
		if(it->l)
			it->l->rev ^= true;
		if(it->r)
			it->r->rev ^= true;
	}
}

Node* merge(Node *left, Node *right)
{
	push(left), push(right);
	if(!left || !right)
		return left ? left : right;
	if(left->prior > right->prior)
	{
		left->r = merge(left->r, right);
		upd(left);
		return left;
	}
	else
	{
		right->l = merge(left, right->l);
		upd(right);
		return right;
	}
}

pair<Node*, Node*> split(Node* root, int key, int add = 0)
{
	if(!root)
		return {0, 0};
	push(root);
	int true_key = add + cnt(root->l);
	if(key <= true_key)
	{
		auto split_left = split(root->l, key, add);
		root->l = split_left.se;
		upd(root);
		return {split_left.fi, root};
	}
	else
	{
		auto split_right = split(root->r, key, add + 1 + cnt(root->l));
		root->r = split_right.fi;
		upd(root);
		return {root, split_right.se};
	}
}


Node* insert(Node *root, Node *elem, int pos)
{
	auto spl = split(root, pos);
	return merge(merge(spl.fi, elem), spl.se);
}

Node* erase(Node *root, int pos, int add = 0)
{
	if(!root)
		return 0;
	push(root);
	int true_key = add + cnt(root->l);
	if(true_key == pos)
	{
		root = merge(root->l, root->r);
		upd(root);
		return root;
	}
	if(true_key > pos)
		root->l = erase(root->l, pos, add);
	else
		root->r = erase(root->r, pos, add + 1 + cnt(root->l));
	upd(root);
	return root;
}

int find_mini(Node *root, int add = 0)
{
	if(!root)
		return -1;
	push(root);
	int true_key = add + cnt(root->l);
	if(root->mini == root->value)
		return true_key;
	if(mini(root->l) < mini(root->r))
		return find_mini(root->l, add);
	return find_mini(root->r, add + 1 + cnt(root->l));
}

Node* revers(Node *root, int l, int r)
{
	Node *t1, *t2, *t3;
	auto spl = split(root, l);
	t1 = spl.fi, t2 = spl.se;
	spl = split(t2, r - l + 1);
	t2 = spl.fi, t3 = spl.se;
	t2->rev ^= true;
	return merge(merge(t1, t2), t3);
}

void write(Node *root)
{
	if(!root)
		return;
	push(root);
	write(root->l);
	printf("%d ", root->value);
	write(root->r);
}

int main()
{
	int n, a;
	scanf("%d%d", &n, &a);
	Node *root = new Node(a);
	for(int i = 2; i <= n; ++i)
	{
		scanf("%d", &a);
		Node *elem = new Node(a);
		root = merge(root, elem);
	}
	for(int i = 1; i <= n; ++i)
	{
		int index = find_mini(root);
		printf("%d ", index + i);
		root = revers(root, 0, index);
		root = erase(root, 0);
	}
}

