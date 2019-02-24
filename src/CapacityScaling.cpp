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

void print_flow(graph &G){
	cout << "=====Graph=====" << endl;
	REP(i, G.size()){
		REP(j, G[i].size()){
			if(G[i][j].cost<0){
				printf("from: %d, to: %d, flow: %d\n", G[i][j].to, i, G[i][j].cap);
			}
		}
	}
	cout << "===============" << endl;
}

ll CapacityScaling(graph &G, vector<ll> &excess){
	ll V = G.size();

	ll total_cost = 0;
	vector<ll> pot(V, 0);
	vector<ll> prevv(V);
	vector<ll> preve(V);
	ll scaling_factor = 4;

	ll maximum_excess = -1;
	for(int i=0;i<V;i++) maximum_excess = max(maximum_excess, abs(excess[i]));
	ll delta = 1;
	while(maximum_excess>0){
		maximum_excess /= scaling_factor;
		delta *= scaling_factor;
	}

	while(delta>0){
		// delta-scaling phase

		// satisfy reduced cost optimality
		for(int i=0;i<V;i++){
			for(int j=0;j<G[i].size();j++){
				edge &e = G[i][j];
				if(e.cap >= delta && e.cost+pot[i]-pot[e.to] < 0){
					ll push_flow = e.cap;
					G[e.to][e.rev].cap += push_flow;
					e.cap = 0;
					excess[i] -= push_flow;
					excess[e.to] += push_flow;
					total_cost += push_flow*e.cost; // don't forget add cost when saturating negative edge
				}
			}
		}

		// determine S_delta and T_delta
		map<ll, ll> S_delta, T_delta;
		for(int i=0;i<V;i++){
			if(excess[i]>=delta) S_delta[i] = excess[i];
			if(excess[i]<=-delta) T_delta[i] = excess[i];
		}

		// delta-shortest path
		while(!S_delta.empty()){
			ll s, t;
			s = S_delta.begin()->first;
			
			/*
			cout << "=====S=====" << endl;
			for(auto x: S_delta){
				cout << x.first << " " << x.second << endl;
			}
			cout << "===============" << endl;

			cout << "=====T=====" << endl;
			for(auto x: T_delta){
				cout << x.first << " " << x.second << endl;
			}
			cout << "===============" << endl;
			*/
			

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
					if(e.cap>=delta && dist[e.to]>dist[v]+e.cost+pot[v]-pot[e.to]){
						dist[e.to] = dist[v] + e.cost + pot[v] - pot[e.to];
						prevv[e.to] = v;
						preve[e.to] = i;
						pq.push(pll(dist[e.to], e.to));
					}
				}
			}

			// check_feasibility
			if(!feasibility_flag){
				if(delta==1){
					return -1;
				}else{
					S_delta.erase(s);
					continue;
				}
			}

			// update potential
			for(int i=0;i<V;i++) pot[i] += (visited[i] ? dist[i] : dist[t]);

			
			// update total cost
			///*
			for(int v=t;v!=s;v=prevv[v]){
				total_cost += delta*G[prevv[v]][preve[v]].cost;
			}
			//*/
			

			// update excess and deficit
			S_delta[s] -= delta;
			excess[s] -= delta;
			if(S_delta[s]<delta) S_delta.erase(s);

			T_delta[t] += delta;
			excess[t] += delta;
			if(T_delta[t]>-delta) T_delta.erase(t);

			// update residual graph
			for(int v=t;v!=s;v=prevv[v]){
				edge &e = G[prevv[v]][preve[v]];
				e.cap -= delta;
				G[v][e.rev].cap += delta;
			}

			/*
			print_flow(G);

			cout << "=====excess=====" << endl;
			REP(i, V){
				cout << excess[i] << " ";
			}
			cout << endl;
			cout << "===============" << endl;
			*/

		}

		// update delta
		delta /= scaling_factor;
	}

	/*
	for(int i=0;i<V;i++){
		for(int j=0;j<G[i].size();j++){
			edge &e = G[i][j];
			if(e.cost < 0) total_cost += (-e.cost)*e.cap;
		}
	}
	*/

	return total_cost;
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

	ll res = CapacityScaling(G, excess);
	cout << res << endl;
    return 0;
}