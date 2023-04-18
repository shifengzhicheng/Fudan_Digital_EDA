#include <queue>
#include <cstdio>
#define UNVISITED 0
#define VISITED 1
#define INFINITY 32767
using namespace std;
class Edge { // 边类
	public:
		int from, to, weight; // from是边的始点,to是终点,weight是边的权
		Edge() { // 缺省构造函数
			from = -1;//没有from
			to = -1;//没有to
			weight = 0;//权重为0
		}
		Edge(int f, int t, int w) { // 给定参数的构造函数
			from = f;
			to = t;
			weight = w;
		}
};
class Graph {
	public:
		int numVertex; // 图中顶点的个数
		int numEdge; // 图中边的条数
		int *Mark; // 标记图的顶点是否被访问过
		int *Indegree; // 存放图中顶点的入度

		Graph(int numVert) { // 图的构造函数
			numVertex = numVert;
			numEdge = 0;
			Indegree = new int[numVertex];
			Mark = new int[numVertex];
			for (int i = 0; i < numVertex; i++) {
				Mark[i] = UNVISITED; // 标志位设为UNVISITED
				Indegree[i] = 0; // 入度设为0
			}
		}

//		~Graph() { // 析构函数
//			delete [] Mark; // 释放Mark数组
//			delete [] Indegree; // 释放Indegree数组
//		}

		int VerticesNum() { // 返回图中顶点的个数
			return numVertex;
		}

		bool IsEdge(Edge oneEdge) { // oneEdge是否是边
			if (oneEdge.weight > 0 && oneEdge.weight < INFINITY && oneEdge.to >= 0)
				return true;
			else return false;
		}

		int EdgesNum(); //返回图的边数

		//返回与顶点oneVertex相关联的第一条边
		Edge FirstEdge(int oneVertex);

		//返回与边PreEdge有相同关联顶点oneVertex的
		//下一条边
		Edge NextEdge(Edge preEdge);

		//添加一条边
		bool setEdge(int fromVertex, int toVertex, int weight);

		//删一条边
		bool delEdge(int fromVertex, int toVertex);

		//如果oneEdge是边则返回TRUE，否则返回FALSE
		//返回边oneEdge的始点
		int FromVertex(Edge oneEdge) {
			return oneEdge.from;
		}

		//返回边oneEdge的终点
		int ToVertex(Edge oneEdge) {
			return oneEdge.to;
		}

		//返回边oneEdge的权
		int Weight(Edge oneEdge) {
			return oneEdge.weight;
		}

		//地图清空访问记录
		void clear() {
			for (int i = 0; i < numVertex; i++) {
				Mark[i] = UNVISITED;
			}
		}
		void Visit(Graph &G, int v);
		void TopsortbyQueue(Graph& G) {
			for (int i = 0; i < G.VerticesNum(); i++) // 初始化Mark数组
				G.Mark[i] = UNVISITED;
			using std::queue; // 使用STL中的队列
			queue<int> Q;
			for (int i = 0; i < G.VerticesNum(); i++) // 入度为0的顶点入队
				if (G.Indegree[i] == 0)
					Q.push(i);
			while (!Q.empty()) { // 如果队列非空
				int v = Q.front(); // 获得队列顶部元素
				Q.pop(); // 队列顶部元素出队
				Visit(G, v); // 问顶点v
				G.Mark[v] = VISITED; // 将标记位设置为VISITED
				for (Edge e = G.FirstEdge(v); G.IsEdge(e); e = G.NextEdge(e)) {
					G.Indegree[G.ToVertex(e)]--; // 与该顶点相邻的顶点入度减1
					if (G.Indegree[G.ToVertex(e)] == 0) // 如果顶点入度减为0则入队
						Q.push(G.ToVertex(e));
				}
			}
			for (int i = 0; i < G.VerticesNum(); i++) // 利用标记位可以判断图中是否有环
				if (G.Mark[i] == UNVISITED) {
					printf("有环");
					break;
				}
		}
};

