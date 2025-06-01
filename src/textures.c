#include <err.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

#include <SDL3/SDL_opengl.h>
#include <png.h>

#include "gutl.h"
#include "hashmap.h"
#include "resources.h"
#include "textures.h"

struct hashmap *idmap;
size_t texcount;
static unsigned textures[4096];

static int readpng(FILE *, long *width, long *height, int *internalformat,
		int *format, png_bytep *image);
static int typepng2gl(int bit_depth, int color_type, int *format);

__attribute__((constructor)) static void idmap_init(void) {
	idmap = HMAP_create(4096, NULL);

	if (idmap == NULL)
		err(2, "HMAP_create", NULL);
}

__attribute__((destructor)) static void idmap_delete(void) {
	HMAP_destroy(idmap);
}

long UBLC_textures_loadtexture(const char *resource, int mode) {
	unsigned *id_p = HMAP_get(idmap, resource);
	if (id_p != NULL)
		return *id_p;

	if (texcount >= 4096)
		return -1;

	glGenTextures(1, &textures[texcount]);
	unsigned id = textures[texcount++];

	UBLC_textures_bind(id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, mode);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mode);

	int texture_fd = openat(UBLC_fs.resources, resource, O_RDONLY |
			O_SHLOCK);
	if (texture_fd < 0)
		err(2, "openat: %i %s", UBLC_fs.resources, resource);

	FILE *texfile = fdopen(texture_fd, "rb");
	if (texfile == NULL)
		err(2, "fdopen: %i", texture_fd);

	long width, height;
	int internalformat;
	int format;
	png_bytep pixels;
	if (readpng(texfile, &width, &height, &internalformat, &format,
				&pixels))
		errx(2, "readpng: Failed to read PNG", NULL);

	fclose(texfile);

	glTexImage2D(GL_TEXTURE_2D, 0, internalformat, width, height, 0, format,
			GL_UNSIGNED_BYTE, pixels);
	GLenum glerr = glGetError();
	if (glerr)
		warnx("glTexImage2D: %s", GUTL_errorstr(glerr));
	/* glGenerateMipmap(GL_TEXTURE_2D); */

	free(pixels);

	return id;
}

void UBLC_textures_bind(unsigned id) {
	static long lastid = -1;
	if (id != lastid) {
		glBindTexture(GL_TEXTURE_2D, id);
		lastid = id;
	}
}

static int readpng(FILE *infile, long *width, long *height, int *internalformat,
		int *format, png_bytep *image) {
	unsigned char sig[8];
	fread(sig, 1, 8, infile);

	if (png_sig_cmp(sig, 0, 8)) {
		warnx("libpng: Bad signature");
		return -1;
	}

	png_structp png_reader = png_create_read_struct(PNG_LIBPNG_VER_STRING,
			NULL, NULL, NULL);
	if (png_reader == NULL) {
		warnx("libpng: Can't create reader", NULL);
		return -1;
	}

	png_set_palette_to_rgb(png_reader);

	png_init_io(png_reader, infile);
	png_set_sig_bytes(png_reader, 8);

	png_infop png_info = png_create_info_struct(png_reader);
	if (png_info == NULL) {
		warnx("libpng: Can't create info", NULL);
		png_destroy_read_struct(&png_reader, NULL, NULL);
		return -1;
	}

	png_read_info(png_reader, png_info);

	if (setjmp(png_jmpbuf(png_reader))) {
		warnx("libpng: Weird error");
		png_read_end(png_reader, png_info);
		png_destroy_read_struct(&png_reader, &png_info, NULL);
		return -1;
	}

	int bit_depth, color_type;
	png_uint_32 w, h;
	png_get_IHDR(png_reader, png_info, &w, &h, &bit_depth,
			&color_type, NULL, NULL, NULL);
	*width = w;
	*height = h;

	*internalformat = typepng2gl(bit_depth, color_type, format);

	if (setjmp(png_jmpbuf(png_reader))) {
		warnx("libpng: Another weird error");
		png_read_end(png_reader, png_info);
		png_destroy_read_struct(&png_reader, &png_info, NULL);
		return -1;
	}

	size_t rowbytes = png_get_rowbytes(png_reader, png_info);

	if (color_type == PNG_COLOR_TYPE_PALETTE)
		rowbytes *= 3;

	*image = malloc((*height) * sizeof(png_bytep) * rowbytes);
	if (*image == NULL) {
		warn("malloc", NULL);
		png_read_end(png_reader, png_info);
		png_destroy_read_struct(&png_reader, &png_info, NULL);
		return -1;
	}

	png_bytep *rows = malloc((*height) * sizeof(png_bytep));
	if (rows == NULL) {
		free(*image);
		*image = NULL;
		png_read_end(png_reader, png_info);
		png_destroy_read_struct(&png_reader, &png_info, NULL);
		return -1;
	}

	for (long i = 0; i < *height; ++i)
		rows[i] = &((*image)[i * rowbytes]);

	png_read_image(png_reader, rows);

	free(rows);

	png_destroy_read_struct(&png_reader, &png_info, NULL);

	return 0;
}

/* TODO: Add support for more bit depths and color types */
static int typepng2gl(int bit_depth, int color_type, int *format) {
	int internalformat;
	switch(bit_depth) {
		case 4:
			switch(color_type) {
				case PNG_COLOR_TYPE_GRAY:
					internalformat = GL_LUMINANCE4;
					*format = GL_LUMINANCE;
					break;
				default:
					warnx("pngreader: bad color type: %i\n",
							color_type);
					internalformat = -1;
					*format = -1;
			}
			break;
		case 8:
			switch(color_type) {
				case PNG_COLOR_TYPE_GRAY:
					internalformat = GL_LUMINANCE8;
					*format = GL_LUMINANCE;
					break;
				case PNG_COLOR_TYPE_GRAY_ALPHA:
					internalformat = GL_LUMINANCE8_ALPHA8;
					*format = GL_LUMINANCE_ALPHA;
					break;
				case PNG_COLOR_TYPE_PALETTE:
				case PNG_COLOR_TYPE_RGB:
					internalformat = GL_RGB8;
					*format = GL_RGB;
					break;
				case PNG_COLOR_TYPE_RGB_ALPHA:
					internalformat = GL_RGBA8;
					*format = GL_RGBA;
					break;
				default:
					warnx("pngreader: bad color type: %i\n",
							color_type);
					internalformat = -1;
					*format = -1;
			}
			break;
		default:
			internalformat = -1;
			*format = -1;
	}

	return internalformat;
}
