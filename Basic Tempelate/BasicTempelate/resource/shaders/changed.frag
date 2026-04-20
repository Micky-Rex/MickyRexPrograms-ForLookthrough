#version 460 core
precision highp float;

const float pi = 3.14159265358979323846;

// Ray tracing element constants
const int TEXTURE_SOLID_COLOR      = 0;
const int TEXTURE_CHECKER_TEXTURE  = 1;
const int TEXTURE_IMAGE_TEXTURE    = 2;
const int MATERIAL_LAMBERTIAN      = 3;
const int MATERIAL_METAL           = 4;
const int MATERIAL_DIELECTRIC      = 5;
const int MATERIAL_DISSUSE_LIGHT   = 6;
const int HITTABLE_SPHERE         = 7;
const int HITTABLE_TRIANGLE       = 8;
const int HITTABLE_QUAD           = 9;
const int HITTABLE_MODEL          = 10;
const int HITTABLE_LIST           = 11;

struct camera {
	vec3 look_from;
	//vec4 background;
	float pixel_samples_scale;
	int samples_per_pixel;
	vec3 pixel00_center;
	vec3 pixel_delta_u;
	vec3 pixel_delta_v;
};

// Scene and data
uniform sampler2D scene;
uniform sampler2D data;
uniform sampler2D images[32];
uniform float rdSeed[4]; // Random seed for random number generation
uniform camera cam;

// Output data
layout (location = 0) out vec4 Color;
out vec4 FragColor;

float linear_to_gamma(float x) {
	if(x > 0) return sqrt(x);
	return 0;
}

vec3 linear_to_gamma(vec3 x) {
	return vec3(
		linear_to_gamma(x.x),
		linear_to_gamma(x.y),
		linear_to_gamma(x.z)
	);
}

struct interval {
	float min, max;
};

struct aabb {
	interval x, y, z;
};

bool in_interval(const float _min, const float _max, const float value) {
	return value >= _min && value <= _max;
}

bool in_interval(const interval itv, const float value) {
	return in_interval(itv.min, itv.max, value);
}

struct Texture {
	int type;

	// for Solid color
	vec3 albedo;
	
	// for Checker Texture
	float inv_scale; // for checker Texture
	vec3 albedo_odd;	// index for even Texture
	// index for even Texture is albedo

	// for Image Texture
	int image;
};
//Texture textures[8192];
Texture textures[32];

vec3 value(const int tex_id, const float u, const float v, const vec3 p) {
	const Texture tex = textures[tex_id];
	if(tex.type == TEXTURE_SOLID_COLOR)
	{
		return tex.albedo;
	}
	else if(tex.type == TEXTURE_CHECKER_TEXTURE)
	{
		int x = int(floor(tex.inv_scale * p.x));
		int y = int(floor(tex.inv_scale * p.y));
		int z = int(floor(tex.inv_scale * p.z));
		bool isEven = (x + y + z) % 2 == 0;
		return isEven ? tex.albedo : tex.albedo_odd;
	}
	else if(tex.type == TEXTURE_IMAGE_TEXTURE)
	{
		if(tex.image < 0 || tex.image >= images.length()) {
			return vec3(0.0, 0.0, 0.0); // Return black if image index is invalid
		}
		return texture(images[tex.image], vec2(u, v)).rgb;
	}
	return vec3(1.0, 1.0, 1.0); // Default to white if Texture type is unknown
}

struct material {
    int type;

    // for Lambertian
    int tex;

    // for Metal
    int albedo;
    int fuzz;

    // for Dielectric
    int ref_idx;

    // for Diffuse light
    // "tex" already defined in lambertian
};
//material materials[4096];
material materials[32];

struct hittable {
    int type;
	aabb bbox;
    int mat;
	mat4 transform;
	mat4 inverse_transform;

    // for Sphere
    vec3 center;
    float radius;

    // for Triangle
    vec3 p0;
    vec3 p1;
    vec3 p2;
    // outward_normal is normal

    // for Quad
    // Q is p0
    // u is p1
    // v is p2
    // w is center
    // D is radius
    vec3 normal;
};

struct hittable_list {
    int size;
	aabb bbox;
	mat4 transform;
	mat4 inverse_transform;
    //hittable hittables[65536]; // Assuming a maximum of 65536 hittables
    hittable hittables[64]; // Assuming a maximum of 65536 hittables
};

//hittable_list hittables[640];
hittable_list hittables[64];

struct ray {
	vec3 origin;
	vec3 direction;
};
vec3 at(const ray r, float t) {
	return vec3(r.origin + t * r.direction);
}

void located_ray(inout ray r, const mat4 inverse_matrix) {
	r.origin = (inverse_matrix * vec4(r.origin, 1.0f)).xyz;
	r.direction = (inverse_matrix * vec4(r.direction, 0.0f)).xyz;
	return;
}

struct hit_record {
	vec3 p;
	vec3 normal;
	int mat;
	float t, u, v;
	bool front_face;
};

void set_face_normal(inout hit_record rec, ray r, vec3 outward_normal) {
	rec.front_face = dot(r.direction, outward_normal) < 0.0;
	rec.normal = rec.front_face ? outward_normal : -outward_normal;
	return;
}

float rdCnt = 0.f;
float RandXY(float x, float y) {
     return fract(cos(x * (12.9898) + y * (4.1414)) * 43758.5453);
}
float rand() {
    float a = RandXY(gl_FragCoord.x, rdSeed[0]);
    float b = RandXY(rdSeed[1], gl_FragCoord.y);
    float c = RandXY(rdCnt++, rdSeed[2]);
    float d = RandXY(rdSeed[3], a);
    float e = RandXY(b, c);
    float f = RandXY(d, e);

    return RandXY(e, f);
}

vec3 random_unit_vector() {
    float z = rand() * 2.0 - 1.0;
    float a = rand() * 2.0 * pi;
    float r = sqrt(1.0 - z*z);
    float x = r * cos(a);
    float y = r * sin(a);
    return vec3(x, y, z);
}

bool scatter_lambertian(const material mat, const ray r_in, inout hit_record rec, inout vec3 attenuation, inout ray scattered) {
	vec3 scatter_direction = rec.normal + random_unit_vector();
	if (length(scatter_direction) < 1e-8) {
		scatter_direction = rec.normal; // Handle the case where the random vector is too small
	}
	scattered = ray(rec.p, normalize(scatter_direction));
	attenuation = value(mat.tex, rec.u, rec.v, rec.p);
	return true;
}

bool scatter(const material mat, const ray r_in, inout hit_record rec, inout vec3 attenuation, inout ray scattered) {
	if (mat.type == MATERIAL_LAMBERTIAN)
		return scatter_lambertian(mat, r_in, rec, attenuation, scattered);
	return false;
}

vec3 emit(const int mat_id, const float u, const float v, const vec3 p) {
	const material mat = materials[mat_id];
	if (mat.type == MATERIAL_DISSUSE_LIGHT) {
		return value(mat.tex, u, v, p);
	}
	return vec3(0,0,0);
}

bool _hit_located_ray(const aabb bbox, const ray r, interval ray_t) {
	const float adinv_x = 1.0 / r.direction.x;
	const float adinv_y = 1.0 / r.direction.y;
	const float adinv_z = 1.0 / r.direction.z;

	const float t0x = (bbox.x.min - r.origin.x) * adinv_x;
	const float t1x = (bbox.x.max - r.origin.x) * adinv_x;
	const float t0y = (bbox.y.min - r.origin.y) * adinv_y;
	const float t1y = (bbox.y.max - r.origin.y) * adinv_y;
	const float t0z = (bbox.z.min - r.origin.z) * adinv_z;
	const float t1z = (bbox.z.max - r.origin.z) * adinv_z;

	float tmin = max(max(min(t0x, t1x), min(t0y, t1y)), min(t0z, t1z));
	float tmax = min(min(max(t0x, t1x), max(t0y, t1y)), max(t0z, t1z));

	if (tmax < tmin || tmax < ray_t.min || tmin > ray_t.max) 
		return false;
	
	return true;
}

void get_uv_sphere(const vec3 p, inout float u, inout float v) {
	float theta = acos(-p.y);
	float phi = atan(-p.z, p.x) + pi;
	u = phi / (2 * pi);
	v = theta / pi;
	return;
}

bool _hit_located_ray_sphere(const hittable sphere, const ray r, interval ray_t, inout hit_record rec) {
    //return true;
	vec3 oc = sphere.center - r.origin;
	float a = dot(r.direction, r.direction);
	float h = dot(r.direction, oc);
	float c = dot(oc,oc) - sphere.radius * sphere.radius;
	float discriminant = h * h - a * c;
	if (discriminant < 0) return false;

	float sqrtd = sqrt(discriminant);
	float root = (h - sqrtd) / a;
	if (!in_interval(ray_t, root)) {
		root = (h + sqrtd) / a;
		if (!in_interval(ray_t, root)) return false;
	}
	rec.t = root;
	rec.p = at(r, rec.t);
	vec3 outward_normal = (rec.p - sphere.center) / sphere.radius;
	set_face_normal(rec, r, outward_normal);
	get_uv_sphere(outward_normal, rec.u, rec.v);
	rec.mat = sphere.mat;
	return true;
}

bool _hit_located_ray(const hittable obj, const ray r, interval ray_t, inout hit_record rec) {
	if (obj.type == HITTABLE_SPHERE)
		return _hit_located_ray_sphere(obj, r, ray_t, rec);
	return false;
}

bool hit(const hittable obj, const ray r, const interval ray_t, inout hit_record rec) {
	ray located = r;
	located_ray(located, obj.inverse_transform);

	//if(!_hit_located_ray(world.bbox, located, ray_t))
	//	return true;
	bool result = _hit_located_ray(obj, located, ray_t, rec);
    rec.p = (vec4(rec.p, 1.0) * obj.transform).xyz;
    rec.normal = (obj.transform * vec4(rec.normal, 1.0)).xyz; // Transform normal
    return result;
}

bool _hit_located_ray(const hittable_list world, const ray r, interval ray_t, inout hit_record rec) {
	bool hit_anything = false;
	float closest_so_far = ray_t.max;
	for(int i = 0; i < world.size; i++) {
		if(hit(world.hittables[i], r, interval(ray_t.min, closest_so_far), rec)) {
			hit_anything = true;
			closest_so_far = rec.t;
		}
	}
	return hit_anything;
}

bool hit(const hittable_list world, const ray r, const interval ray_t, inout hit_record rec) {
	ray located = r;
	located_ray(located, world.inverse_transform);

	//if(!_hit_located_ray(world.bbox, located, ray_t))
	//	return true;
	bool result = _hit_located_ray(world, located, ray_t, rec);
    rec.p = (vec4(rec.p, 1.0) * world.transform).xyz;
    rec.normal = (world.transform * vec4(rec.normal, 1.0)).xyz; // Transform normal
    return result;
}

in vec4 color;

vec4 getScene(ivec2 pos) {
	return texelFetch(scene, pos, 0);
}

vec4 getData(ivec2 pos) {
	return texelFetch(data, pos, 0);
}

vec3 ray_color(ray r, const hittable_list world, int depth) {
	vec3 current_attenuation = vec3(1.0);
	vec3 result_color = vec3(0.0);
	
	hit_record rec;
	for (int i = depth; i > 0; i--) {
		if (!hit(world, r, interval(1e-5f, 1e8f), rec)) {
			float a = 0.5f * (normalize(r.direction).y + 1.f);
			result_color += current_attenuation * ((1.f - a) * vec3(1.f, 1.f, 1.f) + a * vec3(0.5f, 0.7f, 1.f));//cam.background.xyz;
			break;
		}
		//result_color = vec3(1,0,0);
		//break;
		vec3 emitted = emit(rec.mat, rec.u, rec.v, rec.p);
		result_color += current_attenuation * emitted;
		
		vec3 attenuation;
		ray scattered;
		if (!scatter(materials[rec.mat], r, rec, attenuation, scattered))
			break;
			
		//attenuation = rec.normal;
		current_attenuation *= attenuation;
		r = scattered;
	}
	return result_color;
}

vec3 render_pixel(ray r, const hittable_list world, int depth) {
	vec3 color = vec3(0,0,0);
	for (int i = 0; i < cam.samples_per_pixel; i++) {
		color += ray_color(r, world, depth) * cam.pixel_samples_scale;
	}
	return color;
}
/*
void init()
{
	cam.image_height = int(cam.image_width / cam.aspect_ratio);
	cam.pixel_samples_scale = 1.f / cam.samples_per_pixel;
	float focal_length = length(cam.look_from - cam.look_at);
	float theta = radians(cam.vfov);
	float h = tan(theta / 2.f);
	float viewport_height = 2.f * h * focal_length;
	float viewport_width = viewport_height * cam.aspect_ratio;
	cam.w = normalize(cam.look_from - cam.look_at);
	cam.u = normalize(cross(cam.vup, cam.w));
	cam.v = normalize(cross(cam.w, cam.u));
	vec3 viewport_u = viewport_width * cam.u;
	vec3 viewport_v = viewport_height * cam.v;
	cam.pixel_delta_u = viewport_u / cam.image_width;
	cam.pixel_delta_v = viewport_v / cam.image_height;
	vec3 viewport_upper_left = cam.look_from - cam.w * focal_length - viewport_u / 2.f - viewport_v / 2.f;
	cam.pixel00_center = viewport_upper_left + cam.pixel_delta_u * 0.5f + cam.pixel_delta_v * 0.5f;
	return;
}
*/
vec3 sample_square() {
	return vec3(rand() - 0.5f, rand() - 0.5f, 0.f);
}

ray get_ray(const int i, const int j) {
	vec3 offset = sample_square();
	vec3 pixel_sample = cam.pixel00_center + cam.pixel_delta_u * (i + offset.x) + cam.pixel_delta_v * (j + offset.y);
	vec3 direction = normalize(pixel_sample - cam.look_from);
	return ray(cam.look_from, direction);
}

void main() {
/*
	cam.image_width = 1600;
	cam.vup = vec3(0.f, 1.f, 0.f);
	cam.vfov = 90.f;
	cam.samples_per_pixel = 50;
	cam.aspect_ratio = 16.f/9.f;
	cam.look_from = vec3(0.0, 0.0, 0.0);
	cam.look_at = vec3(0.0, 0.0, -1.0);
	cam.background = vec4(0.5, 0.7, 1.0, 1.0); // Light blue background
	init();
*/
	//hittables[0].type = HITTABLE_SPHERE;
	hittable_list world;
	world.transform = mat4(1.0);
	world.inverse_transform = mat4(1.0);
	world.bbox = aabb(interval(-1e8, 1e8), interval(-1e8, 1e8), interval(-1e8, 1e8));
	hittable obj;
	obj.type = HITTABLE_SPHERE;
	obj.center = vec3(0.0, 0.0, -1.0);
	obj.radius = 0.5;
	obj.mat = 0;
	obj.transform = mat4(1.0);
	obj.inverse_transform = mat4(1.0);
	obj.bbox.x = interval(-0.5, 0.5);
	obj.bbox.y = interval(-0.5, 0.5);
	obj.bbox.z = interval(-1.5, -0.5);
	materials[0].type = MATERIAL_LAMBERTIAN;
	materials[0].tex = 0;
	textures[0].type = TEXTURE_SOLID_COLOR;
	textures[0].albedo = vec3(0.8, 0.3, 0.3);
	world.hittables[0] = obj;

	obj.type = HITTABLE_SPHERE;
	obj.center = vec3(0.0, -1000.5, -1.0);
	obj.radius = 1000.f;
	obj.mat = 1;
	obj.transform = mat4(1.0);
	obj.inverse_transform = mat4(1.0);
	obj.bbox.x = interval(-0.5, 0.5);
	obj.bbox.y = interval(-0.5, 0.5);
	obj.bbox.z = interval(-1.5, -0.5);
	materials[1].type = MATERIAL_LAMBERTIAN;
	materials[1].tex = 1;
	textures[1].type = TEXTURE_SOLID_COLOR;
	textures[1].albedo = vec3(0.4, 0.4, 0.4);
	world.hittables[1] = obj;
	world.size = 2;

    int x = int(gl_FragCoord.x);
    int y = int(gl_FragCoord.y);
    ray r = get_ray(x, y);
    //vec3 col = ray_color(r, world, 50);
    vec3 col = render_pixel(r, world, 50);
    
    FragColor = vec4(col, 1.0);
    //FragColor = vec4(linear_to_gamma(col), 1.0);
}