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
	edge(ll to, ll cap, ll cost, ll rev): to(to), cap(cap), cost(cost), rev(rev) {}
};

typedef vector<edge> edges;
typedef vector<edges> graph;
ll INF= LLONG_MAX/3;

void add_edge(graph &G, ll from, ll to, ll cap, ll cost){
	G[from].push_back(edge(to, cap, cost, G[to].size()));
	G[to].push_back(edge(from, 0, -cost, G[from].size()-1));
}

ll SuccessiveShortestPath_Unit(graph &G, vector<ll> &excess){
	ll V = G.size();
	map<ll, ll>  E, D;
	REP(i, V){
		if(excess[i]>0) E[i] = excess[i];
		if(excess[i]<0) D[i] = excess[i];
	}

	ll total_cost = 0;
	vector<ll> pot(V, 0);
	vector<ll> prevv(V);
	vector<ll> preve(V);
	while(!E.empty()){
		ll s, t;
		s = E.begin()->first;

		// dijkstra
		priority_queue< pll, vector<pll>, greater<pll> > pq;
		bool feasibility_flag = false;
		vector<ll> dist(V, INF);
		vector<bool> visited(V, false);
		dist[s] = 0;
		pq.push(pll(0, s));
		while(!pq.empty()){
			pll p = pq.top();
			pq.pop();
			ll v = p.second;
			visited[v] = true;
			if(excess[v]<0){
				t = v;
				feasibility_flag = true;
				break;
			}
			if(dist[v]<p.first) continue;
			for(int i=0;i<G[v].size();i++){
				edge &e = G[v][i];
				if(e.cap>0 && dist[e.to]>dist[v]+e.cost+pot[v]-pot[e.to]){
					dist[e.to] = dist[v] + e.cost + pot[v] - pot[e.to];
					prevv[e.to] = v;
					preve[e.to] = i;
					pq.push(pll(dist[e.to], e.to));
				}
			}
		}

		// check fesibility
		if(!feasibility_flag) return -1;

		// update potential
		for(int i=0;i<V;i++) pot[i] += (visited[i] ? dist[i] : dist[t]);

		// update total cost
		for(int v=t;v!=s;v=prevv[v]){
			total_cost += G[prevv[v]][preve[v]].cost;
		}

		// update excess and deficit
		E[s] -= 1;
		if(E[s]==0) E.erase(s);
		D[t] += 1;
		if(D[t]==0) D.erase(t);

		// update residual graph
		for(int v=t;v!=s;v=prevv[v]){
			edge &e = G[prevv[v]][preve[v]];
			e.cap -= 1;
			G[v][e.rev].cap += 1;
		}
	}
	return total_cost;
}

void print_flow(graph &G){
	REP(i, G.size()){
		REP(j, G[i].size()){
			if(G[i][j].cost<0){
				printf("from: %d, to: %d, flow: %d\n", G[i][j].to, i, G[i][j].cap);
			}
		}
	}
}

int main(){

	///*
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
	//*/

	/*
	ll V, E;
	cin >> V >> E;
	vector<ll> excess(V);
	REP(i, V) cin >> excess[i];
	graph G(V);
	REP(i, E){
		ll u, v, c, d;
		cin >> u >> v >> c >> d;
		add_edge(G, u, v, c, d);
	}
	*/

	ll res = SuccessiveShortestPath_Unit(G, excess);
	cout << res << endl;
	//print_flow(G);
    return 0;
}