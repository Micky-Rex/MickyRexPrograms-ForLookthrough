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
	vec4 background;
	float pixel_samples_scale;
	int samples_per_pixel;
	vec3 pixel00_center;
	vec3 pixel_delta_u;
	vec3 pixel_delta_v;
	int max_depth;
};

// Scene and data
uniform sampler2D data;
uniform sampler2D images[32];
uniform float rdSeed[4]; // Random seed for random number generation
uniform camera cam;

// Output data
layout (location = 0) out vec4 Color;
in vec2 TexCoord;
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
    vec3 albedo;
    float fuzz;

    // for Dielectric
    float ref_idx;

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

    return RandXY(f, e);
}

vec3 random_unit_vector() {
    float z = rand() * 2.0 - 1.0;
    float a = rand() * 2.0 * pi;
    float r = sqrt(1.0 - z*z);
    float x = r * cos(a);
    float y = r * sin(a);
    return vec3(x, y, z);
}
float randomf(const float min, const float max) {
	return min + (max - min) * rand();
}

vec3 randomvec3(const float min, const float max) {
	return vec3(
		randomf(min, max),
		randomf(min, max),
		randomf(min, max)
	);
}

bool scatter_lambertian(const material mat, const ray r_in, inout hit_record rec, inout vec3 attenuation, inout ray scattered) {
	vec3 scatter_direction = rec.normal + random_unit_vector();
	if (length(scatter_direction) < 1e-8) {
		scatter_direction = rec.normal; // Handle the case where the random vector is too small
	}

	scattered = ray(rec.p, normalize(scatter_direction));
	attenuation = value(mat.tex, rec.u, rec.v, rec.p);
	//attenuation = rec.normal;
	return true;
}

vec3 _reflect(vec3 normal, vec3 vec) {
	return vec - 2.f * dot(vec, normal) * normal;
}

bool scatter_metal(const material mat, const ray r_in, inout hit_record rec, inout vec3 attenuation, inout ray scattered) {

	vec3 reflected = _reflect(rec.normal, r_in.direction);
	reflected = normalize(reflected) + (mat.fuzz * random_unit_vector());
	scattered = ray(rec.p, reflected);
	attenuation = mat.albedo;
	return (dot(scattered.direction, rec.normal) > 0);
}

vec3 _refract(vec3 uv, vec3 normal, float etai_over_etat){
	float cos_theta = min(dot(-uv, normal), 1.f);
	vec3 r_out_perp = etai_over_etat * (uv + cos_theta * normal);
	vec3 r_out_parallel = -sqrt(abs(1.f - pow(length(r_out_perp), 2.f))) * normal;
	return r_out_perp + r_out_parallel;
}

float reflectance(float cosine, float _refraction_index) {
	float r0 = (1 - _refraction_index) / (1 + _refraction_index);
	r0 = r0 * r0;
	return r0 + (1 - r0) * pow(1 - cosine, 5);
}

bool scatter_dielectric(const material mat, const ray r_in, inout hit_record rec, inout vec3 attenuation, inout ray scattered) {
	attenuation = vec3(1.0, 1.0, 1.0);
	float ri = rec.front_face ? (1.0 / mat.ref_idx) : mat.ref_idx;

	vec3 unit_direction = normalize(r_in.direction);
	float cos_theta = min(dot(-unit_direction, rec.normal), 1.0);
	float sin_theta = sqrt(1.0 - cos_theta * cos_theta);
	bool cannot_refract = ri * sin_theta > 1.0;
	vec3 direction;
	if (cannot_refract || reflectance(cos_theta, ri) > rand()) {
		direction = _reflect(rec.normal, unit_direction);
	} else {
		direction = _refract(unit_direction, rec.normal, ri);
	}
	scattered = ray(rec.p, direction);
	return true;
}

bool scatter(const material mat, const ray r_in, inout hit_record rec, inout vec3 attenuation, inout ray scattered) {
	if (mat.type == MATERIAL_LAMBERTIAN)
		return scatter_lambertian(mat, r_in, rec, attenuation, scattered);
	if (mat.type == MATERIAL_METAL)
		return scatter_metal(mat, r_in, rec, attenuation, scattered);
	if (mat.type == MATERIAL_DIELECTRIC)
		return scatter_dielectric(mat, r_in, rec, attenuation, scattered);
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
	rec.t = root;// - 1e-3f;
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

vec3 sample_square() {
	return vec3(rand() - 0.5f, rand() - 0.5f, 0.f);
}

ray get_ray(const int i, const int j) {
	vec3 offset = sample_square();
	vec3 pixel_sample = cam.pixel00_center + cam.pixel_delta_u * (i + offset.x) + cam.pixel_delta_v * (j + offset.y);
	vec3 direction = normalize(pixel_sample - cam.look_from + randomvec3(-2e-3, 2e-3));
	return ray(cam.look_from + randomvec3(-1e-3, 1e-3), direction);
}

void main() {
	hittable_list world;
	world.transform = mat4(1.0);
	world.inverse_transform = mat4(1.0);
	world.bbox = aabb(interval(-1e8, 1e8), interval(-1e8, 1e8), interval(-1e8, 1e8));
	hittable obj;
	obj.type = HITTABLE_SPHERE;
	obj.center = vec3(0.0, 0.0, -1.2);
	obj.radius = 0.5;
	obj.mat = 0;
	obj.transform = mat4(1.0);
	obj.inverse_transform = mat4(1.0);
	obj.bbox.x = interval(-0.5, 0.5);
	obj.bbox.y = interval(-0.5, 0.5);
	obj.bbox.z = interval(-1.7, -0.7);
	materials[0].type = MATERIAL_LAMBERTIAN;
	materials[0].tex = 0;
	textures[0].type = TEXTURE_SOLID_COLOR;
	textures[0].albedo = vec3(0.1, 0.2, 0.5);
	world.hittables[0] = obj;

	obj.type = HITTABLE_SPHERE;
	obj.center = vec3(1.0, 0.0, -1.0);
	obj.radius = 0.5;
	obj.mat = 1;
	obj.transform = mat4(1.0);
	obj.inverse_transform = mat4(1.0);
	obj.bbox.x = interval(0.5, 1.5);
	obj.bbox.y = interval(-0.5, 0.5);
	obj.bbox.z = interval(-1.5, -0.5);
	materials[1].type = MATERIAL_LAMBERTIAN;
	materials[1].tex = 1;
	textures[1].type = TEXTURE_SOLID_COLOR;
	textures[1].albedo = vec3(0.8, 0.8, 0.8);
	world.hittables[1] = obj;

	obj.type = HITTABLE_SPHERE;
	obj.center = vec3(-1.0, 0.0, -1.0);
	obj.radius = 0.5;
	obj.mat = 2;
	obj.transform = mat4(1.0);
	obj.inverse_transform = mat4(1.0);
	obj.bbox.x = interval(-0.5, -1.5);
	obj.bbox.y = interval(-0.5, 0.5);
	obj.bbox.z = interval(-1.5, -0.5);
	materials[2].type = MATERIAL_LAMBERTIAN;
	materials[2].tex = 2;
	textures[2].type = TEXTURE_SOLID_COLOR;
	textures[2].albedo = vec3(0.8, 0.3, 0.3);
	world.hittables[2] = obj;
	
	materials[2].type = MATERIAL_METAL;
	materials[2].fuzz = 0.2;
	materials[2].albedo = vec3(0.8, 0.3, 0.3);
	world.hittables[2] = obj;

	materials[2].type = MATERIAL_DIELECTRIC;
	materials[2].ref_idx = 1.5;

	obj.type = HITTABLE_SPHERE;
	obj.center = vec3(0.0, -100.5, -1.0);
	obj.radius = 100.f;
	obj.mat = 3;
	obj.transform = mat4(1.0);
	obj.inverse_transform = mat4(1.0);
	obj.bbox.x = interval(-0.5, 0.5);
	obj.bbox.y = interval(-0.5, 0.5);
	obj.bbox.z = interval(-1.5, -0.5);
	materials[3].type = MATERIAL_LAMBERTIAN;
	materials[3].tex = 3;
	textures[3].type = TEXTURE_SOLID_COLOR;
	textures[3].albedo = vec3(0.8, 0.8, 0.0);
	world.hittables[3] = obj;
	world.size = 4;

	textures[3].type = TEXTURE_CHECKER_TEXTURE;
	textures[3].inv_scale = 1;
	textures[3].albedo = vec3(0.8, 0.8, 0.0);
	textures[3].albedo_odd = vec3(1,1,1);

    int x = int(gl_FragCoord.x);
    int y = int(gl_FragCoord.y);
    ray r = get_ray(x, y);
    //vec3 col = ray_color(r, world, 50);
    vec3 col = render_pixel(r, world, cam.max_depth);
    
    //FragColor = vec4(col, 1.0);
    FragColor = vec4(linear_to_gamma(col), 1.0);
}