#include <stdio.h>
#include <stddef.h>
#include <time.h>
#include "rbtree.h"
#define NODES       10
#define PERF_LOOPS  3
#define CHECK_LOOPS 3

struct test_node {
	unsigned int  key;
	struct rb_node rb;

	/* following fields used for testing augmented rbtree functionality */
	unsigned int val;
	unsigned int augmented;
};

static struct rb_root root = RB_ROOT;
static struct test_node nodes[NODES];


static void insert(struct test_node *node, struct rb_root *root)
{
	struct rb_node **new = &root->rb_node, *parent = NULL;
	unsigned int key = node->key;

	while (*new) {
		parent = *new;
		if (key < rb_entry(parent, struct test_node, rb)->key)
			new = &parent->rb_left;
		else
			new = &parent->rb_right;
	}

	rb_link_node(&node->rb, parent, new);
	rb_insert_color(&node->rb, root);
}

static inline void erase(struct test_node *node, struct rb_root *root)
{
	rb_erase(&node->rb, root);
}


static void init(void)
{
	int i;
	for (i = 0; i < NODES; i++) {
		nodes[i].key = (unsigned int)random();
		nodes[i].val = (unsigned int)random();
		printf("nodes[%d] key = %u val = %u\n", i, nodes[i].key, nodes[i].val);
	}
}

static int is_red(struct rb_node *rb)
{
	return !(rb->__rb_parent_color & 1);
}

static int black_path_count(struct rb_node *rb)
{
	int count;
	for (count = 0; rb; rb = rb_parent(rb))
		count += !is_red(rb);
	return count;
}
char *yes = "result = yes";
char *no = "result = no";
#define WARN_ON_ONCE(is)  
//( {char *str = is ? yes : no;  printf("%s %d: %s\n",__func__, __LINE__,str);})
static void check_postorder_foreach(int nr_nodes)
{
	struct test_node *cur, *n;
	int count = 0;
	rbtree_postorder_for_each_entry_safe(cur, n, &root, rb)
		count++;

	WARN_ON_ONCE(count != nr_nodes);
}

static void check_postorder(int nr_nodes)
{
	struct rb_node *rb;
	int count = 0;
	for (rb = rb_first_postorder(&root); rb; rb = rb_next_postorder(rb))
		count++;

	WARN_ON_ONCE(count != nr_nodes);
}

static void check(int nr_nodes)
{
	struct rb_node *rb;
	int count = 0, blacks = 0;
	unsigned int prev_key = 0;

	for (rb = rb_first(&root); rb; rb = rb_next(rb)) {
		struct test_node *node = rb_entry(rb, struct test_node, rb);
		WARN_ON_ONCE(node->key < prev_key);
		WARN_ON_ONCE(is_red(rb) &&
			     (!rb_parent(rb) || is_red(rb_parent(rb))));
		if (!count)
			blacks = black_path_count(rb);
		else
			WARN_ON_ONCE((!rb->rb_left || !rb->rb_right) &&
				     blacks != black_path_count(rb));
		prev_key = node->key;
		count++;
	}

	WARN_ON_ONCE(count != nr_nodes);
	WARN_ON_ONCE(count < (1 << black_path_count(rb_last(&root))) - 1);

	check_postorder(nr_nodes);
	check_postorder_foreach(nr_nodes);
}

int main (void)
{
	int i, j;
	time_t time1, time2;

	printf("rbtree testing");
	struct test_node *tmp;
	struct test_node *pos;
	struct rb_node *nod;
	init();
	time1 = time(NULL);

	for (i = 0; i < 1; i++) {
		for (j = 0; j < NODES; j++)
			insert(nodes + j, &root);
		j = 0;
		rbtree_postorder_for_each_entry_safe(pos, tmp, &root, rb)
			printf("test node %d:  key = %u val = %u\n", j++, pos->key, pos->val);

		j = 0;
		nod = rb_first(&root);
	        while (nod != NULL){
			pos = rb_entry(nod, struct test_node, rb);
			printf("nodes  %d:  key = %u val = %u\n",j++, pos->key, pos->val);
			nod = rb_next(nod);
		}
		nod = rb_last(&root);
	        while (nod != NULL){
			pos = rb_entry(nod, struct test_node, rb);
			printf("node1 %d:  key = %u val = %u\n",j++, pos->key, pos->val);
			nod = rb_prev(nod);
		}
		for (j = 0; j < NODES; j++)
			erase(nodes + j, &root);
	}

	time2 = time(NULL);

	printf(" -> %f cycles\n", difftime(time2, time1));



	return 0; /* Fail will directly unload the module */
}

//static inline unsigned int augment_recompute(struct test_node *node)
//{
//	unsigned int max = node->val, child_augmented;
//	if (node->rb.rb_left) {
//		child_augmented = rb_entry(node->rb.rb_left, struct test_node,
//					   rb)->augmented;
//		if (max < child_augmented)
//			max = child_augmented;
//	}
//	if (node->rb.rb_right) {
//		child_augmented = rb_entry(node->rb.rb_right, struct test_node,
//					   rb)->augmented;
//		if (max < child_augmented)
//			max = child_augmented;
//	}
//	return max;
//}
//
//RB_DECLARE_CALLBACKS(static, augment_callbacks, struct test_node, rb,
//		     unsigned int, augmented, augment_recompute)
//
//static void insert_augmented(struct test_node *node, struct rb_root *root)
//{
//	struct rb_node **new = &root->rb_node, *rb_parent = NULL;
//	unsigned int key = node->key;
//	unsigned int val = node->val;
//	struct test_node *parent;
//
//	while (*new) {
//		rb_parent = *new;
//		parent = rb_entry(rb_parent, struct test_node, rb);
//		if (parent->augmented < val)
//			parent->augmented = val;
//		if (key < parent->key)
//			new = &parent->rb.rb_left;
//		else
//			new = &parent->rb.rb_right;
//	}
//
//	node->augmented = val;
//	rb_link_node(&node->rb, rb_parent, new);
//	rb_insert_augmented(&node->rb, root, &augment_callbacks);
//}
//
//static void erase_augmented(struct test_node *node, struct rb_root *root)
//{
//	rb_erase_augmented(&node->rb, root, &augment_callbacks);
//}
//
//static void check_augmented(int nr_nodes)
//{
//	struct rb_node *rb;
//
//	check(nr_nodes);
//	for (rb = rb_first(&root); rb; rb = rb_next(rb)) {
//		struct test_node *node = rb_entry(rb, struct test_node, rb);
//		WARN_ON_ONCE(node->augmented != augment_recompute(node));
//	}
//}
