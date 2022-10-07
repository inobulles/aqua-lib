#if !defined(__AQUA_LIB__ROOT)
#define __AQUA_LIB__ROOT

#define __STDC_WANT_LIB_EXT2__ 1 // ISO/IEC TR 24731-2:2010 standard library extensions

#if defined(__linux__)
	#define _GNU_SOURCE // cf. 'feature_test_macros(7)'
#endif

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef uint64_t (*kos_query_device_t) (uint64_t _, uint64_t name);
typedef uint64_t (*kos_send_device_t) (uint64_t _, uint64_t device, uint64_t command, uint64_t data);

static kos_query_device_t kos_query_device = NULL;
static kos_send_device_t kos_send_device = NULL;

void aqua_set_kos_functions(kos_query_device_t _kos_query_device, kos_send_device_t _kos_send_device) {
	kos_query_device = _kos_query_device;
	kos_send_device = _kos_send_device;
}

static void must_run_with_aqua_kos(void) {
	fprintf(stderr, "You must run this program with the AQUA KOS\n");
}

typedef uint64_t device_t;

device_t query_device(const char* name) {
	if (!kos_query_device) {
		must_run_with_aqua_kos();
		return -1;
	}

	return kos_query_device(0, (uint64_t) name);
}

uint64_t send_device(device_t device, uint16_t command, void* data) {
	if (!kos_send_device) {
		must_run_with_aqua_kos();
		return -1;
	}

	return kos_send_device(0, device, (uint64_t) command, (uint64_t) data);
}

// entry symbol

int main(void);

int __native_entry(void) {
	int rv = main();
	return rv;
}

#endif
