#include <bits/stdc++.h>
#define fi first
#define se second
using namespace std;

typedef pair<int, int> PII;

const int M = 8;

int DX[] = {0, 0, 1, 1, 1, -1, -1, -1};
int DY[] = {-1, 1, 0, -1, 1, 0, -1, 1};
	
double glos[2][200][M][M];

struct MCTS;

struct board
{
	int jest[M][M];
	
	int player, licznik, o, o2, ja;
	
	vector < PII > history;
	
	board(int _ja = 1)
	{
		ja = _ja;
		for(int i = 0; i < M; ++i)
			for(int j = 0; j < M; ++j)
				jest[i][j] = 2;
		jest[3][3] = jest[4][4] = 1;
		jest[4][3] = jest[3][4] = 0;
		player = 0;
		licznik = 0;
	}
	
	
	bool end()
	{
		return (licznik >= 2 && o == o2 && o == -1);
	}
	
	bool inside(int x, int y)
	{
		return (min(x, y) >= 0 && max(x, y) < M);
	}
	
	bool canBeat(int x, int y, int dir)
	{
		int ok = 0, opp = player ^ 1;
		while(1)
		{
			x += DX[dir];
			y += DY[dir];
			if(!inside(x, y) || jest[x][y] == 2)
				return false;
			else if(jest[x][y] == opp)
				ok = 1;
			else if(jest[x][y] == player)
				break;
		}
		return ok;
	}
	
	void doBeat(int x, int y, int dir)
	{
		int opp = player ^ 1;
		int dx = x, dy = y, ix = x, iy = y;
		while(1)
		{
			x += DX[dir];
			y += DY[dir];
			if(!inside(x, y) || jest[x][y] == 2)
				break;
			else if(jest[x][y] == player)
			{
				dx = x;
				dy = y;
			}
		}
		x = ix, y = iy;
		while(x != dx || y != dy)
		{
			x += DX[dir];
			y += DY[dir];
			if(jest[x][y] == opp)
				jest[x][y] = player;
		}
	}
	
	bool isMoveValid(int x, int y)
	{
		for(int i = 0; i < 8; ++i)
			if(canBeat(x, y, i))
				return true;
		return false;
	}
	
	void Move(int x, int y)
	{
		if(x != -1)
		{
			jest[x][y] = player;
			for(int i = 0; i < 8; ++i)
				doBeat(x, y, i);
		}
		player ^= 1;
		++licznik;
		o2 = o, o = x;
		history.push_back({x, y});
	}
	
	vector < PII > Moves()
	{
		vector < PII > moves;
		for(int i = 0; i < M; ++i)
			for(int j = 0; j < M; ++j)
				if(jest[i][j] == 2 && isMoveValid(i, j))
					moves.push_back({i, j});
		if(moves.size() == 0)
			moves.push_back({-1, -1});
		return moves;
	}
	
	void randomMove()
	{
		auto aval = Moves();
		int s = aval.size();
		PII ret = aval[rand() % s];
		Move(ret.fi, ret.se);
	}
	
	int wynik()
	{
		int suma = 0;
		for(int i = 0; i < M; ++i)
			for(int j = 0; j < M; ++j)
				if(jest[i][j] == 0)
					--suma;
				else if(jest[i][j] == 1)
					++suma;
		return suma;
	}
	
	int simulate()
	{
		while(1)
		{
			if(end())
				break;
			randomMove();
			if(end())
				break;
			randomMove();
		}
		int wyn = wynik();
		if(wyn <= 0)
			return 0;
		return 1;
	}
	
	
};

const double C = 2.137;

int cmp_time, cmp_player;

bool cmp(PII a, PII b)
{
	return (glos[cmp_player][cmp_time][a.fi][a.se] > glos[cmp_player][cmp_time][b.fi][b.se]);
}

struct MCTS
{
	PII mv;
	vector < MCTS* > sons;
	int wins, sims, S;
	
	void init(PII _mv)
	{
		mv = _mv;
		wins = sims = 0;
	}
	
	void genSons(board* B)
	{
		auto av = (*B).Moves();
		cmp_time = B->licznik + 1;
		cmp_player = B->player;
		sort(av.begin(), av.end(), cmp);
		S = av.size();
		sons.resize(S);
		for(int i = 0; i < S; ++i)
		{
			sons[i] = new MCTS;
			sons[i]->init(av[i]);
		}
		S = sons.size();
	}
	
	int sim(board* B, int player)
	{
		int ret = (*B).simulate();
		++sims;
		if(ret == player)
			++wins;
		return ret;
	}
	
	int dfs(board *B, int player)
	{
		if((*B).end())
		{
			++sims;
			int res = ((*B).wynik() > 0);
			if(res == player)
				++wins;
			return res;
				
		}
		int s;
		if(sons.size() == 0)
		{
			//we're in leaf, but still playing
			genSons(B);
			//random_shuffle(sons.begin(), sons.end());
			(*B).Move(sons[0]->mv.fi, sons[0]->mv.se);
			int ret = sons[0]->sim(B, player ^ 1);
			++sims;
			if(ret == player)
				++wins;
			return ret;
		}
		double bc = 0.0;
		s = 0;
		for(int i = 0; i < S; ++i)
		{
			if(sons[i]->sims == 0)
			{
				s = i;
				break;
			}
			double magic = (double)((double)(sons[i]->sims - sons[i]->wins) / (double)sons[i]->sims) + C * sqrt((double)(log((double)(sims))/(double)sons[i]->sims));
			if(magic > bc)
			{
				bc = magic;
				s = i;
			}
		}
		(*B).Move(sons[s]->mv.fi, sons[s]->mv.se);
		int ret = sons[s]->dfs(B, player ^ 1);
		++sims;
		if(ret == player)
			++wins;
		return ret;	
	}
	int find(PII m)
	{
		for(int i = 0; i < S; ++i)
			if(sons[i]->mv == m)
				return i;
		assert(0);
	}
};

MCTS mcts;

int czas;

void smartMove(board *B)
{
	int N = 200;
	MCTS* root = &mcts;
	for(auto m : (*B).history)
		root = root->sons[root->find(m)];
	while(N--)
	{
		board C = (*B);
		root->dfs(&C, C.player);
	}
	int idx = 0, naj = 0, S = root->S;
	for(int i = 0; i < S; ++i)
		if(root->sons[i]->sims > naj)
		{
			idx = i;
			naj = root->sons[i]->sims;
		}
	(*B).Move(root->sons[idx]->mv.fi, root->sons[idx]->mv.se);
}

int graj(int moj)
{
	board b(moj);
	if(moj == 0)
	{
		smartMove(&b);
	}
	while(1)
	{
		b.randomMove();
		if(b.end())
			break;
		smartMove(&b);
		if(b.end())
			break;
	}
	int wyn = b.wynik();
	if(moj == 0 && wyn > 0) return 1;
	if(moj == 1 && wyn < 0) return 1;
	return 0;
}

int main()
{
	int *seed = new int;
	srand((long long)seed);
	
	for(int i = 0; i < 2; ++i)
		for(int j = 0; j < 200; ++j)
			for(int k = 0; k < M; ++k)
				for(int l = 0; l < M; ++l)
					scanf("%lf", &glos[i][j][k][l]);
	
	mcts.init({0, 0});
	int gry = 200, tot = 200, por = 0;
	while(gry)
	{
		por += graj(gry & 1);
		gry -= 1;
		++czas;
		if(gry % 10 == 0)
			printf("%d games, %d lost, win ratio %.8lf\n", tot - gry, por, (double)(tot - gry - por)/(double)(tot - gry));
	}
	printf("Per %d games %d lost, win ratio %.8lf\n", tot, por, (double)(tot - por)/(double)tot);
}
