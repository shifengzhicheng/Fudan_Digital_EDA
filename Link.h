#include"Graph.h"
#include<cstdio>
#include<queue>
struct listUnit { // 邻接表表目中数据部分的结构定义
	int vertex; // 边的终点
	int weight; // 边的权
};
template<class Elem>
class Link { // 链表元素
	public:
		Elem element; // 表目的数据
		Link *next; // 表目指针，指向下一个表目
		Link(const Elem& elemval, Link *nextval = NULL) { // 构造函数
			element = elemval;
			next = nextval;
		}
		Link(Link *nextval = NULL) { // 构造函数
			next = nextval;
		}
};
template<class Elem>
class LList { // 链表类
	public:
		Link<Elem> *head; // head保存一个虚的头结点，以方便操作
		LList() { // 构造函数
			head = new Link<Elem>();
		}
};
class Graphl: public Graph {
	private:
		LList<listUnit> *graList; // 保存所有边表的数组
	public:
		Graphl(int numVert): Graph(numVert) { // 构造函数
			graList = new LList<listUnit>[numVertex]; // 为边表graList数组申请空间
		}

		Edge FirstEdge(int oneVertex) { // 返回顶点oneVertex的第一条边
			Edge myEdge; // myEdge的初始成员变量to为-1
			myEdge.from = oneVertex; // 将顶点oneVertex作为边myEdge的始点
			Link<listUnit> *temp = graList[oneVertex].head;
			if (temp->next != NULL) { // 顶点oneVertex边表非空
				myEdge.to = temp->next->element.vertex; // 边表第一个表目的顶点
				myEdge.weight = temp->next->element.weight;
			}
			return myEdge; // 如果没有找到第一条边，myEdge.to=-1
		}
		Edge NextEdge(Edge preEdge) { // 返回与边有相同关联顶点的下一条边
			Edge myEdge; // myEdge的初始成员变量to为-1
			myEdge.from = preEdge.from; // 将边的始点置为与上一条边的相同
			Link<listUnit> *temp = graList[preEdge.from].head; // temp指向边表头一个
			while (temp->next != NULL && temp->next->element.vertex <= preEdge.to)
				temp = temp->next; // 确定边preEdge的位置
			if (temp->next != NULL) { // 边preEdge的下一条边存在
				myEdge.to = temp->next->element.vertex;
				myEdge.weight = temp->next->element.weight;
			}
			return myEdge;
		}
		void setdoubleEdge(int i, int j, int w) {
			setEdge(i,j,w);
			setEdge(j,i,w);
		}
		void setEdge(int from, int to, int weight) { // 为图设定一条边
			Link<listUnit> *temp = graList[from].head; // temp指向边表头一个
			while (temp->next != NULL && temp->next->element.vertex < to)
				temp = temp->next; // 确定边(from,to)在边表中的位置
			if (temp->next == NULL) { // 边(from,to)在边表中不存在
				temp->next = new Link<listUnit>; // 在边表最后加入这条新边
				temp->next->element.vertex = to;
				temp->next->element.weight = weight;
				numEdge++;
				Indegree[to]++;
				return;
			}
			if (temp->next->element.vertex == to) { // 边(from,to)在边表中已存在
				temp->next->element.weight = weight; // 只需要改变边的权值
				return;
			}
			if (temp->next->element.vertex > to) { // 边(from,to)在边表中不存在
				Link<listUnit> *other = temp->next;
				temp->next = new Link<listUnit>;
				temp->next->element.vertex = to;
				temp->next->element.weight = weight;
				temp->next->next = other; // 连接边表中其后的其他边
				numEdge++;
				Indegree[to]++;
				return;
			}
		}
		void delEdge(int from, int to) { // 删掉图的一条边
			Link<listUnit> *temp = graList[from].head; // temp指向边表头一个
			while (temp->next != NULL && temp->next->element.vertex < to)
				temp = temp->next; // 确定边(from,to)在边表中的位置
			if (temp->next == NULL)
				return; // 边(from,to)在边表中不存在，直接返回
			if (temp->next->element.vertex > to)
				return; // 边(from,to)在边表中不存在，直接返回
			if (temp->next->element.vertex == to) { // 边(from,to)在边表中存在
				Link<listUnit> *other = temp->next->next;
				delete temp->next; // 从边表中将其删掉
				temp->next = other; // 其他表目挂接
				numEdge--; // 边数减1
				Indegree[to]--; // 终点的入度减1
				return;
			}
		}
		void Visit(Graphl& G, int v);
		void DFS(Graphl& G, int v) { // 深度优先搜索的递规实现
			G.Mark[v] = VISITED; // 将标记位设置为VISITED
			Visit(G, v); // 访问顶点v
			for (Edge e = G.FirstEdge(v); G.IsEdge(e); e = G.NextEdge(e))
				if (G.Mark[G.ToVertex(e)] == UNVISITED)
					DFS(G, G.ToVertex(e));
		}
		void BFS(Graphl& G, int v) {
			using std::queue; // 使用STL中的队列
			queue<int> Q;
			Visit(G, v); // 访问顶点v
			G.Mark[v] = VISITED; // 将标记位设置为VISITED
			Q.push(v); // 顶点v入队列
			while (!Q.empty()) { // 如果队列非空
				int u = Q.front (); // 获得队列顶部元素
				Q.pop(); // 队列顶部元素出队
				// 与该顶点邻接的所有未访问顶点入队
				for (Edge e = G.FirstEdge(u); G.IsEdge(e); e = G.NextEdge(e))
					if (G.Mark[G.ToVertex(e)] == UNVISITED) {
						Visit(G, G.ToVertex(e));
						G.Mark[G.ToVertex(e)] = VISITED;
						Q.push(G.ToVertex(e));
					}
			}
		}
		void TopsortbyQueue(Graphl& G) {
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
					printf("\n 此图有环！");
					break;
				}
		}
};
