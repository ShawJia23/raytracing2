#include <iostream>
#include<fstream>
#include"tool.h"
#include "vector.h"
#include "ray.h"
#include "hittable.h"
#include"camera.h"
#include"material.h"
#include "pdf.h"

#define STB_IMAGE_IMPLEMENTATION
#include"stb_image.h"

int worldx = 1960;
int worldy = 1080;
int simple = 500;

Camera *cam_main;
Hittable *world_main;
Hittable *light_main;

inline Vector3 de_nan(const Vector3& c) {
	Vector3 temp = c;
	if (!(temp[0] == temp[0])) temp[0] = 0;
	if (!(temp[1] == temp[1])) temp[1] = 0;
	if (!(temp[2] == temp[2])) temp[2] = 0;
	return temp;
}

float hit_sphere(const Vector3& center, float radius, const Ray& r) {
	Vector3 oc = r.origin() - center;
	float a = dot(r.direction(), r.direction());
	float b = 2.0 * dot(oc, r.direction());
	float c = dot(oc, oc) - radius * radius;
	float discriminant = b * b - 4 * a*c;
	if (discriminant < 0) return -1.0;
	else return (-b - sqrt(discriminant)) / (2.0*a);
}

Vector3 shading_color(const Ray& r,Hittable *world,Hittable* light_shape,int depth) 
{
	hit_record rec;
	if (world->hit(r, 0.001, FLT_MAX, rec)) 
	{
		scatter_record srec;
		Vector3 emitted = rec.mat->emitted(r, rec, rec.u, rec.v, rec.p);
		if (depth < 20 && rec.mat->scatter(r, rec, srec))
		{
			if (srec.is_specular) 
			{
				return srec.attenuation *shading_color(srec.specular_ray, world, light_shape, depth + 1);
			}
			HittablePDF plight(light_shape, rec.p);
			MixturePDF p(&plight, srec.pdf_ptr);
			Ray scattered = Ray(rec.p, p.generate(), r.time());
			float pdf_val = p.value(scattered.direction());
			return emitted+srec.attenuation * rec.mat->scattering_pdf(r, rec, scattered) *shading_color(scattered, world, light_shape,depth + 1)/pdf_val;
		}
		else return emitted;
		//变暗前
		/*if (depth < 50 && rec.mat->scatter(r, rec, attenuation, scattered)) 
		{
			return attenuation* shading_color(scattered,world,depth+1);
		}
		else 
		{
			return Vector3(0, 0, 0);
		}*/

		//Vector3 target = rec.p + rec.normal + random_in_unit_sphere();
		//return 0.5 * shading_color(Ray(rec.p, target - rec.p), world);
	}
	else 
	{
		return Vector3(0, 0, 0); 
		//变暗前
		/*Vector3 unit_direction = unit_vector(r.direction());
		float t = 0.5*(unit_direction.y() + 1.0);
		return (1.0 - t)*Vector3(1.0, 1.0, 1.0) + t * Vector3(0.5, 0.7, 1.0);*/
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
	Vector3 lookat(0, 0, 0.5);
	float dist_to_focus = (lookfrom - lookat).length();
	float aperture = 2.0;

	*cam = new Camera(lookfrom, lookat, Vector3(0, 1, 0), 20, float(worldx) / float(worldy), aperture, dist_to_focus, 0, 1);
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

	*cam = new Camera(lookfrom, lookat, Vector3(0, 1, 0), 20, float(worldx) / float(worldy), aperture, dist_to_focus, 0, 1);
	*world = new BvhNode(list, 2,0,0);
}

//void two_perlin_spheres(Hittable ** world,Camera** cam) {
//	Texture *pertext = new NoiseTexture();
//	Hittable **list = new Hittable*[2];
//	list[0] = new Sphere(Vector3(0, -1000, 0), 1000, new Lambertian(pertext));
//	list[1] = new Sphere(Vector3(0, 2, 0), 2, new Lambertian(pertext));
//
//	Vector3 lookfrom(13, 2, 3);
//	Vector3 lookat(0, 0, 0);
//	float dist_to_focus = 10;
//	float aperture = 0;
//
//	*cam=new Camera(lookfrom, lookat, Vector3(0, 1, 0), 20, float(worldx) / float(worldy), aperture, dist_to_focus, 0, 1);
//	*world = new HittableList(list, 2);
//}

void image_scene(Hittable ** world, Camera** cam)
{
	int nx, ny, nn;
	unsigned char* texture_data = stbi_load("src/earthmap.jpg", &nx, &ny, &nn,0);
	Material *mat = new Lambertian(new ImageTexture(texture_data, nx, ny));
	//stbi_image_free(texture_data);
	Hittable **list = new Hittable*[1];
	list[0] = new Sphere(Vector3(0, 0, 0), 2,mat);
	
	Vector3 lookfrom(13, 2, 3);
	Vector3 lookat(0, 0, 0);
	float dist_to_focus = (lookfrom - lookat).length();
	float aperture = 2;

	*cam = new Camera(lookfrom, lookat, Vector3(0, 1, 0), 20, float(worldx) / float(worldy), aperture, dist_to_focus, 0, 1);
	*world = new HittableList(list, 1);
}

//void simple_light(Hittable ** world, Camera** cam) 
//{
//	Texture *pertext = new NoiseTexture();
//	Hittable **list = new Hittable*[4];
//	list[0] = new Sphere(Vector3(0, -1000, 0), 1000, new Lambertian(pertext));
//	list[1] = new Sphere(Vector3(0, 2, 0), 2, new Lambertian(pertext));
//	list[2] = new Sphere(Vector3(0, 7, 0), 2,new DiffuseLight(new ConstantTexture(Vector3(4, 4, 4))));
//	list[3] = new XYRect(3, 5, 1, 3, -2,new DiffuseLight(new ConstantTexture(Vector3(4, 4, 4))));
//	
//	Vector3 lookfrom(13, 2, 3);
//	Vector3 lookat(0, 0, 0);
//	float dist_to_focus = (lookfrom - lookat).length();
//	float aperture = 2;
//
//	*cam = new Camera(lookfrom, lookat, Vector3(0, 1, 0), 20, float(worldx) / float(worldy), aperture, dist_to_focus, 0, 1);
//	*world = new HittableList(list, 4);
//}

void cornell_box(Hittable ** world, Camera** cam,Hittable** light_shape)
{
	Hittable **list = new Hittable*[8];
	Material *red = new Lambertian(new ConstantTexture(Vector3(0.65, 0.05, 0.05)));
	Material *white = new Lambertian(new ConstantTexture(Vector3(0.73, 0.73, 0.73)));
	Material *green = new Lambertian(new ConstantTexture(Vector3(0.12, 0.45, 0.15)));
	Material *light = new DiffuseLight(new ConstantTexture(Vector3(15, 15, 15)));
	Material *isotropic1 = new Isotropic(new ConstantTexture(Vector3(1.0, 1.0, 1.0)));
	Material *isotropic2 = new Isotropic(new ConstantTexture(Vector3(0,0,0)));
	Material *aluminum = new Metal(Vector3(0.8, 0.85, 0.88), 0.0);
	Material *glass = new Dielectric(1.5);

	list[0] = new FlipNormals(new YZRect(0, 555, 0, 555, 655, green));
	list[1] = new YZRect(0, 555, 0, 555, -100, red);
	list[2] = new FlipNormals(new XZRect(163, 393, 167, 382, 554, light));
	list[3] = new FlipNormals(new XZRect(-100, 655, 0, 555, 555, white));
	list[4] = new XZRect(-100, 655, 0, 555, 0, white);
	list[5] = new FlipNormals(new XYRect(-100, 655, 0, 555, 555, white));
	Hittable* b1 = new Translate(
		new RotateY(new Box(Vector3(0, 0, 0), Vector3(165, 165, 165), white), -18),
		Vector3(130, 0, 65)
	);
	Hittable* b2 = new Translate(
		new RotateY(new Box(Vector3(0, 0, 0), Vector3(165, 330, 165), aluminum), 30),
		Vector3(265, 0, 295));
	Hittable *s= new Sphere(Vector3(190, 90, 190), 90, glass);
	list[6] = b2;
	list[7] = s;
	/*list[6] = new ConstantMedium(b1, 0.01,isotropic1);
	list[7] = new ConstantMedium(b2, 0.01, isotropic2);*/

	Vector3 lookfrom(278, 278, -800);
	Vector3 lookat(278, 278, 0);
	float dist_to_focus = (lookfrom - lookat).length();
	float aperture = 2;
	float vfov = 40.0;
	*cam = new Camera(lookfrom, lookat, Vector3(0, 1, 0), vfov, float(worldx) / float(worldy), aperture, dist_to_focus, 0, 1);
	*world = new HittableList(list, 8);

	Hittable *glass_sphere = new Sphere(Vector3(190, 90, 190), 90, 0);
	Hittable *l= new XZRect(213, 343, 227, 332, 554, 0);
	Hittable **list_sample = new Hittable*[2];
	list_sample[0] = l;
	list_sample[1] = glass_sphere;
	*light_shape = new HittableList(list_sample, 2);
}
int main() 
{

	std::ofstream outfile("MyTest.txt", std::ios::out);
	outfile << "P3\n" << worldx << " " << worldy << "\n255\n";
	
	cornell_box(&world_main,&cam_main,&light_main);
	for (int j = worldy - 1; j >= 0; j--) {
		for (int i = 0; i < worldx; i++) {
			Vector3 color(0, 0, 0);
			for (int s = 0; s < simple; s++) {
				float u = float(i + random_double()) / float(worldx);
				float v = float(j + random_double()) / float(worldy);
				Ray r = cam_main->get_ray(u, v);
				color += de_nan(shading_color(r, world_main,light_main,0));
			}
			color /= float(simple);
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

