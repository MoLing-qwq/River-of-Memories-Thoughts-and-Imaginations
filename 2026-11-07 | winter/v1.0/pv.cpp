#include<bits/stdc++.h>
#include<unistd.h>
using namespace std;
mt19937 mt(182376);
inline int ri(int l,int r){return mt()%(r-l+1)+l;}
uniform_real_distribution<double> rd(0.,1.);
const double pi=acos(-1.);
#define eps 1e-8
namespace geometry{
	struct Point2D{
		double x,y;
		inline double len() const{
			return sqrt(x*x+y*y);
		}
		inline friend Point2D operator+(Point2D a,Point2D b){
			return {a.x+b.x,a.y+b.y};
		}
		inline friend Point2D operator-(Point2D a,Point2D b){
			return {a.x-b.x,a.y-b.y};
		}
		inline Point2D operator*(double k) const{
			return {x*k,y*k};
		}
		inline Point2D operator-() const{
			return {-x,-y};
		}
	};

    Point2D rotate(Point2D p,Point2D center,double theta){
		Point2D v=p-center;
		return center+Point2D({
            v.x*cos(theta)-v.y*sin(theta),
            v.x*sin(theta)+v.y*cos(theta)
        });
	}
	inline Point2D zoom(Point2D p,Point2D center,double k){
		return (p-center)*k+center;
	}
    
	typedef Point2D Vec2D;
	inline Vec2D operator*(double k,Vec2D p){
		return {p.x*k,p.y*k};
	}
	inline double dot(Vec2D a,Vec2D b){
		return a.x*b.x+a.y*b.y;
	}
	inline double cross(Vec2D a,Vec2D b){
		return a.x*b.y-a.y*b.x;
	}
	inline bool perpendicular(Vec2D a,Vec2D b){
		// dot = 0
		return fabs(dot(a,b))<eps;
	}
	inline bool parallel(Vec2D a,Vec2D b){
        // cross = 0
		return fabs(cross(a,b))<eps;
	}
	Vec2D normal(Vec2D v){
		double len=v.len();
		if(fabs(len)<eps)return v;
		return {v.x/len,v.y/len};
	}
	
	struct Line2D{
		Vec2D pos,dir; // half plane: cross(dir,v)>0
	};
	Line2D P2_to_L_2D(Point2D a,Point2D b){
		// Direction: a->b
		// a=b handle?
		return {a,normal(b-a)};
	}
	pair<Point2D,int> intersect(Line2D l,Line2D r){
		if(parallel(l.dir,r.dir)){
			if(parallel(l.pos-r.pos,l.dir))return {l.pos,2};
			else return {{0,0},0};
		}
		// l.pos+l.dir*a = r.pos+r.dir*b (on [x,y]-axis)
		// lpx*rdy+ldx*rdy*a=rpx*rdy+rdx*rdy*b
		// lpy*rdx+ldy*rdx*a=rpy*rdx+rdy*rdx*b
		// (ldx*rdy-ldy*rdx)*a = rdx*(lpy-rpy)-rdy*(lpx-rpx)
		return {cross(r.dir,l.pos-r.pos)/cross(l.dir,r.dir)
			*l.dir+l.pos, 1};
	}
}
using namespace geometry;

const int N=3600,M=1080;
const int bg=0xdcf0ff;
const int white=0xfdfdfd;
// text at bg: 0xefefef (light grey)
// text at white: 0xcaeaff (formal value of bg)
int clr[N][M],acc[N][M]; // scene / accumulated snow

constexpr inline int mix(int l,int r,double L){
	int lr=l>>16,lg=(l>>8)&255,lb=l&255;
	int rr=r>>16,rg=(r>>8)&255,rb=r&255;
	int cr=round(lr*L+rr*(1-L));
	int cg=round(lg*L+rg*(1-L));
	int cb=round(lb*L+rb*(1-L));
	return (cr<<16)|(cg<<8)|cb;
}
inline double melt_rate(int c){
	return ((white>>16)-(c>>16))*1./((white>>16)-(bg>>16));
}

void init_canvas(){
	for(int x=0;x<N;++x)for(int y=0;y<M;++y)acc[x][y]=bg;
}

const int Ns=2048;
struct data{
	int p[Ns+1][Ns+1];
}sn[2];
namespace _flake_shape{
	const double r0=134,r1=168;
	const double w1=96, // width of main chain
	             ml=900, // length of main chain (center circle included)
	             al=104; // additional length of thorn
	const double yl[]={220,402,586,762}, // expand from yl
	             yr[]={308,492,672,850}, // expand to yr
				 rL[]={270,450,272,336}, // expansion length at yr
				 eL[]={90,104,92,96}; // extra length at side thorns
	const double a=pi/5; // 36 degrees
	inline bool _in_plane(Point2D s,Point2D t,Point2D p){
		return cross(t-s,p-s)>-eps;
	}
	bool _right(double x,double y){
		double rx=w1/2;
		Vec2D dir={sin(a),cos(a)};
		Point2D l,r,L,R,c,p={x,y};
		for(int i=0;i<4;++i){
			l={rx,yl[i]},r={rx,yr[i]};
			R=r+dir*rL[i];
			L=l+dir*(rL[i]+cos(a)*(yr[i]-yl[i]));
			c=(L+R)*0.5+dir*eL[i];
			if(_in_plane(l,L,p)
			 &&_in_plane(L,c,p)
			 &&_in_plane(c,R,p)
			 &&_in_plane(R,r,p)
			)return true;
		}
		return false;
	}
	bool _fy(double x,double y){
		if(y<0)return false;
		if(fabs(x)<w1/2+eps){
			if(y<ml+eps)return true;
			if(y>ml+al+eps)return false;
			double dx=(ml+al-y)/al*w1/2;
			return fabs(x)<dx+eps;
		}
		else return _right(x,y)||_right(-x,y);
	}
	bool _inside(double x,double y){
		double d=sqrt(x*x+y*y);
		if(d<r0-eps)return false;
		if(d<r1+eps)return true;
		for(int i=0;i<6;++i){
			Point2D p=rotate({x,y},{0,0},pi/3*i);
			if(_fy(p.x,p.y))return true;
		}
		return false;
	}
}
void _generate_sn0(){
	for(int px=0;px<=Ns;++px)for(int py=0;py<=Ns;++py){
		if(_flake_shape::_inside(px-1024,py-1024))sn[0].p[px][py]=1;
		else sn[0].p[px][py]=0;
	}
}
void _generate_sn1(){
	for(int px=0;px<=Ns;++px)for(int py=0;py<=Ns;++py){
		if(_flake_shape::_inside(px-1024,py-1024))sn[1].p[px][py]=1;
		else sn[1].p[px][py]=0;
	}
}
void init_data(){
	_generate_sn0();
	_generate_sn1();
}

const double vy=8.04;
double wind;
struct flake{
	double siz; // length of side of boundary
	double x,y; // center
	double a; // rotation angle
	double vx; // velocity on x-axis
	int type; // shape of snowflake
};
vector<flake> flakes,nxt;

void paint(const flake&s,int c[N][M]){
	int lx=floor(s.x-s.siz/2),rx=ceil(s.x+s.siz/2),ly=floor(s.y-s.siz/2),ry=ceil(s.y+s.siz/2);
	double ss=sin(-s.a),cs=cos(-s.a);
	for(int X=lx;X<=rx;++X)for(int Y=ly;Y<=ry;++Y){
		if(X<0||X>=N||Y<0||Y>=M)continue;
		int _px=(X-s.x)/s.siz*2048,_py=(Y-s.y)/s.siz*2048;
		int px=cs*_px-ss*_py+1024,py=ss*_px+cs*_py+1024;
		if(px<0||px>Ns||py<0||py>Ns)continue;
		if(sn[s.type].p[px][py])c[X][Y]=white;
	}
}
struct _graph_node{
	double m;
	int x,y;
	inline friend bool operator<(const _graph_node&s,const _graph_node&t){return s.m<t.m;}
};
void melt(){
	static double mr[N][M],fr[N][M];
	for(int x=0;x<N;++x)for(int y=0;y<M;++y)mr[x][y]=melt_rate(acc[x][y]);
	int maxy=M-1; // assume snow does not accumulate to the top
	while(maxy>0){
		bool ok=1;
		for(int x=0;x<N;++x)if(fabs(mr[x][maxy-1])<1e-8){ok=0;break;}
		if(!ok)break;
		--maxy;
	}
	if(maxy==0)return;
	
	priority_queue<_graph_node> pq;
	static bool done[N][M];
	for(int x=0;x<N;++x)for(int y=0;y<M;++y)fr[x][y]=0,done[x][y]=0;
	for(int x=0;x<N;++x)fr[x][maxy]=1,pq.push({1,x,maxy});
	while(pq.size()){
		_graph_node nd=pq.top();pq.pop();
		if(done[nd.x][nd.y])continue;
		done[nd.x][nd.y]=1;
		if(nd.m<0.0196)continue;
		int nx,ny;
		double ratio=min(0.7+(maxy-nd.y)*1./1107,1.);
		double mtp=mr[nd.x][nd.y]*ratio+(1-ratio);
		nx=nd.x-1,ny=nd.y;
		if(nx>=0&&!done[nx][ny]&&nd.m*mtp>fr[nx][ny])
			fr[nx][ny]=nd.m*mtp,pq.push({nd.m*mtp,nx,ny});
		nx=nd.x+1,ny=nd.y;
		if(nx<N&&!done[nx][ny]&&nd.m*mtp>fr[nx][ny])
			fr[nx][ny]=nd.m*mtp,pq.push({nd.m*mtp,nx,ny});
		nx=nd.x,ny=nd.y-1;
		if(ny>=0&&!done[nx][ny]&&nd.m*mtp>fr[nx][ny])
			fr[nx][ny]=nd.m*mtp,pq.push({nd.m*mtp,nx,ny});
	}
	for(int x=0;x<N;++x)for(int y=0;y<M;++y){
		mr[x][y]=1-((1-mr[x][y])*(1-fr[x][y]));
		acc[x][y]=mix(bg,white,mr[x][y]);
	}
}

void generate_flakes(int cnt){
	static std::uniform_real_distribution<double> siz(4.8,20.6);
	static std::uniform_real_distribution<double> x(-420,3420);
	static std::uniform_real_distribution<double> y(1120,1280);
	static std::uniform_real_distribution<double> a(-pi,pi);
	while(cnt--){
		flakes.push_back({siz(mt),x(mt),y(mt),a(mt),0,mt()&1});
	}
}
double calc_dvx(const flake&s){
	double F_wind=wind;
	double F_re=-pow(s.siz,1.3)*s.vx;
	return (F_wind+F_re)/pow(s.siz,1.5)*1.;
}
bool grounded(const flake&s,double c){
	int lx=floor(s.x-s.siz/2),rx=ceil(s.x+s.siz/2),ly=floor(s.y-s.siz/2),ry=ceil(s.y+s.siz/2);
	if(ry<0)return true;
	int cy=0;
	double avg=0;
	for(int X=lx;X<=rx;++X)for(int Y=ly;Y<=ry;++Y){
		if((X-s.x)*(X-s.x)+(Y-s.y)*(Y-s.y)<s.siz*s.siz/4+eps){
			double w=1+(4.-1)*(Y-ly)/(ry-ly);
			cy+=1.*w;
			if(Y<0){avg+=1;continue;}
			if(X<0||X>=N||Y>=M)continue;
			double r=1-melt_rate(acc[X][Y]);
			avg+=r*r*w;
		}
	}
	if(cy==0)return false;
	avg/=cy;
	return avg>=c;
}
void move(){
	nxt.clear();
	for(const flake&_s:flakes){
		flake s=_s;
		s.y-=vy;
		s.x+=s.vx;
		s.vx+=calc_dvx(s);
		s.a+=2.68;
		if(grounded(s,0.15+0.0125*s.siz))paint(s,acc);//,cerr<<"Grounded "<<s.siz/2+s.y<<endl;
		else nxt.push_back(s);
	}
	swap(flakes,nxt);
}
void draw(){
	for(int x=0;x<N;++x)for(int y=0;y<M;++y)clr[x][y]=acc[x][y];
	for(const flake&s:flakes)paint(s,clr);
}

void cyro_growth(){
	static const int dx[]={0,-1,0,1};
	static const int dy[]={-1,0,1,0};
	static std::uniform_real_distribution<double> rnd(0,53.6);
	int maxy=M-1;
	while(maxy>=0){
		bool ok=1;
		for(int x=0;x<N;++x)if(acc[x][maxy]!=bg){ok=0;break;}
		if(!ok)break;
		--maxy;
	}
	if(maxy<0)return; 
	for(int x=0;x<N;++x)for(int y=0;y<=maxy;++y){
		if(acc[x][y]==white)continue;
		double c=0,r;int nx,ny,ad=4;
		for(int i=0;i<4;++i){
			nx=x+dx[i],ny=y+dy[i];
			if(nx<0||nx>=N||ny<0||ny>=M){--ad;continue;}
			r=melt_rate(acc[nx][ny]);
			c+=(1-r)*(i?1:2);
		}
		if(rnd(mt)<c*4/ad)acc[x][y]=white;
	}
}
void gravity(){
	for(int x=0;x<N;++x){
		int ptr=0;
		for(int y=0;y<M;++y)if(acc[x][y]!=bg)
			acc[x][ptr++]=acc[x][y];
		while(ptr<M)acc[x][ptr++]=bg;
	}
}
void flatten(){
	if(fabs(wind)<eps)return;
	static int maxy[N];
	for(int x=0;x<N;++x){
		int y=M-1;
		while(y>=0&&acc[x][y]==bg)--y;
		maxy[x]=y;
	}
	static std::uniform_real_distribution<double> p(0,1);
	if(wind>0){
		double pr=min(1.,pow(wind,0.6)/30);
		for(int x=N-1;x>=0;--x){
			if(maxy[x]<0)continue;
			if(p(mt)>pr)continue;
			if(x==N-1){acc[x][maxy[x]]=bg;--maxy[x];continue;}
			if(acc[x+1][maxy[x]]!=bg)continue;
			// maxy[x+1] < M-1
			acc[x+1][maxy[x+1]+1]=acc[x][maxy[x]];
			acc[x][maxy[x]]=bg;--maxy[x];
		}
	}
	else{
		double pr=min(1.,pow(-wind,0.6)/30);
		for(int x=0;x<=N-1;++x){
			if(maxy[x]<0)continue;
			if(p(mt)>pr)continue;
			if(x==0){acc[x][maxy[x]]=bg;--maxy[x];continue;}
			if(acc[x-1][maxy[x]]!=bg)continue;
			// maxy[x-1] < M-1
			acc[x-1][maxy[x-1]+1]=acc[x][maxy[x]];
			acc[x][maxy[x]]=bg;--maxy[x];
		}
	}
}
void bubble(){
	for(int x=0;x<=N-1;++x){
		for(int y=M-1;y>=1;--y)if(melt_rate(acc[x][y])<melt_rate(acc[x][y-1]))
			swap(acc[x][y],acc[x][y-1]);
	}
}

const double bpm = 64.8;

// as 64
// [weak start 1 3.75s] [intro 8 30s] [1.1 8 30s] [1.0 8 30s] [4+1+8 48.75s]
// [2.1 8 30s] [2.2 8 30s] [2.0 8 30s] [8+8 60s] [3.2 8 30s] [4.2 8 30s] [outro 8+2 37.5s] [8s+]
// as 64: 6:38-, as 64.8: 6:34, frames = 11820
// 111, 1000, 1889, 2778, 4222
// 5111, 6000, 6889, 8667, 9556, 10444, 11556, 11820
// snow:
// [light] [light] [light] [medium] [light(melt)]
// [medium(melt)] [heavy] [heavy] [medium(melt++)] [light] [heavy] [light(melt++)] [light(melt++)] 
// wind:
// [gentle] [gentle] [gentle] [gentle] [gentle]
// [medium] [gentle] [strong] [strong] [light] [medium] [light] [light]

double _light_r[1<<16];

void set_light(){
	// implement
	for(int i=1;i<=11820;++i)_light_r[i]=1;
	for(int i=1;i<=168;++i)_light_r[i]=0.5-0.5*cos(pi*(i-0.)/(168.-0.));
	for(int i=11556;i<=11725;++i)_light_r[i]=0.5+0.5*cos(pi*(i-11555.)/(11725.-11555.));
	for(int i=11726;i<=11820;++i)_light_r[i]=0;
}

double t_wind[1<<16],v_wind,a_wind;

void s_wind(){
	// gentle 40
	// medium 80
	// strong 140
	for(int i=1;i<=4120;++i)t_wind[i]=40;
	for(int i=4121;i<=5040;++i)t_wind[i]=92;
	for(int i=5041;i<=6000;++i)t_wind[i]=48;
	for(int i=6001;i<=8640;++i)t_wind[i]=162;
	for(int i=8641;i<=9525;++i)t_wind[i]=40;
	for(int i=9526;i<=10444;++i)t_wind[i]=92;
	for(int i=10445;i<=10454;++i)t_wind[i]=92-1.16*(i-10444);
	for(int i=10455;i<=11820;++i)t_wind[i]=max(80.4-2.68*sqrt((i-10455)*0.8+1e-4)+(i-10455.)/100,1e-4);
	for(int i=1;i<=10400;++i){
		if((i/130) % 2) t_wind[i] -= 4;
		else  t_wind[i] += 4;
	}
	
	v_wind=26.8,a_wind=0.4;
}

const double co_wind = 0.04;

double c_wind(int t){
	v_wind+=a_wind;//cerr<<v_wind<<" "<<a_wind<<endl;
	double tg=(t<=0?40:t_wind[t]),mx=tg*1.1+12,mn=tg*0.9-12;
	double after_1s=v_wind+30*a_wind;
	if(after_1s>=mx)a_wind -= rd(mt) * co_wind;
	else if(after_1s<=mn)a_wind += rd(mt) * co_wind;
	else{
		double c = rd(mt), r = (after_1s-mn) / (mx-mn);
		a_wind += (c-r) * co_wind;
	}
	return max(v_wind,0.);
}

// as 64
// [weak start 1 3.75s] [intro 8 30s] [1.1 8 30s] [1.0 8 30s] [4+1+8 48.75s]
// [2.1 8 30s] [2.2 8 30s] [2.0 8 30s] [8+8 60s] [3.2 8 30s] [4.2 8 30s] [outro 8+2 37.5s] [8s+]
// as 64: 6:38-, as 64.8: 6:34, frames = 11820
// 111, 1000, 1889, 2778, 4222
// 5111, 6000, 6889, 8667, 9556, 10444, 11556, 11820
// snow:
// [light] [light] [light] [medium] [light(melt)]
// [medium(melt)] [heavy] [heavy] [medium(melt++)] [light] [heavy] [light(melt++)] [light(melt++)] 
// wind:
// [gentle] [gentle] [gentle] [gentle] [gentle]
// [medium] [gentle] [strong] [strong] [light] [medium] [light] [light]

int t_cflakes[1<<16];

void s_cflakes(){
	const int delta=42;
	for(int i=1;i<=1889-delta;++i)t_cflakes[i]=12;
	for(int i=1890-delta,j=1;i<=2778-delta;++i,++j)t_cflakes[i]=min(12+j/16,28);
	for(int i=2779-delta,j=1;i<=4222-delta;++i,++j)t_cflakes[i]=max(28-j/16,15);
	for(int i=4223-delta,j=1;i<=5111-delta;++i,++j)t_cflakes[i]=min(15+j/16,26);
	for(int i=5112-delta,j=1;i<=6000-delta;++i,++j)t_cflakes[i]=min(26+j/16,44);
	for(int i=6001-delta,j=1;i<=6889-delta;++i,++j)t_cflakes[i]=min(44+j/16,48);
	for(int i=6890-delta,j=1;i<=7777-delta;++i,++j)t_cflakes[i]=max(48-j/16,28);
	for(int i=7778-delta,j=1;i<=8667-delta;++i,++j)t_cflakes[i]=max(28-j/16,24);
	for(int i=8668-delta,j=1;i<=9556-delta/2*3;++i,++j)t_cflakes[i]=max(24-j/16,14);
	for(int i=9557-delta/2*3,j=1;i<=10444-delta;++i,++j)t_cflakes[i]=min(14+j/4,48);
	for(int i=10445-delta,j=1;i<=11820;++i,++j)t_cflakes[i]=max(48-j/7,21-(i-10445)/64);
}

int c_cflakes(int t){
	return t<=0?18:t_cflakes[t];
}

bitset<1<<16> t_melt;

void s_melt(){
	for(int i=1;i<=11820;++i)t_melt[i]=0;
	int p=2779 + 92;
	for(int d=16;d>8;--d)for(int c=1;c<=4;++c)p+=d,t_melt[p]=1;
	while(p<5111){
		p+=ri(7,11);
		t_melt[p]=1;
	}
	for(int d=9;d<=16;++d)for(int c=1;c<=2;++c)p+=ri(d,d+4),t_melt[p]=1;
	
	p=6889 - 92;
	for(int d=12;d>=6;--d)for(int c=1;c<=4;++c)p+=d,t_melt[p]=1;
	while(p<8667){
		p+=5;
		t_melt[p]=1;
		p+=4;
		t_melt[p]=1;
	}
	for(int d=6;d<=12;++d)for(int c=1;c<=4;++c)p+=ri(d,d+4),t_melt[p]=1;
	
	p=10444 - 42;
	for(int d=12;d>4;--d)for(int c=1;c<=4;++c)p+=d,t_melt[p]=1;
	while(p<11820){
		p+=4;
		t_melt[p]=1;
	}
}

bool c_melt(int t){
	return t<=0?false:t_melt[t];
}

bool c_cyro(int t){
	if(t<5124||t>6874)return t%14==0&&t>=268;
	if(t<=5254)return t%13==5254%13;
	if(t>=6744)return t%13==6744%13;
	if(t<=5374)return t%12==5374%12;
	if(t>=6624)return t%12==6624%12;
	if(t<=5484)return t%11==5484%11;
	if(t>=6514)return t%11==6514%11;
	return t%10==4;
}

int c_flatten(int t){
	int cnt=2;
	if((5111<=t&&t<=8667&&t%2==0)||(9556<=t&&t<=10444&&t%4==0))++cnt;
	if(c_melt(t-1))++cnt;
	return cnt;
}

void init_animation(){
	set_light();
	s_wind();
	s_cflakes();
	s_melt();
}

double capture_next(int t){
	// returns lightness
	//wind+=4*sin(t*0.04);
	wind=c_wind(t);
	generate_flakes(c_cflakes(t));
	move();
	if(c_melt(t))melt();
	if(c_cyro(t))cyro_growth();
	if(t%4==0)gravity();
	int cf=c_flatten(t);
	for(int i=0;i<cf;++i)flatten();
	bubble();
	draw();
	if(t<=0)return 1;
	return _light_r[t];
}


const int WIDTH = 1920;
const int HEIGHT = 1080;
const int FPS = 30;
const int TOTAL_FRAMES = 11820;

int wcp[WIDTH][HEIGHT];
std::vector<uint8_t> rgbBuffer(WIDTH * HEIGHT * 3);

/*int main() {
	init_data();
	init_canvas();
	init_animation();
	FILE *_log_=fopen("flake","w");
	for(int i=0;i<=Ns;++i)for(int j=0;j<=Ns;++j){
		fprintf(_log_,"%d",sn[0].p[i][j]);
		if(j==Ns)fprintf(_log_,"\n");
	}
	fclose(_log_);
	// Written by DeepSeek 
    int t = 0;
    
    HANDLE hReadPipe, hWritePipe;
    SECURITY_ATTRIBUTES sa = { sizeof(SECURITY_ATTRIBUTES), nullptr, TRUE };
    if (!CreatePipe(&hReadPipe, &hWritePipe, &sa, 0)) {
        fprintf(stderr, "CreatePipe failed\n");
        return 1;
    }
    SetHandleInformation(hReadPipe, HANDLE_FLAG_INHERIT, HANDLE_FLAG_INHERIT);
    SetHandleInformation(hWritePipe, HANDLE_FLAG_INHERIT, 0);
    char cmdLine[] = "ffmpeg -y -f rawvideo -pixel_format rgb24 -video_size 1920x1080 -framerate 30 -i - -c:v libx265 -preset medium -crf 23 -pix_fmt yuv420p output.mp4";
    STARTUPINFOW si = { sizeof(si) };
    PROCESS_INFORMATION pi = {};
    si.dwFlags = STARTF_USESTDHANDLES;
    si.hStdInput = hReadPipe;
    si.hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
    si.hStdError = GetStdHandle(STD_ERROR_HANDLE);
    wchar_t wCmdLine[512];
    MultiByteToWideChar(CP_UTF8, 0, cmdLine, -1, wCmdLine, 512);
    BOOL success = CreateProcessW(
        nullptr,
        wCmdLine,
        nullptr, nullptr,
        TRUE,
        CREATE_NO_WINDOW,
        nullptr, nullptr,
        &si, &pi
    );
    if (!success) {
        fprintf(stderr, "CreateProcess failed (error %lu)\n", GetLastError());
        CloseHandle(hReadPipe);
        CloseHandle(hWritePipe);
        return 1;
    }
    CloseHandle(hReadPipe);
    
    int preset = 112;
	t = -preset;
    
    for (int frameIdx = -preset; frameIdx < TOTAL_FRAMES; ++frameIdx) {
        double light = capture_next(++t);
        if(light<0 || light>1)
			fprintf(stderr, "Warning: frame %d, light = %.12lf\n",
                    frameIdx, light);
        
        if(t<=0) continue;

        size_t idx = 0;
        for (int y = 0; y < HEIGHT; ++y) {
            for (int x = 0; x < WIDTH; ++x) {
                int pixel = clr[x + 540][M - 1 - y];
                rgbBuffer[idx++] = (pixel >> 16) & 0xFF;
                rgbBuffer[idx++] = (pixel >> 8)  & 0xFF;
                rgbBuffer[idx++] =  pixel        & 0xFF;
            }
        }
        
        for(unsigned i = 0; i < idx; ++i)
			rgbBuffer[i]=floor(rgbBuffer[i]*light);
        
        DWORD bytesWritten = 0;
        if (!WriteFile(hWritePipe, rgbBuffer.data(), rgbBuffer.size(), &bytesWritten, nullptr)) {
            fprintf(stderr, "WriteFile failed at frame %d\n", frameIdx);
            break;
        }
        if (bytesWritten != rgbBuffer.size()) {
            fprintf(stderr, "Partial write at frame %d\n", frameIdx);
            break;
        }
        cerr<<"Frame "<<frameIdx+1<<endl;
        
        if((frameIdx+1) % 402 == 0) Sleep(4000);
    }
    CloseHandle(hWritePipe);
    WaitForSingleObject(pi.hProcess, INFINITE);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    printf("Finished: output.mp4\n");
    return 0;
}*/

int main() {
    init_data();
    init_canvas();
    init_animation();
    FILE *_log_ = fopen("flake", "w");
    for (int i = 0; i <= Ns; ++i) for (int j = 0; j <= Ns; ++j) {
        fprintf(_log_, "%d", sn[0].p[i][j]);
        if (j == Ns) fprintf(_log_, "\n");
    }
    fclose(_log_);

    // Written by DeepSeek
    int t = 0;

    signal(SIGPIPE, SIG_IGN);

    const char* cmd =
        "ffmpeg -y -loglevel error -nostats "
        "-f rawvideo -pixel_format rgb24 -video_size 1920x1080 "
        "-framerate 30 -i - "
        "-c:v libx265 -preset medium -crf 23 -pix_fmt yuv420p output.mp4";

    FILE* pipe = popen(cmd, "w");
    if (!pipe) {
        fprintf(stderr, "popen failed: %s\n", strerror(errno));
        return 1;
    }

	int preset = 112;
	t = -preset;

    for (int frameIdx = -preset; frameIdx < TOTAL_FRAMES; ++frameIdx) {
        double light = capture_next(++t);
        if(light<0 || light>1)
			fprintf(stderr, "Warning: frame %d, light = %.12lf\n",
                    frameIdx, light);
        
        if(t<=0) continue;

        size_t idx = 0;
        for (int y = 0; y < HEIGHT; ++y) {
            for (int x = 0; x < WIDTH; ++x) {
                int pixel = clr[x + 540][M - 1 - y];
                wcp[x][y] = pixel;
            }
        }
        for (int y = 0; y < HEIGHT; ++y) {
            for (int x = 0; x < WIDTH; ++x) {
                int pixel = wcp[x][y];
                // if(x%120==0||y%120==0)pixel=0;
                rgbBuffer[idx++] = (pixel >> 16) & 0xFF;
                rgbBuffer[idx++] = (pixel >> 8)  & 0xFF;
                rgbBuffer[idx++] =  pixel        & 0xFF;
            }
        }
        
        for(size_t i=0;i<idx;++i)rgbBuffer[i]=floor(rgbBuffer[i]*light);

        size_t written = fwrite(rgbBuffer.data(), 1, rgbBuffer.size(), pipe);
        if (written != rgbBuffer.size()) {
            fprintf(stderr, "fwrite failed at frame %d (%zu/%zu bytes)\n",
                    frameIdx, written, rgbBuffer.size());
            break;
        }
        fprintf(stderr, "Frame %d\n", frameIdx + 1);
        
        if((frameIdx+1) % 402 == 0) sleep(4);
    }

    int status = pclose(pipe);
    if (status == -1) {
        fprintf(stderr, "pclose failed: %s\n", strerror(errno));
        return 1;
    }
    if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
        fprintf(stderr, "ffmpeg exited abnormally (status=%d)\n", status);
        return 1;
    }

    printf("Finished: output.mp4\n");
    return 0;
}
