#include <bits/stdc++.h>
using namespace std;

typedef unsigned long long ull;
typedef long long ll;
typedef pair<int, int> pii;
typedef pair<ll, ll> pll;
typedef pair<double, double> pdd;
const ull mod = 1e9 + 7;
#define REP(i,n) for(int i=0;i<(int)n;++i)

//debug
#define dump(x)  cerr << #x << " = " << (x) << endl;
#define debug(x) cerr << #x << " = " << (x) << " (L" << __LINE__ << ")" << " " << __FILE__ << endl;

template < typename T >
void vprint(T &v){
	REP(i, v.size()){
		cout << v[i] << " ";
	}
	cout << endl;
}

struct edge{
	ll to, cap, cost, rev;
	edge(ll to, ll cap, ll cost, ll rev); to(to), cap(cap), cost(cost), rev(rev) {}
};

typedef vector<edge> edges;
typedef vector<edges> graph;
ll INF= LLONG_MAX/3;

void add_edge(graph &G, ll from, ll to, ll cap, ll cost){
	G[from].push_back(edge(to, cap, cost, G[to].size()));
	G[to].push_back(edge(from, 0, -cost, G[from].size()-1));
}

ll SSC(graph &G, vector<ll> &excess){
	ll V = G.size();
	map<ll, ll>  E, D;
	REP(i, V){
		if(excess[i]>0) E[i] = excess[i];
		if(excess[i]<0) D[i] = excess[i];
	}

	ll total_cost = 0;
	vector<ll> pot(V, 0);
	while(!E.empty()){
		ll s = E.begin()->first;
		priority_queue<pll, vector<pll>, greater<pll>> pq;
		vector<ll> dist(V, INF);
		dist[s] = 0;
		pq.push(P(0, s));

	}
}

int main(){
	ll V, E, F;
	cin >> V >> E >> F;
	graph G(V);
	REP(i, E){
		ll u, v, c, d;
		cin >> u >> v >> c >> d;
		add_edge(G, u, v, c, d);
	}
	vector<ll> excess(V, 0);
	excess[0] = F;
	excess[V-1] = -F;
	ll res = SSC(G, excess);
	cout << res << endl;
    return 0;
}