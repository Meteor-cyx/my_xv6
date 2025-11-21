// Mutual exclusion lock.
struct spinlock {
  uint locked;       // 是否被持有

  // For debugging:
  char *name;        // Name of lock.
  struct cpu *cpu;   // The cpu holding the lock.
};

