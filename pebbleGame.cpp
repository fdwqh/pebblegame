#include<iostream>
#include<vector>
#include<algorithm>
#include<unordered_map>
#include<unordered_set>
#include<queue>
#define mp make_pair
#define pii pair<int, int>
using namespace std;
const int maxn = 1e5+10;
const int RIGID = 1;
const int FLOPPY = -1;
struct pair_hash
{
	template<class T1, class T2>
	std::size_t operator() (const std::pair<T1, T2>& p) const
	{
		auto h1 = std::hash<T1>{}(p.first);
		auto h2 = std::hash<T2>{}(p.second);
		return h1 ^ h2;
	}
};
struct edge
{
	int pebbleState;
	int label;
	int u, v; //Two ends of the edge
	edge(int s, int t) :pebbleState(-1), label(-1), u(s), v(t)  {}
};
class graph
{
public:
	int n;
	int m;
	vector<unordered_set<int>> nodeArray; 
	vector<edge> edgeArray;
	vector<vector<int>> adjacencyList;
	unordered_map<pii, int, pair_hash> edgeMap;
	void input();
	void output();
	int runPebbleGame();
	bool Enlarge_Cover(int edgeID, int k);
	bool Find_Pebble(int startPoint, vector<bool> &seen);
	void Find_rigidCluster();
private:
	void coverEdge(int edgeID, int coverNode);
	void Gather_Pebble(int edgeID);
	bool Free_Pebble(int pointID,vector<int> &isRigidSite, vector<int> &nodeinSearch, vector<bool> &seen);
};
void graph::coverEdge(int e, int u)
{
	//The other vertex of this edge
	int v = edgeArray[e].u == u ? edgeArray[e].v : edgeArray[e].u; 
	if (edgeArray[e].pebbleState == -1) //If the edge has not been covered
		//cover
		nodeArray[u].insert(v);
	else if(edgeArray[e].pebbleState == v) //If the edge has already been covered by u
	{
		//release & cover
		nodeArray[v].erase(u);
		nodeArray[u].insert(v);
	}
	/*
	Otherwise, the edge has already been covered by the 
	pebble from vertex u, nothing should to be done.
	*/
	edgeArray[e].pebbleState = u;
}
void graph::input()
{
	cin >> n >> m;
	nodeArray.resize(n);
	adjacencyList.resize(n);
	for (int i = 0; i < m; i++)
	{
		int u, v;
		cin >> u >> v;
		adjacencyList[u].push_back(v);
		adjacencyList[v].push_back(u);
		edgeArray.push_back(edge(u, v));
		edgeMap[mp(u, v)] = i, edgeMap[mp(v, u)] = i; 
	}
}
void graph::output()
{
	bool isRigid = runPebbleGame();
	Find_rigidCluster();
	for (int i = 0; i < m; i++)
		cout << 'e' << i << ": " << edgeArray[i].pebbleState << ' ' << edgeArray[i].label << endl;
}
int graph::runPebbleGame()
{
	int freePebble = 2 * n;
	for (int i = 0; i < edgeArray.size(); i++)
	{
		bool enlarge = Enlarge_Cover(i, 4);
		if (enlarge)
			coverEdge(i, edgeArray[i].u);
	}
	if (freePebble > 3) return 0;
	return 1;
}
bool graph::Enlarge_Cover(int e, int k)
{
	int a = edgeArray[e].u;
	int b = edgeArray[e].v;
	int pebbleCount = 4 - (nodeArray[a].size() + nodeArray[b].size());
	if (pebbleCount >= k)
		return 1;
	vector<int> aNeighbor;
	vector<int> bNeighbor;
	for (unordered_set<int>::iterator it = nodeArray[a].begin(); it != nodeArray[a].end(); it++)
		if(*it != b) aNeighbor.push_back(*it);
	for (unordered_set<int>::iterator it = nodeArray[b].begin(); it != nodeArray[b].end(); it++)
		if(*it != a) bNeighbor.push_back(*it);
	for (int i = 0; i < aNeighbor.size(); i++)
	{
		vector<bool> seen(n);
		seen[a] = seen[b] = 1;
		int x = aNeighbor[i];
		bool found = Find_Pebble(x, seen);
		if (found)
		{
			coverEdge(edgeMap[mp(a, x)], x);
			pebbleCount++;
			if (pebbleCount >= k)
				return 1;
		}
	}
	for (int i = 0; i < bNeighbor.size(); i++)
	{
		vector<bool> seen(n);
		seen[a] = seen[b] = 1;
		int x = bNeighbor[i];
		bool found = Find_Pebble(x, seen);
		if (found)
		{
			coverEdge(edgeMap[mp(b, x)], x);
			pebbleCount++;
			if (pebbleCount >= k)
				return 1;
		}
	}
	return 0;
}
bool graph::Find_Pebble(int u, vector<bool> &seen)
{
	seen[u] = 1;
	if (nodeArray[u].size() < 2)
		return 1;
	for (unordered_set<int>::iterator it = nodeArray[u].begin(); it != nodeArray[u].end(); it++)
	{
		int x = *it;
		if (!seen[x])
		{
			bool isFound = Find_Pebble(x, seen);
			if (isFound)
			{
				coverEdge(edgeMap[mp(u, x)], x);
				return 1;
			}
		}
	}
	return 0;
}
bool graph::Free_Pebble(int u, vector<int> &p, vector<int> &a, vector<bool> &seen)
{
	a.push_back(u);
	seen[u] = 1;
	if (nodeArray[u].size() < 2)
	{
		p[u] = FLOPPY;
		return 1;
	}
	for (unordered_set<int>::iterator it = nodeArray[u].begin(); it != nodeArray[u].end(); it++)
	{
		int v = *it;
		if (seen[v]) continue;
		if (Free_Pebble(v, p, a, seen))
		{
			p[u] = FLOPPY;
			return 1;
		}
	}
	return 0;
}
void graph::Find_rigidCluster()
{
	int rigidLabel = 1;
	for (int i = 0; i < edgeArray.size(); i++)
	{
		if (edgeArray[i].label != -1)
			continue;
		int u = edgeArray[i].u;
		int v = edgeArray[i].v;
		Enlarge_Cover(i, 3);
		cout << i << ":";  for (int j = 0; j < m; j++) cout << edgeArray[j].pebbleState << ' '; cout << endl;
		vector<int> isRigidSite(n); //whether vertex(i) is a rigid site
		queue<int> q;
		q.push(u);
		q.push(v);
		isRigidSite[u] = isRigidSite[v] = RIGID;
		while (!q.empty())
		{
			int x = q.front();
			q.pop();
			for (int i = 0; i < adjacencyList[x].size(); i++)
			{
				int y = adjacencyList[x][i];
				if (isRigidSite[y])
					continue;
				vector<int> nodeinSearch;
				vector<bool> seen(n);
				seen[u] = seen[v] = 1;
				bool flag = Free_Pebble(y, isRigidSite, nodeinSearch, seen);
				if (!flag)
				{
					for (int i = 0; i < nodeinSearch.size(); i++)
					{
						isRigidSite[nodeinSearch[i]] = RIGID;
						q.push(nodeinSearch[i]);
					}
				}
			}
		}
		for (int i = 0; i < edgeArray.size(); i++)
		{
			if (isRigidSite[edgeArray[i].u] == RIGID && isRigidSite[edgeArray[i].v] == RIGID)
				edgeArray[i].label = rigidLabel;
		}
		rigidLabel++;
	} 
}
int main()
{
	graph G;
	G.input();
	G.output();
	system("pause");
	return 0;
}