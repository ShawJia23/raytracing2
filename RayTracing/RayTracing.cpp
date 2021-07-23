#include <iostream>
#include<fstream>
#include"tool.h"
#include "vector.h"
#include "ray.h"
#include "hittable.h"
#include"camera.h"
#include"material.h"

int nx = 2000;
int ny = 1000;
int ns = 1000;

Camera *cam;
Hittable *world;

float hit_sphere(const Vector3& center, float radius, const Ray& r) {
	Vector3 oc = r.origin() - center;
	float a = dot(r.direction(), r.direction());
	float b = 2.0 * dot(oc, r.direction());
	float c = dot(oc, oc) - radius * radius;
	float discriminant = b * b - 4 * a*c;
	if (discriminant < 0) return -1.0;
	else return (-b - sqrt(discriminant)) / (2.0*a);
}

Vector3 shading_color(const Ray& r,Hittable *world,int depth) 
{
	hit_record rec;
	if (world->hit(r, 0.001, FLT_MAX, rec)) 
	{
		Ray scattered;
		Vector3 attenuation;
		if (depth < 50 && rec.mat->scatter(r, rec, attenuation, scattered)) 
		{
			return attenuation* shading_color(scattered,world,depth+1);
		}
		else 
		{
			return Vector3(0, 0, 0);
		}
		//Vector3 target = rec.p + rec.normal + random_in_unit_sphere();
		//return 0.5 * shading_color(Ray(rec.p, target - rec.p), world);
	}
	else 
	{
		Vector3 unit_direction = unit_vector(r.direction());
		float t = 0.5*(unit_direction.y() + 1.0);
		return (1.0 - t)*Vector3(1.0, 1.0, 1.0) + t * Vector3(0.5, 0.7, 1.0);
	}
}

void test_scene(Hittable ** world, Camera** cam)
{
	Texture *checker = new CheckerTexture(
		new ConstantTexture(Vector3(0.2, 0.3, 0.1)),
		new ConstantTexture(Vector3(0.9, 0.9, 0.9))
	);
	Texture *back = new ConstantTexture(Vector3(.8, 0.8, 0));
	Texture *blue_daoqi = new ConstantTexture(Vector3(0.11, 0.56, 1));
	Hittable **list=new Hittable*[26];
	int n = 1;
	list[0] = new Sphere(Vector3(0, -100.5, -1), 100, new Lambertian(back));
	for (int i = -2; i < 3; i++) 
	{
		for (int j = -2; j < 3; j++) 
		{
			if (random_double() < 0.33) 
			{
				Texture *ran_tex = new ConstantTexture(Vector3(random_double(), random_double(), random_double()));
				list[n] = new Sphere(Vector3(i, 0, j), 0.5, new Lambertian(ran_tex));
			}
			else if (random_double() > 0.66) 
			{
				list[n] = new Sphere(Vector3(i, 0, j), 0.5, new Metal(Vector3(0.8, 0.6, 0.2),0.3));
			}
			else 
			{
				list[n] = new Sphere(Vector3(i, 0, j), 0.5, new Dielectric(1.5));
			}
			n++;
		}
	}
	list[n] = new Sphere(Vector3(-3, 0, 0), 0.5, new Lambertian(blue_daoqi));
	Vector3 lookfrom(5, 5, 7);
	Vector3 lookat(0, 0, -1);
	float dist_to_focus = (lookfrom - lookat).length();
	float aperture = 2.0;

	*cam = new Camera(lookfrom, lookat, Vector3(0, 1, 0), 20, float(nx) / float(ny), aperture, dist_to_focus, 0, 1);
	*world = new BvhNode(list, 27,0,0);
}

//void *random_scene(Hittable ** world, Camera** cam)
//{
//	int n = 500;
//	Hittable **list = new Hittable*[n + 1];
//	list[0] = new Sphere(Vector3(0, -1000, 0), 1000, new Lambertian(Vector3(0.5, 0.5, 0.5)));
//	int i = 1;
//	for (int a = -11; a < 11; a++) {
//		for (int b = -11; b < 11; b++) {
//			float choose_mat = random_double();
//			Vector3 center(a + 0.9*random_double(), 0.2, b + 0.9*random_double());
//			if ((center - Vector3(4, 0.2, 0)).length() > 0.9) {
//				if (choose_mat < 0.8) {  // diffuse
//					list[i++] = new MoveSphere(center, center+Vector3(0,0.5*random_double(),0),
//						0.0,1.0,0.2,
//						new Lambertian(Vector3(random_double()*random_double(),
//							random_double()*random_double(),
//							random_double()*random_double())
//						)
//					);
//				}
//				else if (choose_mat < 0.95) { // metal
//					list[i++] = new Sphere(center, 0.2,
//						new Metal(Vector3(0.5*(1 + random_double()),
//							0.5*(1 + random_double()),
//							0.5*(1 + random_double())),
//							0.5*random_double()));
//				}
//				else {  // glass
//					list[i++] = new Sphere(center, 0.2, new Dielectric(1.5));
//				}
//			}
//		}
//	}
//	list[i++] = new Sphere(Vector3(0, 1, 0), 1.0, new Dielectric(1.5));
//	list[i++] = new Sphere(Vector3(-3, 1, 0), 1.0, new Lambertian(Vector3(0.4, 0.2, 0.1)));
//	list[i++] = new Sphere(Vector3(4, 1, 0), 1.0, new Metal(Vector3(0.7, 0.6, 0.5), 0.0));
//
//	Vector3 lookfrom(13, 2, 3);
//	Vector3 lookat(0, 0, 0);
//	float dist_to_focus = 10;
//	float aperture = 0;
//
//	*cam = new Camera(lookfrom, lookat, Vector3(0, 1, 0), 20, float(nx) / float(ny), aperture, dist_to_focus, 0, 1);
//	*world = new BvhNode(list, i, 0, 0);
//}

void two_sphere(Hittable ** world, Camera** cam)
{
	Texture *checker = new CheckerTexture(
		new ConstantTexture(Vector3(0.2, 0.3, 0.1)),
		new ConstantTexture(Vector3(0.9, 0.9, 0.9))
	);
	Hittable **list = new Hittable*[2];
	list[0] = new Sphere(Vector3(0, -10, 0), 10, new Lambertian(checker));
	list[1] = new Sphere(Vector3(0, 10, 0), 10, new Lambertian(checker));

	Vector3 lookfrom(13, 2, 3);
	Vector3 lookat(0, 0, 0);
	float dist_to_focus = 10;
	float aperture = 0;

	*cam = new Camera(lookfrom, lookat, Vector3(0, 1, 0), 20, float(nx) / float(ny), aperture, dist_to_focus, 0, 1);
	*world = new BvhNode(list, 2,0,0);
}

void two_perlin_spheres(Hittable ** world,Camera** cam) {
	Texture *pertext = new NoiseTexture();
	Hittable **list = new Hittable*[2];
	list[0] = new Sphere(Vector3(0, -1000, 0), 1000, new Lambertian(pertext));
	list[1] = new Sphere(Vector3(0, 2, 0), 2, new Lambertian(pertext));

	Vector3 lookfrom(13, 2, 3);
	Vector3 lookat(0, 0, 0);
	float dist_to_focus = 10;
	float aperture = 0;

	*cam=new Camera(lookfrom, lookat, Vector3(0, 1, 0), 20, float(nx) / float(ny), aperture, dist_to_focus, 0, 1);
	*world = new HittableList(list, 2);
}

int main() 
{

	std::ofstream outfile("MyTest.txt", std::ios::out);
	outfile << "P3\n" << nx << " " << ny << "\n255\n";
	
	
	test_scene(&world,&cam);
	for (int j = ny - 1; j >= 0; j--) {
		for (int i = 0; i < nx; i++) {
			Vector3 color(0, 0, 0);
			for (int s = 0; s < ns; s++) {
				float u = float(i + random_double()) / float(nx);
				float v = float(j + random_double()) / float(ny);
				Ray r = cam->get_ray(u, v);
				color += shading_color(r, world,0);
			}
			color /= float(ns);
			color = Vector3(sqrt(color[0]), sqrt(color[1]), sqrt(color[2]));
			int ir = int(255.99*color[0]);
			int ig = int(255.99*color[1]);
			int ib = int(255.99*color[2]);
			outfile << ir << " " << ig << " " << ib << "\n";
		}
		std::cout << j << std::endl;
	}
	outfile.close();
}

