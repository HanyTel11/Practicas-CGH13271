#include <cmath>
#include <cstdio>
#include <vector>
#include <glew.h>
#include <glfw3.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include "Mesh.h"
#include "Shader.h"
#include "Window.h"
using namespace std;
using namespace glm;
constexpr float PI=3.14159265358979323846f;
struct Vertex { vec3 p,c; };
vector<MeshColor*> meshes;
enum Shape { CUBE,PYRAMID,CYLINDER,CONE,SPHERE,RED_CONE,YELLOW_CONE,GREEN_CONE,OLIVE_CUBE };
void tri(vector<Vertex>& v,vec3 a,vec3 b,vec3 c,vec3 color){v.push_back({a,color});v.push_back({b,color});v.push_back({c,color});}
void quad(vector<Vertex>& v,vec3 a,vec3 b,vec3 c,vec3 d,vec3 color){tri(v,a,b,c,color);tri(v,a,c,d,color);}
void upload(vector<Vertex>& v){vector<GLfloat> data;for(auto& a:v)data.insert(data.end(),{a.p.x,a.p.y,a.p.z,a.c.r,a.c.g,a.c.b});auto* m=new MeshColor();m->CreateMeshColor(data.data(),(unsigned)v.size());meshes.push_back(m);v.clear();}
void makeMeshes(){
 vector<Vertex> v; vec3 metal(.7f,.78f,.87f),blue(0,0,1);
 vec3 a(-.5f,-.5f,.5f),b(.5f,-.5f,.5f),c(.5f,.5f,.5f),d(-.5f,.5f,.5f),e(-.5f,-.5f,-.5f),f(.5f,-.5f,-.5f),g(.5f,.5f,-.5f),h(-.5f,.5f,-.5f);
 quad(v,a,b,c,d,metal);quad(v,f,e,h,g,metal);quad(v,e,a,d,h,metal);quad(v,b,f,g,c,metal);quad(v,d,c,g,h,metal);quad(v,e,f,b,a,metal);upload(v);
 vec3 p0(-.5f,-.5f,.5f),p1(.5f,-.5f,.5f),p2(.5f,-.5f,-.5f),p3(-.5f,-.5f,-.5f),tip(0,.5f,0);
 tri(v,p0,p1,tip,{1,0,0});tri(v,p1,p2,tip,{0,1,0});tri(v,p2,p3,tip,{1,1,0});tri(v,p3,p0,tip,{1,0,1});quad(v,p3,p2,p1,p0,blue);upload(v);
 constexpr int N=36;
 for(int i=0;i<N;i++){float t=2*PI*i/N,u=2*PI*(i+1)/N;vec3 lo(.5f*cos(t),-.5f,.5f*sin(t)),ln(.5f*cos(u),-.5f,.5f*sin(u)),hi=lo+vec3(0,1,0),hn=ln+vec3(0,1,0);quad(v,lo,ln,hn,hi,metal);tri(v,{0,-.5f,0},ln,lo,metal);tri(v,{0,.5f,0},hi,hn,metal);}upload(v);
 for(int i=0;i<N;i++){float t=2*PI*i/N,u=2*PI*(i+1)/N;vec3 lo(.5f*cos(t),-.5f,.5f*sin(t)),ln(.5f*cos(u),-.5f,.5f*sin(u));tri(v,lo,ln,{0,.5f,0},{.94f,.26f,.12f});tri(v,{0,-.5f,0},ln,lo,{.94f,.26f,.12f});}upload(v);
 auto point=[](float lat,float lon){return vec3(.5f*cos(lat)*cos(lon),.5f*sin(lat),.5f*cos(lat)*sin(lon));};
 for(int j=0;j<20;j++)for(int i=0;i<N;i++){float t=-PI/2+PI*j/20,u=-PI/2+PI*(j+1)/20,q=2*PI*i/N,r=2*PI*(i+1)/N;quad(v,point(t,q),point(t,r),point(u,r),point(u,q),{.16f,.78f,.98f});}upload(v);
 // Colores propios de las piezas del cohete; nunca dependen de un uniform global.
 for(vec3 shade : {vec3(1.f,.06f,.05f),vec3(1.f,.94f,0.f),vec3(.05f,.92f,.06f)}){
  for(int i=0;i<N;i++){float t=2*PI*i/N,u=2*PI*(i+1)/N;
   vec3 lo(.5f*cos(t),-.5f,.5f*sin(t)),ln(.5f*cos(u),-.5f,.5f*sin(u));
   tri(v,lo,ln,{0,.5f,0},shade);tri(v,{0,-.5f,0},ln,lo,shade);
  }upload(v);
 }
 vec3 olive(.48f,.47f,.04f);
 quad(v,a,b,c,d,olive);quad(v,f,e,h,g,olive);quad(v,e,a,d,h,olive);
 quad(v,b,f,g,c,olive);quad(v,d,c,g,h,olive);quad(v,e,f,b,a,olive);upload(v);

}
void draw(Shape s,mat4 root,vec3 position,vec3 size,GLint uniform,mat4 rotation=mat4(1)){
 mat4 m=translate(root,position)*rotation*scale(mat4(1),size);glUniformMatrix4fv(uniform,1,GL_FALSE,value_ptr(m));meshes[s]->RenderMeshColor();
}
void rocket(mat4 root,GLint model){
 // Cohete estilizado que conserva las dos columnas y tres niveles de la figura 2.
 draw(OLIVE_CUBE,root,{-1.08f,0,0},{.19f,4.8f,.22f},model);
 draw(OLIVE_CUBE,root,{ 1.08f,0,0},{.19f,4.8f,.22f},model);
 draw(OLIVE_CUBE,root,{0,-2.46f,0},{3.1f,.16f,.85f},model);
 draw(CYLINDER,root,{0,.21f,-.42f},{.58f,2.65f,.58f},model);
 draw(CONE,root,{0,1.78f,-.42f},{.59f,.75f,.59f},model);
 // Tres piezas con las puntas hacia abajo, orden amarillo, rojo, verde.
 mat4 down=rotate(mat4(1),PI,vec3(1,0,0));
 draw(YELLOW_CONE,root,{0,1.46f,.19f},{1.85f,1.23f,.42f},model,down);
 draw(RED_CONE,root,{0,.20f,.22f},{1.85f,1.23f,.42f},model,down);
 draw(GREEN_CONE,root,{0,-1.08f,.25f},{1.85f,1.23f,.42f},model,down);
 draw(SPHERE,root,{0,2.21f,-.03f},{.27f,.27f,.25f},model);
 draw(PYRAMID,root,{-.68f,-1.72f,-.25f},{.55f,.65f,.22f},model);
 draw(PYRAMID,root,{ .68f,-1.72f,-.25f},{.55f,.65f,.22f},model);
}
int main(){
 Window window(1100,700);window.Initialise();makeMeshes();
 Shader shader;shader.CreateFromFiles("shaders/shadercolor.vert","shaders/shadercolor.frag");
 float angle=0;
 puts("EJERCICIO 1 - COHETE | flechas: girar | E/R/T: girar | ESC: salir");
 while(!window.getShouldClose()){
  glfwPollEvents();auto keys=window.getsKeys();
  if(keys[GLFW_KEY_LEFT])angle-=.025f;if(keys[GLFW_KEY_RIGHT])angle+=.025f;
  glClearColor(.035f,.055f,.12f,1);glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);glEnable(GL_DEPTH_TEST);
  shader.useShader();float aspect=float(window.getBufferWidth())/float(window.getBufferHeight());
  mat4 projection=perspective(radians(52.f),aspect,.1f,100.f);
  mat4 view=lookAt(vec3(0,2.1f,13.5f),vec3(0),vec3(0,1,0));
  glUniformMatrix4fv(shader.getProjectLocation(),1,GL_FALSE,value_ptr(projection));
  glUniformMatrix4fv(shader.getViewLocation(),1,GL_FALSE,value_ptr(view));
  mat4 spin=rotate(mat4(1),angle+radians(window.getrotay()),vec3(0,1,0));
  spin=rotate(spin,radians(window.getrotax()),vec3(1,0,0));spin=rotate(spin,radians(window.getrotaz()),vec3(0,0,1));
  rocket(spin,shader.getModelLocation());
  glUseProgram(0);window.swapBuffers();
 }
 for(auto* m:meshes)delete m;return 0;
}
