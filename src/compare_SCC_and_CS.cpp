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

ll CapacityScaling(graph &G, vector<ll> &excess, ll scaling_factor){
	ll V = G.size();

	ll total_cost = 0;
	vector<ll> pot(V, 0);
	vector<ll> prevv(V);
	vector<ll> preve(V);

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
	ll tmp = 0;
	for(int i=0;i<V;i++){
		for(int j=0;j<G[i].size();j++){
			edge &e = G[i][j];
			if(e.cost < 0) tmp += (-e.cost)*e.cap;
			dump(tmp)
		}
	}
	*/
	
	return total_cost;
}

ll SuccessiveShortestPath(graph &G, vector<ll> &excess){
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

		// get flow amount
		ll f = min(E[s], -D[t]);
		ll d = f;
		for(int v=t;v!=s;v=prevv[v]){
			d = min(d, G[prevv[v]][preve[v]].cap);
		}

		// update total cost
		for(int v=t;v!=s;v=prevv[v]){
			total_cost += d*G[prevv[v]][preve[v]].cost;
		}

		// update excess and deficit
		E[s] -= d;
		if(E[s]==0) E.erase(s);
		D[t] += d;
		if(D[t]==0) D.erase(t);

		// update residual graph
		for(int v=t;v!=s;v=prevv[v]){
			edge &e = G[prevv[v]][preve[v]];
			e.cap -= d;
			G[v][e.rev].cap += d;
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

int main(int argc, char* argv[]){

	ll expand = atoll(argv[1]);
	ll scaling_factor = atoll(argv[2]);

	ll V, E, F;
	cin >> V >> E >> F;
	F *= expand;
	graph G1(V), G2(V), G3(V);
	vector<ll> excess1(V, 0), excess2(V, 0), excess3(V, 0);

	REP(i, E){
		ll u, v, c, d;
		cin >> u >> v >> c >> d;
		c *= expand;
		add_edge(G1, u, v, c, d);
		add_edge(G2, u, v, c, d);
		add_edge(G3, u, v, c, d);
	}
	excess1[0] = F;
	excess1[V-1] = -F;
	excess2[0] = F;
	excess2[V-1] = -F;
	excess3[0] = F;
	excess3[V-1] = -F;

	clock_t start, end;

	/*
	start = clock();
	ll res1 = SuccessiveShortestPath_Unit(G1, excess1);
	end = clock();
	double time1 = (double)(end-start)/CLOCKS_PER_SEC;
	printf("SSC_Unit: res %ld, time: %f\n", res1, time1);
	*/

	start = clock();
	ll res2 = CapacityScaling(G2, excess2, scaling_factor);
	end = clock();
	double time2 = (double)(end-start)/CLOCKS_PER_SEC;
	printf("CS: res %ld, time: %f\n", res2, time2);

	start = clock();
	ll res3 = SuccessiveShortestPath(G3, excess3);
	end = clock();
	double time3 = (double)(end-start)/CLOCKS_PER_SEC;
	printf("SSC: res %ld, time: %f\n", res3, time3);
    return 0;
}