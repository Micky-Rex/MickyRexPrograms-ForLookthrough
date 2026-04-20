#version 460 core
#extension GL_ARB_bindless_texture : enable
#extension GL_EXT_nonuniform_qualifier : require

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
	float pixel_samples_scale;
	int image_width, image_height, samples_per_pixel, max_depth;
	vec3 pixel00_center, pixel_delta_u, pixel_delta_v, look_from, background;
}; uniform camera cam;

struct interval { float min, max; };
struct aabb { interval x, y, z; };
/*
CodeBlock{
	struct Texture {
		int type;

		// for Solid color
		vec3 albedo;
		
		// for Checker Texture
		float inv_scale; // for checker Texture
		int even, odd;	// id of Texture
		// color for even Texture is albedo

		// for Image Texture
		// image id is even
	};
	struct Material {
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
	struct Hittable {
		aabb bbox;
		int type, mat, transform_idx, inverse_transform_idx;
		// === for Sphere ===
		vec3 center;
		float radius;
		// === for Triangle ===
		vec3 p1, p2, p3, e0, e1, normal;
		// outward_normal is normal
		// === for Quad ===
		// Q is p1
		// u is p2
		// v is p3
		// w is e0
		// D is radius
		// normal already defined in Triangle
	};

	struct HittableList {
		int size;
		int objects_idx[8];
	};};
*/
struct Texture {
	int type, even, odd;
	float inv_scale;
	vec3 albedo;
	//float p;
};
struct Material {
	int type, tex;
	float fuzz;
	float ref_idx;
	vec3 albedo;
	//float p;
};
struct Hittable {
	int type, mat, transform_idx, inverse_transform_idx;
	vec3 center;
	//float _padding1;
	vec3 p1;
	//float _padding2;
	vec3 p2;
	//float _padding3;
	vec3 p3;
	//float _padding4;
	vec3 e0;
	//float _padding5;
	vec3 e1;
	//float _padding6;
	vec3 normal;
	float radius;
	//aabb bbox;
};
struct HittableList {
	int size;
	int objects_idx[8];
	//int _p[3];
};
uniform int scene = 0;	// HittableList index
//uniform int hittable_count; // Number of hittables in the scene
//uniform int image_count; // Number of images in the scene
uniform float rdSeed[4]; // Random seed for random number generation
//uniform sampler2D images[32]; // Array of images for texture sampling
layout(std430, binding = 0) buffer HittableBuffer {
    Hittable hittables[2];
};
layout(std430, binding = 1) buffer MaterialBuffer {
    Material materials[];
};
layout(std430, binding = 2) buffer TextureBuffer {
    Texture textures[];
};
layout(std430, binding = 3) buffer TransformBuffer {
    mat4 transformations[];
};
layout(std430, binding = 4) buffer HittableListBuffer {
	HittableList world[];
};
layout(bindless_sampler, binding = 5) uniform ImageBuffer {
	sampler2D images[]; // Bindless texture array for images
};
//layout(bindless_sampler) uniform sampler2D images[128];

// Output data
//layout (location = 0) out vec4 Color;
in vec2 TexCoord;
out vec4 FragColor;

bool in_interval(const float _min, const float _max, const float value) {
	return value >= _min && value <= _max;
}

bool in_interval(const interval itv, const float value) {
	return in_interval(itv.min, itv.max, value);
}

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

vec3 _checker_value(const int tex_id, const float u, const float v, const vec3 p) {
	int tex_stack[5];
	int stack_ptr = 0;
	vec3 final_color = vec3(0, 0, 0);
	tex_stack[stack_ptr++] = tex_id;
	while(stack_ptr > 0) {
		Texture tex = textures[tex_stack[--stack_ptr]];
		if(tex.type == TEXTURE_CHECKER_TEXTURE) {
			int x = int(floor(tex.inv_scale * p.x));
			int y = int(floor(tex.inv_scale * p.y));
			int z = int(floor(tex.inv_scale * p.z));
			bool isEven = (x + y + z) % 2 == 0;
			tex_stack[stack_ptr++] = isEven ? tex.even : tex.odd;
		}
		else if(tex.type == TEXTURE_SOLID_COLOR) {
			return tex.albedo;
		}
		else if(tex.type == TEXTURE_IMAGE_TEXTURE) {
			//if(tex.even < 0 || tex.even >= image_count) {
			//	return vec3(0.0, 0.0, 0.0); // Return black if image index is invalid
			//}
			final_color = texture(images[tex.even], vec2(u, v)).rgb;
			return final_color; // Return the color from the image texture
		}
		else {
			return vec3(1.0, 1.0, 1.0); // Default to white if Texture type is unknown
		}

	}
}

vec3 value(const int tex_id, const float u, const float v, const vec3 p) {
	if(tex_id < 0 || tex_id >= textures.length()) {
		return vec3(0.0, 0.0, 0.0);
	}
	const Texture tex = textures[tex_id];
	if(tex.type == TEXTURE_SOLID_COLOR)
	{
		return tex.albedo;
	}
	else if(tex.type == TEXTURE_CHECKER_TEXTURE)
	{
		return _checker_value(tex_id, u, v, p);
	}
	else if(tex.type == TEXTURE_IMAGE_TEXTURE)
	{
		//if(tex.even < 0 || tex.even >= image_count) {
		//	return vec3(0.0, 0.0, 0.0); // Return black if image index is invalid
		//}
		return texture(images[tex.even], vec2(u, v)).rgb;
	}
	return vec3(1.0, 1.0, 1.0); // Default to white if Texture type is unknown
}

struct ray {
	vec3 origin;
	vec3 direction;
};
vec3 at(const ray r, float t) {
	return vec3(r.origin + t * r.direction);
}

bool is_zero(const mat4 mat) {
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			if(abs(mat[i][j]) > 1e-6f) return false;
		}
	}
	return true;
}

void located_ray(inout ray r, const mat4 inverse_matrix) {
	if (is_zero(inverse_matrix)) return;
	r.origin = (inverse_matrix * vec4(r.origin, 1.0f)).xyz;
	r.direction = (inverse_matrix * vec4(r.direction, 0.0f)).xyz;
	//r.direction = normalize((inverse_matrix * vec4(r.direction, 0.0f)).xyz);
	return;
}

struct hit_record {
	vec3 p;
	int mat;
	vec3 normal;
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
    float a = RandXY(TexCoord.x, rdSeed[0]);
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

bool scatter_lambertian(const Material mat, const ray r_in, inout hit_record rec, inout vec3 attenuation, inout ray scattered) {
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

bool scatter_metal(const Material mat, const ray r_in, inout hit_record rec, inout vec3 attenuation, inout ray scattered) {

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

float _reflectance(float cosine, float _refraction_index) {
	float r0 = (1 - _refraction_index) / (1 + _refraction_index);
	r0 = r0 * r0;
	return r0 + (1 - r0) * pow(1 - cosine, 5);
}

bool scatter_dielectric(const Material mat, const ray r_in, inout hit_record rec, inout vec3 attenuation, inout ray scattered) {
	attenuation = vec3(1.0, 1.0, 1.0);
	float ri = rec.front_face ? (1.0 / mat.ref_idx) : mat.ref_idx;

	vec3 unit_direction = normalize(r_in.direction);
	float cos_theta = min(dot(-unit_direction, rec.normal), 1.0);
	float sin_theta = sqrt(1.0 - cos_theta * cos_theta);
	bool cannot_refract = ri * sin_theta > 1.0;
	vec3 direction;
	if (cannot_refract || _reflectance(cos_theta, ri) > rand()) {
		direction = _reflect(rec.normal, unit_direction);
	} else {
		direction = _refract(unit_direction, rec.normal, ri);
	}
	scattered = ray(rec.p, direction);
	return true;
}

bool scatter(const Material mat, const ray r_in, inout hit_record rec, inout vec3 attenuation, inout ray scattered) {
	if (mat.type == MATERIAL_LAMBERTIAN)
		return scatter_lambertian(mat, r_in, rec, attenuation, scattered);
	if (mat.type == MATERIAL_METAL)
		return scatter_metal(mat, r_in, rec, attenuation, scattered);
	if (mat.type == MATERIAL_DIELECTRIC)
		return scatter_dielectric(mat, r_in, rec, attenuation, scattered);
	return false;
}

vec3 emit(const int mat_id, const float u, const float v, const vec3 p) {
	const Material mat = materials[mat_id];
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

bool _hit_located_ray_sphere(const Hittable sphere, const ray r, interval ray_t, inout hit_record rec) {
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

bool _hit_located_ray(const Hittable obj, const ray r, interval ray_t, inout hit_record rec) {
	if (obj.type == HITTABLE_SPHERE)
		return _hit_located_ray_sphere(obj, r, ray_t, rec);
	return false;
}

bool hit(const Hittable obj, const ray r, const interval ray_t, inout hit_record rec) {
	ray located = r;
	located_ray(located, transformations[obj.inverse_transform_idx]);

	bool result = _hit_located_ray(obj, located, ray_t, rec);
	if (is_zero(transformations[obj.transform_idx])) return result;
    rec.p = (transformations[obj.transform_idx] * vec4(rec.p, 1.0)).xyz;
    rec.normal = normalize(mat3(transpose(transformations[obj.inverse_transform_idx])) * rec.normal); // Transform normal
    return result;
}

bool _hit_located_ray(const HittableList _world, const ray r, interval ray_t, inout hit_record rec) {
	bool hit_anything = false;
	float closest_so_far = ray_t.max;
	for(int i = 0; i < _world.size; i++) {
		if(hit(hittables[_world.objects_idx[i]], r, interval(ray_t.min, closest_so_far), rec)) {
			hit_anything = true;
			closest_so_far = rec.t;
		}
	}
	return hit_anything;
}

bool hit(const HittableList _world, const ray r, const interval ray_t, inout hit_record rec) {

	//if(!_hit_located_ray(world.bbox, located, ray_t))
	//	return true;
	bool result = _hit_located_ray(_world, r, ray_t, rec);
    return result;
}

vec3 ray_color(ray _r, const HittableList _world, int depth) {
	vec3 current_attenuation = vec3(1.0);
	vec3 result_color = vec3(0.0);
	ray r = _r;
	for (int i = depth; i > 0; --i) {
		hit_record rec;
		if (!hit(_world, r, interval(1e-5f, 1e8f), rec)) {
			float a = 0.5f * (normalize(r.direction).y + 1.f);
			result_color += current_attenuation * ((1.f - a) * vec3(1.f, 1.f, 1.f) + a * vec3(0.5f, 0.7f, 1.f));//cam.background.xyz;
			//result_color += current_attenuation * cam.background;//cam.background.xyz;
			break;
		}
		//result_color = vec3(1,0,0);
		//break;
		vec3 emitted = emit(rec.mat, rec.u, rec.v, rec.p);
		result_color += current_attenuation * emitted;
		
		ray scattered;
		vec3 attenuation;
		if (!scatter(materials[rec.mat], r, rec, attenuation, scattered))
			break;
			
		//attenuation = rec.normal;
		current_attenuation *= attenuation;
		r = scattered;
	}
	return result_color;
}

vec3 sample_square() {
	return vec3(rand() - 0.5f, rand() - 0.5f, 0.f);
}

ray get_ray(const int i, const int j) {
	vec3 offset = sample_square();
	vec3 pixel_sample = cam.pixel00_center + cam.pixel_delta_u * (i + offset.x) + cam.pixel_delta_v * (j + offset.y);
	//vec3 direction = (pixel_sample - cam.look_from + randomvec3(-2e-3, 2e-3));
	vec3 direction = normalize(pixel_sample - cam.look_from + randomvec3(-2e-3, 2e-3));
	return ray(cam.look_from + randomvec3(-1e-3, 1e-3), direction);
}

vec3 render_pixel(int x, int y, const HittableList _world, int depth) {
	vec3 color = vec3(0,0,0);
    ray r;
	for (int i = 0; i < cam.samples_per_pixel; ++i) {
		r = get_ray(x, y);
		color += ray_color(r, _world, depth);
	}
	return color * cam.pixel_samples_scale;
}

void main() {
    int x = int(TexCoord.x * cam.image_width);
    int y = int(TexCoord.y * cam.image_height);
    //vec3 col = ray_color(r, world, 50);
    vec3 col = render_pixel(x, y, world[scene], cam.max_depth);
    //FragColor = vec4(world[scene].size, 0, 0, 1.0);
    //FragColor = vec4(linear_to_gamma(FragColor.xyz), 1.0);
    FragColor = vec4(linear_to_gamma(col), 1.0);
	//FragColor = vec4(hittables[0].radius,0,0,1);
	//FragColor = vec4(textures[materials[hittables[world[scene].objects_idx[0]].mat].tex].albedo,1);
	//FragColor = vec4(transformations[hittables[world[scene].objects_idx[0]].inverse_transform_idx][2]);
	//FragColor = vec4(sizeof(hittables), 0, 0, 1);;
	//FragColor = vec4(materials[hittables[world[scene].objects_idx[0]].mat].tex-5, 0, 0,1);
	//FragColor = vec4(textures[1].albedo, 1);
}
