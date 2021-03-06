!!ver 130
!!permu LIGHTSTYLED
!!samps diffuse reflectcube normalmap

!!permu FAKESHADOWS
!!cvardf r_glsl_pcf
!!samps =FAKESHADOWS shadowmap

!!samps lightmap
!!samps =LIGHTSTYLED lightmap1 lightmap2 lightmap3
!!cvardf gl_mono=0

#include "sys/defs.h"

varying vec2 tex_c;

varying vec2 lm0;
#ifdef LIGHTSTYLED
varying vec2 lm1, lm2, lm3;
#endif

#ifdef REFLECTCUBE
varying vec3 eyevector;
varying mat3 invsurface;
#endif

#ifdef FAKESHADOWS
	varying vec4 vtexprojcoord;
#endif

#ifdef VERTEX_SHADER
	void lightmapped_init(void)
	{
		lm0 = v_lmcoord;
		#ifdef LIGHTSTYLED
		lm1 = v_lmcoord2;
		lm2 = v_lmcoord3;
		lm3 = v_lmcoord4;
		#endif
	}

	void main ()
	{
		lightmapped_init();
		tex_c = v_texcoord;
		gl_Position = ftetransform();
		
#ifdef REFLECTCUBE
		invsurface[0] = v_svector;
		invsurface[1] = v_tvector;
		invsurface[2] = v_normal;
		vec3 eyeminusvertex = e_eyepos - v_position.xyz;
		eyevector.x = dot( eyeminusvertex, v_svector.xyz );
		eyevector.y = dot( eyeminusvertex, v_tvector.xyz );
		eyevector.z = dot( eyeminusvertex, v_normal.xyz );
#endif
	}
#endif

#ifdef FRAGMENT_SHADER
	#include "sys/pcf.h"

	vec3 lightmap_fragment(void)
	{
		vec3 lightmaps;

#ifdef LIGHTSTYLED
		lightmaps  = texture2D(s_lightmap0, lm0).rgb * e_lmscale[0].rgb;
		lightmaps += texture2D(s_lightmap1, lm1).rgb * e_lmscale[1].rgb;
		lightmaps += texture2D(s_lightmap2, lm2).rgb * e_lmscale[2].rgb;
		lightmaps += texture2D(s_lightmap3, lm3).rgb * e_lmscale[3].rgb;
#else
		lightmaps  = texture2D(s_lightmap, lm0).rgb * e_lmscale.rgb;
#endif
		return lightmaps;
	}

	void main ( void )
	{
		vec4 diffuse_f = texture2D(s_diffuse, tex_c);

/* get the alphatesting out of the way first */
#ifdef MASK
		if (diffuse_f.a < 0.6) {
			discard;
		}
#endif
		/* lighting */
		diffuse_f.rgb *= lightmap_fragment();

#ifdef REFLECTCUBE
	#ifdef BUMP
		#ifndef FLATTENNORM
			vec3 normal_f = normalize(texture2D(s_normalmap, tex_c).rgb - 0.5);
		#else
			// For very flat surfaces and gentle surface distortions, the 8-bit precision per channel in the normalmap
			// can be insufficient. This is a hack to instead have very wobbly normalmaps that make use of the 8 bits
			// and then scale the wobblyness back once in the floating-point domain.
			vec3 normal_f = texture2D(s_normalmap, tex_c).rgb - 0.5;
			normal_f.x *= 0.0625;
			normal_f.y *= 0.0625;
			normal_f = normalize(normal_f);
		#endif
	#else
			vec3 normal_f = vec3(0, 0, 1);
	#endif
		vec3 cube_c;

		cube_c = reflect( normalize(-eyevector), normal_f);
		cube_c = cube_c.x * invsurface[0] + cube_c.y * invsurface[1] + cube_c.z * invsurface[2];
		cube_c = ( m_model * vec4(cube_c.xyz, 0.0)).xyz;
		diffuse_f.rgb = mix( textureCube(s_reflectcube, cube_c ).rgb, diffuse_f.rgb, diffuse_f.a);
#endif

		diffuse_f *= e_colourident;

		if (gl_mono == 1.0) {
			float bw = (diffuse_f.r + diffuse_f.g + diffuse_f.b) / 3.0;
			diffuse_f.rgb = vec3(bw, bw, bw);
		}

	#ifdef FAKESHADOWS
		diffuse_f.rgb *= ShadowmapFilter(s_shadowmap, vtexprojcoord);
	#endif

		gl_FragColor = diffuse_f;
		
	}
#endif
