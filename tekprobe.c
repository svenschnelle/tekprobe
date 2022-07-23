#include <json-c/json_object.h>
#include <json-c/json_tokener.h>
#include <json-c/json_util.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <stdint.h>
#include <json-c/json.h>
#include <ctype.h>
#include <getopt.h>
#include <strings.h>

#define ARRAY_SIZE(x) ((int)(sizeof(x)/sizeof(x[0])))

typedef enum {
	ELEM_INSTRUMENT_INSTANCE=2,
	ELEM_ELEMENTS_REQUIRING_SUPPORT=3,
	ELEM_TEKPROBE_IDENTIFY=5,
	ELEM_TEKCONNECT_LOOPBACK=6,
	ELEM_TIMED_SEQUENCE=7,
	ELEM_IO_EXPANDER=8,
	ELEM_DAC_SETTING=9,
	ELEM_ENUMERATION=11,
	ELEM_ENUM_ASSERTION=14,
	ELEM_ENUM_NEXT_BUTTON=15,
	ELEM_SCALE=16,
	ELEM_INPUT_TRANSDUCER=17,
	ELEM_INSERT_DELAY=18,
	ELEM_LOWPASS=20,
	ELEM_POST_COMPENSATION1=21,
	ELEM_INPUT_RESISTANCE=22,
	ELEM_LOWPASS1=23,
	ELEM_INPUT_CAPACITANCE=24,
	ELEM_OFFSET_INJECTOR=25,
	ELEM_DYNAMIC_RANGE=26,
	ELEM_MAX_OPER_RANGE=27,
	ELEM_MAX_NONDESTRUCT_VOLTAGE=28,
	ELEM_COUPLING=30,
	ELEM_PATH_SWITCH=34,
	ELEM_I2C=36,
	ELEM_SCALE_MINMAX=37,
	ELEM_TEKPROBE_VERSION=38,
	ELEM_CURRENT_DRAW=39,
	ELEM_OUTPUT_RESISTANCE=58,
	ELEM_POT_SETTING=59,
	ELEM_POST_COMPENSATION0=70,
	ELEM_DAC_SETTING1=74,
	ELEM_TEKCONNECT_CALIBRATION=75,
	ELEM_DAC_SETTING2=78,
	ELEM_INTERRUPT=79,
	ELEM_INTERRUPT1=80,
	ELEM_CASCADE_CAPABLE=81,
	ELEM_REFLEX=82,
	ELEM_USER_LINEAR_CONTROL1=83,
	ELEM_VTERMSOURCE=84,
	ELEM_DYNAMIC_RANGE_EXTENDED=85,
	ELEM_COMMAND_EVENT_QUEUE=86,
	ELEM_POWER_DRAW=87,
	ELEM_OFFSET_INJECTOR1=88,
	ELEM_BUTTON_MONITOR=89,
	ELEM_TEMPERATURE_MONITOR=90,
	ELEM_OVERLOAD_MONITOR=91,
	ELEM_DYNAMIC_RANGE_FORCE=92,
	ELEM_EXPECTED_HOST_RESISTANCE=93,
	ELEM_COMMAND=94,
	ELEM_JAWS_MONITOR=129,
	ELEM_DEGAUSSER=130,
	ELEM_BYPASSER=131,
	ELEM_DRIFT_ERROR=132,
	ELEM_EXPECTED_HOST_COUPLING=133,
	ELEM_INPUT_CONNECTION_MODE=134,
	ELEM_INPUT_MODE_NEXT_BUTTON=136
} element_types_t;

static const char *elem_names[] = {
	[ELEM_INSTRUMENT_INSTANCE]		= "instrument_instance",
	[ELEM_ELEMENTS_REQUIRING_SUPPORT]	= "elements_requiring_support",
	[ELEM_TEKPROBE_IDENTIFY]		= "tekprobe_identify",
	[ELEM_TEKCONNECT_LOOPBACK]		= "tekconnect_loopback",
	[ELEM_TIMED_SEQUENCE]			= "timed_sequence",
	[ELEM_IO_EXPANDER]			= "io_expander",
	[ELEM_DAC_SETTING]			= "dac_setting",
	[ELEM_ENUMERATION]			= "enumeration",
	[ELEM_ENUM_ASSERTION]			= "enum_assertion",
	[ELEM_ENUM_NEXT_BUTTON]			= "enum_next_button",
	[ELEM_SCALE]				= "scale",
	[ELEM_INPUT_TRANSDUCER]			= "input_transducer",
	[ELEM_INSERT_DELAY]			= "insert_delay",
	[ELEM_LOWPASS]				= "lowpass",
	[ELEM_POST_COMPENSATION1]		= "post_compensation1",
	[ELEM_INPUT_RESISTANCE]			= "input_resistance",
	[ELEM_LOWPASS1]				= "lowpass1",
	[ELEM_INPUT_CAPACITANCE]		= "input_capacitance",
	[ELEM_OFFSET_INJECTOR]			= "offset_injector",
	[ELEM_DYNAMIC_RANGE]			= "dynamic_range",
	[ELEM_MAX_OPER_RANGE]			= "max_oper_range",
	[ELEM_MAX_NONDESTRUCT_VOLTAGE]		= "max_nondestruct_voltage",
	[ELEM_COUPLING]				= "coupling",
	[ELEM_PATH_SWITCH]			= "path_switch",
	[ELEM_I2C]				= "i2c",
	[ELEM_SCALE_MINMAX]			= "scale_minmax",
	[ELEM_TEKPROBE_VERSION]			= "tekprobe_version",
	[ELEM_CURRENT_DRAW]			= "current_draw",
	[ELEM_OUTPUT_RESISTANCE]		= "output_resistance",
	[ELEM_POT_SETTING]			= "pot_setting",
	[ELEM_POST_COMPENSATION0]		= "post_compensation0",
	[ELEM_DAC_SETTING1]			= "dac_setting1",
	[ELEM_TEKCONNECT_CALIBRATION]		= "tekconnect_calibration",
	[ELEM_DAC_SETTING2]			= "dac_setting2",
	[ELEM_INTERRUPT]			= "interrupt",
	[ELEM_INTERRUPT1]			= "interrupt1",
	[ELEM_CASCADE_CAPABLE]			= "cascade_capable",
	[ELEM_REFLEX]				= "reflex",
	[ELEM_USER_LINEAR_CONTROL1]		= "user_linear_control1",
	[ELEM_VTERMSOURCE]			= "vtermsource",
	[ELEM_DYNAMIC_RANGE_EXTENDED]		= "dynamic_range_extended",
	[ELEM_COMMAND_EVENT_QUEUE]		= "command_event_queue",
	[ELEM_POWER_DRAW]			= "power_draw",
	[ELEM_OFFSET_INJECTOR1]			= "offset_injector1",
	[ELEM_BUTTON_MONITOR]			= "button_monitor",
	[ELEM_TEMPERATURE_MONITOR]		= "temperature_monitor",
	[ELEM_OVERLOAD_MONITOR]			= "overload_monitor",
	[ELEM_DYNAMIC_RANGE_FORCE]		= "dynamic_range_force",
	[ELEM_EXPECTED_HOST_RESISTANCE]		= "expected_host_resistance",
	[ELEM_COMMAND]				= "command",
	[ELEM_JAWS_MONITOR]			= "jaws_monitor",
	[ELEM_DEGAUSSER]			= "degausser",
	[ELEM_BYPASSER]				= "bypasser",
	[ELEM_DRIFT_ERROR]			= "drift_error",
	[ELEM_EXPECTED_HOST_COUPLING]		= "expected_host_coupling",
	[ELEM_INPUT_CONNECTION_MODE]		= "input_connection_mode",
	[ELEM_INPUT_MODE_NEXT_BUTTON]		= "input_mode_next_button",
};

static const char *get_elem_name(unsigned int num)
{
	if (num > ARRAY_SIZE(elem_names) || !elem_names[num]) {
		printf("unknown type %d\n", num);
		return "invalid";
	}
	return elem_names[num];
}

static int get_elem_number(const char *name)
{
	for(int i = 0; i < ARRAY_SIZE(elem_names); i++)
		if (elem_names[i] && !strcasecmp(elem_names[i], name))
			return i;
	return -1;
}

static unsigned long get_block_len(unsigned char *p, int *len_bytes)
{
	unsigned long len = 0;
	unsigned char c;
	unsigned int shift = 0;

	if (len_bytes)
		*len_bytes = 0;
	do {
		c = *p++;
		len |= (c & 0x7f) << shift;
		shift += 7;
		if (len_bytes)
			(*len_bytes)++;
	} while(c & 0x80 && shift < 42);

	return len;
}

static unsigned char csum_block(unsigned char *p, unsigned long len)
{
	unsigned char csum = 0;
	unsigned long i;
	char c;

	for (i = 0; i < len; i++) {
		c = csum * 2;
		if (csum & 0x80)
			c = c + 1;
		csum = c + p[i];
	}
	return csum;
}

static unsigned char csum_gen(unsigned char *p, unsigned long len, int sum)
{
	unsigned char csum = csum_block(p, len);

	if (csum & 0x80)
		csum = csum * 2 + 1;
	else
		csum = csum * 2;
	csum = sum - csum;
	return csum;
}

static void json_add_array_int(struct json_object *obj, int len, uint8_t *p)

{
	struct json_object *array;
	int i;

	array = json_object_new_array_ext(len);

	for (i = 0; i < len; i++)
		json_object_array_put_idx(array, i, json_object_new_int(p[i]));

	json_object_object_add(obj, "elements", array);
}

static void json_add_int(struct json_object *obj, const char *name, int value)
{
	json_object_object_add(obj, name, json_object_new_int(value));
}

static void json_add_float(struct json_object *obj, const char *name, float value)
{
	json_object_object_add(obj, name, json_object_new_double(value));
}

static void parse_block(struct json_object *obj, uint8_t *p, int len);

static void parse_entry(struct json_object *parent, int type, uint8_t *p, int entrylen, int hdrlen)
{
	struct json_object *obj, *array;
	int j;

	obj = json_object_new_object();
	json_object_object_add(obj, "type", json_object_new_string(get_elem_name(type)));
//	json_object_object_add(obj, "offset", json_object_new_int((unsigned long)p & 0xfff));

	switch(type) {
	case ELEM_INSTRUMENT_INSTANCE:
		json_object_object_add(obj, "name", json_object_new_string((char *)p + hdrlen));
		json_object_object_add(obj, "serial", json_object_new_string((char *)p + strlen((char *)p)+1));
		break;

	case ELEM_ELEMENTS_REQUIRING_SUPPORT:
	{
		int len = entrylen - hdrlen;

		array = json_object_new_array_ext(len);
		for (int i = 0; i < entrylen - hdrlen; i++)
			json_object_array_put_idx(array, i, json_object_new_string(get_elem_name(p[hdrlen+i])));
		json_object_object_add(obj, "elements", array);
	}
	break;

	case ELEM_TEKPROBE_IDENTIFY:
		json_add_int(obj, "val0", p[hdrlen]);
		json_add_int(obj, "val1", p[hdrlen+1]);
		json_add_int(obj, "val2", p[hdrlen+2]);
		break;

	case ELEM_TIMED_SEQUENCE:
		array = json_object_new_array();
		for (j = 0; j < entrylen - hdrlen; j+=2) {
			struct json_object *obj3 = json_object_new_object();
			json_object_object_add(obj3, "setting", json_object_new_int(p[hdrlen+j]));
			json_object_object_add(obj3, "settling_time", json_object_new_int(p[hdrlen+j+1]));
			json_object_array_add(array, obj3);
		}
		json_object_object_add(obj, "members", array);
		break;

	case ELEM_IO_EXPANDER:
		json_add_int(obj, "slave", p[hdrlen]);
		json_add_int(obj, "address", p[hdrlen+1]);
		json_add_int(obj, "value", p[hdrlen+2]);
		break;

	case ELEM_ENUMERATION:
		json_object_object_add(obj, "value", json_object_new_string((char *)p + hdrlen));
		break;

	case ELEM_ENUM_ASSERTION:
		json_add_int(obj, "number", p[hdrlen]);
		json_add_int(obj, "value", p[hdrlen+1]);
		break;

	case ELEM_ENUM_NEXT_BUTTON:
		json_add_int(obj, "number", p[hdrlen]);
		json_add_int(obj, "slave", p[hdrlen+1]);
		json_add_int(obj, "pollvalue", p[hdrlen+2]);
		break;

	case ELEM_SCALE:
	case ELEM_INSERT_DELAY:
	case ELEM_LOWPASS:
	case ELEM_INPUT_RESISTANCE:
	case ELEM_INPUT_CAPACITANCE:
	case ELEM_OFFSET_INJECTOR:
	case ELEM_DYNAMIC_RANGE:
	case ELEM_MAX_OPER_RANGE:
		json_object_object_add(obj, "value", json_object_new_double((double)*(float *)(p + hdrlen)));
		break;

	case ELEM_POST_COMPENSATION1:
		json_add_int(obj, "val0", p[hdrlen]);
		json_add_int(obj, "val1", p[hdrlen+1]);
		break;

	case ELEM_LOWPASS1:
		json_object_object_add(obj, "val0", json_object_new_int(p[hdrlen]));
		json_object_object_add(obj, "val1", json_object_new_int(p[hdrlen+1]));
		array = json_object_new_array();
		parse_block(array, p + hdrlen + 2, entrylen-hdrlen-2);
		json_object_object_add(obj, "members", array);
		break;

	case ELEM_COUPLING:
		json_object_object_add(obj, "value", json_object_new_int(p[hdrlen]));
		break;

	case ELEM_PATH_SWITCH:
	{
		struct json_object *array2;
		int len_bytes, branch, count = 0;

		array = json_object_new_array();

		p += hdrlen + 1;
		entrylen -= hdrlen + 1;

		while(entrylen > 0) {
			branch = get_block_len(p, &len_bytes);
			array2 = json_object_new_array();
			parse_block(array2, p + len_bytes, branch - len_bytes);
			json_object_array_add(array, array2);
			entrylen -= branch;
			p += branch;
			count++;
		}
		json_add_int(obj, "count", count);
		json_object_object_add(obj, "members", array);
		break;
	}
	case ELEM_I2C:
		json_add_int(obj, "slave", p[hdrlen]);
		json_add_int(obj, "address", p[hdrlen+1]);
		json_add_int(obj, "val0", p[hdrlen+2]);
		json_add_int(obj, "val1", p[hdrlen+3]);
		break;

	case ELEM_SCALE_MINMAX:
		json_add_float(obj, "min", *(float *)(p+hdrlen));
		json_add_float(obj, "max", *(float *)(p+hdrlen+4));
		break;

	case ELEM_TEKPROBE_VERSION:
		json_add_int(obj, "val0", p[hdrlen]);
		json_add_int(obj, "val1", p[hdrlen+1]);
		break;

	case ELEM_CURRENT_DRAW:
		json_add_int(obj, "current_5v_pos", p[hdrlen]);
		json_add_int(obj, "current_15v_pos", p[hdrlen+1]);
		json_add_int(obj, "current_5v_neg", p[hdrlen+2]);
		json_add_int(obj, "current_15v_neg", p[hdrlen+3]);
		break;

	case ELEM_POT_SETTING:
		json_add_int(obj, "val0", p[hdrlen]);
		json_add_int(obj, "val1", p[hdrlen+1]);
		json_add_int(obj, "val2", p[hdrlen+2]);
		break;

	case ELEM_CASCADE_CAPABLE:
		json_add_int(obj, "val0", p[hdrlen]);
		json_add_int(obj, "val1", p[hdrlen+1]);
		break;

	case ELEM_USER_LINEAR_CONTROL1:
		json_add_float(obj, "usermax", *(float *)(p+hdrlen+2));
		json_add_float(obj, "resolution", *(float *)(p+hdrlen+6));
		json_add_float(obj, "controlmax", *(float *)(p+hdrlen+10));
		json_add_float(obj, "controlmin", *(float *)(p+hdrlen+14));
		json_add_float(obj, "usermin", *(float *)(p+hdrlen+20));
		break;

	case ELEM_VTERMSOURCE:
		json_add_int(obj, "value", p[hdrlen]);
		break;
	case ELEM_POST_COMPENSATION0:
		json_add_int(obj, "val0", p[hdrlen]);
		json_add_int(obj, "val1", p[hdrlen+1]);
		json_add_int(obj, "val2", *(uint32_t *)(p+hdrlen+2));
		break;
	default:
		printf("unknown type %d, len=%d @%lx\n", type, entrylen, (unsigned long)p & 0xfff);
		json_add_array_int(obj, entrylen - hdrlen, p + hdrlen);
		break;
	}
	json_object_array_add(parent, obj);
}

static void parse_block(struct json_object *obj, uint8_t *p, int len)
{
	int len_bytes, i, entrylen, hdrlen;

	for (i = 0; i < len; i += entrylen, p += entrylen) {
		entrylen = get_block_len(p, &len_bytes);

		if (!entrylen)
			break;
		if (i + entrylen > len) {
			printf("record too long: pos %x, entrylen %x, max is %x, p %lx\n", i, entrylen, len,
			       (unsigned long)p & 0xfff);
			break;
		}
		int type = p[len_bytes];

		hdrlen = len_bytes + 1;
		parse_entry(obj, type, p, entrylen, hdrlen);
	}
}

struct tekprobe_level2 {
	uint8_t len;
	uint8_t flags;
	float scale;
	float offsetscale;
	float impedance;
	float dynamic_range;
	float probe_power;
	char input_units;
	char nul;
	char model[];
} __attribute__((packed));

const struct option options[] = {
	{ "read", required_argument, 0, 'r' },
	{ "write", required_argument, 0, 'w' },
	{ "json", required_argument, 0, 'j' },
	{ NULL, 0, 0, 0 }
};

struct output_buffer {
	uint8_t *buf;
	unsigned int alloc_len;
	unsigned int used_len;
};

static void output_buffer_append(struct output_buffer *out, uint8_t data)
{
	if (!out->buf) {
		out->buf = malloc(1024);
		out->alloc_len = 1024;
		out->used_len = 0;
	}
	if (out->alloc_len == out->used_len)
		out->buf = realloc(out->buf, out->alloc_len * 2);
	out->buf[out->used_len++] = data;
}

static void output_add_json_int(struct output_buffer *out,
				struct json_object *obj,
				const char *key)
{
	output_buffer_append(out, json_object_get_int(json_object_object_get(obj, key)));
}

static void output_add_json_int32(struct output_buffer *out,
				struct json_object *obj,
				const char *key)
{
	uint32_t val = json_object_get_int(json_object_object_get(obj, key));
	output_buffer_append(out, ((uint8_t *)&val)[0]);
	output_buffer_append(out, ((uint8_t *)&val)[1]);
	output_buffer_append(out, ((uint8_t *)&val)[2]);
	output_buffer_append(out, ((uint8_t *)&val)[3]);
}

static void output_add_json_float(struct output_buffer *out,
				struct json_object *obj,
				const char *key)
{
	float f = json_object_get_double(json_object_object_get(obj, key));
	output_buffer_append(out, ((uint8_t *)&f)[0]);
	output_buffer_append(out, ((uint8_t *)&f)[1]);
	output_buffer_append(out, ((uint8_t *)&f)[2]);
	output_buffer_append(out, ((uint8_t *)&f)[3]);
}

static void write_object(struct json_object *obj, struct output_buffer *out);
static int append_json_obj(struct output_buffer *out, struct json_object *obj)
{
	for (int i = 0, len = json_object_array_length(obj); i < len; i++)
		write_object(json_object_array_get_idx(obj, i), out);
	return 0;
}

static int get_required_len_length(unsigned int len)
{
	int ret = 0;
	while(len) {
		len >>= 7;
		ret++;
	}
	return ret;
}

static void output_buffer_append_len(struct output_buffer *out, unsigned int len)
{
	uint8_t byte;

	len += get_required_len_length(len);

	while(len) {
		byte = len & 0x7f;
		len >>= 7;
		if (len)
			byte |= 0x80;
		output_buffer_append(out, byte);
	}

}

static void output_buffer_append_hdr(struct output_buffer *out, int nr, unsigned int len)
{
	output_buffer_append_len(out, len + 1);
	output_buffer_append(out, nr);
}

static void write_object(struct json_object *obj, struct output_buffer *out)
{
	struct json_object *type;
	const char *stype;

	type = json_object_object_get(obj, "type");
	if (!type) {
		fprintf(stderr, "invalid object, no type: %s\n",
			json_object_to_json_string(obj));
		return;
	}

	stype = json_object_get_string(type);
	if (!stype) {
		fprintf(stderr, "failed to get type string\n");
		return;
	}

	int nr = get_elem_number(stype);
	switch(nr) {
	case ELEM_INSTRUMENT_INSTANCE:
	{
		const char *name = json_object_get_string(json_object_object_get(obj, "name"));
		const char *serial = json_object_get_string(json_object_object_get(obj, "serial"));
		output_buffer_append_hdr(out, nr, strlen(name) + strlen(serial) + 2);
		for (size_t i = 0; i < strlen(name); i++)
			output_buffer_append(out, name[i]);
		output_buffer_append(out, 0);
		for (size_t i = 0; i < strlen(serial); i++)
			output_buffer_append(out, serial[i]);
		output_buffer_append(out, 0);
		break;
	}

	case ELEM_ELEMENTS_REQUIRING_SUPPORT:
	{
		struct output_buffer out2 = {};
		struct json_object *members = json_object_object_get(obj, "elements");
		for (size_t i = 0; i < json_object_array_length(members); i++) {
			struct json_object *obj2 = json_object_array_get_idx(members, i);
			const char *name = json_object_get_string(obj2);
			int num = get_elem_number(name);
			if (num == -1) {
				fprintf(stderr, "invalid element name %s\n", name);
				continue;
			}
			output_buffer_append(&out2, num);
		}
		output_buffer_append_hdr(out, nr, out2.used_len);
		for (unsigned int i = 0; i < out2.used_len; i++)
			output_buffer_append(out, out2.buf[i]);
		free(out2.buf);
		break;
	}

	case ELEM_CURRENT_DRAW:
		output_buffer_append_hdr(out, nr, 4);
		output_add_json_int(out, obj, "current_5v_pos");
		output_add_json_int(out, obj, "current_15v_pos");
		output_add_json_int(out, obj, "current_5v_neg");
		output_add_json_int(out, obj, "current_15v_neg");
		break;

	case ELEM_CASCADE_CAPABLE:
	case ELEM_TEKPROBE_VERSION:
		output_buffer_append_hdr(out, nr, 2);
		output_add_json_int(out, obj, "val0");
		output_add_json_int(out, obj, "val1");
		break;

	case ELEM_INPUT_RESISTANCE:
	case ELEM_INPUT_CAPACITANCE:
	case ELEM_MAX_OPER_RANGE:
	case ELEM_INSERT_DELAY:
	case ELEM_DYNAMIC_RANGE:
	case ELEM_LOWPASS:
	case ELEM_SCALE:
	case ELEM_OFFSET_INJECTOR:
		output_buffer_append_hdr(out, nr, 4);
		output_add_json_float(out, obj, "value");
		break;

	case ELEM_ENUMERATION:
		const char *s = json_object_get_string(json_object_object_get(obj, "value"));
		output_buffer_append_hdr(out, nr, strlen(s) + 1);
		for (size_t i = 0; i < strlen(s); i++)
			output_buffer_append(out, s[i]);
		output_buffer_append(out, 0);
		break;
	case ELEM_ENUM_ASSERTION:
		output_buffer_append_hdr(out, nr, 2);
		output_add_json_int(out, obj, "number");
		output_add_json_int(out, obj, "value");
		break;

	case ELEM_ENUM_NEXT_BUTTON:
		output_buffer_append_hdr(out, nr, 3);
		output_add_json_int(out, obj, "number");
		output_add_json_int(out, obj, "slave");
		output_add_json_int(out, obj, "pollvalue");
		break;

	case ELEM_I2C:
		output_buffer_append_hdr(out, nr, 4);
		output_add_json_int(out, obj, "slave");
		output_add_json_int(out, obj, "address");
		output_add_json_int(out, obj, "val0");
		output_add_json_int(out, obj, "val1");
		break;

	case ELEM_COUPLING:
		output_buffer_append_hdr(out, nr, 1);
		output_add_json_int(out, obj, "value");
		break;

	case ELEM_PATH_SWITCH:
	{
		struct output_buffer out2 = {};
		struct json_object *array = json_object_object_get(obj, "members");
		size_t i, j;

		for (i = 0; i < json_object_array_length(array); i++) {
			struct output_buffer out3 = {};
			append_json_obj(&out3, json_object_array_get_idx(array, i));
			output_buffer_append_len(&out2, out3.used_len);
			for (j = 0; j < out3.used_len; j++)
				output_buffer_append(&out2, out3.buf[j]);
			free(out3.buf);
			out3.buf = NULL;
			out3.alloc_len = 0;
			out3.used_len = 0;
		}
		output_buffer_append_hdr(out, nr, out2.used_len + 1);
		output_add_json_int(out, obj, "count");
		for (unsigned int i = 0; i < out2.used_len; i++)
			output_buffer_append(out, out2.buf[i]);
		free(out2.buf);
		break;
	}

	case ELEM_TEKPROBE_IDENTIFY:
	{
		output_buffer_append_hdr(out, nr, 3);
		output_add_json_int(out, obj, "val0");
		output_add_json_int(out, obj, "val1");
		output_add_json_int(out, obj, "val2");
		break;
	}
	case ELEM_TEKCONNECT_LOOPBACK:
	{
		struct output_buffer out2 = {};
		append_json_obj(&out2, json_object_object_get(obj, "members"));
		output_buffer_append_hdr(out, nr, out2.used_len-1);
		for (unsigned int i = 1; i < out2.used_len; i++)
			output_buffer_append(out, out2.buf[i]);
		free(out2.buf);
		break;
	}

	case ELEM_IO_EXPANDER:
		output_buffer_append_hdr(out, nr, 3);
		output_add_json_int(out, obj, "slave");
		output_add_json_int(out, obj, "address");
		output_add_json_int(out, obj, "value");
		break;

	case ELEM_SCALE_MINMAX:
		output_buffer_append_hdr(out, nr, 8);
		output_add_json_float(out, obj, "min");
		output_add_json_float(out, obj, "max");
		break;

	case ELEM_POST_COMPENSATION0:
		output_buffer_append_hdr(out, nr, 6);
		output_add_json_int(out, obj, "val0");
		output_add_json_int(out, obj, "val1");
		output_add_json_int32(out, obj, "val2");
		break;

	case ELEM_POST_COMPENSATION1:
		output_buffer_append_hdr(out, nr, 2);
		output_add_json_int(out, obj, "val0");
		output_add_json_int(out, obj, "val1");
		break;

	case ELEM_TIMED_SEQUENCE:
	{
		struct output_buffer out2 = {};
		struct json_object *members = json_object_object_get(obj, "members");
		for (size_t i = 0; i < json_object_array_length(members); i++) {
			struct json_object *obj2 = json_object_array_get_idx(members, i);
			output_add_json_int(&out2, obj2, "setting");
			output_add_json_int(&out2, obj2, "settling_time");
		}
		output_buffer_append_hdr(out, nr, out2.used_len);
		for (unsigned int i = 0; i < out2.used_len; i++)
			output_buffer_append(out, out2.buf[i]);
		free(out2.buf);
		break;
	}

	case ELEM_POT_SETTING:
		output_buffer_append_hdr(out, nr, 3);
		output_add_json_int(out, obj, "val0");
		output_add_json_int(out, obj, "val1");
		output_add_json_int(out, obj, "val2");
		break;

	case ELEM_VTERMSOURCE:
		output_buffer_append_hdr(out, nr, 1);
		output_add_json_int(out, obj, "value");
		break;

	case ELEM_LOWPASS1:
	{
		struct output_buffer out2 = {};
		append_json_obj(&out2, json_object_object_get(obj, "members"));
		output_buffer_append_hdr(out, nr, out2.used_len);
		output_add_json_int(out, obj, "val0");
		output_add_json_int(out, obj, "val1");
		for (unsigned int i = 0; i < out2.used_len; i++)
			output_buffer_append(out, out2.buf[i]);
		free(out2.buf);
		break;
	}

	case ELEM_USER_LINEAR_CONTROL1:
		output_buffer_append_hdr(out, nr, 20);
		output_add_json_float(out, obj, "usermax");
		output_add_json_float(out, obj, "resolution");
		output_add_json_float(out, obj, "controlmax");
		output_add_json_float(out, obj, "controlmin");
		output_add_json_float(out, obj, "usermin");
		break;
	default:
		fprintf(stderr, "unhandled type: %d (%s)\n", nr, stype);
		break;
	}
}

static int write_file(const char *pgm, const char *json, const char *outfile)
{
	enum json_tokener_error error;
	struct output_buffer out = {}, out2 = {};
	struct json_object *obj, *l2, *l3, *array;
	struct stat stat;
	int fd, jsonfd;
	char *buf;
	size_t i;

	fd = open(outfile, O_WRONLY|O_TRUNC|O_CREAT, 0644);
	if (fd == -1) {
		fprintf(stderr, "%s: failed to open %s: %m\n", pgm, outfile);
		return 1;
	}

	if (!json)
		json = "/dev/stdin";
	jsonfd = open(json, O_RDONLY);
	if (fd == -1) {
		fprintf(stderr, "%s: failed to open %s: %m\n", pgm, json);
		return 1;
	}

	if (fstat(jsonfd, &stat) == -1) {
		fprintf(stderr, "fstat: %m\n");
		close(fd);
		return 1;
	}

	buf = mmap(NULL, stat.st_size, MAP_SHARED, PROT_READ, jsonfd, 0);
	if (buf == MAP_FAILED) {
		fprintf(stderr, "mmap: %m\n");
		close(fd);
		return 1;
	}

	obj = json_tokener_parse_verbose(buf, &error);
	if (!obj) {
		fprintf(stderr, "failed to parse json file: %d\n", error);
		close(fd);
		return 1;
	}

	l2 = json_object_object_get(obj, "l2");
	array = json_object_object_get(l2, "elements");
	output_buffer_append(&out, json_object_array_length(array)+2);
	for (i = 0; i < json_object_array_length(array); i++)
		output_buffer_append(&out, json_object_get_int(json_object_array_get_idx(array, i)));
	output_buffer_append(&out, csum_gen(out.buf, out.used_len, 0xaa));
	write(fd, out.buf, out.used_len);

	free(out.buf);
	out.buf = NULL;
	l3 = json_object_object_get(obj, "l3");
	if (!append_json_obj(&out, l3)) {
		output_buffer_append_len(&out2, out.used_len + 1);
		for (i = 0; i < out.used_len; i++)
			output_buffer_append(&out2, out.buf[i]);
		output_buffer_append(&out2, csum_gen(out2.buf, out2.used_len, 0xaa));
		output_buffer_append(&out2, 0);
		write(fd, out2.buf, out2.used_len);
		free(out2.buf);
		free(out.buf);
	}
	else
		fprintf(stderr, "append_json_obj failed\n");

	close(fd);
	return 0;
}

static int read_file(const char *pgm, const char *infile, const char *outfile)
{
	struct json_object *l2, *l3;
	int len_bytes, len, fd;
	struct json_object *obj;
	unsigned char *buf;
	struct stat stat;

	fd = open(infile, O_RDONLY);
	if (fd == -1) {
		fprintf(stderr, "%s: failed to open %s: %m\n", pgm, infile);
		return 1;
	}

	if (fstat(fd, &stat) == -1) {
		fprintf(stderr, "fstat: %m\n");
		close(fd);
		return 1;
	}

	buf = mmap(NULL, stat.st_size, MAP_SHARED, PROT_READ, fd, 0);
	if (buf == MAP_FAILED) {
		fprintf(stderr, "mmap: %m\n");
		close(fd);
		return 1;
	}

	obj = json_object_new_object();
	l2 = json_object_new_object();
	json_object_object_add(obj, "l2", l2);
	json_add_array_int(l2, buf[0]-2, buf+1);

	len = get_block_len(buf + buf[0], &len_bytes);
	l3 = json_object_new_array();
	json_object_object_add(obj, "l3", l3);
	parse_block(l3, buf + buf[0] + len_bytes, len - len_bytes - 1);

	if (outfile)
		json_object_to_file_ext(outfile, obj, JSON_C_TO_STRING_PRETTY);
	else
		printf("%s", json_object_to_json_string_ext(obj, JSON_C_TO_STRING_PRETTY));
	return 0;
}

int main(int argc, char **argv)
{
	int c, optidx, read = 0, write = 0;
	const char *filename = NULL, *json = NULL;

	while ((c = getopt_long(argc, argv, "r:w:j:", options, &optidx)) != -1) {
		switch(c) {
		case 'r':
			filename = optarg;
			read = 1;
			break;
		case 'w':
			filename = optarg;
			write = 1;
			break;
		case 'j':
			json = optarg;
			break;
		default:
			break;
		}
	}

	if (!filename) {
		fprintf(stderr, "missing filename\n");
		return 1;
	}

//        unsigned char csum = csum_gen(buf, buf[0] - 1, 0);
//	printf("csum block 0 = %x, should be %x\n", csum_block(buf, buf[0]), csum);
//	buf[0x26] = csum;
//	struct tekprobe_level2 *l2 = buf;
//	if (l2->len > sizeof(struct tekprobe_level2))
//		printf("type %02x, attenuation %3.4f, val2 %3.1f, impedance %2.0eOhm, dynamic range +/- %3.1f, val4 %3.1f, unit %c model %s serial %s\n",
//		       l2->type, l2->attenuation, l2->val2, l2->impedance, l2->dynamic_range, l2->val4, l2->unit, l2->model, l2->model + strlen(l2->model)+1);

//	printf("csum block 1 = %x\n", csum_block(buf + buf[0], len + len_bytes));
//	dump_block(buf + buf[0] + len_bytes, len, 0);
//	}

	if (read)
		return read_file(argv[1], filename, json);

	if (write)
		return write_file(argv[1], json, filename);
	return 1;
}
