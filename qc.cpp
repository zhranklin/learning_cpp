#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <list>
#include <queue>
#include <deque>
#include <stack>
#include <string>
#include <algorithm>
#include <cassert>
#include <cctype>
using namespace std;

///图, 碎片, 环, 面以及道路的存储结构
typedef vector< list<int>* > graph;
typedef vector< list<int>* > fragment;
typedef list<int> circle;		///环用链表表示, 链表按顺时针记录环的点
typedef list<int> face;		///面用链表表示即可, 链表按顺时针记录面的点
typedef list<int> path;

const int MAXINT = ((unsigned)-1)>>1;
const int MAXPOINTCOUNT = 200;

int colour[MAXPOINTCOUNT];		///为遍历设置的数组
const int WHITE = 0;
const int GRAY = 1;
const int BLACK = 2;

int parent[MAXPOINTCOUNT];		///为遍历设置的数组
int depth[MAXPOINTCOUNT];

int pointKind[MAXPOINTCOUNT];
const int EMBEDED = -1;
const int AVAILABLE = 0;

bool pointEmbed[MAXPOINTCOUNT];
bool edgeEmbed[MAXPOINTCOUNT][MAXPOINTCOUNT];

int lowpoint[MAXPOINTCOUNT];		///为找割点设置的数组
int lowneighbour[MAXPOINTCOUNT];
int pointType[MAXPOINTCOUNT];
const int NONCUTVERTEX = 0;
const int CUTVERTEX = 1;

int leave[MAXPOINTCOUNT];
int enter[MAXPOINTCOUNT];

int visitTime;

void removeBidegreePoint(graph *g) {}
void removeSingleDegreePoint(graph *g) {}
void removeParallelEdge(graph *g) {}
graph* createGraph(int pointCount) 
{
	graph* g = new graph;
	for (int i=0; i<pointCount; i++) {
		g->push_back(new list<int>);
	}
	return g;
}

graph* inputGraph(void) 
{
	int pointCount, edgeCount, from, to;
	printf("请输入顶点个数：");
	 cin >> pointCount;
	printf("\n请输入边的条数: ");
	cin >> edgeCount;
    graph *g = createGraph(pointCount);
    cout<<"\n请输入图的信息:"<<endl;
    for (int i=0; i<edgeCount; i++) 
    {
        cin >> from >> to;
	g->at(from)->push_back(to);
        g->at(to)->push_back(from);
    }
    return g;
}

circle* getOneCircle(const graph *g, int start) 
{
    int pointCount = g->size();//图的节点数
	for (int i=0; i<pointCount; i++) 
    {
		colour[i] = WHITE;
	}
	queue<int> q;
	q.push(start);
	colour[start] = GRAY;
	parent[start] = -1;
	depth[start] = 0;
	list<int>::iterator tra;
	list<int>::iterator end;
	int u,v;
	
	///对图进行一次广度优先遍历即可以找到一个环
	while (!q.empty()) 
    {
		u = q.front(); q.pop();
		tra = g->at(u)->begin();
		end = g->at(u)->end();
		for (; tra != end; tra++) 
        {
			v = *tra;//解引用操作
			if (colour[v] == WHITE) 
            {
				q.push(v);
				colour[v] = GRAY;
				parent[v] = u;
				depth[v] = depth[u]+1;
			}
			///广度优先树的某一片树叶有在队列中的邻居, 则可以找到一个环
			else if (colour[v] == GRAY) 
            {
				goto buildTheCircle;
			}
		}
		colour[u] = BLACK;
	}
	assert(!q.empty());

///从树叶结点u和邻居v回溯就可以找到一个环
buildTheCircle:
	circle *c = new circle;
	if (depth[v] > depth[u]) 
    {
		c->push_front(v);
		v = parent[v];
	}
	while (u != v) 
    {
		c->push_front(u);
		c->push_back(v);
		u = parent[u];
		v = parent[v];
	}
	c->push_front(u);
	return c;
}

vector< fragment* >* getFragments(const graph *g) 
{
	int pointCount = g->size();
	for (int i=0; i<pointCount; i++) 
    {
		colour[i] = WHITE;
		if (pointEmbed[i] || g->at(i)->empty()) 
        {
			pointKind[i] = EMBEDED;
		}
		else 
        {
			pointKind[i] = AVAILABLE;
		}
	}
	
	vector< fragment* >* fragmentsVector = new vector< fragment* >;
	int fragmentsCount = 0;
	queue<int> q;
	list<int>::iterator ltra;
	list<int>::iterator lend;
	///广度优先遍历找fragment,
	///将fragment分成两类: 1包含末嵌入到平面的结点的, 2不包含...的
	for (i=0; i<pointCount; i++) 
    {
		if (colour[i] == WHITE) 
        {		///fragment有可能不只一个
			if (pointKind[i] == AVAILABLE) 
            {		///从末嵌入平面的结点出发遍历
				fragment *f = createGraph(pointCount);
				fragmentsCount++;
				colour[i] = GRAY;
				parent[i] = -1;
				pointKind[i] = fragmentsCount;
				q.push(i);
				while (!q.empty()) 
                {
					int u = q.front(); q.pop();
					ltra = g->at(u)->begin();
					lend = g->at(u)->end();
					for (; ltra != lend; ltra++) 
                    {
						int v = *ltra;
						if (colour[v] == WHITE) 
                        {
							if (pointKind[v] == AVAILABLE) 
                            {
								pointKind[v] = fragmentsCount;
								parent[v] = u;
								colour[v] = GRAY;
								q.push(v);
								f->at(u)->push_back(v);
								f->at(v)->push_back(u);
							}
							///如果邻居结点已经嵌入平面,
							///则只将边加入到fragment而不入队(中止从此类结点出发的遍历);
							else if (pointKind[v] == EMBEDED) 
                            {
								f->at(u)->push_back(v);
								f->at(v)->push_back(u);
							}
						}
						///如果邻居结点在队列中或者结点已经嵌入平面, 
						///则只将边加入到fragment而不入队(中止从此类结点出发的遍历);
						else if (colour[v] == GRAY || pointKind[v] == EMBEDED) 
                        {
							f->at(u)->push_back(v);
							f->at(v)->push_back(u);
						}
					}
					colour[u] = BLACK;
				}
				fragmentsVector->push_back(f);
			}
			///如果出发结点已经嵌入平面, 则只找那些仅包含已经嵌入平面的结点的fragment
			///这些fragment只有一条边, edgeEmbed[]是用来提供已经嵌入平面的边的信息的
			else if (pointKind[i] == EMBEDED) 
            {
				ltra = g->at(i)->begin();
				lend = g->at(i)->end();
				for (; ltra!=lend; ltra++) 
                {
					assert(i != *ltra);
					if (pointKind[*ltra] == EMBEDED && !edgeEmbed[i][*ltra]) 
                    {
						fragment *f = createGraph(pointCount);
						f->at(i)->push_back(*ltra);
						f->at(*ltra)->push_back(i);
						fragmentsVector->push_back(f);
						parent[*ltra] = i;
						colour[*ltra] = BLACK;
					}
				}
				colour[i] = BLACK;
			}
		}
	}
	return fragmentsVector;
}

vector< face* >* getFacesFromOneCircle(circle*& c) 
{
    vector< face* > *faces = new vector< face* >;
    face* tempFace = new face;			///面用链表表示即可, 链表按顺时针记录面的点
    tempFace->push_back(MAXINT);		///带MAXINT(当作无穷)的面是外面
	circle::iterator ctra = c->begin();
	circle::iterator cend = c->end();
	for (; ctra != cend; ctra++) 
    {
		tempFace->push_back(*ctra);
	}
	faces->push_back(tempFace);
	faces->push_back(c);
	c = NULL;			///circle c已经转变为其中一个face, 防止通过指针c改变它
	return faces;
}

///标记已经嵌入到平面的点
void markEmbedPoint(const circle *c, int pointCount) 
{
	memset(pointEmbed, false, sizeof(bool)*pointCount);
	circle::const_iterator ctra = c->begin();
	circle::const_iterator cend = c->end();
	for (; ctra != cend; ctra++) 
    {
		if (*ctra != MAXINT) 
        {
			pointEmbed[*ctra] = true;
		}
	}
}
///每次将alpha path嵌入到平面时, 更新已经嵌入到平面的结点
void addPoint(const path *alphaPath) 
{
	path::const_iterator ptra = alphaPath->begin();
	path::const_iterator pend = alphaPath->end();
	for (; ptra != pend; ptra++) 
    {
		pointEmbed[*ptra] = true;
	}
}

///标记已经嵌入到平面的边
void markEmbedEdge(const circle *c, int pointCount) 
{
	for (int i=0; i<pointCount; i++) 
    {
		for (int j=0; j<pointCount; j++) 
        {
			edgeEmbed[i][j] = false;
		}
	}
	circle::const_iterator ctra = c->begin();
	circle::const_iterator cend = c->end();
	circle::const_iterator pre;
	for (pre=ctra++; ctra != cend; ctra++) 
    {
		edgeEmbed[*pre][*ctra] = true;
		edgeEmbed[*ctra][*pre] = true;
		pre = ctra;
	}
	edgeEmbed[*(c->begin())][*pre] = true;
	edgeEmbed[*pre][*(c->begin())] = true;
}
///每次将alpha path嵌入到平面时更新已经嵌入到平面的边
void addEdge(const path *alphPath) 
{
	path::const_iterator ptra = alphPath->begin();
	path::const_iterator pend = alphPath->end();
	path::const_iterator pre;
	for (pre = ptra++; ptra!=pend; ptra++) 
    {
		edgeEmbed[*pre][*ptra] = true;
		edgeEmbed[*ptra][*pre] = true;
		pre = ptra;
	}
}
	

///fragment与已经嵌入平面的子图的公共点就是contact point
bool* findContactPoint(const fragment *f) 
{
	int pointCount = f->size();
	bool *contactPoint = new bool[pointCount];
	memset(contactPoint, false, sizeof(bool) * pointCount);
	for(int i=0; i < pointCount; i++) 
    {
		if (pointEmbed[i] && !(f->at(i)->empty())) 
        {
			contactPoint[i] = true;
		}
	}
	return contactPoint;
}
///找一个face所包含的所有点
bool *findFacePoint(const face *fa, int pointCount) 
{
	bool *facePoint = new bool[pointCount];
	memset(facePoint, false, pointCount * sizeof(bool));
	face::const_iterator fatra = fa->begin();
	face::const_iterator faend = fa->end();
	for (; fatra != faend; fatra++) 
    {
		if (*fatra != MAXINT) {
			facePoint[*fatra] = true;
		}
	}
	return facePoint;
}
bool isAdmissible(const fragment *fr, const face *fa) 
{
	int pointCount = fr->size();
	bool *contactPoint = findContactPoint(fr);
	bool *facePoint = findFacePoint(fa, pointCount);
	bool admissible = true;
	for (int i=0; i<pointCount; i++) 
    {
		///face只有包含fragment的所有contact point, 它才是admissible face
		if (contactPoint[i] && !facePoint[i]) 
        {
			admissible = false;
			break;
		}
	}
	delete[] contactPoint;
	delete[] facePoint;
	return admissible;
}
///如果存在fragment没有admissible face, 就返回false, 否则返回true
///引用型参数embedFaceIterator, embedFragmentIterator用来返回嵌入面和将要嵌入到面的碎片
bool getAdmissibleFace(const vector< fragment* > *fragments, const vector< face* > *faces,
					vector< fragment* >::const_iterator &embedFragmentIterator,
					vector< face* >::const_iterator &embedFaceIterator) 
{
	vector< fragment* >::const_iterator frtra = fragments->begin();
	vector< fragment* >::const_iterator frend = fragments->end();
	vector< face* >::const_iterator fctra = faces->begin();
	vector< face* >::const_iterator fcend = faces->end();
	bool foundEmbedPair = false;
	for (; frtra != frend; frtra++) 
    {
		int admissibleFaceCount = 0;
		vector< face* >::const_iterator admissibleFaceIterator;
		///对每一个fragment寻找其admissible face
		fctra = faces->begin();
		for (; fctra != fcend; fctra++) 
        {
			bool temp = isAdmissible(*frtra, *fctra);
			if (temp) 
            {
				admissibleFaceCount++;
				admissibleFaceIterator = fctra;
			}
		}
		///有一个fragment没有admissible face, 这个图就不是平面图
		if (admissibleFaceCount == 0) 
        {
            //cout << "not planarity graph" << endl;
			return false;
		}
		///如果有一个fragment只有一个admissible face, 则必需先将该fragment嵌入
		else if (admissibleFaceCount == 1) 
        {
			embedFaceIterator = admissibleFaceIterator;
			embedFragmentIterator = frtra;
			foundEmbedPair = true;	///用这个变量标记是否找到只有一个adimissible face的fragment
		}
		else if (!foundEmbedPair) 
        {
			embedFaceIterator = admissibleFaceIterator;
			embedFragmentIterator = frtra;
		}
	}
	return true;
}	
	
void removeEdge(fragment *f, int p1, int p2) 
{
	f->at(p1)->remove(p2);
	f->at(p2)->remove(p1);
}
///广度优先遍历, 从一个contact point出发找到另一个contact point即可建立一条alpha path
///函数会从fragment中去掉相应的alpha path;
path* getAlphaPath(fragment *f) 
{
	bool *contactPoint = findContactPoint(f);
	fragment::iterator ftra = f->begin();
	fragment::iterator fend = f->end();
	int start=0;
	for (; ftra != fend; ftra++, start++) 
    {		///找到alpha的出发点
		if ((*ftra)->size() != 0 && contactPoint[start]) 
        {
			break;
		}
	}
	assert(ftra != fend);		
	
	int pointCount = f->size();
	memset(colour, WHITE, pointCount * sizeof(int));
	queue<int> q;
	q.push(start);
	colour[start] = GRAY;
	parent[start] = -1;
	int end;
	///广度优先遍历
	while (!q.empty()) 
    {
		int u = q.front(); q.pop();
		list<int>::iterator ltra = f->at(u)->begin();
		list<int>::iterator lend = f->at(u)->end();
		for (; ltra != lend; ltra++) 
        {
			int v = *ltra;
			if (colour[v] == WHITE) 
            {
				if (contactPoint[v]) 
                {		///找到一个contact point即可以建立alpha path
					parent[v] = u;
					end = v;
					goto haveGottenAlphaPath;
				}
				q.push(v);
				colour[v] = GRAY;
				parent[v] = u;
			}
		}
		colour[u] = BLACK;
	}
	assert(!q.empty());	
haveGottenAlphaPath:
	path* p = new path;
	int tra = end;
	while (tra != start) 
    {
		p->push_back(tra);
		assert(parent[tra]!=-1);		
		removeEdge(f, tra, parent[tra]);		///从fragment里去掉在alpha path里的边
		tra = parent[tra];
	}
	p->push_back(start);
	delete[] contactPoint;
	return p;
}

///去掉alpha之后测试fragment是否为空图
bool isEmpty(const fragment* f) 
{
	fragment::const_iterator ftra = f->begin();
	fragment::const_iterator fend = f->end();
	for (; ftra != fend; ftra++) 
    {
		if (!(*ftra)->empty()) 
        {
			return false;
		}
	}
	return true;
}

//将alpha paph从一个面嵌入到平面, 嵌入后被嵌入的面将会改变, 函数返回从被嵌入的面分割出来的新面
face* splitFace(face *embedFace, path*& alphaPath) 
{
	int start = alphaPath->front();
	int end = alphaPath->back();
	
	face::iterator ftra = embedFace->begin();
	face::iterator fend = embedFace->end();
	face::iterator embedPoint1, embedPoint2;
	///调整alpha path的方向以方便嵌入平面
	for (; ftra != fend; ftra++) 
    {
		if (*ftra == start) 
        {
			break;
		}
		else if (*ftra == end) 
        {
			reverse(alphaPath->begin(), alphaPath->end());
			int temp = start;
			start = end;
			end = temp;
			break;
		}
	}
	///嵌入之后要保证面的表示依然是点的顺时针表示
	///嵌入应是嵌入到面的内部以保证不影响其它面, 注意"外面"的内部是无穷
	face tempPath(*((face*)alphaPath));
	face::iterator temp = ftra;
	ftra++;
	embedFace->erase(temp);
	for (; *ftra != end; ) 
    {
		alphaPath->push_front(*ftra);
		temp = ftra;
		ftra++;
		embedFace->erase(temp);
	}
	temp = ftra;
	ftra++;
	embedFace->erase(temp);
	embedFace->splice(ftra, tempPath);
	if (embedFace->front() != MAXINT) 
    {
		reverse(alphaPath->begin(), alphaPath->end());
	}
	
	face* newFace = alphaPath;
	alphaPath = NULL;
	return newFace;
}
///名字有问题, 但是找不到好的函数名, 这个函数是在getComponents()函数处理完之后用的
///getComponents()产生的某个component如果只有两条边, 那么这个component是一个bridge
bool isBridge(const graph* g, pair<int, int>& bridge) 
{
	int pointCount = g->size();
	int count = 0;
	int point[2];
	for (int i=0; i<pointCount; i++) 
    {
		if (!g->at(i)->empty()) 
        {
			if (count == 2) return false;
			point[count++]=i;
		}
	}
	bridge.first = point[0];
	bridge.second = point[1];
	return true;
}


void destroyFaces(vector< face* >*& faces) 
{
	vector< face* >::iterator fstra = faces->begin();
	vector< face* >::iterator fsend = faces->end();
	for (; fstra!=fsend; fstra++) 
    {
		delete *fstra;
	}
	delete faces;
	faces = NULL;
}
void destroyGraph(graph *g) 
{
	graph::iterator gtra = g->begin();
	graph::iterator gend = g->end();
	for (; gtra != gend; gtra++) 
    {
		delete *gtra;
	}
	delete g;
	g = NULL;
}
void destroyGraphs(vector< graph* >* g) 
{
	vector< graph* >::iterator vtra = g->begin();
	vector< graph* >::iterator vend = g->end();
	for (; vtra!=vend; vtra++) 
    {
		destroyGraph(*vtra);
	}
	delete g;
	g = NULL;
}
string integerToString(int x) 
{
	string s;
	if(x!=MAXINT&&x!=0)
    {
		while ((x+9) / 10) 
        {
			s.insert(s.begin(), x%10+'0');
			x /= 10;
		}
	}
	return s;
}
bool testPlanarityOfBiconnectedGraph(const graph *g, string& out) 
{
	int start = 0;
	int pointCount = g->size();
	for (int i=0; i<pointCount; i++) 
    {
		if (!(g->at(i)->empty())) 
        {
			start = i;
			break;
		}
	}
    circle *c = getOneCircle(g, start);
	markEmbedPoint(c, g->size());
	markEmbedEdge(c, g->size());
	vector< face* >* faces = getFacesFromOneCircle(c);

    vector< fragment* >* fragments = getFragments(g);

	bool isPlain = true;
	while (!fragments->empty()) 
    {
		vector< fragment* >::const_iterator embedFragmentIterator;
		vector< face* >::const_iterator embedFaceIterator;
		isPlain = getAdmissibleFace(fragments, faces, 
									embedFragmentIterator, embedFaceIterator);
		if (!isPlain) 
        {
			break;
		}

		fragment *embedFragment = *embedFragmentIterator;
		face *embedFace = *embedFaceIterator;		
		path *alphaPath = getAlphaPath(embedFragment);
		addPoint(alphaPath);
		addEdge(alphaPath);
		face* newFace = splitFace(embedFace, alphaPath);
		faces->push_back(newFace);
		destroyGraphs(fragments);
		fragments = getFragments(g);
	}

	if (isPlain) 
    {	
		destroyGraphs(fragments);
		destroyFaces(faces);
		return true;
	}
	else 
    {
		out="";
		destroyGraphs(fragments);
		destroyFaces(faces);
		return false;
	}
}
graph* createComponent(int pointCount, list< pair<int, int> >* l) 
{
	graph* g=createGraph(pointCount);
	pair<int, int> temp;
	while (!l->empty()) 
    {
		temp = l->front();
		l->pop_front();
		g->at(temp.first)->push_back(temp.second);
		g->at(temp.second)->push_back(temp.first);
	}
	return g;
}
void DFSVisit(const graph *g, int u, list< pair<int, int> >* l, vector< graph* >* components) 
{
	graph* component;
    colour[u] = GRAY;
    visitTime++;
    enter[u] = visitTime;
    list<int>::iterator ltra = g->at(u)->begin();
    list<int>::iterator lend = g->at(u)->end();
    list< pair<int, int> >* ul = new list< pair<int, int> >;
    int pointCount = g->size();
	int tempLowpoint = MAXINT;
    int tempLowneighbour = MAXINT;

    
    for (; ltra != lend; ltra++) 
    {
        int v = *ltra;
        if (colour[v] == WHITE) 
        {
            parent[v] = u;
            depth[v] = depth[u]+1;
            DFSVisit(g, v, ul, components);
			
			///用于判断割点.
			///lowpoint[v]是v的所有子孙结点的邻居结点里深度最小的点的深度
			///lowneighbour[v]是v的所有邻居结点里深度最小的点的深度
			if (lowpoint[v] < tempLowpoint) 
            {
				tempLowpoint = lowpoint[v];
			}
			if (lowneighbour[v] < tempLowpoint) 
            {
				tempLowpoint = lowneighbour[v];
			}
			
			///一个点u是割点当且仅当它的某个儿子v的lowpoint[v]或者lowneghbour[v]>=depth[u];
	        ///如果u是割点, 就用以v为根的子树的末加入前一component的所有边以及这些边的交叉边创建一个component
			if (lowpoint[v] >= depth[u] && lowneighbour[v] >= depth[u]) 
            {
				pointType[u] = CUTVERTEX;
				component = createComponent(pointCount, ul);
				components->push_back(component);
				component = NULL;
			}
			///如果u不是割点, 则将v子树末加入前一component的所有边以及这些边的交叉边加入到u子树中
			else 
            {
				l->splice(l->end(), *ul);
			}
        }
        ///将深度优先树的交叉边也加入到用于构建component的u子树中
        else if (colour[v] == GRAY) 
        {
        	l->push_back(pair<int, int>(u, v));
        }
        
        ///用于更新lowneighbour;
		if (depth[v] < tempLowneighbour) 
        {
    		tempLowneighbour = depth[v];
    	}
    }
    
    lowpoint[u] = tempLowpoint;
    lowneighbour[u] = tempLowneighbour;
    
    colour[u] = BLACK;
    leave[u] = ++visitTime;
    delete ul;
}
///将图拆分为若干个biconne component;
//在深度优先树回溯找割点的过程中就可以将biconnected component找出来
vector< graph* >* getComponents(const graph* g) 
{
	int i;
	vector< graph* >* components = new vector< graph* >;
	list<int>::iterator it;
	list<int>::iterator ie;
	list< pair<int, int> >* l = new list< pair<int, int> >;
	int pointCount = g->size();
    for (i=0; i<pointCount; i++) 
    {
        colour[i] = WHITE;
        parent[i] = -1;
        depth[i] = 0;
        lowneighbour[i] = MAXINT;
        pointType[i] = NONCUTVERTEX;
    }
    visitTime = -1;
	int childCount;
    
    for (i=0; i<pointCount; i++) 
    {
        if (colour[i] == WHITE) 
        {
            DFSVisit(g, i, l, components);

            childCount = 0;			///对于根结点, 如果它有两个以上的儿子, 则该结点是割点
            it = g->at(i)->begin();
            ie = g->at(i)->end();
            for (; it != ie; it++) 
            {
            	if (parent[*it] == i) 
                {
            		childCount++;
            	}
            }
            if (childCount >= 2) 
            {
            	pointType[i] = CUTVERTEX;
            }
            else 
            {
            	pointType[i] = NONCUTVERTEX;
            }
        }
    }
    delete l;
    return components;
}

///如果g是平面图, 函数返回true, 并且用字符串类返回嵌入平面的方法
bool testPlanarity(const graph* g, string& out) 
{
	vector< graph* >* components;
	graph* component;
	///将图分为若干个biconnected graph, 一个图是平面图当且仅当它的每个component都是平面图
	components = getComponents(g);
	int componentCount = components->size();
	bool isPlane = true;
	string temp;
	for (int i=0; i<componentCount; i++) 
    {
		component = components->at(i);
		pair<int, int> bridge;
		if (!isBridge(component, bridge)) 
        {
			///对每个component进行检测
		 	isPlane = testPlanarityOfBiconnectedGraph(component, temp);
		 	if (!isPlane) 
            {
		 		out="";
		 		return false;
		 	}
		 	out += temp;
		 	temp="";
		}
	}
	destroyGraphs(components);
	return true;
}
int main() 
{
    char c;
    do{
    graph* g;
    g = inputGraph();
    bool isPlane;
	string out;
	isPlane = testPlanarity(g, out);
	if (isPlane) 
    {
		cout << endl << "这是一个平面图." << endl;
		cout << endl;
	}
	else 
    {
		cout << "这是一个非平面图." << endl;
		cout<<endl;
	}
	destroyGraph(g);
	cout<<"是否继续(y|n):"<<endl
    <<"（不区分大小写）"<<endl; 
    cin>>c;
    c=tolower(c);
    assert(c=='y'||c=='n');
       }while(c=='y');
	system("PAUSE");
	return 0;
}

