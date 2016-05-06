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

///ͼ, ��Ƭ, ��, ���Լ���·�Ĵ洢�ṹ
typedef vector< list<int>* > graph;
typedef vector< list<int>* > fragment;
typedef list<int> circle;		///���������ʾ, ����˳ʱ���¼���ĵ�
typedef list<int> face;		///���������ʾ����, ����˳ʱ���¼��ĵ�
typedef list<int> path;

const int MAXINT = ((unsigned)-1)>>1;
const int MAXPOINTCOUNT = 200;

int colour[MAXPOINTCOUNT];		///Ϊ�������õ�����
const int WHITE = 0;
const int GRAY = 1;
const int BLACK = 2;

int parent[MAXPOINTCOUNT];		///Ϊ�������õ�����
int depth[MAXPOINTCOUNT];

int pointKind[MAXPOINTCOUNT];
const int EMBEDED = -1;
const int AVAILABLE = 0;

bool pointEmbed[MAXPOINTCOUNT];
bool edgeEmbed[MAXPOINTCOUNT][MAXPOINTCOUNT];

int lowpoint[MAXPOINTCOUNT];		///Ϊ�Ҹ�����õ�����
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
	printf("�����붥�������");
	 cin >> pointCount;
	printf("\n������ߵ�����: ");
	cin >> edgeCount;
    graph *g = createGraph(pointCount);
    cout<<"\n������ͼ����Ϣ:"<<endl;
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
    int pointCount = g->size();//ͼ�Ľڵ���
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
	
	///��ͼ����һ�ι�����ȱ����������ҵ�һ����
	while (!q.empty()) 
    {
		u = q.front(); q.pop();
		tra = g->at(u)->begin();
		end = g->at(u)->end();
		for (; tra != end; tra++) 
        {
			v = *tra;//�����ò���
			if (colour[v] == WHITE) 
            {
				q.push(v);
				colour[v] = GRAY;
				parent[v] = u;
				depth[v] = depth[u]+1;
			}
			///�����������ĳһƬ��Ҷ���ڶ����е��ھ�, ������ҵ�һ����
			else if (colour[v] == GRAY) 
            {
				goto buildTheCircle;
			}
		}
		colour[u] = BLACK;
	}
	assert(!q.empty());

///����Ҷ���u���ھ�v���ݾͿ����ҵ�һ����
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
	///������ȱ�����fragment,
	///��fragment�ֳ�����: 1����ĩǶ�뵽ƽ��Ľ���, 2������...��
	for (i=0; i<pointCount; i++) 
    {
		if (colour[i] == WHITE) 
        {		///fragment�п��ܲ�ֻһ��
			if (pointKind[i] == AVAILABLE) 
            {		///��ĩǶ��ƽ��Ľ���������
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
							///����ھӽ���Ѿ�Ƕ��ƽ��,
							///��ֻ���߼��뵽fragment�������(��ֹ�Ӵ���������ı���);
							else if (pointKind[v] == EMBEDED) 
                            {
								f->at(u)->push_back(v);
								f->at(v)->push_back(u);
							}
						}
						///����ھӽ���ڶ����л��߽���Ѿ�Ƕ��ƽ��, 
						///��ֻ���߼��뵽fragment�������(��ֹ�Ӵ���������ı���);
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
			///�����������Ѿ�Ƕ��ƽ��, ��ֻ����Щ�������Ѿ�Ƕ��ƽ��Ľ���fragment
			///��Щfragmentֻ��һ����, edgeEmbed[]�������ṩ�Ѿ�Ƕ��ƽ��ıߵ���Ϣ��
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
    face* tempFace = new face;			///���������ʾ����, ����˳ʱ���¼��ĵ�
    tempFace->push_back(MAXINT);		///��MAXINT(��������)����������
	circle::iterator ctra = c->begin();
	circle::iterator cend = c->end();
	for (; ctra != cend; ctra++) 
    {
		tempFace->push_back(*ctra);
	}
	faces->push_back(tempFace);
	faces->push_back(c);
	c = NULL;			///circle c�Ѿ�ת��Ϊ����һ��face, ��ֹͨ��ָ��c�ı���
	return faces;
}

///����Ѿ�Ƕ�뵽ƽ��ĵ�
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
///ÿ�ν�alpha pathǶ�뵽ƽ��ʱ, �����Ѿ�Ƕ�뵽ƽ��Ľ��
void addPoint(const path *alphaPath) 
{
	path::const_iterator ptra = alphaPath->begin();
	path::const_iterator pend = alphaPath->end();
	for (; ptra != pend; ptra++) 
    {
		pointEmbed[*ptra] = true;
	}
}

///����Ѿ�Ƕ�뵽ƽ��ı�
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
///ÿ�ν�alpha pathǶ�뵽ƽ��ʱ�����Ѿ�Ƕ�뵽ƽ��ı�
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
	

///fragment���Ѿ�Ƕ��ƽ�����ͼ�Ĺ��������contact point
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
///��һ��face�����������е�
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
		///faceֻ�а���fragment������contact point, ������admissible face
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
///�������fragmentû��admissible face, �ͷ���false, ���򷵻�true
///�����Ͳ���embedFaceIterator, embedFragmentIterator��������Ƕ����ͽ�ҪǶ�뵽�����Ƭ
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
		///��ÿһ��fragmentѰ����admissible face
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
		///��һ��fragmentû��admissible face, ���ͼ�Ͳ���ƽ��ͼ
		if (admissibleFaceCount == 0) 
        {
            //cout << "not planarity graph" << endl;
			return false;
		}
		///�����һ��fragmentֻ��һ��admissible face, ������Ƚ���fragmentǶ��
		else if (admissibleFaceCount == 1) 
        {
			embedFaceIterator = admissibleFaceIterator;
			embedFragmentIterator = frtra;
			foundEmbedPair = true;	///�������������Ƿ��ҵ�ֻ��һ��adimissible face��fragment
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
///������ȱ���, ��һ��contact point�����ҵ���һ��contact point���ɽ���һ��alpha path
///�������fragment��ȥ����Ӧ��alpha path;
path* getAlphaPath(fragment *f) 
{
	bool *contactPoint = findContactPoint(f);
	fragment::iterator ftra = f->begin();
	fragment::iterator fend = f->end();
	int start=0;
	for (; ftra != fend; ftra++, start++) 
    {		///�ҵ�alpha�ĳ�����
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
	///������ȱ���
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
                {		///�ҵ�һ��contact point�����Խ���alpha path
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
		removeEdge(f, tra, parent[tra]);		///��fragment��ȥ����alpha path��ı�
		tra = parent[tra];
	}
	p->push_back(start);
	delete[] contactPoint;
	return p;
}

///ȥ��alpha֮�����fragment�Ƿ�Ϊ��ͼ
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

//��alpha paph��һ����Ƕ�뵽ƽ��, Ƕ���Ƕ����潫��ı�, �������شӱ�Ƕ�����ָ����������
face* splitFace(face *embedFace, path*& alphaPath) 
{
	int start = alphaPath->front();
	int end = alphaPath->back();
	
	face::iterator ftra = embedFace->begin();
	face::iterator fend = embedFace->end();
	face::iterator embedPoint1, embedPoint2;
	///����alpha path�ķ����Է���Ƕ��ƽ��
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
	///Ƕ��֮��Ҫ��֤��ı�ʾ��Ȼ�ǵ��˳ʱ���ʾ
	///Ƕ��Ӧ��Ƕ�뵽����ڲ��Ա�֤��Ӱ��������, ע��"����"���ڲ�������
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
///����������, �����Ҳ����õĺ�����, �����������getComponents()����������֮���õ�
///getComponents()������ĳ��component���ֻ��������, ��ô���component��һ��bridge
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
			
			///�����жϸ��.
			///lowpoint[v]��v��������������ھӽ���������С�ĵ�����
			///lowneighbour[v]��v�������ھӽ���������С�ĵ�����
			if (lowpoint[v] < tempLowpoint) 
            {
				tempLowpoint = lowpoint[v];
			}
			if (lowneighbour[v] < tempLowpoint) 
            {
				tempLowpoint = lowneighbour[v];
			}
			
			///һ����u�Ǹ�㵱�ҽ�������ĳ������v��lowpoint[v]����lowneghbour[v]>=depth[u];
	        ///���u�Ǹ��, ������vΪ����������ĩ����ǰһcomponent�����б��Լ���Щ�ߵĽ���ߴ���һ��component
			if (lowpoint[v] >= depth[u] && lowneighbour[v] >= depth[u]) 
            {
				pointType[u] = CUTVERTEX;
				component = createComponent(pointCount, ul);
				components->push_back(component);
				component = NULL;
			}
			///���u���Ǹ��, ��v����ĩ����ǰһcomponent�����б��Լ���Щ�ߵĽ���߼��뵽u������
			else 
            {
				l->splice(l->end(), *ul);
			}
        }
        ///������������Ľ����Ҳ���뵽���ڹ���component��u������
        else if (colour[v] == GRAY) 
        {
        	l->push_back(pair<int, int>(u, v));
        }
        
        ///���ڸ���lowneighbour;
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
///��ͼ���Ϊ���ɸ�biconne component;
//����������������Ҹ��Ĺ����оͿ��Խ�biconnected component�ҳ���
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

            childCount = 0;			///���ڸ����, ��������������ϵĶ���, ��ý���Ǹ��
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

///���g��ƽ��ͼ, ��������true, �������ַ����෵��Ƕ��ƽ��ķ���
bool testPlanarity(const graph* g, string& out) 
{
	vector< graph* >* components;
	graph* component;
	///��ͼ��Ϊ���ɸ�biconnected graph, һ��ͼ��ƽ��ͼ���ҽ�������ÿ��component����ƽ��ͼ
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
			///��ÿ��component���м��
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
		cout << endl << "����һ��ƽ��ͼ." << endl;
		cout << endl;
	}
	else 
    {
		cout << "����һ����ƽ��ͼ." << endl;
		cout<<endl;
	}
	destroyGraph(g);
	cout<<"�Ƿ����(y|n):"<<endl
    <<"�������ִ�Сд��"<<endl; 
    cin>>c;
    c=tolower(c);
    assert(c=='y'||c=='n');
       }while(c=='y');
	system("PAUSE");
	return 0;
}

