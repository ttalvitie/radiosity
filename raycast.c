#include "raycast.h"

#include <string.h>

struct raycast_tree_node_ {
	// Bounding box.
	float lo[3];
	float hi[3];
	
	triangle* trgs;
	size_t trgcount;
	
	raycast_tree_node* child1;
	raycast_tree_node* child2;
};

static inline int segment_intersects_node(
	vec3 a, vec3 b, raycast_tree_node* node
) {
	float ca[3] = {a.x, a.y, a.z};
	float cb[3] = {b.x, b.y, b.z};
	for(int i = 0; i < 3; ++i) {
		ca[i] = (ca[i] - node->lo[i]) / (node->hi[i] - node->lo[i]);
		cb[i] = (cb[i] - node->lo[i]) / (node->hi[i] - node->lo[i]);
	}
	
	if(
		ca[0] >= 0.0f && ca[0] <= 1.0f &&
		ca[1] >= 0.0f && ca[1] <= 1.0f &&
		ca[2] >= 0.0f && ca[2] <= 1.0
	) return 1;
	if(
		cb[0] >= 0.0f && cb[0] <= 1.0f &&
		cb[1] >= 0.0f && cb[1] <= 1.0f &&
		cb[2] >= 0.0f && cb[2] <= 1.0
	) return 1;
	
	for(int i = 0; i < 3; ++i) {
		for(int p = 0; p < 2; ++p) {
			float pf = (float)p;
			if((ca[i] < pf) == (cb[i] < pf)) continue;
			float t = (pf - ca[i]) / (cb[i] - ca[i]);
			
			int in = 1;
			for(int j = 0; j < 3; ++j) {
				if(i == j) continue;
				float c = (1.0f - t) * ca[j] + t * cb[j];
				if(c < 0.0f || c > 1.0f) in = 0;
			}
			
			if(in) return 1;
		}
	}
	
	return 0;
}

static inline float vec3_coord(vec3 v, int i) {
	if(i == 0) return v.x;
	if(i == 1) return v.y;
	return v.z;
}
static inline float triangle_min(triangle trg, int i) {
	float ret = 1.0f / 0.0f;
	for(int j = 0; j < 3; ++j) {
		float c = vec3_coord(trg.corners[j], i);
		if(c < ret) ret = c;
	}
	return ret;
}
static inline float triangle_max(triangle trg, int i) {
	float ret = -1.0f / 0.0f;
	for(int j = 0; j < 3; ++j) {
		float c = vec3_coord(trg.corners[j], i);
		if(c > ret) ret = c;
	}
	return ret;
}

static raycast_tree_node* create_tree(triangle* trgs, size_t trgcount) {
	if(trgcount == 0) return NULL;
	raycast_tree_node* tree = checked_malloc(sizeof(raycast_tree_node));
	
	for(int i = 0; i < 3; ++i) {
		tree->lo[i] = 1.0f / 0.0f;
		tree->hi[i] = -1.0f / 0.0f;
	}
	
	for(size_t i = 0; i < trgcount; ++i) {
		for(int j = 0; j < 3; ++j) {
			vec3 v = trgs[i].corners[j];
			for(int k = 0; k < 3; ++k) {
				float c = vec3_coord(v, k);
				if(c < tree->lo[k]) tree->lo[k] = c;
				if(c > tree->hi[k]) tree->hi[k] = c;
			}
		}
	}
	for(int i = 0; i < 3; ++i) {
		if(tree->lo[i] >= tree->hi[i]) {
			tree->hi[i] = tree->lo[i] + 1.0f;
		}
	}
	
	if(trgcount <= 8) {
		tree->trgs = trgs;
		tree->trgcount = trgcount;
		tree->child1 = NULL;
		tree->child2 = NULL;
	} else {
		// Split the bounding box by its longest side.
		int split_dir = 0;
		float split_len = -1.0f;
		for(int i = 0; i < 3; ++i) {
			float len = tree->hi[i] - tree->lo[i];
			if(len > split_len) {
				split_len = len;
				split_dir = i;
			}
		}
		
		float split_pos = 0.5f * (tree->lo[split_dir] + tree->hi[split_dir]);
		
		// Split the triangles to this node and the children by the split_dir
		// coordinates.
		size_t i = 0;
		for(size_t j = 0; j < trgcount; ++j) {
			if(
				triangle_min(trgs[j], split_dir) < split_pos &&
				triangle_max(trgs[j], split_dir) > split_pos
			) {
				triangle tmp = trgs[i];
				trgs[i] = trgs[j];
				trgs[j] = tmp;
				++i;
			}
		}
		
		tree->trgs = trgs;
		tree->trgcount = i;
		
		trgs += i;
		trgcount -= i;
		i = 0;
		for(size_t j = 0; j < trgcount; ++j) {
			if(triangle_max(trgs[j], split_dir) <= split_pos) {
				triangle tmp = trgs[i];
				trgs[i] = trgs[j];
				trgs[j] = tmp;
				++i;
			}
		}
		
		tree->child1 = create_tree(trgs, i);
		tree->child2 = create_tree(trgs + i, trgcount - i);
	}
	
	return tree;
}

raycast raycast_init(triangle* trgs, size_t trgcount) {
	raycast ctx;
	ctx.trgs = checked_malloc2(trgcount, sizeof(triangle));
	memcpy(ctx.trgs, trgs, trgcount * sizeof(triangle));
	
	ctx.tree = create_tree(trgs, trgcount);
	
	return ctx;
}

static int raycast_query_tree(raycast_tree_node* tree, vec3 a, vec3 b) {
	if(tree == NULL) return 0;
	if(!segment_intersects_node(a, b, tree)) return 0;
	for(size_t i = 0; i < tree->trgcount; ++i) {
		if(segment_intersects_triangle(a, b, tree->trgs[i])) return 1;
	}
	if(raycast_query_tree(tree->child1, a, b)) return 1;
	if(raycast_query_tree(tree->child2, a, b)) return 1;
	return 0;
}

int raycast_query(raycast ctx, vec3 a, vec3 b) {
	return raycast_query_tree(ctx.tree, a, b);
}

static void free_tree(raycast_tree_node* tree) {
	if(tree->child1) free_tree(tree->child1);
	if(tree->child2) free_tree(tree->child2);
	free(tree);
}

void raycast_free(raycast ctx) {
	free_tree(ctx.tree);
	free(ctx.trgs);
}
