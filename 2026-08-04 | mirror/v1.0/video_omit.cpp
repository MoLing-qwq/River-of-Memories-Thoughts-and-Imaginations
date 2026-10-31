// Some data in the code are omitted, and they are replaced by ellipses and code comments.
// Search "omitted" in the code for more information.
#include<bits/stdc++.h>
#include<windows.h>
using namespace std;
#define eps 1e-8
namespace geometry{
	inline bool proportionate(double a,double b,double c,double d){
		return ((fabs(a)<eps||fabs(d)<eps)&&(fabs(b)<eps||fabs(c)<eps))||fabs(a/b-c/d)<eps;
	}
	
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
		return proportionate(a.x,b.x,a.y,b.y);
	}
	Vec2D normal(Vec2D v){
		double len=v.len();
		if(fabs(len)<eps)return v;
		return {v.x/len,v.y/len};
	}
	
	struct Line2D{
		Vec2D pos,dir;
	};
	Line2D P2_to_L_2D(Point2D a,Point2D b){
		// Direction: a->b
		// a=b handle?
		return {a,normal(b-a)};
	}
	pair<Vec2D,int> intersect(Line2D l,Line2D r){
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
	Point2D rotate(Point2D p,Point2D center,double theta){
		Point2D v=p-center;
		return center+Point2D({v.x*cos(theta)-v.y*sin(theta),v.x*sin(theta)+v.y*cos(theta)});
	}
	inline Point2D zoom(Point2D p,Point2D center,double k){
		return (p-center)*k+center;
	}
	
	struct Point3D{
		double x,y,z;
		inline double len() const{
			return sqrt(x*x+y*y+z*z);
		}
		inline friend Point3D operator+(Point3D a,Point3D b){
			return {a.x+b.x,a.y+b.y,a.z+b.z};
		}
		inline friend Point3D operator-(Point3D a,Point3D b){
			return {a.x-b.x,a.y-b.y,a.z-b.z};
		}
		inline Point3D operator*(double k) const{
			return {x*k,y*k,z*k};
		}
		inline Point3D operator-() const{
			return {-x,-y,-z};
		}
	};
	typedef Point3D Vec3D;
	inline Vec3D operator*(double k,Vec3D p){
		return {p.x*k,p.y*k,p.z*k};
	}
	inline double dot(Vec3D a,Vec3D b){
		return a.x*b.x+a.y*b.y+a.z*b.z;
	}
	inline bool perpendicular(Vec3D a,Vec3D b){
		// dot = 0
		return fabs(dot(a,b))<eps;
	}
	inline bool parallel(Vec3D a,Vec3D b){
		return proportionate(a.x,b.x,a.y,b.y)&&proportionate(a.y,b.y,a.z,b.z);
	}
	Vec3D normal(Vec3D v){
		double len=v.len();
		if(fabs(len)<eps)return v;
		return {v.x/len,v.y/len,v.z/len};
	}
	
	struct Line3D{
		Vec3D pos,dir;
	};
	Line3D P2_to_L_3D(Point3D a,Point3D b){
		// Direction: a->b
		// a=b handle?
		return {a,normal(b-a)};
	}
}
using namespace geometry;

namespace Rand{
	mt19937 mt(182376);
	inline int randint(int l,int r){
		return mt()%(r-l+1)+l;
	}
}
using Rand::randint;

constexpr int dim(int clr,double k){
	// 0<=r<=1
	int r=(clr>>16)&255,g=(clr>>8)&255,b=clr&255;
	r=floor(r*k+eps),g=floor(g*k+eps),b=floor(b*k+eps);
	return (r<<16)|(g<<8)|b;
}

struct Shape{
	int _color;
	double _transp;
	
	Shape(){_color=0xffffff,_transp=1;}
	Shape(const Shape &other) = default;
	Shape(Shape &&other) = default;
	virtual unique_ptr<Shape> copy_clone() = 0;
	virtual unique_ptr<Shape> move_clone() = 0;
	virtual ~Shape(){ }
	
	virtual Vec3D intersect(const Line3D&l) const = 0;
	// First (Nearest) intersection from Line.pos, direction Line.dir
	// Point with the wrong direction won't be counted.
	virtual bool on(Vec3D p) const = 0;
	// If not intersect, the return value "vec" of intersect(const Line&)
	// should make sure that "on(vec)" = false.
	// Otherwise the on function determines whether the point isn't transparent/blank here.
	// This is to let intersect function be written easier.
	virtual int getcolor(Vec3D p) const{return dim(_color,_transp);} // need override
	// Somehow the following returns 1 when object isn't transparent at all, while 0 means it cannot be seen...
	// It should be called "intransparency"... Returns a real number in [0,1].
	virtual double gettransparency(Vec3D p) const{return _transp;} // need override
};
struct Shape_parallel_XZ: public Shape{
	Vec3D intersect_y(const Line3D&l,double y) const{
		if(fabs(l.pos.y-y)<eps)return l.pos; // may be on the plane
		if(fabs(l.dir.y)<eps)return {l.pos.x,y+1,l.pos.z}; // don't intersect
		double d=(y-l.pos.y)/l.dir.y;
		return l.pos+l.dir*d;
	}
};
struct Shape_parallel_XY: public Shape{
	Vec3D intersect_z(const Line3D&l,double z) const{
		if(fabs(l.pos.z-z)<eps)return l.pos; // may be on the plane
		if(fabs(l.dir.z)<eps)return {l.pos.x,l.pos.y,z+1}; // don't intersect
		double d=(z-l.pos.z)/l.dir.z;
		return l.pos+l.dir*d;
	}
};
struct XZplane_Gridline: public Shape_parallel_XZ{
	XZplane_Gridline(){ }
	XZplane_Gridline(const XZplane_Gridline &other) = default;
	XZplane_Gridline(XZplane_Gridline &&other) = default;
	
	inline Vec3D intersect(const Line3D&l) const{
		return intersect_y(l,0);
	}
	inline bool on(Point3D p) const{
		return fabs(p.y)<eps && (fabs(round(p.x/4096)-p.x/4096)<0.025 || fabs(round(p.z/4096)-p.z/4096)<0.025);
	}
	int getcolor(Point3D p) const override{
		return 0x7a7a7a;
	}
	
	unique_ptr<Shape> copy_clone(){return make_unique<XZplane_Gridline>(*this);}
	unique_ptr<Shape> move_clone(){return make_unique<XZplane_Gridline>(move(*this));}
	~XZplane_Gridline(){ }
};
struct Mirror_Lake: public Shape_parallel_XZ{
	
	vector<Point2D> pts;
	vector<double> w;
	double mxsd;
	double xmt,zmt;
	
	Mirror_Lake(){ }
	Mirror_Lake(const Mirror_Lake &other) = default;
	Mirror_Lake(Mirror_Lake &&other) = default;
	
	Mirror_Lake(vector<Point2D> _pts,vector<double> _w,double _m,double _xmt=1,double _zmt=1){
		// points, weight, max sum distance, x-axis multiplier, z-axis multiplier
		pts=_pts,w=_w,mxsd=_m,xmt=_xmt,zmt=_zmt;
		if(_pts.size()!=_w.size()){
			cerr<<"Mirror_Lake(vector<Point2D> _pts,vector<double> _w,double _m): Size of _pts and _w are not the same."<<endl;
			throw exception();
		}
	}
	
	inline Vec3D intersect(const Line3D&l) const{
		return intersect_y(l,0);
	}
	inline bool on(Point3D p) const{
		// if(fabs(p.y)>eps||p.z>33550....)return false; // omitted
		Point2D h={p.x/xmt,p.z/zmt};
		double sd=0;
		for(unsigned i=0;i<pts.size();++i)sd+=(h-Point2D({pts[i].x/xmt,pts[i].y/zmt})).len()*w[i];
		return fabs(sd-mxsd)<156+pow((p.z-8440)/128,2.23)/640;
	}
	inline bool in(Point3D p) const{
		// if(fabs(p.y)>eps||p.z>33550....)return false; // omitted
		Point2D h={p.x/xmt,p.z/zmt};
		double sd=0;
		for(unsigned i=0;i<pts.size();++i)sd+=(h-Point2D({pts[i].x/xmt,pts[i].y/zmt})).len()*w[i];
		return sd<=mxsd;
	}
	int getcolor(Point3D p) const override{
		double d=Point2D({(pts[0].x-p.x)/xmt,(pts[0].y-p.z)/zmt}).len()*w[0];
		double dd=fabs(d-mxsd);
		double wt=156+pow((p.z-8440)/128,2.23)/640;
		int t;
		if(dd<wt*0.75)t=0xff;
		else t=floor((4*(wt-dd)/wt)*0xff);
		return 0x010101*t;
	}
	
	unique_ptr<Shape> copy_clone(){return make_unique<Mirror_Lake>(*this);}
	unique_ptr<Shape> move_clone(){return make_unique<Mirror_Lake>(move(*this));}
	~Mirror_Lake(){ }
};
struct XYplane_Gridline: public Shape_parallel_XY{
	XYplane_Gridline(){ }
	XYplane_Gridline(const XYplane_Gridline &other) = default;
	XYplane_Gridline(XYplane_Gridline &&other) = default;
	
	inline Vec3D intersect(const Line3D&l) const{
		return intersect_z(l,0);
	}
	inline bool on(Point3D p) const{
		return fabs(p.z)<eps && (fabs(round(p.x/2048)-p.x/2048)<0.025 || fabs(round(p.y/2048)-p.y/2048)<0.025);
	}
	int getcolor(Point3D p) const override{
		return 0xd6d6d6;
	}
	
	unique_ptr<Shape> copy_clone(){return make_unique<XYplane_Gridline>(*this);}
	unique_ptr<Shape> move_clone(){return make_unique<XYplane_Gridline>(move(*this));}
	~XYplane_Gridline(){ }
};
struct Straight_Segment_onXY: public Shape_parallel_XY{
	Point2D a,b; // (x,y)
	double z;
	double width;
	
	Straight_Segment_onXY(){ }
	Straight_Segment_onXY(const Straight_Segment_onXY &other) = default;
	Straight_Segment_onXY(Straight_Segment_onXY &&other) = default;
	
	Straight_Segment_onXY(Point2D _a,Point2D _b,double _z=0,double _w=30):a(_a),b(_b),z(_z),width(_w){ }
	Straight_Segment_onXY(Point3D _a,Point3D _b,double _z=0,double _w=30):a({_a.x,_a.y}),b({_b.x,_b.y}),z(_z),width(_w){ }
	
	inline Vec3D intersect(const Line3D&l) const{
		return intersect_z(l,z);
	}
	bool on(Point3D p) const{
		if(fabs(p.z-z)>eps)return 0;
		// dis <= width
		// coarse check
		if(p.x<min(a.x,b.x)-width || p.x>max(a.x,b.x)+width
		|| p.y<min(a.y,b.y)-width || p.y>max(a.y,b.y)+width) return 0;
		Point2D sh_p={p.x,p.y};
		Line2D l={sh_p,Vec2D({a.y-b.y,b.x-a.x})};
		Point2D h=geometry::intersect(l,P2_to_L_2D(a,b)).first;
		if((h.x-a.x)*(h.x-b.x)>0)h=(fabs(h.x-a.x)<fabs(h.x-b.x)?a:b);
		return (h-sh_p).len()<=width;
	}
	int getcolor(Point3D p) const override{
		return 0x66ccff;
	}
	
	unique_ptr<Shape> copy_clone(){return make_unique<Straight_Segment_onXY>(*this);}
	unique_ptr<Shape> move_clone(){return make_unique<Straight_Segment_onXY>(move(*this));}
	~Straight_Segment_onXY(){ }
};
struct Parabola_onXY: public Shape_parallel_XY{
	double a;Point2D t; // a(x-x[t])^2+y[t]
	double l,r; // only x in [l,r]
	double z;
	double width;
	
	Parabola_onXY(){ }
	Parabola_onXY(const Parabola_onXY &other) = default;
	Parabola_onXY(Parabola_onXY &&other) = default;
	
	Parabola_onXY(double _a,Point2D _t,double _l,double _r,double _z=0,double _w=30):a(_a),t(_t),l(_l),r(_r),z(_z),width(_w){ }
	
	inline double calc(double x) const{
		return a*(x-t.x)*(x-t.x)+t.y;
	}
	
	inline Vec3D intersect(const Line3D&l) const{
		return intersect_z(l,z);
	}
	bool _basic_chk(Point2D p) const{
		// a != 0
		double lx=max(p.x,l)-width,rx=min(p.x,r)+width,ly=p.y-width,ry=p.y+width;
		double y;
		y=calc(lx);
		if(ly<=y&&y<=ry)return 1;
		y=calc(rx);
		if(ly<=y&&y<=ry)return 1;
		double d;
		if(a>0){
			if(ry-t.y<0)return 0;
			d=sqrt((ry-t.y)/a);
			return (lx<=t.x-d&&t.x-d<=rx)||(lx<=t.x+d&&t.x+d<=rx);
		}
		else{
			if(ly-t.y>0)return 0;
			d=sqrt((ly-t.y)/a);
			return (lx<=t.x-d&&t.x-d<=rx)||(lx<=t.x+d&&t.x+d<=rx);
		}
	}
	bool on(Point3D p) const{
		if(fabs(p.z-z)>eps)return 0;
		// dis <= width
		if(fabs(a)<eps){
			// reduce to a segment x in [l,r], y=y[t]
			return (fabs(p.y-t.y)<=width&&l<=p.x&&p.x<=r)
				|| (sqrt((p.x-l)*(p.x-l)+(p.y-t.y)*(p.y-t.y))<=width)
				|| (sqrt((p.x-r)*(p.x-r)+(p.y-t.y)*(p.y-t.y))<=width);
		}
		Point2D sh_p={p.x,p.y};
		// coarsely check first
		if(!_basic_chk(sh_p))return 0;
		// corner case
		if((Point2D({l,calc(l)})-sh_p).len()<=width || (Point2D({r,calc(r)})-sh_p).len()<=width)return 1;
		
		double b=-2*a*t.x,c=a*t.x*t.x+t.y;
		// 2a(y[p]-ax^2-bx-c)(x-x[t])+x[p]-x=0
		double c3=2*a*(-a),c2=2*a*(a*t.x-b),c1=2*a*(b*t.x-c+p.y)-1,c0=2*a*(p.y-c)*(-t.x)+p.x;
		// c3*x^3+c2*x^2+c1*x+c0=0 (c3<0)
		// dif: 3*c3*x^2+2*c2*x+c1=0
		double delta=4*c2*c2-12*c1*c3,lx,rx,mid;
		double Li,Ri;
		// (x-x[t])^2=(y[p]-y[t])/a
		if((p.y-t.y)/a>0){
			double x1=t.x-sqrt((p.y-t.y)/a),x2=t.x+sqrt((p.y-t.y)/a);
			Li=min(p.x,min(x1,x2)),Ri=max(p.x,max(x1,x2));
		}
		else{
			Li=p.x,Ri=t.x;
			if(Li>Ri)swap(Li,Ri);
		}
		
		if(delta<=0){
			// dif <= 0
			lx=Li,rx=Ri;
			while(rx-lx>eps){
				mid=(lx+rx)/2;
				if(c0+c1*mid+mid*mid*(c2+c3*mid) > 0)lx=mid;
				else rx=mid;
			}
			// corner (edge) already considered
			if(l<=lx&&lx<=r){
				Point2D h={lx,calc(lx)};
				return (sh_p-h).len()<=width;
			}
			else return 0;
		}
		else{
			double dx1=(-4*c2-sqrt(delta))/(6*c3),dx2=(-4*c2+sqrt(delta))/(6*c3);
			if(Li<=dx1){
				lx=Li,rx=dx1;
				while(rx-lx>eps){
					mid=(lx+rx)/2;
					if(c0+c1*mid+mid*mid*(c2+c3*mid) > 0)lx=mid;
					else rx=mid;
				}
				if(l<=lx&&lx<=r){
					Point2D h={lx,calc(lx)};
					if((sh_p-h).len()<=width)return 1;
				}
			}
			if(Ri>=dx2){
				lx=dx2,rx=Ri;
				while(rx-lx>eps){
					mid=(lx+rx)/2;
					if(c0+c1*mid+mid*mid*(c2+c3*mid) > 0)lx=mid;
					else rx=mid;
				}
				if(l<=lx&&lx<=r){
					Point2D h={lx,calc(lx)};
					if((sh_p-h).len()<=width)return 1;
				}
			}
			lx=dx1,rx=dx2;
			while(rx-lx>eps){
				mid=(lx+rx)/2;
				if(c0+c1*mid+mid*mid*(c2+c3*mid) < 0)lx=mid;
				else rx=mid;
			}
			if(l<=lx&&lx<=r){
				Point2D h={lx,calc(lx)};
				if((sh_p-h).len()<=width)return 1;
			}
			return 0;
		}
	}
	int getcolor(Point3D p) const override{
		return 0xee0000;
	}
	
	unique_ptr<Shape> copy_clone(){return make_unique<Parabola_onXY>(*this);}
	unique_ptr<Shape> move_clone(){return make_unique<Parabola_onXY>(move(*this));}
	~Parabola_onXY(){ }
};

unique_ptr<Mirror_Lake> global_lake;
struct Shadow{
	inline bool can_see(const Line3D&l) const{
		return global_lake->in(global_lake->intersect(l));
	}
};

struct Star_Model{
	double c[1210][1610];
	void init(){
		for(int x=0;x<=1206;++x)for(int y=0;y<=1608;++y){
			// int X=x<...?...-x:x-...; // omitted
			// int Y=y<...?...-y:y-...;
			// double clr=0;int p;
			// p=Y*...+X*...; // max=161604
			// if(p<=......)clr=max(clr,1.);
			// else if(p<=......)clr=max(clr,(161605.-p)/18748.);
			// p=Y*...+X*...; // max=161604
			// if(p<=......)clr=max(clr,1.);
			// else if(p<=......)clr=max(clr,(161605.-p)/18748.);
			// c[x][y]=clr;
			// cerr<<clr<<(y==1608?'\n':' ');
		}
	}
}star_model;
struct Stars: public Shape_parallel_XY{
	double z;
	struct star{
		Point2D pos;
		double siz;
		// Default is 1206*1608, siz is the multiplier -> (1206*siz)*(1608*siz)
		int clr;
	};
	vector<star> stars;
	static inline bool cmp(const star&a,const star&b){
		return a.pos.x<b.pos.x;
	}
	
	bool _init_ed;
	void init(){
		// Must ensure no stars share a same x-axis
		sort(stars.begin(),stars.end(),cmp);
		_init_ed=true;
	}
	
	Stars(){_init_ed=false;stars.clear();}
	Stars(const Stars &other) = default;
	Stars(Stars &&other) = default;
	
	Stars(double _z){
		_init_ed=false;
		z=_z,stars.clear();
	}
	
	inline Vec3D intersect(const Line3D&l) const{
		return intersect_z(l,z);
	}
	
	bool on(Point3D p) const{
		if(!_init_ed){
			cerr<<"Stars(_Shadow)::on(Point3D): Stars object not initialized."<<endl;
			throw exception();
		}
		if(fabs(p.z-z)>eps)return false;
		int l=0,r=((int)stars.size())-1,mid,pos=-1;
		while(l<=r){
			mid=(l+r)>>1;
			if(fabs(stars[mid].pos.x-p.x)<=1206*stars[mid].siz){pos=mid;break;}
			if(p.x<stars[mid].pos.x)r=mid-1;
			else l=mid+1;
		}
		if(pos==-1)return false;
		if(fabs(stars[pos].pos.y-p.y)>1608*stars[pos].siz)return false;
		return true;
	}
	
	int getcolor(Point3D p) const override{
		if(!_init_ed){
			cerr<<"Stars(_Shadow)::getcolor(Point3D): Stars object not initialized."<<endl;
			throw exception();
		}
		if(fabs(p.z-z)>eps)return 0;
		int l=0,r=((int)stars.size())-1,mid,pos=-1;
		while(l<=r){
			mid=(l+r)>>1;
			if(fabs(stars[mid].pos.x-p.x)<=1206*stars[mid].siz){pos=mid;break;}
			if(p.x<stars[mid].pos.x)r=mid-1;
			else l=mid+1;
		}
		if(pos==-1)return 0;
		if(fabs(stars[pos].pos.y-p.y)>1608*stars[pos].siz)return 0;
		double dx=p.x-(stars[pos].pos.x-stars[pos].siz*1206);
		double dy=(stars[pos].pos.y+stars[pos].siz*1608)-p.y;
		int px=floor(dx/stars[pos].siz+eps),py=floor(dy/stars[pos].siz+eps);
		if(px<0||px>1206||py<0||py>1608)return 0;
		double cr=star_model.c[px][py];
		int rc=(stars[pos].clr>>16)&255,gc=(stars[pos].clr>>8)&255,bc=stars[pos].clr&255;
		rc=floor(rc*cr+eps),gc=floor(gc*cr+eps),bc=floor(bc*cr+eps);
		return (rc<<16)|(gc<<8)|bc;
	}
	
	unique_ptr<Shape> copy_clone(){return make_unique<Stars>(*this);}
	unique_ptr<Shape> move_clone(){return make_unique<Stars>(move(*this));}
	~Stars(){ }
};
struct Stars_Shadow: public Stars, public Shadow{
	Stars_Shadow(){_init_ed=false;stars.clear();}
	Stars_Shadow(const Stars_Shadow &other) = default;
	Stars_Shadow(Stars_Shadow &&other) = default;
	
	inline Vec3D intersect(const Line3D&l) const{
		return can_see(l)?intersect_z(l,z):Vec3D({0,0,z+1});
	}
	
	unique_ptr<Shape> copy_clone(){return make_unique<Stars_Shadow>(*this);}
	unique_ptr<Shape> move_clone(){return make_unique<Stars_Shadow>(move(*this));}
	~Stars_Shadow(){ }
};
Stars_Shadow shadow(const Stars&starmap){
	Stars_Shadow res;
	res.z=starmap.z;
	for(Stars::star st:starmap.stars){
		res.stars.push_back({{st.pos.x,-st.pos.y},st.siz,dim(st.clr,0.804)});
	}
	res.init();
	return res;
}

struct Moon: public Shape_parallel_XY{
	double z;
	Point2D O;
	double r;
	double full_per;
	double theta;
	int clr;
	
	Moon(){ }
	Moon(const Moon &other) = default;
	Moon(Moon &&other) = default;
	
	Moon(double _z,Point2D _O,double _r,double _fp,double _theta,double _clr=0xeeee66){
		z=_z,O=_O,r=_r,full_per=_fp,theta=_theta,clr=_clr;
	}
	
	inline Vec3D intersect(const Line3D&l) const{
		return intersect_z(l,z);
	}
	
	bool on(Point3D p) const{
		if(fabs(p.z-z)>eps)return false;
		Point2D h={p.x,p.y};
		if((h-O).len()>r)return false;
		h=rotate(h,O,-theta);
		double arcL=sqrt(r*r-(h.y-O.y)*(h.y-O.y))*2;
		double dx=(O.x+arcL/2)-h.x;
		return dx<full_per*arcL;
	}
	
	int getcolor(Point3D p) const override{
		return clr;
	}
	
	unique_ptr<Shape> copy_clone(){return make_unique<Moon>(*this);}
	unique_ptr<Shape> move_clone(){return make_unique<Moon>(move(*this));}
	~Moon(){ }
};

struct Rain_position{
	double x[804],z[804],yl[804],yr[804],v[804];
	int cnt;
	inline int maxy(int z){return 8040+z*0.68;}
	void init(){
		double xdz=-2;
		cnt=400;
		for(int i=1;i<=cnt;++i){
			xdz+=randint(90,110)*0.0001; // randint should be big enough. Now >=23
			z[i]=randint(11831,115200),x[i]=z[i]*xdz;
			// double er=128-96.*(115200.-z)/98304.;
			// cerr<<"["<<(x-er)/z<<","<<(x+er)/z<<"]"<<endl;
			yl[i]=randint(0,maxy(z[i])-1),yr[i]=randint(0,maxy(z[i])-1);
			if(yl[i]>yr[i])swap(yl[i],yr[i]);
			// yr[i]=((int)yl[i]+randint(...+z[i]*0.0...,...+z[i]*0.1...))%(int)maxy(z[i]);
			// cerr<<yl<<" "<<yr<<endl;
			
			// v[i]=randint(...,...); // omitted
		}
	}
}rain_pos;
struct Rain: public Shape{
	double k;
	// When rain shows, the camera is always placed at the initial position (0,8040,0)
	struct raindrop{
		double x,z; // on the lake
		double yl,yr;
		double er; // error (width)
	};
	vector<raindrop> rains;
	static inline bool cmp(const raindrop&a,const raindrop&b){
		return a.x/a.z<b.x/b.z;
	}
	
	bool _init_ed;
	void init(){
		sort(rains.begin(),rains.end(),cmp);
		_init_ed=true;
	}
	
	Rain(){_init_ed=false;rains.clear();}
	Rain(const Rain &other) = default;
	Rain(Rain &&other) = default;
	
	inline Vec3D intersect(const Line3D&l) const{
		if(!_init_ed){
			cerr<<"Rain::intersect(const Line3D&): Rain object not initialized."<<endl;
			throw exception();
		}
		int pl=0,pr=((int)rains.size())-1,mid,pos=-1;
		// double tg=l.dir.x/l.dir.z;
		// while(pl<=pr){
			// mid=(pl+pr)>>1; // omitted
			// Point2D ph=(Vec2D({l.dir.x,l.dir.y})*rains[mid].z*(1./l.dir.z))+Point2D({0,8...}),o={rains[mid].x,0},v={1/k,1};
			// if(Straight_Segment_onXY(o+v*rains[mid].yl,o+v*rains[mid].yr,0,rains[mid].er).on({ph.x,ph.y,0})){pos=mid;break;}
			// if(/*tg<rains[mid].x/rains[mid].z*/ph.x-ph.y/k<rains[mid].x)pr=mid-1;
			// else pl=mid+1;
		// }
		if(pos==-1)return {0,0,0};
		// Point2D ph=(Vec2D({l.dir.x,l.dir.y})*rains[mid].z*(1./l.dir.z))+Point2D({0,...}); // omitted
		// return {ph.x,ph.y,rains[pos].z};
	}
	
	inline bool on(Point3D p) const{
		return fabs(p.x)>eps||fabs(p.z)>eps;
	}
	
	int getcolor(Point3D p) const override{
		if(!_init_ed){
			cerr<<"Rain::getcolor(Point3D): Rain object not initialized."<<endl;
			throw exception();
		}
		return 0xf5f5f5;
	}
	double gettransparency(Point3D p) const override{
		if(!_init_ed){
			cerr<<"Rain::gettransparency(Point3D): Rain object not initialized."<<endl;
			throw exception();
		}
		return _transp*0.892; // self
	}
	
	unique_ptr<Shape> copy_clone(){return make_unique<Rain>(*this);}
	unique_ptr<Shape> move_clone(){return make_unique<Rain>(move(*this));}
	~Rain(){ }
};
struct Rain_Shadow: public Rain, public Shadow{
	Rain_Shadow(){_init_ed=false;rains.clear();}
	Rain_Shadow(const Rain_Shadow &other) = default;
	Rain_Shadow(Rain_Shadow &&other) = default;
	
	inline Vec3D intersect(const Line3D&l) const{
		return can_see(l)?Rain::intersect(l):Vec3D({0,0,0});
	}
	
	static constexpr int clr=dim(0xf5f5f5,0.804);
	
	int getcolor(Point3D p) const override{
		if(!_init_ed){
			cerr<<"Rain::getcolor(Point3D): Rain object not initialized."<<endl;
			throw exception();
		}
		return clr;
	}
	
	unique_ptr<Shape> copy_clone(){return make_unique<Rain_Shadow>(*this);}
	unique_ptr<Shape> move_clone(){return make_unique<Rain_Shadow>(move(*this));}
	~Rain_Shadow(){ }
};
Rain_Shadow shadow(const Rain&sky){
	Rain_Shadow res;
	res.k=-sky.k;
	for(Rain::raindrop r:sky.rains){
		res.rains.push_back({r.x,r.z,-r.yr,-r.yl,r.er});
	}
	res.init();
	return res;
}

struct Lightning: public Shape{	};
struct Leaf: public Shape{ };
struct Cloud: public Shape{ };

struct Upright_Camera{
	// An "upright" camera assumes that 
	// the x-axis of the camera is parallel to the xz-plane of the coordinate, and
	// greater y-axis on camera always means greater y-axis on coordinate.
	double v_dis;int xlen,ylen;
	// xlen is width, ylen is height
	// grid is a square which edges' length are (gridsiz=1).
	Vec3D pos,dir; // pos is the position of the glass
	// Looking from the dir direction, left/right indicates 
	// greater/smaller x-axis on light screen,
	// while up/down (defined as increase/decrease of y-axis in coordinate) 
	// indicates greater/smaller y-axis on light screen.
	// Which is, all inverted. 
	// On light screen the grids are numbered (0...xlen-1)(0...ylen-1).
	inline void setting(double v,int xl,int yl){
		v_dis=v,xlen=xl,ylen=yl;
	}
	inline void setpos(Vec3D p){pos=p;}
	inline void setdir(Vec3D p){dir=normal(p);}
	Vec3D _convert_coord(int a,int b){
		Vec3D xv,yv;
		xv=normal({-dir.z,0,dir.x});
		if(fabs(dir.y)>eps)yv=normal({dir.x,-(dir.x*dir.x+dir.z*dir.z)/dir.y,dir.z});
		else yv={0,1,0};
		if(yv.y<0)yv=-yv;
		// when x/y-axis of light screen increase by 1, in the coordinate the point moves xv/yv.
		return pos+(a-(xlen-1.)/2.)*xv+(b-(ylen-1.)/2.)*yv;
	}
	vector<vector<int> > capture(const vector<unique_ptr<Shape>>&v,int background=0x000000){
		Vec3D xv,yv;
		xv=normal({-dir.z,0,dir.x});
		if(fabs(dir.y)>eps)yv=normal({dir.x,-(dir.x*dir.x+dir.z*dir.z)/dir.y,dir.z});
		else yv={0,1,0};
		if(yv.y<0)yv=-yv;
		// when x/y-axis of light screen increase by 1, in the coordinate the point moves xv/yv.
		vector<vector<int> > frame(ylen,vector<int>(xlen,background));
		Vec3D cell,glass,inter;
		Line3D light;
		glass=pos;
		for(int j=0;j<ylen;++j)for(int i=0;i<xlen;++i){
			cell=pos-normal(dir)*v_dis+(i-(xlen-1.)/2.)*xv+(j-(ylen-1.)/2.)*yv;
			light=P2_to_L_3D(cell,glass);
			// bool flag=0;double len,minlen;
			int clr;double trs; // color, transparency
			vector<tuple<double,int,double> > vec;
			for(const unique_ptr<Shape>& sh:v){
				inter=sh->intersect(light);
				if(!sh->on(inter))continue;
				if((inter.x-glass.x)*(cell.x-glass.x)>0)continue;
				// len=(inter-glass).len();
				clr=sh->getcolor(inter);
				trs=sh->gettransparency(inter);
				vec.push_back({(inter.x-glass.x)*(glass.x-cell.x),clr,trs});
				/*if(!flag||len<minlen){
					flag=1;
					clr=sh->getcolor(inter,(inter-glass).len());
					minlen=len;
					frame[j][i]=clr;
				}*/
			}
			if(vec.size()==0)frame[j][i]=background;
			else{
				sort(vec.begin(),vec.end());
				double rest=1,r=0,g=0,b=0;
				for(auto obj:vec){
					double k=rest*std::get<2>(obj);
					int c=std::get<1>(obj);
					r+=((c>>16)&255)*k;
					g+=((c>>8)&255)*k;
					b+=(c&255)*k;
					rest-=k;
					if(fabs(rest)<eps)break;
				}
				if(fabs(rest)>=eps){
					r+=((background>>16)&255)*rest;
					g+=((background>>8)&255)*rest;
					b+=(background&255)*rest;
				}
				frame[j][i]=(((int)floor(r+eps))<<16)|(((int)floor(g+eps))<<8)|((int)floor(b+eps));
			}
		}
		return frame;
	}
}cam;

namespace Dynamics{
	unique_ptr<Shape> make_transparent(unique_ptr<Shape> sh,double trs){
		unique_ptr<Shape> ret=sh->copy_clone();
		ret->_transp*=trs;
		return ret;
	}
	struct Dynamic_Shape{
		virtual unique_ptr<Shape> operator()(int tim,double trs) const = 0;
	};
	struct Dynamic_Star: public Dynamic_Shape{
		unique_ptr<Shape> _stars;
		
		Dynamic_Star(){
			Stars stars(99200);
			int clr=0xffff77;
			stars.stars.push_back({Point2D({20480,52200}),1,clr});
			// stars.stars.push_back(...); // omitted
			// stars.stars.push_back(...);
			// stars.stars.push_back(...);
			// stars.stars.push_back(...);
			// stars.stars.push_back(...);
			// stars.stars.push_back(...);
			// stars.stars.push_back(...);
			// stars.stars.push_back(...);
			// stars.stars.push_back(...);
			// stars.stars.push_back(...);
			// stars.stars.push_back(...);
			// stars.stars.push_back(...);
			// stars.stars.push_back(...);
			// stars.stars.push_back(...);
			// stars.stars.push_back(...);
			// stars.stars.push_back(...);
			// stars.stars.push_back(...);
			// stars.stars.push_back(...);
			stars.stars.push_back({Point2D({36100,21090}),1,clr});
			stars.z*=2;
			for(unsigned i=0;i<stars.stars.size();++i){
				double x=stars.stars[i].pos.x,y=stars.stars[i].pos.y,siz=stars.stars[i].siz;
				stars.stars[i]={{x*2,y*2},siz*2,stars.stars[i].clr};
			}
			stars.init();
			_stars=stars.copy_clone();
		}
		
		unique_ptr<Shape> operator()(int tim,double trs) const override{
			return make_transparent(_stars->copy_clone(),trs);
		}
	};
	struct Dynamic_Star_Shadow: public Dynamic_Shape{
		unique_ptr<Shape> _shd;
		
		Dynamic_Star_Shadow(){
			Stars stars(99200);
			int clr=0xffff77;
			stars.stars.push_back({Point2D({20480,52200}),1,clr});
			// stars.stars.push_back(...); // omitted
			// stars.stars.push_back(...);
			// stars.stars.push_back(...);
			// stars.stars.push_back(...);
			// stars.stars.push_back(...);
			// stars.stars.push_back(...);
			// stars.stars.push_back(...);
			// stars.stars.push_back(...);
			// stars.stars.push_back(...);
			// stars.stars.push_back(...);
			// stars.stars.push_back(...);
			// stars.stars.push_back(...);
			// stars.stars.push_back(...);
			// stars.stars.push_back(...);
			// stars.stars.push_back(...);
			// stars.stars.push_back(...);
			// stars.stars.push_back(...);
			// stars.stars.push_back(...);
			stars.stars.push_back({Point2D({36100,21090}),1,clr});
			stars.z*=2;
			for(unsigned i=0;i<stars.stars.size();++i){
				double x=stars.stars[i].pos.x,y=stars.stars[i].pos.y,siz=stars.stars[i].siz;
				stars.stars[i]={{x*2,y*2},siz*2,stars.stars[i].clr};
			}
			stars.init();
			_shd=shadow(stars).copy_clone();
		}
		
		unique_ptr<Shape> operator()(int tim,double trs) const override{
			return make_transparent(_shd->copy_clone(),trs);
		}
	};
	struct Dynamic_Moon: public Dynamic_Shape{
		unique_ptr<Shape> _moon;
		
		Dynamic_Moon(){
			_moon=Moon(262144,{48000,98304},10240,0.223,0.201,0xfdfdcf).copy_clone();
		}
		
		unique_ptr<Shape> operator()(int tim,double trs) const override{
			return make_transparent(_moon->copy_clone(),trs);
		}
	};
	struct Dynamic_Moon_Shadow: public Dynamic_Shape{
		unique_ptr<Shape> _moon_shd;
		
		Dynamic_Moon_Shadow(){
			_moon_shd=Moon(262144,{48000,-98304},10240,0.223,-0.201,dim(0xfdfdcf,0.804)).copy_clone();
		}
		
		unique_ptr<Shape> operator()(int tim,double trs) const override{
			return make_transparent(_moon_shd->copy_clone(),trs);
		}
	};
	struct Dynamic_Lake: public Dynamic_Shape{
		unique_ptr<Shape> _lake;
		
		Dynamic_Lake(){
			_lake=Mirror_Lake(vector<Point2D>({{0,63200}}),vector<double>({1}),30720,1,1.6).copy_clone();
		}
		
		unique_ptr<Shape> operator()(int tim,double trs) const override{
			return make_transparent(_lake->copy_clone(),trs);
		}
	};
	struct Dynamic_Rain: public Dynamic_Shape{
		unique_ptr<Shape> operator()(int tim,double trs) const override{
			Rain rain;
			rain.k=2.026;
			for(int i=1;i<=rain_pos.cnt;++i){
				double x=rain_pos.x[i],z=rain_pos.z[i];
				int yl=rain_pos.yl[i],yr=rain_pos.yr[i],v=rain_pos.v[i],My=rain_pos.maxy(z);
				yl=(yl-tim*v%My+My)%My,yr=(yr-tim*v%My+My)%My;
				if(yl>yr)yl=0;
				rain.rains.push_back({x,z,(double)yl,(double)yr,128-96.*(115200.-z)/98304.});
			}
			rain.init();
			// Rain_Shadow shd=shadow(rain);
			return make_transparent(rain.copy_clone(),trs);
		}
	};
	struct Dynamic_Rain_Shadow: public Dynamic_Shape{
		unique_ptr<Shape> operator()(int tim,double trs) const override{
			Rain rain;
			rain.k=2.026;
			for(int i=1;i<=rain_pos.cnt;++i){
				double x=rain_pos.x[i],z=rain_pos.z[i];
				int yl=rain_pos.yl[i],yr=rain_pos.yr[i],v=rain_pos.v[i],My=rain_pos.maxy(z);
				yl=(yl-tim*v%My+My)%My,yr=(yr-tim*v%My+My)%My;
				if(yl>yr)yl=0;
				rain.rains.push_back({x,z,(double)yl,(double)yr,128-96.*(115200.-z)/98304.});
			}
			rain.init();
			Rain_Shadow shd=shadow(rain);
			return make_transparent(shd.copy_clone(),trs);
		}
	};
}
using namespace Dynamics;

namespace PV{
	inline int beat(int x,int y){
		return (x-1)*4+(y-1);
	}
	inline double to_time(int bt){
		return 1+bt*60./80.4;
	}
	inline double to_time(int x,int y){
		return to_time(beat(x,y));
	}
	vector<unique_ptr<Shape> > objects[20000];
	void range_modify(const Dynamic_Shape &sh,double l,double r,double L,double R){
		l-=eps,r-=eps;
		int lf=max(floor(l*30)-1,1.),rf=ceil(r*30)+1;
		for(int f=lf;f<=rf;++f){
			double t=f/30.;
			if(t<l||t>r)continue;
			objects[f].push_back(sh(f,L+(R-L)*(t-l)/(r-l))); 
		}
	}
	void add_object(const Dynamic_Shape &sh,double l,double r,double L=2.68,double R=2.68,double trs_mx=1){
		if(trs_mx>1||trs_mx<0){
			cerr<<"add(Shape,double,double,double,double): Warning: trs_mx invalid, ignored: trs_mx = "<<trs_mx<<"."<<endl;
		}
		if(r-l<L+R){
			cerr<<"add(Shape,double,double,double,double): Warning: r - l < L + R, ignored. l = "<<l<<", r = "<<r<<", L = "<<L<<", R = "<<R<<"."<<endl;
			return;
		}
		int lf=max(floor(l*30)-1,1.),rf=ceil(r*30)+1;
		for(int f=lf;f<=rf;++f){
			double t=f/30.;
			if(t<l||t>r)continue;
			double trs;
			if(t<l+L)trs=(t-l)/L;
			else if(t>r-R)trs=(r-t)/R;
			else trs=1;
			//sh->_transp=trs*trs_mx;
			//objects[f].push_back(sh->copy_clone());
			objects[f].push_back(sh(f,trs)); 
		}
	}
	void set_pv(){
		
		double spb = 60.0/80.4;
		
		// Set Lake
		
		{
			// add_object(...); // omitted
			
			// add_object(...);
			
			// add_object(...);
		}
		
		// Set Moon
		
		{
			double t;
			
			// 13 -> 49
			
			add_object(Dynamic_Moon(),to_time(13,1),to_time(49,1)+6.4,2.68,4);
			add_object(Dynamic_Moon_Shadow(),to_time(15,1),to_time(49,1)+6.4,2.68,4);
			
			// 57 -> 112
			
			t=to_time(57,1)-0.804;
			
			range_modify(Dynamic_Moon(),t,t+4,0,1);
			range_modify(Dynamic_Moon_Shadow(),t,t+4,0,1);
			
			range_modify(Dynamic_Moon(),t+4,to_time(74,1),1,1);
			range_modify(Dynamic_Moon_Shadow(),t+4,to_time(74,1),1,1);
			
			// range_modify(...); // omitted
			// range_modify(...);
			
			// range_modify(...);
			// range_modify(...);
			
			range_modify(Dynamic_Moon(),to_time(77,1),to_time(77,1)+1,0.75,0.5);
			range_modify(Dynamic_Moon_Shadow(),to_time(77,1),to_time(77,1)+1,0.75,0.5);
			
			range_modify(Dynamic_Moon(),to_time(77,1)+1,to_time(79,4)+0.1,0.5,0.5);
			range_modify(Dynamic_Moon_Shadow(),to_time(77,1)+1,to_time(79,4)+0.1,0.5,0.5);
			
			// t=...;
			
			range_modify(Dynamic_Moon(),t,t+0.32/3,0.5,1);
			range_modify(Dynamic_Moon_Shadow(),t,t+0.32/3,0.5,1);
			t+=0.32/3;
			
			// range_modify(...); // omitted
			// range_modify(...);
			
			// range_modify(...);
			
			// 114 -> 120 -> outro
			
			// range_modify(...); // omitted
			// range_modify(...);
			
			// range_modify(...);
			// range_modify(...);
			
			// range_modify(...);
			// range_modify(...);
			
			add_object(Dynamic_Moon(),to_time(122,1),to_time(162,1),2.68,spb*4);
			add_object(Dynamic_Moon_Shadow(),to_time(122,1),to_time(162,1),2.68,spb*4);
		}
		
		// Set stars
		{
			double t;
			
			// add_object(Dynamic_Star(),to_time(9,1),to_time(49,1)+1,2.68,4); // omitted
			// add_object(Dynamic_Star_Shadow(),to_time(11,1),to_time(49,1)+1,2.68,4);
			
			// 9 -> 30 (4) -> 49
			
			t=to_time(9,1);
			
			range_modify(Dynamic_Star(),t,t+2.68,0,1);
			range_modify(Dynamic_Star_Shadow(),t+spb*8,t+spb*8+2.68,0,1);
			
			range_modify(Dynamic_Star(),t+2.68,to_time(30,1)+0.24,1,1);
			range_modify(Dynamic_Star_Shadow(),t+spb*8+2.68,to_time(30,1)+0.24,1,1);
			
			// t=...;
			
			for(int i=1;i<=4;++i){
				range_modify(Dynamic_Star(),t,t+0.16,1,0.25);
				range_modify(Dynamic_Star_Shadow(),t,t+0.16,1,0.25);
				range_modify(Dynamic_Star(),t+0.16,t+0.32,0.25,1);
				range_modify(Dynamic_Star_Shadow(),t+0.16,t+0.32,0.25,1);
				t+=0.32;
			}
			
			range_modify(Dynamic_Star(),t,to_time(49,1)+0.4,1,1);
			range_modify(Dynamic_Star_Shadow(),t,to_time(49,1)+0.4,1,1);
			
			range_modify(Dynamic_Star(),to_time(49,1)+0.4,to_time(49,1)+4.4,1,0);
			range_modify(Dynamic_Star_Shadow(),to_time(49,1)+0.4,to_time(49,1)+4.4,1,0);
			
			// 57 -> 74 -> 77 -> 80 -> 112
			
			// t=...;
			
			range_modify(Dynamic_Star(),t,t+4,0,1);
			range_modify(Dynamic_Star_Shadow(),t,t+4,0,1);
			
			// range_modify(...); // omitted
			// range_modify(...);
			
			// range_modify(...);
			// range_modify(...);
			
			// range_modify(...);
			// range_modify(...);
			
			// range_modify(...);
			
			range_modify(Dynamic_Star(),to_time(77,1)+1,to_time(79,4)+0.1,0.5,0.5);
			range_modify(Dynamic_Star_Shadow(),to_time(77,1)+1,to_time(79,4)+0.1,0.5,0.5);
			
			// t=...;
			
			range_modify(Dynamic_Star(),t,t+0.32/3,0.5,1);
			range_modify(Dynamic_Star_Shadow(),t,t+0.32/3,0.5,1);
			t+=0.32/3;
			
			for(int i=1;i<4;++i){
				range_modify(Dynamic_Star(),t,t+0.16,1,0.25);
				range_modify(Dynamic_Star_Shadow(),t,t+0.16,1,0.25);
				range_modify(Dynamic_Star(),t+0.16,t+0.32,0.25,1);
				range_modify(Dynamic_Star_Shadow(),t+0.16,t+0.32,0.25,1);
				t+=0.32;
			}
			
			range_modify(Dynamic_Star(),t,t+0.16,1,0.25);
			range_modify(Dynamic_Star_Shadow(),t,t+0.16,1,0.25);
			range_modify(Dynamic_Star(),t+0.16,t+0.16+0.16*67/75,0.25,0.92);
			range_modify(Dynamic_Star_Shadow(),t+0.16,t+0.16+0.16*67/75,0.25,0.92);
			t+=(0.16+0.16*67/75);
			
			range_modify(Dynamic_Star(),t,to_time(112,1),0.92,0.92);
			range_modify(Dynamic_Star_Shadow(),t,to_time(112,1),0.92,0.92);
			
			range_modify(Dynamic_Star(),to_time(112,1),to_time(113,1),0.92,0);
			range_modify(Dynamic_Star_Shadow(),to_time(112,1),to_time(113,1),0.92,0);
			
			// 114 -> 118 -> outro
			
			// range_modify(...); // omitted
			// range_modify(...);
			
			// range_modify(...);
			// range_modify(...);
			
			// range_modify(...);
			// range_modify(...);
			
			// add_object(...);
			// add_object(...);
		}
		
		// Set rain
		
		{
			range_modify(Dynamic_Rain(),to_time(24,4)-0.28,to_time(24,4)+4.4,0,1);
			range_modify(Dynamic_Rain_Shadow(),to_time(24,4)-0.28,to_time(24,4)+4.4,0,1);
			
			// range_modify(...); // omitted
			// range_modify(...);
			
			// range_modify(...);
			// range_modify(...);
			
			// range_modify(...);
			// range_modify(...);
			
			// range_modify(...);
			// range_modify(...);
			
			add_object(Dynamic_Rain(),to_time(81,1),to_time(89,1)+5.04,4.92,5.04);
			add_object(Dynamic_Rain_Shadow(),to_time(81,1),to_time(89,1)+5.04,4.92,5.04);
			
			// add_object(...); // omitted
			// add_object(...);
		}
	}
}

double pos_x[804],first_y,pos_dx[804];int pos_len;
void init_resource(){
	star_model.init();
	rain_pos.init();
}
void init_camera(){
	cam.setting(804,1920,1080);
	cam.setpos({0,8040,0});
	cam.setdir({0,-0.0,1});
}
int tim;
vector<unique_ptr<Shape> > sh;
void init_shapes(int tim){
	//sh=PV::objects[tim];
	/*sh.clear();*/
	Mirror_Lake local_lake(vector<Point2D>({{0,63200}}),vector<double>({1}),30720,1,1.6);
	//sh.push_back(local_lake.copy_clone());
	global_lake=make_unique<Mirror_Lake>(local_lake);
}
std::vector<std::vector<int>> capture(int tim){
	//cam.setpos({0,10240,-32768+tim*128});
	//cam.setdir({0,-0.3,1});
	//cam.setpos({0,81920,10240});
	//cam.setdir({0,0,1});
	
	//return init_shapes(tim),cam.capture(sh);
	return init_shapes(tim),cam.capture(PV::objects[tim]);
}
int main() {
	init_resource();
	init_camera();
	PV::set_pv();
	// init_shapes();
	// Written by DeepSeek 
    const int WIDTH = 1920;
    const int HEIGHT = 1080;
    const int FPS = 30;
    const int TOTAL_FRAMES = (8 * 60 + 4) * FPS;
    tim = 0;
    
    HANDLE hReadPipe, hWritePipe;
    SECURITY_ATTRIBUTES sa = { sizeof(SECURITY_ATTRIBUTES), nullptr, TRUE };
    if (!CreatePipe(&hReadPipe, &hWritePipe, &sa, 0)) {
        fprintf(stderr, "CreatePipe failed\n");
        return 1;
    }
    SetHandleInformation(hReadPipe, HANDLE_FLAG_INHERIT, HANDLE_FLAG_INHERIT);
    SetHandleInformation(hWritePipe, HANDLE_FLAG_INHERIT, 0);
    char cmdLine[] = "ffmpeg -y -f rawvideo -pixel_format rgb24 -video_size 1920x1080 -framerate 30 -i - -c:v libx264 -preset medium -crf 18 -pix_fmt yuv420p output.mp4";
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
    for (int frameIdx = 0; frameIdx < TOTAL_FRAMES; ++frameIdx) {
        auto frameData = capture(++tim); 
        std::vector<uint8_t> rgbBuffer;
        rgbBuffer.reserve(WIDTH * HEIGHT * 3);
        for (int y = 0; y < HEIGHT; ++y) {
        	const auto &row = frameData[y];
            for (int x = 0; x < WIDTH; ++x) {
                int pixel = row[x];
                uint8_t r = (pixel >> 16) & 0xFF;
                uint8_t g = (pixel >> 8) & 0xFF;
                uint8_t b = pixel & 0xFF;
                rgbBuffer.push_back(r);
                rgbBuffer.push_back(g);
                rgbBuffer.push_back(b);
            }
        }
        DWORD bytesWritten = 0;
        if (!WriteFile(hWritePipe, rgbBuffer.data(), rgbBuffer.size(), &bytesWritten, nullptr)) {
            fprintf(stderr, "WriteFile failed at frame %d\n", frameIdx);
            break;
        }
        if (bytesWritten != rgbBuffer.size()) {
            fprintf(stderr, "Partial write at frame %d\n", frameIdx);
            break;
        }
        cerr<<"Frame "<<frameIdx<<endl;
    }
    CloseHandle(hWritePipe);
    WaitForSingleObject(pi.hProcess, INFINITE);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    printf("Finished: output.mp4\n");
    return 0;
}
