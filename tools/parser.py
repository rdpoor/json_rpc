import json
from cffi import FFI

"""
Overview

Given a C structure that contains some local state, such as:

    typedef struct {
      timestamp_t timestamp;
      bool is_pressed;
    } button_state_t;

... generate C code encoder and decoder functions along these lines:

// emits a JSON string like:
//
// {"fn": "button_state", "args":{"timestamp": 987654321, "is_pressed":true}}
//
// Here is the generated encoder:
//
void button_state_encode(button_state_t *state, jems_t *jems) {
  jems_reset(jems);
  jems_object_open(jems);
  jems_string(jems, "fn");
  jems_string(jems, "button_state");
  jems_string(jems, "args");
  jems_object_open(jems);
  jems_string(jems, "timestamp");
  jems_integer(jems, state->timestamp);
  jems_bool(jems, state->is_pressed);
  jems_object_close(jems);
  jems_object_close(jems);
}

// Here is the generated decoder.  Note that it returns true iff the JSON string
// matches the function signature.
//
bool button_state_decode(button_state_t *state, jrpc_t *jrpc) {
  // jrpc_parse is common to all decoders -- called previously.
  // By the time we get here, we know that the json string has "fn" and "args"
  // tokens in the right positions.  Check the button_state specific tokens.
  if (jrpc_token_count(jrpc) != 9) {   // 5 + 2n, where n is # of args
    return false;
  } else if (!jrpc_token_match(jrpc, 2, "button_state")) {
    return false;
  } else if (!jrpc_token_match(jrpc, 5, "timestamp") {
    return false;
  } else if (!jrpc_parse_integer(jrpc, 6, &state->timestamp)) {
    return false;
  } else if (!jrpc_token_match(jrpc, 7, "is_pressed")) {
    return false;
  } else if (!jrpc_parse_bool(jrpc, 8, &state->is_pressed)) {
    return false;
  } else {
    return true;
  }
}

"""

ffi = FFI()
ffi.cdef("""
// ffi.typeof('timestamp_t').kind = 'primitive'
typedef uint32_t timestamp_t;

// ffi.typeof('button_state_t').kind = 'struct'
typedef struct _button_state {
  timestamp_t timestamp;
  bool is_pressed;
} button_state_t;

// ffi.typeof('light_sensor_t').kind = 'enum'
typedef enum {
  COLOR_RED,
  COLOR_GRN,
  COLOR_BLU
} color_t;

// ffi.typeof('light_sensor_t').kind = 'struct'
typedef struct {
  float value;
  color_t color;
} light_sensor_t;

""")

class JrpcDescriptor(object):

    def __init__(self, ffi):
        self._ffi = ffi

    def ffi(self):
        return self._ffi

    def struct_names(self):
        """
        Iterate over the list of the ffi typedefs, return only those that
        define structs.
        """
        typedefs = self._ffi.list_types()[0]  # all typedefs
        return [x for x in typedefs if self._ffi.typeof(x).kind == 'struct']

    def message_name(self, typedef_name):
        """
        Convert `foo_t` to `foo`
        """
        return typedef_name.removesuffix('_t')

    def dump_descriptor_typedefs(self):
        return """
typedef struct {
    const char *name;
    const char *type;
} slot_descriptor_t;

typedef struct {
    const char *name;
    const slot_descriptor_t slots[]; // flexible array members (null terminated)
} message_descriptor_t;
"""

    def dump_descriptors(self):
        struct_names = self.struct_names()
        return ''.join([self.dump_descriptor(name) for name in struct_names])

    def dump_descriptor(self, typedef_name):
        message_name = self.message_name(typedef_name)
        return f"""
static const message_descriptor_t s_{message_name} = {{
    .name="{message_name}",
    .slots=
        {self.dump_slot_descriptors(typedef_name)},
        NULL
    }}
}};
"""

    def dump_slot_descriptors(self, typedef_name):
        t = ffi.typeof(typedef_name)
        slots = [f'{{.name="{field_name}", .type="{field.type.cname}"}}' for field_name, field in t.fields]
        return ',\n        '.join(slots)

j = JrpcDescriptor(ffi)
print(j.dump_descriptor_typedefs())
print(j.dump_descriptors())
