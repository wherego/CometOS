#define SPIN_DECLARE_LOCK(name) volatile bool name ## Locked
#define SPIN_LOCK(name) \
	while (!__sync_bool_compare_and_swap(& name ## Locked, 0, 1)); \
	__sync_synchronize();
#define SPIN_UNLOCK(name) \
	__sync_synchronize(); \
	name ## Locked = 0;