#pragma once
#include <map>
#include "shader.h"
#include "Graphics.h"
#include "ray_tracing.h"

namespace GpuDataSetter
{
	
	struct GpuAABB
	{
		GLfloat x1, x2, y1, y2, z1, z2;
		GpuAABB() = default;
		GpuAABB(const aabb& bbox)
		{
			x1 = bbox.x.min;
			x2 = bbox.x.max;
			y1 = bbox.y.min;
			y2 = bbox.y.max;
			z1 = bbox.z.min;
			z2 = bbox.z.max;
			return;
		}
	};

	struct alignas(4) GpuTexture
	{
		GLint type, even, odd;
		GLfloat inv_scale;
		glm::vec3 albedo;
		GLfloat p;
		//float p;
	};
	struct alignas(4) GpuMaterial
	{
		GLint type, tex;
		GLfloat fuzz;
		GLfloat ref_idx;
		glm::vec3 albedo;
		GLfloat p;
	};
	struct GpuHittable
	{
		GLint type, mat, transform_idx, inverse_transform_idx;
		glm::vec3 center;
		//GLfloat _padding1;
		
		glm::vec3 p1;
		//GLfloat _padding2;
		
		glm::vec3 p2;
		//GLfloat _padding3;
		
		glm::vec3 p3;
		//GLfloat _padding4;
		
		glm::vec3 e0;
		//GLfloat _padding5;
		
		glm::vec3 e1;
		//GLfloat _padding6;
		
		glm::vec3 normal;
		GLfloat radius, _p;
		//GpuAABB bbox;
	};
	struct alignas(4) GpuHittableList
	{
		GLint size = 0;
		GLint objects_idx[8];
		//GLint _p[3];
	};
	class GpuDataSetter
	{
	private:
		GLint obj_cur;
		GLint tex_cur;
		vector<GpuHittable> objs;
		vector<GpuMaterial> mats;
		vector<GpuTexture> texs;
		vector<mat4> trans;
		vector<GpuHittableList> worlds;

		map<shared_ptr<texture>, int> _texs;
		map<shared_ptr<material>, int> _mats;

		// images
		vector<uint64_t> tex_handles;
		vector<unsigned int> tex_glids;

		// SSBO
		unsigned int HittableBuffer;
		unsigned int MaterialBuffer;
		unsigned int TextureBuffer;
		unsigned int TransformBuffer;
		unsigned int HittableListBuffer;

		// UBO
		unsigned int ImageBuffer;

		// binding texture
		//int add(image* img)
		//{
		//	glActiveTexture(GL_TEXTURE0 + tex_cur);
		//	glBindTexture(GL_TEXTURE_2D, texids[tex_cur]);
		//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img->width, img->height, 0, GL_RGBA32F, GL_FLOAT, img->data);
		//	return texids[tex_cur++];
		//}

		int add(image* img)
		{
			unsigned int tex;
			glGenTextures(1, &tex);
			glBindTexture(GL_TEXTURE_2D, tex);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img->width, img->height, 0, GL_RGBA32F, GL_FLOAT, img->data);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			uint64_t tex_hndl = glad_glGetTextureHandleARB(tex);
			glMakeTextureHandleResidentARB(tex_hndl);
			tex_glids.push_back(tex);
			tex_handles.push_back(tex_hndl);
			return tex_handles.size() - 1;
		}

		int add(shared_ptr<texture> tex)
		{
			if (_texs.count(tex) != 0)
				return _texs[tex];
			GpuTexture gputex;
			gputex.type = tex->type();
			switch (gputex.type)
			{
				case TEXTURE_SOLID_COLOR:
				{
					solid_color* _tex = dynamic_cast<solid_color*>(tex.get());
					gputex.albedo = _tex->albedo;
					//gputex.albedo = vec4(_tex->albedo, 1.f);
					break;
				}
				case TEXTURE_CHECKER_TEXTURE:
				{
					checker_texture* _tex = dynamic_cast<checker_texture*>(tex.get());
					gputex.inv_scale = _tex->inv_scale;
					gputex.even = add(_tex->even);
					gputex.odd = add(_tex->odd);
					break;
				}
				case TEXTURE_IMAGE_TEXTURE:
				{
					image_texture* _tex = dynamic_cast<image_texture*>(tex.get());
					gputex.even = add(&_tex->_image);
					break;
				}
			}
			_texs[tex] = texs.size();
			texs.push_back(gputex);
			printf(" New Texture:\n - type = %d; idx = %d\n", gputex.type, _texs[tex]);
			return _texs[tex];
		}

		int add(shared_ptr<material> mat)
		{
			if (_mats.count(mat) != 0)
				return _mats[mat];
			GpuMaterial gpumat;

			gpumat.type = mat->type();
			switch (gpumat.type)
			{
				case MATERIAL_LAMBERTIAN:
				{
					lambertian* _mat = dynamic_cast<lambertian*>(mat.get());
					gpumat.tex = add(_mat->tex);
					break;
				}
				case MATERIAL_METAL:
				{
					metal* _mat = dynamic_cast<metal*>(mat.get());
					gpumat.albedo = _mat->albedo;
					//gpumat.albedo = vec4(_mat->albedo, 1.f);
					gpumat.fuzz = _mat->fuzz;
					break;
				}
				case MATERIAL_DIELECTRIC:
				{
					dielectric* _mat = dynamic_cast<dielectric*>(mat.get());
					gpumat.ref_idx = _mat->refraction_index;
					break;
				}
				case MATERIAL_DISSUSE_LIGHT:
				{
					dissuse_light* _mat = dynamic_cast<dissuse_light*>(mat.get());
					gpumat.tex = add(_mat->tex);
					break;
				}
			}
			_mats[mat] = mats.size();
			mats.push_back(gpumat);
			printf(" New Material:\n - type = %d; idx = %d\n", gpumat.type-3, _mats[mat]);
			return _mats[mat];
		}

	public:
		GpuDataSetter()
		{
			obj_cur = tex_cur = 0;
			objs.clear();
			mats.clear();
			texs.clear();
			trans.clear();
			worlds.clear();
			_texs.clear();
			_mats.clear();
			tex_handles.clear();
			// glGenTextures(32, texids);
			glGenBuffers(1, &HittableBuffer);
			glGenBuffers(1, &MaterialBuffer);
			glGenBuffers(1, &TextureBuffer);
			glGenBuffers(1, &TransformBuffer);
			glGenBuffers(1, &HittableListBuffer);
			glGenBuffers(1, &ImageBuffer);
			return;
		}
		void add(shared_ptr<hittable> obj, int scene = 0)
		{
			if (worlds.size() <= scene) {
				worlds.resize(scene + 1);
			}
			worlds[scene].size++;
			worlds[scene].objects_idx[worlds[scene].size - 1] = objs.size();
			printf("=====%d=====", worlds[scene].objects_idx[worlds[scene].size - 1]);

			GpuHittable gpuobj;
			gpuobj.type = obj->type();
			//gpuobj.bbox = obj->bbox;
			//gpuobj.x1 = obj->bbox.x.min;
			//gpuobj.x2 = obj->bbox.x.max;
			//gpuobj.y1 = obj->bbox.y.min;
			//gpuobj.y2 = obj->bbox.y.max;
			//gpuobj.z1 = obj->bbox.z.min;
			//gpuobj.z2 = obj->bbox.z.max;
			gpuobj.mat = add(obj->mat);
			//printf("%d", int(gpuobj.mat));
			trans.push_back(obj->transformation);
			trans.push_back(obj->inversed_trans);
			gpuobj.transform_idx = trans.size() - 2;
			gpuobj.inverse_transform_idx = trans.size() - 1;

			switch (gpuobj.type)
			{
				case HITTABLE_SPHERE:
				{
					sphere* _obj = dynamic_cast<sphere*>(obj.get());
					gpuobj.center = _obj->center;
					gpuobj.radius = _obj->radius;
					break;
				}
			}
			objs.push_back(gpuobj);
			//printf("World Size in worlds: %d\n", worlds[scene].size);
			//printf("World Size in objs: %d\n", objs.size());
			printf(" New Hittable:\n - type = %d; idx = %d\n", objs[worlds[scene].objects_idx[worlds[scene].size - 1]].type, worlds[scene].objects_idx[worlds[scene].size - 1]);
			return;
		}

		void add(hittable_list* world, int scene = 0)
		{
			for (const auto& obj : world->objects)
			{
				add(obj, scene);
			}
			return;
		}

		void set_scene()
		{
			int _size;
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, HittableBuffer);
			glBufferData(GL_SHADER_STORAGE_BUFFER, _size = objs.size() * sizeof(GpuHittable), objs.data(), GL_STATIC_DRAW);
			printf("%d ", _size);

			glBindBuffer(GL_SHADER_STORAGE_BUFFER, MaterialBuffer);
			glBufferData(GL_SHADER_STORAGE_BUFFER, _size = mats.size() * sizeof(GpuMaterial), mats.data(), GL_STATIC_DRAW);
			printf("%d ", _size);

			glBindBuffer(GL_SHADER_STORAGE_BUFFER, TextureBuffer);
			glBufferData(GL_SHADER_STORAGE_BUFFER, _size = texs.size() * sizeof(GpuTexture), texs.data(), GL_STATIC_DRAW);
			printf("%d ", _size);

			glBindBuffer(GL_SHADER_STORAGE_BUFFER, TransformBuffer);
			glBufferData(GL_SHADER_STORAGE_BUFFER, _size = trans.size() * sizeof(mat4), trans.data(), GL_STREAM_DRAW);
			printf("%d ", _size);

			glBindBuffer(GL_SHADER_STORAGE_BUFFER, HittableListBuffer);
			glBufferData(GL_SHADER_STORAGE_BUFFER, _size = worlds.size() * sizeof(GpuHittableList), worlds.data(), GL_STREAM_DRAW);
			printf("%d ", _size);

			glBindBuffer(GL_UNIFORM_BUFFER, ImageBuffer);
			glBufferData(GL_UNIFORM_BUFFER, _size = tex_handles.size() * sizeof(uint64_t), tex_handles.data(), GL_STATIC_DRAW);
			printf("%d\n", _size);

			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, HittableBuffer);
			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, MaterialBuffer);
			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, TextureBuffer);
			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, TransformBuffer);
			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, HittableListBuffer);
			glBindBufferBase(GL_UNIFORM_BUFFER, 5, ImageBuffer);
			GLenum err;
			while ((err = glGetError()) != GL_NO_ERROR) {
				std::cout << "OpenGL error: " << err << std::endl;
			}
			return;
		}
	};
}	// namespace GpuDataSetter