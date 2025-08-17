static extern int x; /* conflicting storage-class specifiers */
int *const volatile p; /* ok at parse, but test conflict detection later */
