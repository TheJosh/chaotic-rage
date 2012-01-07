%{
	#include <stdio.h>
	#include <stdlib.h>
	#include <vector>
	#include <iostream>
	#include "util/objload.h"
	
	using namespace std;
	
	#define YY_DECL WavefrontObj * yylex()

	#define YY_INPUT(buf,result,max_size) { \
		result = zzip_read((ZZIP_FILE*) yyin, buf, max_size); \
	}
%}

%option noyywrap

FLOAT -?[0-9]+(\.[0-9]+)?([eE]-[0-9]+)?
INT [0-9]+
COMMA ,
SLASH [/]
SPACE [ \t]+

%x vertex
%x texture
%x normal
%x face
%x material

%%

	float coordsf[3];
	int coordsi[9];
	int num_coords;
	int curr_mat = 0;
	
	WavefrontObj * ret = new WavefrontObj();
	
	
	/* Vertex */
^"v"{SPACE}	{BEGIN(vertex); num_coords = 0; coordsf[0] = coordsf[1] = coordsf[2] = 0.0;}

<vertex>{SPACE} {num_coords++; if (num_coords > 2) return NULL; }
<vertex>{FLOAT} {coordsf[num_coords] = atof(yytext);}
<vertex>\n {
		Vertex v = {coordsf[0], coordsf[1], coordsf[2]};
		ret->vertexes.push_back(v);
		BEGIN(INITIAL);
	}

	/* Texture */
^"vt"{SPACE}	{BEGIN(texture); num_coords = 0; coordsf[0] = coordsf[1] = coordsf[2] = 0.0;}
<texture>{SPACE} {num_coords++; if (num_coords > 2) return NULL;}
<texture>{FLOAT} {coordsf[num_coords] = atof(yytext);}
<texture>\n {
		TexUV v = {coordsf[0], coordsf[1]};
		ret->texuvs.push_back(v);
		BEGIN(INITIAL);
	}

	/* Normal */
^"vn"{SPACE}	{BEGIN(normal); num_coords = 0; coordsf[0] = coordsf[1] = coordsf[2] = 0.0;}

<normal>{SPACE} {num_coords++; if (num_coords > 2) return NULL;}
<normal>{FLOAT} {coordsf[num_coords] = atof(yytext);}
<normal>\n {
		Vertex v = {coordsf[0], coordsf[1], coordsf[2]};
		ret->normals.push_back(v);
		BEGIN(INITIAL);
	}
	
	
	/* Face */
^"f"{SPACE}	{BEGIN(face); num_coords = 0; coordsi[0] = coordsi[1] = coordsi[2] = coordsi[3] = coordsi[4] = coordsi[5] = coordsi[6] = coordsi[7] = coordsi[8] = 0;}

<face>{SLASH} {num_coords++;}
<face>{INT} {coordsi[num_coords] = atoi(yytext);}
<face>{SPACE} {
	if (num_coords <= 2) {
		num_coords = 3;
	} else if (num_coords <= 5) {
		num_coords = 6;
	}
}
<face>\n {
		Face v = {coordsi[0], coordsi[3], coordsi[6], coordsi[1], coordsi[4], coordsi[7], coordsi[2], coordsi[5], coordsi[8], curr_mat};
		ret->faces.push_back(v);
		BEGIN(INITIAL);
	}
	
	
	/* Use material */
^"usemtl"{SPACE}	{BEGIN(material);}
<material>[a-zA-Z_]+ {
		ret->materials.push_back(yytext);
		curr_mat = ret->materials.size() - 1;
		BEGIN(INITIAL);
	}
	
	
	/* Misc */
"#"[^\n]*$ {}
\n	{BEGIN(INITIAL);}
.	{}

<<EOF>> {
	return ret;
}
%%


WavefrontObj * loadObj(string filename)
{
	ZZIP_FILE *fp;

	fp = zzip_open(filename.c_str(), 0);
	if (fp == NULL) return NULL;

	yyin = (FILE*) fp;
	WavefrontObj * obj = yylex();
	obj->ibo_count = 0;
	
	zzip_close(fp);
	
	return obj;
}

