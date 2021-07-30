#pragma once
#ifndef TOOLH
#define TOOLH
#include<float.h>
#include<random>
#include <functional>
#include"vector.h"

#define M_PI 3.14159265358

inline double random_double();
Vector3 random_in_unit_sphere();
Vector3 random_in_unit_disk();

#endif // !TOOLH